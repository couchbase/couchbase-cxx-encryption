/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <string>
#include <vector>

namespace test::utils
{
[[nodiscard]] auto
to_string(const std::vector<std::byte>& input) -> std::string;

[[nodiscard]] auto
make_bytes(std::vector<unsigned char> v) -> std::vector<std::byte>;
} // namespace test::utils
