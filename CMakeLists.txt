cmake_minimum_required(VERSION 3.19)
message(STATUS "CMake version ${CMAKE_VERSION}")

cmake_policy(SET CMP0074 NEW)

set(couchbase_cxx_encryption_BUILD_NUMBER 1)
if(DEFINED ENV{BUILD_NUMBER})
    set(couchbase_cxx_encryption_BUILD_NUMBER $ENV{BUILD_NUMBER})
endif()

project(
  couchbase_cxx_encryption
  VERSION "0.1.0"
  LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

include(cmake/VersionInfo.cmake)

set(couchbase_cxx_encryption_FILES
        src/utils/base64.cc
        src/utils/json.cxx
        src/aead_aes_256_cbc_hmac_sha512_provider.cxx
        src/default_manager.cxx
        src/encryption_result.cxx
        src/insecure_keyring.cxx
        src/key.cxx
        src/transcoder.cxx
)

add_library(couchbase_cxx_encryption STATIC ${couchbase_cxx_encryption_FILES})

set_target_properties(couchbase_cxx_encryption PROPERTIES LINKER_LANGUAGE CXX)

find_package(couchbase_cxx_client)
if (NOT couchbase_cxx_client_FOUND)
    include(cmake/CPM.cmake)

    cpmaddpackage(
            NAME
            couchbase_cxx_client
            GIT_TAG
            b80886d6b4a75c604d26a4b54252a1e78db42297
            VERSION
            1.2.0
            GITHUB_REPOSITORY
            "couchbase/couchbase-cxx-client"
            OPTIONS
            "COUCHBASE_CXX_CLIENT_INSTALL ON"
            "COUCHBASE_CXX_CLIENT_BUILD_SHARED ON"
            "COUCHBASE_CXX_CLIENT_BUILD_STATIC OFF")
endif()

set(CXX_SDK_TARGET couchbase_cxx_client::couchbase_cxx_client)

target_include_directories(couchbase_cxx_encryption
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/generated
)

target_link_libraries(couchbase_cxx_encryption
        PRIVATE
        ${CXX_SDK_TARGET}
        Microsoft.GSL::GSL
        spdlog::spdlog
        taocpp::json
)

option(COUCHBASE_CXX_ENCRYPTION_BUILD_EXAMPLES "Build example programs" ON)
if(COUCHBASE_CXX_ENCRYPTION_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(COUCHBASE_CXX_ENCRYPTION_BUILD_TESTS "Build test programs" ON)
if(COUCHBASE_CXX_ENCRYPTION_BUILD_TESTS)
    include(cmake/Testing.cmake)
endif()

option(COUCHBASE_CXX_ENCRYPTION_BUILD_DOCS "Build API documentation" ON)
if(COUCHBASE_CXX_ENCRYPTION_BUILD_DOCS)
    include(cmake/Documentation.cmake)
endif()
