/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase/error.hxx>
#include <couchbase_encryption/decrypter.hxx>
#include <couchbase_encryption/encrypter.hxx>
#include <couchbase_encryption/encryption_result.hxx>
#include <couchbase_encryption/keyring.hxx>

#include <memory>
#include <string>
#include <utility>

namespace couchbase::crypto
{
/**
 * Provider for AES-256 in CBC mode authenticated with HMAC SHA-512. Provides a way to create
 * encrypters and decrypters.
 *
 * Requires a 64 byte key.
 *
 * The algorithm is formally described in <a
 * href="https://tools.ietf.org/html/draft-mcgrew-aead-aes-cbc-hmac-sha2-05"> Authenticated
 * Encryption with AES-CBC and HMAC-SHA</a>.
 */
class aead_aes_256_cbc_hmac_sha512_provider
{
public:
  static inline const std::string algorithm_name{ "AEAD_AES_256_CBC_HMAC_SHA512" };

  /**
   * Constructs an instance of an AEAD-AES-256-CBC-HMAC-SHA512 provider, with the given keyring.
   *
   * @param keyring the keyring for obtaining data encryption keys
   *
   * @since 1.0.0
   * @committed
   */
  explicit aead_aes_256_cbc_hmac_sha512_provider(std::shared_ptr<keyring> keyring);

  /**
   * Creates a new encrypter for the encryption key with the given ID.
   *
   * @param key_id the id of the key to use for encryption
   * @return the AEAD-AES-256-CBC-HMAC-SHA512 encrypter
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto encrypter_for_key(const std::string& key_id) const
    -> std::shared_ptr<encrypter>;

  /**
   * Creates a new decrypter for this algorithm.
   *
   * @return the AEAD-AES-256-CBC-HMAC-SHA512 decrypter
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto decrypter() const -> std::shared_ptr<decrypter>;

private:
  std::shared_ptr<keyring> keyring_;
};

class aead_aes_256_cbc_hmac_sha512_encrypter : public encrypter
{
public:
  explicit aead_aes_256_cbc_hmac_sha512_encrypter(std::string key_id,
                                                  std::shared_ptr<keyring> keyring);

  auto encrypt(std::vector<std::byte> plaintext) -> std::pair<error, encryption_result> override;

private:
  std::shared_ptr<keyring> keyring_;
  std::string key_id_;
};

class aead_aes_256_cbc_hmac_sha512_decrypter : public decrypter
{
public:
  explicit aead_aes_256_cbc_hmac_sha512_decrypter(std::shared_ptr<keyring> keyring);

  auto decrypt(encryption_result encrypted) -> std::pair<error, std::vector<std::byte>> override;
  [[nodiscard]] auto algorithm() const -> const std::string& override;

private:
  std::shared_ptr<keyring> keyring_;
};
} // namespace couchbase::crypto
