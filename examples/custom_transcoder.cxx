/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include <couchbase/cluster.hxx>
#include <couchbase/codec/encoded_value.hxx>
#include <couchbase/codec/tao_json_serializer.hxx>
#include <couchbase/logger.hxx>

#include <couchbase_encryption/aead_aes_256_cbc_hmac_sha512_provider.hxx>
#include <couchbase_encryption/default_manager.hxx>
#include <couchbase_encryption/document.hxx>
#include <couchbase_encryption/insecure_keyring.hxx>

#include <spdlog/fmt/bundled/format.h>

#include <couchbase/fmt/error.hxx>

#include <tao/json/forward.hpp>
#include <tao/json/to_string.hpp>
#include <tao/json/type.hpp>

#include <string>
#include <utility>

static constexpr auto connection_string{ "couchbase://192.168.106.128" };
static constexpr auto username{ "Administrator" };
static constexpr auto password{ "password" };
static constexpr auto bucket_name{ "default" };
static constexpr auto scope_name{ couchbase::scope::default_name };
static constexpr auto collection_name{ couchbase::collection::default_name };

auto
make_bytes(std::vector<unsigned char> v) -> std::vector<std::byte>
{
  std::vector<std::byte> out{ v.size() };
  std::transform(v.begin(), v.end(), out.begin(), [](int c) {
    return static_cast<std::byte>(c);
  });
  return out;
}

auto
make_crypto_manager() -> std::shared_ptr<couchbase::crypto::default_manager>
{
  const auto key = make_bytes({
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
  });

  const auto keyring = std::make_shared<couchbase::crypto::insecure_keyring>();
  keyring->add_key(couchbase::crypto::key("test-key", key));

  const auto provider = couchbase::crypto::aead_aes_256_cbc_hmac_sha512_provider(keyring);

  auto manager = std::make_shared<couchbase::crypto::default_manager>();
  manager->register_default_encrypter(provider.encrypter_for_key("test-key"));
  manager->register_decrypter(provider.decrypter());

  return manager;
}

/*
 It is possible to define your own transcoder that performs field-level encryption, which can,
 for example, encrypt a specific field, e.g. "address".
 */
class address_encrypting_transcoder
{
public:
  template<typename Document>
  static auto encode(Document document,
                     const std::shared_ptr<couchbase::crypto::manager>& crypto_manager)
    -> couchbase::codec::encoded_value
  {
    tao::json::value doc_json;
    if constexpr (std::is_same_v<Document, tao::json::value>) {
      doc_json = std::move(document);
    } else {
      doc_json = couchbase::codec::tao_json_serializer::deserialize<tao::json::value>(
        couchbase::codec::tao_json_serializer::serialize(std::move(document)));
    }

    auto serialized_address =
      couchbase::codec::tao_json_serializer::serialize(doc_json.at("address"));
    const auto [encryption_err, encrypted_address] =
      crypto_manager->encrypt(serialized_address, {});
    if (encryption_err) {
      throw std::system_error(encryption_err.ec(),
                              "Failed to encrypt address field: " + encryption_err.message());
    }
    doc_json.erase("address");
    const auto mangled_address_key = crypto_manager->mangle("address");
    doc_json[mangled_address_key] = tao::json::empty_object;
    for (const auto& [k, v] : encrypted_address) {
      doc_json[mangled_address_key][k] = v;
    }

    return {
      couchbase::codec::tao_json_serializer::serialize(doc_json),
      couchbase::codec::codec_flags::json_common_flags,
    };
  }

  template<typename Document>
  static auto decode(const couchbase::codec::encoded_value& encoded,
                     const std::shared_ptr<couchbase::crypto::manager>& crypto_manager) -> Document
  {
    if (!couchbase::codec::codec_flags::has_common_flags(
          encoded.flags, couchbase::codec::codec_flags::json_common_flags)) {
      throw std::system_error(
        couchbase::errc::common::decoding_failure,
        "the address_encrypting_transcoder expects document to have JSON common flags, flags=" +
          std::to_string(encoded.flags));
    }
    auto document =
      couchbase::codec::tao_json_serializer::deserialize<tao::json::value>(encoded.data);
    auto encrypted_address = document.at(crypto_manager->mangle("address"));

    std::map<std::string, std::string> encrypted_node;
    for (const auto& [node_k, node_v] : encrypted_address.get_object()) {
      encrypted_node[node_k] = node_v.get_string();
    }

    const auto [decryption_err, decrypted_address] = crypto_manager->decrypt(encrypted_node);
    if (decryption_err) {
      throw std::system_error(decryption_err.ec(),
                              "Failed to decrypt address field: " + decryption_err.message());
    }

    document.erase(crypto_manager->mangle("address"));
    document["address"] =
      couchbase::codec::tao_json_serializer::deserialize<tao::json::value>(decrypted_address);

    if constexpr (std::is_same_v<Document, tao::json::value>) {
      return document;
    } else {
      return document.as<Document>();
    }
  }
};

/*
 Declare that the address_encrypting_transcoder is a valid transcoder & a crypto transcoder.
 */
template<>
struct couchbase::codec::is_transcoder<address_encrypting_transcoder> : public std::true_type {
};
template<>
struct couchbase::codec::is_crypto_transcoder<address_encrypting_transcoder>
  : public std::true_type {
};

auto
main() -> int
{
  couchbase::logger::initialize_console_logger();
  couchbase::logger::set_level(couchbase::logger::log_level::warn);

  auto options = couchbase::cluster_options(username, password);
  options.crypto_manager(make_crypto_manager()).apply_profile("wan_development");

  auto [connect_err, cluster] = couchbase::cluster::connect(connection_string, options).get();
  if (connect_err) {
    fmt::println("Unable to connect to the cluster. Error: {}", connect_err);
    return 1;
  }

  auto collection = cluster.bucket(bucket_name).scope(scope_name).collection(collection_name);

  const auto document = tao::json::value{
    { "first_name", "John" },
    { "last_name", "Doe" },
    { "address",
      {
        { "street", "999 Street St." },
        { "city", "Some City" },
        { "state", "ST" },
        { "zip", "12345" },
      } },
  };

  /*
   Upserting the document with the address_encrypting_transcoder, will encrypt the address field.
   */
  if (auto [err, res] =
        collection.upsert<address_encrypting_transcoder>("test-user", document).get();
      err) {
    fmt::println("Upsert failed. Error: {}", err);
    cluster.close().get();
    return 1;
  }

  if (auto [err, res] = collection.get("test-user").get(); err) {
    fmt::println("Get failed. Error: {}", err);
  } else {
    /*
     Decoding the document with a standard non-crypto transcoder will return the document with the
     encrypted fields.
    */
    {
      auto content = res.content_as<tao::json::value>();
      fmt::println("Encrypted document:\n{}", tao::json::to_string(content, 2));
    }

    /*
     Decoding the document with the address_encrypting_transcoder will decrypt the address field.
    */
    {
      auto content = res.content_as<tao::json::value, address_encrypting_transcoder>();
      fmt::println("Decrypted document:\n{}", tao::json::to_string(content, 2));
    }
  }

  cluster.close().get();
  return 0;
}
