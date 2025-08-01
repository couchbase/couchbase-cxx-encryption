/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "binary.hxx"

#include <algorithm>

namespace test::utils
{
[[nodiscard]] auto
to_string(const std::vector<std::byte>& input) -> std::string
{
  return { reinterpret_cast<const char*>(input.data()), input.size() };
}

auto
make_bytes(std::vector<unsigned char> v) -> std::vector<std::byte>
{
    std::vector<std::byte> out{ v.size() };
    std::transform(v.begin(), v.end(), out.begin(), [](int c) {
      return static_cast<std::byte>(c);
    });
    return out;
}

} // namespace test::utils
