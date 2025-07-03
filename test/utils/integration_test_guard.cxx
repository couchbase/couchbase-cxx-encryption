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

#include "integration_test_guard.hxx"

#include "logger.hxx"

namespace test::utils
{
integration_test_guard::integration_test_guard()
    : ctx{ test_context::load_from_environment() }
{
    init_logger();
    auto [err, c] = couchbase::cluster::connect(ctx.connection_string, ctx.build_options()).get();
    if (err) {
        throw std::system_error(err.ec(), err.message());
    }
    cluster = std::move(c);
}

integration_test_guard::integration_test_guard(const couchbase::cluster_options& opts)
    : ctx{ test_context::load_from_environment() }
{
    init_logger();
    auto [err, c] = couchbase::cluster::connect(ctx.connection_string, opts).get();
    if (err) {
        throw std::system_error(err.ec(), err.message());
    }
    cluster = std::move(c);
}

integration_test_guard::integration_test_guard(const std::shared_ptr<couchbase::crypto::manager>& crypto_manager)
    : ctx{ test_context::load_from_environment() }
{
    init_logger();
    auto opts = ctx.build_options();
    opts.crypto_manager(crypto_manager);
    auto [err, c] = couchbase::cluster::connect(ctx.connection_string, opts).get();
    if (err) {
        throw std::system_error(err.ec(), err.message());
    }
    cluster = std::move(c);
}

integration_test_guard::~integration_test_guard()
{
    cluster.close().get();
}

auto
integration_test_guard::cluster_version() const -> server_version
{
    return ctx.version;
}
} // namespace test::utils
