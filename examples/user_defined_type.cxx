/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include <couchbase/cluster.hxx>
#include <couchbase/codec/tao_json_serializer.hxx>
#include <couchbase/logger.hxx>

#include <couchbase_encryption/aead_aes_256_cbc_hmac_sha512_provider.hxx>
#include <couchbase_encryption/default_manager.hxx>
#include <couchbase_encryption/default_transcoder.hxx>
#include <couchbase_encryption/encrypted_fields.hxx>
#include <couchbase_encryption/insecure_keyring.hxx>

#include <spdlog/fmt/bundled/format.h>

#include <couchbase/fmt/error.hxx>

#include <tao/json/forward.hpp>
#include <tao/json/to_string.hpp>
#include <tao/json/type.hpp>

#include <string>

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

struct user {
  struct address {
    std::string street;
    std::string city;
    std::string state;
    std::string zip;
  };

  std::string first_name;
  std::string last_name;
  std::string password;
  address address;
  std::string phone_number;

  /*
   Define the fields that should be encrypted, with the paths they will appear with in the
   serialized JSON document. These fields will be encrypter using the default encrypter.
   */
  inline static const std::vector<couchbase::crypto::encrypted_field> encrypted_fields{
    { { "password" } },
    { { "address", "street" } },
    { { "address", "zip" } },
    { { "phone" } },
  };
};

/*
 Define serialization/deserialization with the tao json transcoder, as the default transcoder will
 be used.
 */
template<>
struct tao::json::traits<user> {
  template<template<typename...> class Traits>
  static void assign(tao::json::basic_value<Traits>& v, const user& u)
  {
    v["first_name"] = u.first_name;
    v["last_name"] = u.last_name;
    v["password"] = u.password;
    v["address"] = { { "street", u.address.street },
                     { "city", u.address.city },
                     { "state", u.address.state },
                     { "zip", u.address.zip } };
    v["phone"] = u.phone_number;
  }

  template<template<typename...> class Traits>
  static auto as(const tao::json::basic_value<Traits>& v) -> user
  {
    user result;

    result.first_name = v.at("first_name").get_string();
    result.last_name = v.at("last_name").get_string();
    result.password = v.at("password").get_string();
    result.address.street = v.at("address").at("street").get_string();
    result.address.city = v.at("address").at("city").get_string();
    result.address.state = v.at("address").at("state").get_string();
    result.address.zip = v.at("address").at("zip").get_string();
    result.phone_number = v.at("phone").get_string();

    return result;
  }
};

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

  const auto u = user{
    "John", "Doe", "password123", { "999 Street St.", "Some City", "ST", "12345" }, "12345678",
  };

  /*
   Upserting the user with the crypto transcoder, will encrypt the fields specified, using the
   crypto manager given in the cluster options.
   */
  if (auto [err, res] =
        collection.upsert<couchbase::crypto::default_transcoder>("test-user", u).get();
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
     encrypted fields
    */
    {
      auto content = res.content_as<tao::json::value>();
      fmt::println("Encrypted document:\n{}", tao::json::to_string(content, 2));
    }

    /*
     Decoding the document with a crypto transcoder will return the unencrypted document.
    */
    {
      auto content = res.content_as<tao::json::value, couchbase::crypto::default_transcoder>();
      fmt::println("Decrypted document:\n{}", tao::json::to_string(content, 2));
    }
  }

  cluster.close().get();
  return 0;
}
