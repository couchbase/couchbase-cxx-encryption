/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "document_types/person.hxx"
#include "test_helper.hxx"
#include "utils/crypto.hxx"

#include <couchbase/codec/tao_json_serializer.hxx>
#include <couchbase_encryption/aead_aes_256_cbc_hmac_sha512_provider.hxx>
#include <couchbase_encryption/default_manager.hxx>
#include <couchbase_encryption/default_transcoder.hxx>
#include <couchbase_encryption/encrypted_fields.hxx>
#include <couchbase_encryption/insecure_keyring.hxx>

#include <tao/json/to_string.hpp>
#include <tao/json/value.hpp>

struct doc {
  std::string maxim;

  auto operator==(const doc& other) const -> bool
  {
    return maxim == other.maxim;
  }

  inline static const std::vector<couchbase::crypto::encrypted_field> encrypted_fields{
    {
      /* .field_path = */ { "maxim" },
      /* .encrypter_alias = */ {},
    },
  };
};

template<>
struct tao::json::traits<doc> {
  template<template<typename...> class Traits>
  static void assign(tao::json::basic_value<Traits>& v, const doc& d)
  {
    v = { { "maxim", d.maxim } };
  }

  template<template<typename...> class Traits>
  static auto as(const tao::json::basic_value<Traits>& v) -> doc
  {
    doc d;
    d.maxim = v.at("maxim").get_string();
    return d;
  }
};

const auto KEY =
  test::utils::make_bytes({ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
                            0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                            0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
                            0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
                            0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
                            0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f });

auto
make_crypto_manager() -> std::shared_ptr<couchbase::crypto::default_manager>
{
  auto keyring = std::make_shared<couchbase::crypto::insecure_keyring>();
  keyring->add_key(couchbase::crypto::key("test-key", KEY));

  auto provider = couchbase::crypto::aead_aes_256_cbc_hmac_sha512_provider(keyring);

  auto manager = std::make_shared<couchbase::crypto::default_manager>();
  manager->register_default_encrypter(provider.encrypter_for_key("test-key"));
  manager->register_encrypter("one", provider.encrypter_for_key("test-key"));
  manager->register_decrypter(provider.decrypter());

  return manager;
}

TEST_CASE("unit: crypto transcoder", "[unit]")
{
  const auto crypto_manager = make_crypto_manager();

  const doc d{ "The enemy knows the system." };

  static_assert(couchbase::crypto::has_encrypted_fields_v<doc>, "doc should have encrypted_fields");

  SECTION("encoding")
  {
    const couchbase::codec::encoded_value encoded =
      couchbase::crypto::default_transcoder::encode(d, crypto_manager);
    REQUIRE(encoded.flags == couchbase::codec::codec_flags::json_common_flags);

    auto encrypted_document = couchbase::core::utils::json::parse_binary(encoded.data);
    REQUIRE(encrypted_document.is_object());
    REQUIRE(encrypted_document.find("maxim") == nullptr);
    REQUIRE(encrypted_document.find("encrypted$maxim") != nullptr);
    REQUIRE(encrypted_document["encrypted$maxim"].is_object());
  }

  SECTION("decoding")
  {
    const auto data = couchbase::core::utils::json::generate_binary(tao::json::value{
      { "encrypted$maxim",
        tao::json::value{
          { "alg", "AEAD_AES_256_CBC_HMAC_SHA512" },
          { "kid", "test-key" },
          { "ciphertext",
            "GvOMLcK5b/"
            "3YZpQJI0G8BLm98oj20ZLdqKDV3MfTuGlWL4R5p5Deykuv2XLW4LcDvnOkmhuUSRbQ8QVEmbjq43XHdOm3ColJ"
            "6LzoaAtJihk=" },
        } },
    });
    const auto encoded =
      couchbase::codec::encoded_value{ data, couchbase::codec::codec_flags::json_common_flags };
    const auto decoded_doc =
      couchbase::crypto::default_transcoder::decode<doc>(encoded, crypto_manager);
    REQUIRE(d == decoded_doc);

    const auto decoded_doc_as_tao_json =
      couchbase::crypto::default_transcoder::decode<tao::json::value>(encoded, crypto_manager);
    REQUIRE(decoded_doc_as_tao_json.get_object().size() == 1);
    REQUIRE(decoded_doc_as_tao_json.find("maxim") != nullptr);
    REQUIRE(decoded_doc_as_tao_json.find("encrypted$maxim") == nullptr);
  }

  SECTION("encoding and decoding")
  {
    const couchbase::codec::encoded_value encoded =
      couchbase::crypto::default_transcoder::encode(d, crypto_manager);
    REQUIRE(encoded.flags == couchbase::codec::codec_flags::json_common_flags);

    auto encrypted_document = couchbase::core::utils::json::parse_binary(encoded.data);
    REQUIRE(encrypted_document.is_object());
    REQUIRE(encrypted_document.get_object().size() == 1);
    REQUIRE(encrypted_document.find("maxim") == nullptr);
    REQUIRE(encrypted_document.find("encrypted$maxim") != nullptr);
    REQUIRE(encrypted_document["encrypted$maxim"].is_object());

    auto encrypted_node = encrypted_document["encrypted$maxim"];
    REQUIRE(encrypted_node["ciphertext"].is_string());
    REQUIRE(encrypted_node["kid"].get_string() == "test-key");
    REQUIRE(encrypted_node["alg"].get_string() == "AEAD_AES_256_CBC_HMAC_SHA512");

    const auto decoded_doc =
      couchbase::crypto::default_transcoder::decode<doc>(encoded, crypto_manager);
    REQUIRE(d == decoded_doc);

    const auto decoded_doc_as_tao_json =
      couchbase::crypto::default_transcoder::decode<tao::json::value>(encoded, crypto_manager);
    REQUIRE(decoded_doc_as_tao_json.get_object().size() == 1);
    REQUIRE(decoded_doc_as_tao_json.find("maxim") != nullptr);
    REQUIRE(decoded_doc_as_tao_json.find("encrypted$maxim") == nullptr);
  }
}

struct doc_with_empty_path {
  std::string maxim;

  auto operator==(const doc& other) const -> bool
  {
    return maxim == other.maxim;
  }

  inline static const std::vector<couchbase::crypto::encrypted_field> encrypted_fields{
    {
      /* .field_path = */ { "maxim" },
      /* .encrypter_alias = */ {},
    },
    {
      /* by mistake the user might add an extra empty default-initialized encrypted_field
         which would have empty path */
    },
  };
};

template<>
struct tao::json::traits<doc_with_empty_path> {
  template<template<typename...> class Traits>
  static void assign(tao::json::basic_value<Traits>& v, const doc_with_empty_path& d)
  {
    v = { { "maxim", d.maxim } };
  }

  template<template<typename...> class Traits>
  static auto as(const tao::json::basic_value<Traits>& v) -> doc_with_empty_path
  {
    doc_with_empty_path d;
    d.maxim = v.at("maxim").get_string();
    return d;
  }
};

TEST_CASE("unit: crypto transcoder with document that has empty encrypted field path", "[unit]")
{
  const auto crypto_manager = make_crypto_manager();

  const doc_with_empty_path d{ "The enemy knows the system." };

  try {
    const auto _ = couchbase::crypto::default_transcoder::encode(d, crypto_manager);
    FAIL("Expected exception to be thrown, but was not.");
  } catch (const std::system_error& e) {
    REQUIRE(e.code() == couchbase::errc::field_level_encryption::encryption_failure);
  }
}

struct doc_with_non_existent_path {
  std::string maxim;

  auto operator==(const doc_with_non_existent_path& other) const -> bool
  {
    return maxim == other.maxim;
  }

  inline static const std::vector<couchbase::crypto::encrypted_field> encrypted_fields{
    {
      /* .field_path = */ { "this-does-not-exist" },
    },
  };
};

template<>
struct tao::json::traits<doc_with_non_existent_path> {
  template<template<typename...> class Traits>
  static void assign(tao::json::basic_value<Traits>& v, const doc_with_non_existent_path& d)
  {
    v = { { "maxim", d.maxim } };
  }

  template<template<typename...> class Traits>
  static auto as(const tao::json::basic_value<Traits>& v) -> doc_with_non_existent_path
  {
    doc_with_non_existent_path d;
    d.maxim = v.at("maxim").get_string();
    return d;
  }
};

TEST_CASE("unit: crypto transcoder with document that has encrypted field path that does not exist",
          "[unit]")
{
  const auto crypto_manager = make_crypto_manager();

  const doc_with_non_existent_path d{ "The enemy knows the system." };

  try {
    const auto _ = couchbase::crypto::default_transcoder::encode(d, crypto_manager);
    FAIL("Expected exception to be thrown, but was not.");
  } catch (const std::system_error& e) {
    REQUIRE(e.code() == couchbase::errc::field_level_encryption::encryption_failure);
  }
}

struct doc_with_invalid_encrypter_alias {
  std::string maxim;

  auto operator==(const doc_with_invalid_encrypter_alias& other) const -> bool
  {
    return maxim == other.maxim;
  }

  inline static const std::vector<couchbase::crypto::encrypted_field> encrypted_fields{
    {
      /* .field_path = */ { "maxim" },
      /* .encrypter_alias = */ { "does-not-exist" },
    },
  };
};

template<>
struct tao::json::traits<doc_with_invalid_encrypter_alias> {
  template<template<typename...> class Traits>
  static void assign(tao::json::basic_value<Traits>& v, const doc_with_invalid_encrypter_alias& d)
  {
    v = { { "maxim", d.maxim } };
  }

  template<template<typename...> class Traits>
  static auto as(const tao::json::basic_value<Traits>& v) -> doc_with_invalid_encrypter_alias
  {
    doc_with_invalid_encrypter_alias d;
    d.maxim = v.at("maxim").get_string();
    return d;
  }
};

TEST_CASE("unit: crypto transcoder with document that has encrypter alias that cannot be found",
          "[unit]")
{
  const auto crypto_manager = make_crypto_manager();

  const doc_with_invalid_encrypter_alias d{ "The enemy knows the system." };

  try {
    const auto _ = couchbase::crypto::default_transcoder::encode(d, crypto_manager);
    FAIL("Expected exception to be thrown, but was not.");
  } catch (const std::system_error& e) {
    REQUIRE(e.code() == couchbase::errc::field_level_encryption::encrypter_not_found);
  }
}

TEST_CASE("unit: crypto transcoder with document that has nested encrypted fields", "[unit]")
{
  const auto crypto_manager = make_crypto_manager();
  const person p{
    "Albert",
    "Einstein",
    "password123",
    {
      "1A",
      {
        "my street",
        "my second line",
      },
    },
    {
      "cat",
      std::map<std::string, person::pet::attribute>{
        { "attr1", { "jump" } },
        { "attr2", { "scratch", "extra" } },
      },
    },
  };

  const auto encoded = couchbase::crypto::default_transcoder::encode(p, crypto_manager);
  REQUIRE(encoded.flags == couchbase::codec::codec_flags::json_common_flags);

  auto json = couchbase::codec::tao_json_serializer::deserialize<tao::json::value>(encoded.data);
  test::utils::ensure_field_is_encrypted(json, "password");
  test::utils::ensure_field_is_encrypted(json, "address");
  test::utils::ensure_field_at_path_is_encrypted(json, { "pet", "attributes" });

  {
    std::map<std::string, std::string> encrypted_address;
    for (const auto& [node_k, node_v] : json.at("encrypted$address").get_object()) {
      encrypted_address[node_k] = node_v.get_string();
    }
    const auto [err, address_bytes] = crypto_manager->decrypt(encrypted_address);
    REQUIRE_NO_ERROR(err);
    const auto address_json =
      couchbase::codec::tao_json_serializer::deserialize<tao::json::value>(address_bytes);
    test::utils::ensure_field_is_encrypted(address_json, "street");

    {
      std::map<std::string, std::string> encrypted_street;
      for (const auto& [node_k, node_v] : address_json.at("encrypted$street").get_object()) {
        encrypted_street[node_k] = node_v.get_string();
      }
      const auto [err, street_bytes] = crypto_manager->decrypt(encrypted_street);
      REQUIRE_NO_ERROR(err);
      const auto street_json =
        couchbase::codec::tao_json_serializer::deserialize<tao::json::value>(street_bytes);
      test::utils::ensure_field_is_encrypted(street_json, "second");
    }
  }

  REQUIRE(p == couchbase::crypto::default_transcoder::decode<person>(encoded, crypto_manager));
}
