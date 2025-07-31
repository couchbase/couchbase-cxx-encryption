/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/encrypted_fields.hxx>

#include <map>
#include <string>
#include <vector>

#include <tao/json/forward.hpp>
#include <tao/json/type.hpp>

struct person {
  struct street {
    std::string first_line{};
    std::string second_line{};

    auto operator==(const street& other) const -> bool
    {
      return first_line == other.first_line && second_line == other.second_line;
    }
  };

  struct address {
    std::string number{};
    street str{};

    auto operator==(const address& other) const -> bool
    {
      return number == other.number && str == other.str;
    }
  };

  struct pet {
    struct attribute {
      std::string action{};
      std::string extra{};

      auto operator==(const attribute& other) const -> bool
      {
        return action == other.action && extra == other.extra;
      }
    };

    std::string name{};
    std::map<std::string, attribute> attributes{};

    auto operator==(const pet& other) const -> bool
    {
      return name == other.name && attributes == other.attributes;
    }
  };

  std::string first_name{};
  std::string last_name{};
  std::string password{};
  address addr{};
  pet pet{};

  auto operator==(const person& other) const -> bool
  {
    return first_name == other.first_name && last_name == other.last_name &&
           password == other.password && addr == other.addr && pet == other.pet;
  }

  inline static const std::vector<couchbase::crypto::encrypted_field> encrypted_fields{
    {
      /* .field_path = */ { "password" },
      /* .encrypter_alias = */ { "one" },
    },
    {
      /* .field_path = */ { "address" },
      /* .encrypter_alias = */ {},
    },
    {
      /* .field_path = */ { "address", "street" },
      /* .encrypter_alias = */ {},
    },
    {
      /* .field_path = */ { "address", "street", "second" },
      /* .encrypter_alias = */ { "one" },
    },
    {
      /* .field_path = */ { "pet", "attributes" },
      /* .encrypter_alias = */ {},
    }
  };
};

template<>
struct tao::json::traits<person> {
  template<template<typename...> class Traits>
  static void assign(tao::json::basic_value<Traits>& v, const person& p)
  {
    v = { { "first_name", p.first_name },
          { "last_name", p.last_name },
          { "password", p.password },
          { "address",
            { { "number", p.addr.number },
              { "street",
                { { "first", p.addr.str.first_line }, { "second", p.addr.str.second_line } } } } },
          { "pet", { { "name", p.pet.name }, { "attributes", tao::json::empty_object } } } };

    for (const auto& [attr_key, attr] : p.pet.attributes) {
      v["pet"]["attributes"][attr_key] = { { "action", attr.action }, { "extra", attr.extra } };
    }
  }

  template<template<typename...> class Traits>
  static auto as(const tao::json::basic_value<Traits>& v) -> person
  {
    person result;
    const auto& object = v.get_object();
    result.first_name = object.at("first_name").template as<std::string>();
    result.last_name = object.at("last_name").template as<std::string>();
    result.password = object.at("password").template as<std::string>();
    result.addr.number = object.at("address").at("number").template as<std::string>();
    result.addr.str = { object.at("address").at("street").at("first").template as<std::string>(),
                        object.at("address").at("street").at("second").template as<std::string>() };
    result.pet.name = object.at("pet").at("name").template as<std::string>();
    for (const auto& [attr_key, attr_value] : object.at("pet").at("attributes").get_object()) {
      result.pet.attributes[attr_key] = { attr_value.at("action").template as<std::string>(),
                                          attr_value.at("extra").template as<std::string>() };
    }
    return result;
  }
};
