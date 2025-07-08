/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/manager.hxx>

#include <tao/json/value.hpp>

#include <string>

namespace test::utils
{
void
ensure_field_is_encrypted(const tao::json::value& blob,
                          const std::string& field_name,
                          const std::string& key_id = "test-key");

void
ensure_field_at_path_is_encrypted(const tao::json::value& blob,
                                  const std::vector<std::string>& field_path,
                                  const std::string& key_id = "test-key");
} // namespace test::utils
