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

  virtual auto encrypt(std::vector<std::byte> plaintext,
                       const std::optional<std::string>& encrypter_alias)
    -> std::pair<error, std::map<std::string, std::string>> = 0;
  virtual auto decrypt(std::map<std::string, std::string> encrypted_node)
    -> std::pair<error, std::vector<std::byte>> = 0;
  virtual auto mangle(std::string) -> std::string = 0;
  virtual auto demangle(std::string) -> std::string = 0;
  virtual auto is_mangled(const std::string&) -> bool = 0;
};
} // namespace couchbase::crypto
