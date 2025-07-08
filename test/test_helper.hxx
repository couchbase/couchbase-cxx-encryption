/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include "utils/binary.hxx"
#include "utils/test_context.hxx"

#include <spdlog/fmt/bundled/format.h>

#include <couchbase/error.hxx>
#include <couchbase/fmt/error.hxx>

#include <catch2/catch_test_macros.hpp>

#define REQUIRE_SUCCESS(ec)                                                                        \
  INFO((ec).message());                                                                            \
  REQUIRE_FALSE(ec)
#define REQUIRE_NO_ERROR(err)                                                                      \
  if (err) {                                                                                       \
    INFO(fmt::format("Expected no error. Got: {}", err));                                          \
  }                                                                                                \
  REQUIRE_SUCCESS(err.ec());
