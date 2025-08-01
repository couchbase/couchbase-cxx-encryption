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

#include <string>
#include <utility>
#include <vector>

namespace couchbase::crypto
{
/**
 * A decrypter is responsible for performing decryption for a specific algorithm.
 *
 * @since 1.0.0
 * @committed
 */
class decrypter
{
public:
  decrypter() = default;
  decrypter(const decrypter& other) = default;
  decrypter(decrypter&& other) = default;
  auto operator=(const decrypter& other) -> decrypter& = default;
  auto operator=(decrypter&& other) -> decrypter& = default;
  virtual ~decrypter() = default;

  /**
   * Decrypts the given encrypted message.
   *
   * @param encrypted the encrypted message to decrypt
   * @return the decrypted message, or an error if decryption failed
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] virtual auto decrypt(encryption_result encrypted)
    -> std::pair<error, std::vector<std::byte>> = 0;

  /**
   * Returns the name of the encryption algorithm used by this decrypter.
   *
   * @return the name of the algorithm
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] virtual auto algorithm() const -> const std::string& = 0;
};
} // namespace couchbase::crypto
