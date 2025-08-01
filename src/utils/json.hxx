/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *   Copyright 2020-2021 Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#pragma once

#include <tao/json/forward.hpp>

#include <string>
#include <vector>

namespace couchbase::crypto::impl::utils::json
{
auto
parse(std::string_view input) -> tao::json::value;

auto
parse(const char* input, std::size_t size) -> tao::json::value;

auto
generate(const tao::json::value& object) -> std::string;

auto
parse_binary(const std::vector<std::byte>& input) -> tao::json::value;

auto
generate_binary(const tao::json::value& object) -> std::vector<std::byte>;
} // namespace couchbase::crypto::impl::utils::json
