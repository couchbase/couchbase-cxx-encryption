/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase/error.hxx>
#include <couchbase_encryption/encryption_result.hxx>

#include <utility>
#include <vector>

namespace couchbase::crypto
{
/**
 * An encrypter is responsible for performing encryption for a specific algorithm and
 * encryption key.
 *
 * @since 1.0.0
 * @commmitted
 */
class encrypter
{
public:
  encrypter() = default;
  encrypter(const encrypter& other) = default;
  encrypter(encrypter&& other) = default;
  auto operator=(const encrypter& other) -> encrypter& = default;
  auto operator=(encrypter&& other) -> encrypter& = default;
  virtual ~encrypter() = default;

  /**
   * Encrypts the given message.
   *
   * The encryption result specifies the algorithm of the decrypter, to use when reading this
   * encrypted field. The result also includes any attributes required for decryption, such as
   * the ID of the secret key, or other parameters specific to the algorithm.
   *
   * @param plaintext the bytes to encrypt
   * @return the encryption result, or an error if encryption failed
   *
   * @since 1.0.0
   * @committed
   */
  virtual auto encrypt(std::vector<std::byte> plaintext) -> std::pair<error, encryption_result> = 0;
};
} // namespace couchbase::crypto
