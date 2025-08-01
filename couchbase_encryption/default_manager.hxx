/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase/error.hxx>
#include <couchbase_encryption/manager.hxx>

namespace couchbase::crypto
{
class default_manager : public manager
{
public:
  static const inline std::string default_encrypter_alias{ "__DEFAULT__" };
  static const inline std::string default_encrypted_field_name_prefix{ "encrypted$" };

  /**
   * Crates a new instance of the deafult crypto manager.
   *
   * @param encrypted_field_name_prefix the prefix to use for encrypted field names.
   * Defaults to "encrypted$"
   *
   * @since 1.0.0
   * @committed
   */
  explicit default_manager(
    std::string encrypted_field_name_prefix = default_encrypted_field_name_prefix);

  /**
   * Registers an encrypter and associates it with the given alias.
   *
   * @param alias the alias for to associate with the encrypter
   * @param encrypter the encrypter to register
   * @return an error if the registration failed
   *
   * @since 1.0.0
   * @committed
   */
  auto register_encrypter(std::string alias, std::shared_ptr<encrypter> encrypter) -> error;

  /**
   * Registers a decrypter.
   *
   * @param decrypter the decrypter to register
   * @return an error if the registration failed
   *
   * @since 1.0.0
   * @committed
   */
  auto register_decrypter(std::shared_ptr<decrypter> decrypter) -> error;

  /**
   * Registers a default encrypter that will be used if no encrypter alias is specified for a field.
   *
   * @param encrypter the default encrypter to register
   * @return an error if the registration failed
   *
   * @since 1.0.0
   * @committed
   */
  auto register_default_encrypter(std::shared_ptr<encrypter> encrypter) -> error;

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
  auto encrypt(std::vector<std::byte> plaintext, const std::optional<std::string>& encrypter_alias)
    -> std::pair<error, std::map<std::string, std::string>> override;

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
  auto decrypt(std::map<std::string, std::string> encrypted_node)
    -> std::pair<error, std::vector<std::byte>> override;

  /**
   * Transforms the given field name to indicate its value is encrypted, by prefixing it with this
   * crypto manager's encrypted field prefix.
   *
   * @param field_name the name of the field to mangle
   * @return the mangled field name
   *
   * @since 1.0.0
   * @committed
   */
  auto mangle(std::string field_name) -> std::string override;

  /**
   * Reverses the mangling of a field name, returning it to its original form.
   *
   * @param field_name the mangled field name
   * @return the original field name
   *
   * @since 1.0.0
   * @committed
   */
  auto demangle(std::string field_name) -> std::string override;

  /**
   * Checks if the given field name is mangled, indicating it is encrypted.
   *
   * @param field_name the name of the field to check
   * @return true iff the field name has been mangled
   *
   * @since 1.0.0
   * @committed
   */
  auto is_mangled(const std::string& field_name) -> bool override;

private:
  std::string encrypted_field_name_prefix_;
  std::map<std::string, std::shared_ptr<encrypter>> alias_to_encrypter_{};
  std::map<std::string, std::shared_ptr<decrypter>> algorithm_to_decrypter_{};
};
} // namespace couchbase::crypto
