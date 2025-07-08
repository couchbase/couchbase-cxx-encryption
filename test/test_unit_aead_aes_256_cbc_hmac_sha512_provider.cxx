/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "test_helper.hxx"

#include <couchbase/error_codes.hxx>
#include <couchbase_encryption/aead_aes_256_cbc_hmac_sha512_provider.hxx>
#include <couchbase_encryption/insecure_keyring.hxx>

const auto KEY = test::utils::make_bytes({
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
});

TEST_CASE("unit: aead_aes_256_cbc_hmac_sha512_provider", "[unit]")
{
  auto keyring = std::make_shared<couchbase::crypto::insecure_keyring>();
  keyring->add_key(couchbase::crypto::key("test-key", KEY));
  keyring->add_key(couchbase::crypto::key("invalid-key",
                                          test::utils::make_bytes({
                                            0x00,
                                            0x01,
                                            0x02,
                                          })));

  const auto provider =
    couchbase::crypto::aead_aes_256_cbc_hmac_sha512_provider(std::move(keyring));

  const auto plaintext = test::utils::make_bytes({
    0x22, 0x54, 0x68, 0x65, 0x20, 0x65, 0x6e, 0x65, 0x6d, 0x79, 0x20, 0x6b, 0x6e, 0x6f, 0x77,
    0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x2e, 0x22,
  });

  SECTION("decrypt")
  {
    couchbase::crypto::encryption_result enc_result{ "AEAD_AES_256_CBC_HMAC_SHA512" };
    enc_result.put("kid", "test-key");
    enc_result.put("ciphertext",
                   "GvOMLcK5b/"
                   "3YZpQJI0G8BLm98oj20ZLdqKDV3MfTuGlWL4R5p5Deykuv2XLW4LcDvnOkmhuUSRbQ8QVEmbjq43XHd"
                   "Om3ColJ6LzoaAtJihk=");

    const auto decrypter = provider.decrypter();
    const auto [dec_err, dec_result] = decrypter->decrypt(enc_result);
    REQUIRE_NO_ERROR(dec_err);
    REQUIRE(plaintext == dec_result);
  }

  SECTION("encrypt & decrypt")
  {
    const auto encrypter = provider.encrypter_for_key("test-key");
    const auto [enc_err, enc_result] = encrypter->encrypt(plaintext);
    REQUIRE_NO_ERROR(enc_err);
    REQUIRE(enc_result.algorithm() == "AEAD_AES_256_CBC_HMAC_SHA512");
    REQUIRE(enc_result.get("kid") == std::make_optional("test-key"));
    REQUIRE(enc_result.get("ciphertext").has_value());

    const auto decrypter = provider.decrypter();
    const auto [dec_err, dec_result] = decrypter->decrypt(enc_result);
    REQUIRE_NO_ERROR(dec_err);
    REQUIRE(plaintext == dec_result);
  }

  SECTION("encrypt missing key")
  {
    const auto encrypter = provider.encrypter_for_key("missing-key");
    const auto [enc_err, enc_result] = encrypter->encrypt(plaintext);
    REQUIRE(enc_err.ec() == couchbase::errc::field_level_encryption::crypto_key_not_found);
  }

  SECTION("encrypt invalid key")
  {
    const auto encrypter = provider.encrypter_for_key("invalid-key");
    const auto [enc_err, enc_result] = encrypter->encrypt(plaintext);
    REQUIRE(enc_err.ec() == couchbase::errc::field_level_encryption::invalid_crypto_key);
  }

  SECTION("decrypt invalid key")
  {
    couchbase::crypto::encryption_result enc_result{ "AEAD_AES_256_CBC_HMAC_SHA512" };
    enc_result.put("kid", "invalid-key");
    enc_result.put("ciphertext",
                   "GvOMLcK5b/"
                   "3YZpQJI0G8BLm98oj20ZLdqKDV3MfTuGlWL4R5p5Deykuv2XLW4LcDvnOkmhuUSRbQ8QVEmbjq43XHd"
                   "Om3ColJ6LzoaAtJihk=");

    const auto decrypter = provider.decrypter();
    const auto [dec_err, dec_result] = decrypter->decrypt(enc_result);
    REQUIRE(dec_err.ec() == couchbase::errc::field_level_encryption::invalid_crypto_key);
  }

  SECTION("decrypt result that is missing key id")
  {
    couchbase::crypto::encryption_result enc_result{ "AEAD_AES_256_CBC_HMAC_SHA512" };
    enc_result.put("ciphertext",
                   "GvOMLcK5b/"
                   "3YZpQJI0G8BLm98oj20ZLdqKDV3MfTuGlWL4R5p5Deykuv2XLW4LcDvnOkmhuUSRbQ8QVEmbjq43XHd"
                   "Om3ColJ6LzoaAtJihk=");

    const auto decrypter = provider.decrypter();
    const auto [dec_err, dec_result] = decrypter->decrypt(enc_result);
    REQUIRE(dec_err.ec() == couchbase::errc::field_level_encryption::decryption_failure);
  }

  SECTION("decrypt result key id is missing from keyring")
  {
    couchbase::crypto::encryption_result enc_result{ "AEAD_AES_256_CBC_HMAC_SHA512" };
    enc_result.put("kid", "missing-key");
    enc_result.put("ciphertext",
                   "GvOMLcK5b/"
                   "3YZpQJI0G8BLm98oj20ZLdqKDV3MfTuGlWL4R5p5Deykuv2XLW4LcDvnOkmhuUSRbQ8QVEmbjq43XHd"
                   "Om3ColJ6LzoaAtJihk=");

    const auto decrypter = provider.decrypter();
    const auto [dec_err, dec_result] = decrypter->decrypt(enc_result);
    REQUIRE(dec_err.ec() == couchbase::errc::field_level_encryption::crypto_key_not_found);
  }

  SECTION("decrypt invalid ciphertext")
  {
    SECTION("ciphertext too short")
    {
      couchbase::crypto::encryption_result enc_result{ "AEAD_AES_256_CBC_HMAC_SHA512" };
      enc_result.put("kid", "test-key");
      enc_result.put("ciphertext",
                     test::utils::make_bytes({
                       0x00,
                       0x01,
                       0x02,
                       0x03,
                     }));

      const auto decrypter = provider.decrypter();
      const auto [dec_err, dec_result] = decrypter->decrypt(enc_result);
      REQUIRE(dec_err.ec() == couchbase::errc::field_level_encryption::invalid_ciphertext);
    }

    SECTION("ciphertext is not valid base64 encoded string")
    {
      couchbase::crypto::encryption_result enc_result{ "AEAD_AES_256_CBC_HMAC_SHA512" };
      enc_result.put("kid", "test-key");
      enc_result.put("ciphertext", "GvOMLcK5b/3YZpQJI0G8BLm98oj20ZLdqKDV3MfTuGlWL4R5p5Deykuv2X");

      const auto decrypter = provider.decrypter();
      const auto [dec_err, dec_result] = decrypter->decrypt(enc_result);
      REQUIRE(dec_err.ec() == couchbase::errc::field_level_encryption::invalid_ciphertext);
      REQUIRE(dec_err.message().find("could not be decoded") != std::string::npos);
      REQUIRE(dec_err.message().find("base64") != std::string::npos);
    }
  }
}
