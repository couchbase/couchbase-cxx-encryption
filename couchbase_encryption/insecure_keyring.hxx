/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/key.hxx>
#include <couchbase_encryption/keyring.hxx>

#include <map>
#include <string>

namespace couchbase::crypto
{
/**
 * An insecure keyring implementation that stores keys in memory. Should not be used in
 * production environments. It is intended for testing purposes only.
 *
 * @since 1.0.0
 * @uncommitted
 */
class insecure_keyring : public keyring
{
public:
  /**
   * Constructs an empty insecure keyring.
   *
   * @since 1.0.0
   * @uncommitted
   */
  insecure_keyring() = default;

  /**
   * Constructs an insecure keyring containing the given keys.
   *
   * @param keys the keys to be added to the keyring
   *
   * @since 1.0.0
   * @uncommitted
   */
  explicit insecure_keyring(const std::vector<key>& keys);

  /**
   * Adds a key to the keyring.
   *
   * @param k the key to be added
   *
   * @since 1.0.0
   * @uncommitted
   */
  void add_key(key k);

  /**
   * Retrieves a key from the keyring by its ID.
   *
   * @param key_id the ID of the key to retrieve
   * @return the key if found, or an error if retrieving the key failed
   *
   * @since 1.0.0
   * @uncommitted
   */
  [[nodiscard]] auto get(const std::string& key_id) const -> std::pair<error, key> override;

private:
  std::map<std::string, key> keys_;
};
} // namespace couchbase::crypto
