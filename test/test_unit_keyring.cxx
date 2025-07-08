/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "test_helper.hxx"

#include <couchbase/error_codes.hxx>
#include <couchbase_encryption/insecure_keyring.hxx>

TEST_CASE("unit: insecure keyring", "[unit]")
{
  const auto keyring = std::make_shared<couchbase::crypto::insecure_keyring>(
    std::vector{ couchbase::crypto::key("test-key", test::utils::make_bytes({ 0x2a, 0x43 })) });

  {
    auto [err, key] = keyring->get("test-key");
    REQUIRE_NO_ERROR(err);
    REQUIRE(key.id() == "test-key");
    REQUIRE(key.bytes() == test::utils::make_bytes({ 0x2a, 0x43 }));
  }

  {
    auto [err, key] = keyring->get("test-key-2");
    REQUIRE(err.ec() == couchbase::errc::field_level_encryption::crypto_key_not_found);
  }

  {
    keyring->add_key(couchbase::crypto::key("test-key-2", test::utils::make_bytes({ 0x51, 0x1b })));
    auto [err, key] = keyring->get("test-key-2");
    REQUIRE_NO_ERROR(err);
    REQUIRE(key.id() == "test-key-2");
    REQUIRE(key.bytes() == test::utils::make_bytes({ 0x51, 0x1b }));
  }
}
