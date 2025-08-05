cmake_minimum_required(VERSION 3.19)
message(STATUS "CMake version ${CMAKE_VERSION}")

cmake_policy(SET CMP0074 NEW)
set(CMAKE_CXX_STANDARD 17)

set(couchbase_cxx_encryption_BUILD_NUMBER 1)
if(DEFINED ENV{BUILD_NUMBER})
    set(couchbase_cxx_encryption_BUILD_NUMBER $ENV{BUILD_NUMBER})
endif()

if(NOT DEFINED COUCHBASE_CXX_ENCRYPTION_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(COUCHBASE_CXX_ENCRYPTION_MASTER_PROJECT ON)
    else()
        set(COUCHBASE_CXX_ENCRYPTION_MASTER_PROJECT OFF)
    endif()
endif()

project(
  couchbase_cxx_encryption
  VERSION "1.0.0"
  LANGUAGES CXX C)

message(STATUS "Couchbase C++ Field Level Encryption Library ${couchbase_cxx_encryption_VERSION} build ${couchbase_cxx_encryption_BUILD_NUMBER}, master: ${COUCHBASE_CXX_ENCRYPTION_MASTER_PROJECT}")

include(cmake/TarballRelease.cmake OPTIONAL)
include(cmake/PreventInSourceBuilds.cmake)
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

add_library(couchbase_cxx_encryption SHARED ${couchbase_cxx_encryption_FILES})

set_target_properties(couchbase_cxx_encryption PROPERTIES LINKER_LANGUAGE CXX)

if (COUCHBASE_CXX_ENCRYPTION_MASTER_PROJECT)
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
else()
    find_package(couchbase_cxx_client REQUIRED)
endif()

find_package(spdlog REQUIRED)
find_package(gsl REQUIRED)

set(CXX_SDK_TARGET couchbase_cxx_client::couchbase_cxx_client)
if (NOT TARGET ${CXX_SDK_TARGET})
    set(CXX_SDK_TARGET couchbase_cxx_client::couchbase_cxx_client_static)
endif()

target_include_directories(couchbase_cxx_encryption
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated_$<CONFIG>>
)

target_link_libraries(couchbase_cxx_encryption
        PRIVATE
        ${CXX_SDK_TARGET}
        Microsoft.GSL::GSL
        spdlog::spdlog
        taocpp::json
)

option(COUCHBASE_CXX_ENCRYPTION_BUILD_EXAMPLES "Build example programs" ${COUCHBASE_CXX_ENCRYPTION_MASTER_PROJECT})
if(COUCHBASE_CXX_ENCRYPTION_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(COUCHBASE_CXX_ENCRYPTION_BUILD_TESTS "Build test programs" ${COUCHBASE_CXX_ENCRYPTION_MASTER_PROJECT})
if(COUCHBASE_CXX_ENCRYPTION_BUILD_TESTS)
    include(cmake/Testing.cmake)
endif()

option(COUCHBASE_CXX_ENCRYPTION_BUILD_DOCS "Build API documentation" ON)
if(COUCHBASE_CXX_ENCRYPTION_BUILD_DOCS)
    include(cmake/Documentation.cmake)
endif()

option(COUCHBASE_CXX_ENCRYPTION_INSTALL "Install library" OFF)
if(COUCHBASE_CXX_ENCRYPTION_INSTALL)
    include(cmake/Packaging.cmake)
endif()
