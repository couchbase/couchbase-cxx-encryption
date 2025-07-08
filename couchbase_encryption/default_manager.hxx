/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/manager.hxx>
#include <couchbase/error.hxx>

namespace couchbase::crypto
{
class default_manager : public manager
{
public:
  static const inline std::string default_encrypter_alias{ "__DEFAULT__" };
  static const inline std::string default_encrypted_field_name_prefix{ "encrypted$" };

  explicit default_manager(
    std::string encrypted_field_name_prefix = default_encrypted_field_name_prefix);

  auto register_encrypter(std::string alias, std::shared_ptr<encrypter> encrypter) -> error;
  auto register_decrypter(std::shared_ptr<decrypter> decrypter) -> error;
  auto register_default_encrypter(std::shared_ptr<encrypter> encrypter) -> error;

  auto encrypt(std::vector<std::byte> plaintext, const std::optional<std::string>& encrypter_alias)
    -> std::pair<error, std::map<std::string, std::string>> override;
  auto decrypt(std::map<std::string, std::string> encrypted_node)
    -> std::pair<error, std::vector<std::byte>> override;
  auto mangle(std::string field_name) -> std::string override;
  auto demangle(std::string field_name) -> std::string override;
  auto is_mangled(const std::string& field_name) -> bool override;

private:
  std::string encrypted_field_name_prefix_;
  std::map<std::string, std::shared_ptr<encrypter>> alias_to_encrypter_{};
  std::map<std::string, std::shared_ptr<decrypter>> algorithm_to_decrypter_{};
};
} // namespace couchbase::crypto
