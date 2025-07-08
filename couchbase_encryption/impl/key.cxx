/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include <couchbase_encryption/key.hxx>

namespace couchbase::crypto
{
key::key(std::string id, std::vector<std::byte> bytes)
  : id_{ std::move(id) }
  , bytes_{ std::move(bytes) }
{
}

auto
key::id() const -> const std::string&
{
  return id_;
}

auto
key::bytes() const -> const std::vector<std::byte>&
{
  return bytes_;
}
} // namespace couchbase::crypto
