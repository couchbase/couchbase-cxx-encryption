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
class insecure_keyring : public keyring
{
public:
  insecure_keyring() = default;
  explicit insecure_keyring(const std::vector<key>& keys);

  void add_key(key k);

  [[nodiscard]] auto get(const std::string& key_id) const -> std::pair<error, key> override;

private:
  std::map<std::string, key> keys_;
};
} // namespace couchbase::crypto
