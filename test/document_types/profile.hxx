/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/encrypted_fields.hxx>

#include <cinttypes>
#include <string>

#include <tao/json/forward.hpp>

struct profile {
  std::string username{};
  std::string full_name{};
  std::uint32_t birth_year{};

  auto operator==(const profile& other) const -> bool
  {
    return username == other.username && full_name == other.full_name &&
           birth_year == other.birth_year;
  }

  inline static const std::vector<couchbase::crypto::encrypted_field> encrypted_fields{
    {
      /* .field_path = */ { "full_name" },
      /* .encrypter_alias = */ {},
    },
  };
};

template<>
struct tao::json::traits<profile> {
  template<template<typename...> class Traits>
  static void assign(tao::json::basic_value<Traits>& v, const profile& p)
  {
    v = {
      { "username", p.username },
      { "full_name", p.full_name },
      { "birth_year", p.birth_year },
    };
  }

  template<template<typename...> class Traits>
  static profile as(const tao::json::basic_value<Traits>& v)
  {
    profile result;
    const auto& object = v.get_object();
    result.username = object.at("username").template as<std::string>();
    result.full_name = object.at("full_name").template as<std::string>();
    if (object.count("birth_year") != 0) {
      // expect incomplete JSON here, as we might use projections to fetch reduced document
      // as an alternative we might use std::optional<> here
      result.birth_year = object.at("birth_year").template as<std::uint32_t>();
    }
    return result;
  }
};
