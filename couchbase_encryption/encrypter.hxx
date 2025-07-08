/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/encryption_result.hxx>
#include <couchbase/error.hxx>

#include <utility>
#include <vector>

namespace couchbase::crypto
{
class encrypter
{
public:
  encrypter() = default;
  encrypter(const encrypter& other) = default;
  encrypter(encrypter&& other) = default;
  auto operator=(const encrypter& other) -> encrypter& = default;
  auto operator=(encrypter&& other) -> encrypter& = default;
  virtual ~encrypter() = default;

  virtual auto encrypt(std::vector<std::byte> plaintext) -> std::pair<error, encryption_result> = 0;
};
} // namespace couchbase::crypto
