/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace couchbase::crypto
{
struct encrypted_field {
  std::vector<std::string> field_path;
  std::optional<std::string> encrypter_alias{};

  auto operator==(const encrypted_field& other) const -> bool
  {
    return field_path == other.field_path && encrypter_alias == other.encrypter_alias;
  }
};

template<typename Document, typename = void>
struct has_encrypted_fields : std::false_type {
};

template<typename Document>
struct has_encrypted_fields<Document, std::void_t<decltype(Document::encrypted_fields)>>
  : std::is_same<decltype(Document::encrypted_fields),
                 const std::vector<couchbase::crypto::encrypted_field>> {
};

template<typename Document>
constexpr bool has_encrypted_fields_v = has_encrypted_fields<Document>::value;

} // namespace couchbase::crypto
