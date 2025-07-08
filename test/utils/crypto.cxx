/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "crypto.hxx"

#include <catch2/catch_test_macros.hpp>
#include <tao/json/to_string.hpp>

namespace test::utils
{
void
ensure_field_is_encrypted(const tao::json::value& blob,
                          const std::string& field_name,
                          const std::string& key_id)
{
  INFO("Checking if field `" << field_name << "` is encrypted in `" << tao::json::to_string(blob, 2)
                             << "`");

  const auto mangled_field_name = "encrypted$" + field_name;

  REQUIRE(blob.find(field_name) == nullptr);
  REQUIRE(blob.find(mangled_field_name) != nullptr);
  REQUIRE(blob.at(mangled_field_name).get_object().size() == 3);
  REQUIRE(blob.at(mangled_field_name).at("alg").get_string() == "AEAD_AES_256_CBC_HMAC_SHA512");
  REQUIRE(blob.at(mangled_field_name).at("kid").get_string() == key_id);
  REQUIRE_FALSE(blob.at(mangled_field_name).at("ciphertext").get_string().empty());
}

void
ensure_field_at_path_is_encrypted(const tao::json::value& blob,
                                  const std::vector<std::string>& field_path,
                                  const std::string& key_id)
{
  tao::json::value b = blob;
  for (std::size_t i = 0; i < field_path.size() - 1; ++i) {
    b = b.at(field_path.at(i));
  }
  ensure_field_is_encrypted(b, field_path.at(field_path.size() - 1), key_id);
}
} // namespace test::utils
