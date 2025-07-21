/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace couchbase::crypto
{
/**
 * Represents the result of the encryption for a specific field. It includes metadata, which allows
 * subsequent decryption of the field by a crypto manager with the appropriate keyring.
 *
 * @since 1.0.0
 * @committed
 */
class encryption_result
{
public:
  encryption_result() = default;

  /**
   * Constructs an encryption result with the given algorithm.
   *
   * @param algorithm the algorithm that was used for encryption, e.g.
   * "AEAD_AES_256_CBC_HMAC_SHA512".
   *
   * @since 1.0.0
   * @committed
   */
  explicit encryption_result(std::string algorithm);

  /**
   * Constructs an encryption result from a map containing the encrypted node.
   *
   * @param encrypted_node the map containing the encrypted node, which typically includes the
   * algorithm, ciphertext, and any additional metadata required for decryption.
   *
   * @since 1.0.0
   * @committed
   */
  explicit encryption_result(std::map<std::string, std::string> encrypted_node);

  /**
   * Returns the algorithm that was used to produce this encryption result.
   *
   * @return the algorithm
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto algorithm() const -> std::string;

  /**
   * Retrieves the value of a specific string field from the encryption result.
   *
   * @param field_name the name of the field to retrieve
   * @return an optional containing the field value if it exists, or std::nullopt if the field is
   * not present
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto get(const std::string& field_name) const -> std::optional<std::string>;

  /**
   * Decodes and retrieves the value of a specific byte vector field from the encryption result
   *
   * @param field_name the name of the field to retrieve
   * @return an optional containing the field value if it exists, or std::nullopt if the field is
   * not present
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto get_bytes(const std::string& field_name) const
    -> std::optional<std::vector<std::byte>>;

  /**
   * Returns the internal map representation of the encryption result.
   *
   * @return the encryption result as a map.
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto as_map() const -> std::map<std::string, std::string>;

  /**
   * Adds a new string field to the encryption result with the specified value.
   *
   * @param field_name the name of the field to add
   * @param value the value to associate with the field
   *
   * @since 1.0.0
   * @committed
   */
  void put(std::string field_name, std::string value);

  /**
   * Adds a new byte vector field to the encryption result with the specified value. The value is
   * base64 encoded before being stored.
   *
   * @param field_name the name of the field to add
   * @param value the byte vector value to associate with the field, which will be base64 encoded
   *
   * @since 1.0.0
   * @committed
   */
  void put(std::string field_name, std::vector<std::byte> value);

private:
  std::map<std::string, std::string> internal_;
};
} // namespace couchbase::crypto
