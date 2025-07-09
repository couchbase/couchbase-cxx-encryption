/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "document_types/profile.hxx"
#include "test_helper.hxx"
#include "utils/binary.hxx"
#include "utils/crypto.hxx"

#include <couchbase/codec/tao_json_serializer.hxx>
#include <couchbase_encryption/aead_aes_256_cbc_hmac_sha512_provider.hxx>
#include <couchbase_encryption/default_manager.hxx>
#include <couchbase_encryption/default_transcoder.hxx>
#include <couchbase_encryption/document.hxx>
#include <couchbase_encryption/insecure_keyring.hxx>

#include <tao/json/value.hpp>

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
  const auto keyring = std::make_shared<couchbase::crypto::insecure_keyring>();
  keyring->add_key(couchbase::crypto::key("test-key", KEY));

  const auto provider = couchbase::crypto::aead_aes_256_cbc_hmac_sha512_provider(keyring);

  auto manager = std::make_shared<couchbase::crypto::default_manager>();
  manager->register_default_encrypter(provider.encrypter_for_key("test-key"));
  manager->register_encrypter("one", provider.encrypter_for_key("test-key"));
  manager->register_decrypter(provider.decrypter());

  return manager;
}

TEST_CASE("unit: can use crypto document to specify fields to encrypt", "[unit]")
{
  const tao::json::value doc{
    { "username", "this_guy_again" },
    { "password", "password123!" },
    { "full_name", "Albert Einstein" },
    { "birth_year", 1879 },
  };

  const auto crypto_doc = couchbase::crypto::document<tao::json::value>::from(doc)
                            .with_encrypted_field({ "full_name" })
                            .with_encrypted_field({ "password" }, "one");

  const auto mgr = make_crypto_manager();
  const auto encoded = couchbase::crypto::default_transcoder::encode(crypto_doc, mgr);

  REQUIRE(encoded.flags == couchbase::codec::codec_flags::json_common_flags);

  auto json = couchbase::codec::tao_json_serializer::deserialize<tao::json::value>(encoded.data);

  // Verify the unencrypted fields
  REQUIRE(json.at("username").get_string() == "this_guy_again");
  REQUIRE(json.at("birth_year").get_unsigned() == 1879);

  // Verify the encrypted fields
  test::utils::ensure_field_is_encrypted(json, "full_name");
  test::utils::ensure_field_is_encrypted(json, "password");

  // Verify that the transcoder can decode & decrypt and return the original document
  REQUIRE(doc == couchbase::crypto::default_transcoder::decode<tao::json::value>(encoded, mgr));
}

TEST_CASE("unit: can use crypto document to specify additional fields to encrypt for types that "
          "already have encrypted fields specified",
          "[unit]")
{
  const profile p{
    "this_guy_again",
    "Albert Einstein",
    1879,
  };

  auto crypto_doc = couchbase::crypto::document<profile>::from(p);
  REQUIRE(crypto_doc.encrypted_fields().size() == 1);
  REQUIRE(crypto_doc.encrypted_fields().at(0) ==
          couchbase::crypto::encrypted_field{ { "full_name" } });

  crypto_doc.with_encrypted_field({ "birth_year" }, "one");
  REQUIRE(crypto_doc.encrypted_fields().size() == 2);
  REQUIRE(crypto_doc.encrypted_fields().at(0) ==
          couchbase::crypto::encrypted_field{ { "full_name" } });
  REQUIRE(crypto_doc.encrypted_fields().at(1) ==
          couchbase::crypto::encrypted_field{ { "birth_year" }, "one" });

  const auto mgr = make_crypto_manager();
  const auto encoded = couchbase::crypto::default_transcoder::encode(crypto_doc, mgr);

  REQUIRE(encoded.flags == couchbase::codec::codec_flags::json_common_flags);

  auto json = couchbase::codec::tao_json_serializer::deserialize<tao::json::value>(encoded.data);

  // Verify the unencrypted fields
  REQUIRE(json.at("username").get_string() == "this_guy_again");

  // Verify the encrypted fields
  test::utils::ensure_field_is_encrypted(json, "birth_year");
  test::utils::ensure_field_is_encrypted(json, "full_name");

  // Verify that the transcoder can decode & decrypt and return the original document
  REQUIRE(p == couchbase::crypto::default_transcoder::decode<profile>(encoded, mgr));
}
