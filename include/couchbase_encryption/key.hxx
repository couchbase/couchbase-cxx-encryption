/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <string>
#include <vector>

namespace couchbase::crypto
{
/**
 * Represents a key that can be used for Field Level Encryption (FLE).
 *
 * @since 1.0.0
 * @committed
 */
class key
{
public:
  key() = default;

  /**
   * Constructs a key with the given ID.
   *
   * @param id the id of the key, which is used to identify it in the keyring.
   * @param bytes the key itself, represented as a vector of bytes.
   */
  key(std::string id, std::vector<std::byte> bytes);

  /**
   * Returns the ID of this key.
   *
   * @return the id
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto id() const -> const std::string&;

  /**
   * Returns the value of this key.
   *
   * @return they key, as a vector of bytes
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto bytes() const -> const std::vector<std::byte>&;

private:
  std::string id_;
  std::vector<std::byte> bytes_;
};
} // namespace couchbase::crypto
