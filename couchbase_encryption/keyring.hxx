/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/key.hxx>
#include <couchbase/error.hxx>

namespace couchbase::crypto
{
class keyring
{
public:
  keyring() = default;
  keyring(const keyring& other) = default;
  keyring(keyring&& other) = default;
  auto operator=(const keyring& other) -> keyring& = default;
  auto operator=(keyring&& other) -> keyring& = default;
  virtual ~keyring() = default;

  [[nodiscard]] virtual auto get(const std::string& key_id) const -> std::pair<error, key> = 0;
};
} // namespace couchbase::crypto
