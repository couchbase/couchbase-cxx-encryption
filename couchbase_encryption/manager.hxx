/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/decrypter.hxx>
#include <couchbase_encryption/encrypter.hxx>
#include <couchbase_encryption/encryption_result.hxx>
#include <couchbase_encryption/keyring.hxx>
#include <couchbase/error.hxx>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace couchbase::crypto
{
class manager
{
public:
  manager() = default;
  manager(const manager& other) = default;
  manager(manager&& other) = default;
  auto operator=(const manager& other) -> manager& = default;
  auto operator=(manager&& other) -> manager& = default;
  virtual ~manager() = default;

  /**
   * Encrypts the given data, using the encrypter associated with the given alias, or the default
   * encrypter if no alias is given.
   *
   * @param plaintext the message to encrypt
   * @param encrypter_alias the alias of the encrypter to use, or std::nullopt to use the default
   * @return the encrypted node containing the encrypted message and metadata, or an error if
   * encryption failed
   *
   * @since 1.0.0
   * @committed
   */
  virtual auto encrypt(std::vector<std::byte> plaintext,
                       const std::optional<std::string>& encrypter_alias)
    -> std::pair<error, std::map<std::string, std::string>> = 0;

  /**
   * Selects an appropriate decrypter based on the contents of the encrypted node and uses it to
   * decrypt the data.
   *
   * @param encrypted_node the encrypted node containing the encrypted message and metadata
   * @return the plaintext message if decryption was successful, or an error if it failed
   *
   * @since 1.0.0
   * @committed
   */
  virtual auto decrypt(std::map<std::string, std::string> encrypted_node)
    -> std::pair<error, std::vector<std::byte>> = 0;

  /**
   * Transforms the given field name to indicate its value is encrypted.
   *
   * @param field_name the name of the field to mangle
   * @return the mangled field name
   *
   * @since 1.0.0
   * @committed
   */
  virtual auto mangle(std::string field_name) -> std::string = 0;

  /**
   * Reverses the mangling of a field name, returning it to its original form.
   *
   * @param field_name the mangled field name
   * @return the original field name
   *
   * @since 1.0.0
   * @committed
   */
  virtual auto demangle(std::string field_name) -> std::string = 0;

  /**
   * Checks if the given field name is mangled, indicating it is encrypted.
   *
   * @param field_name the name of the field to check
   * @return true iff the field name has been mangled
   *
   * @since 1.0.0
   * @committed
   */
  virtual auto is_mangled(const std::string& field_name) -> bool = 0;
};
} // namespace couchbase::crypto
