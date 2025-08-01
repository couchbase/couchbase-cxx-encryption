/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase/error.hxx>
#include <couchbase_encryption/key.hxx>

namespace couchbase::crypto
{
/**
 * A collection of keys that can be used for Field Level Encryption (FLE). It is given to the
 * couchbase::crypto::manager to allow it to retrieve the keys needed for encryption and decryption
 * based on the key ID.
 *
 * @since 1.0.0
 * @committed
 */
class keyring
{
public:
  keyring() = default;
  keyring(const keyring& other) = default;
  keyring(keyring&& other) = default;
  auto operator=(const keyring& other) -> keyring& = default;
  auto operator=(keyring&& other) -> keyring& = default;
  virtual ~keyring() = default;

  /**
   * Retrieves a key from the keyring by its ID.
   *
   * @param key_id the ID of the key to retrieve
   * @return the key if found, or an error if retrieving the key failed
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] virtual auto get(const std::string& key_id) const -> std::pair<error, key> = 0;
};
} // namespace couchbase::crypto
