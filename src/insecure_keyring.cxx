/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include <couchbase/error_codes.hxx>
#include <couchbase_encryption/insecure_keyring.hxx>

namespace couchbase::crypto
{
insecure_keyring::insecure_keyring(const std::vector<key>& keys)
{
  for (const auto& k : keys) {
    keys_[k.id()] = k;
  }
}

auto
insecure_keyring::get(const std::string& key_id) const -> std::pair<error, key>
{
  if (const auto it = keys_.find(key_id); it == keys_.end()) {
    return {
      error{ errc::field_level_encryption::crypto_key_not_found, "Key not found: " + key_id }, {}
    };
  }
  return { {}, keys_.at(key_id) };
}

void
insecure_keyring::add_key(key k)
{
  keys_[k.id()] = std::move(k);
}
} // namespace couchbase::crypto
