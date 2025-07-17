if(NOT COUCHBASE_CXX_ENCRYPTION_GIT_REVISION)
    find_program(GIT git)
    if(GIT)
        execute_process(
                COMMAND git rev-parse HEAD
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                OUTPUT_STRIP_TRAILING_WHITESPACE
                OUTPUT_VARIABLE COUCHBASE_CXX_ENCRYPTION_GIT_REVISION)
    else()
        set(COUCHBASE_CXX_ENCRYPTION_GIT_REVISION "unknown")
    endif()
endif()
string(SUBSTRING "${COUCHBASE_CXX_ENCRYPTION_GIT_REVISION}" 0 7 COUCHBASE_CXX_ENCRYPTION_GIT_REVISION_SHORT)

if(NOT COUCHBASE_CXX_ENCRYPTION_GIT_DESCRIBE)
    if(GIT)
        execute_process(
                COMMAND git describe --always --long HEAD
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                OUTPUT_STRIP_TRAILING_WHITESPACE
                OUTPUT_VARIABLE COUCHBASE_CXX_ENCRYPTION_GIT_DESCRIBE)
    else()
        set(COUCHBASE_CXX_ENCRYPTION_GIT_DESCRIBE "unknown")
    endif()
endif()

if(NOT COUCHBASE_CXX_ENCRYPTION_BUILD_TIMESTAMP)
    if(GIT)
        execute_process(
                COMMAND git describe --tags --abbrev=0 HEAD
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                RESULT_VARIABLE tag_result
                OUTPUT_STRIP_TRAILING_WHITESPACE
                OUTPUT_VARIABLE last_tag)

        if (tag_result EQUAL 0)
            execute_process(
                    COMMAND git log --max-count=1 --no-patch --format=%cd --date=format:%Y-%m-%dT%H:%M:%S ${last_tag}
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    OUTPUT_VARIABLE COUCHBASE_CXX_ENCRYPTION_BUILD_TIMESTAMP)
        endif()
    endif()

    if (NOT COUCHBASE_CXX_ENCRYPTION_BUILD_TIMESTAMP)
        string(TIMESTAMP COUCHBASE_CXX_ENCRYPTION_BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%S" UTC)
    endif()
endif()

string(REGEX REPLACE "T.*" "" COUCHBASE_CXX_ENCRYPTION_BUILD_DATE "${COUCHBASE_CXX_ENCRYPTION_BUILD_TIMESTAMP}")

set(COUCHBASE_CXX_ENCRYPTION_SEMVER "${couchbase_cxx_encryption_VERSION}")
set(COUCHBASE_CXX_ENCRYPTION_PACKAGE_VERSION "${couchbase_cxx_encryption_VERSION}")
set(COUCHBASE_CXX_ENCRYPTION_PACKAGE_RELEASE "${couchbase_cxx_encryption_BUILD_NUMBER}")
set(COUCHBASE_CXX_ENCRYPTION_NUMBER_OF_COMMITS 0)
if(COUCHBASE_CXX_ENCRYPTION_GIT_DESCRIBE MATCHES
        "^([0-9]+\\.[0-9]+\\.[0-9]+)(-([a-zA-Z0-9\\.]+))?(-([0-9]+)-g([a-zA-Z0-9]+))?$")
    set(COUCHBASE_CXX_ENCRYPTION_SEMVER "${CMAKE_MATCH_1}")
    set(COUCHBASE_CXX_ENCRYPTION_PACKAGE_VERSION "${CMAKE_MATCH_1}")
    if(CMAKE_MATCH_3) # pre-release
        set(COUCHBASE_CXX_ENCRYPTION_SEMVER "${COUCHBASE_CXX_ENCRYPTION_SEMVER}-${CMAKE_MATCH_3}")
        set(COUCHBASE_CXX_ENCRYPTION_PACKAGE_RELEASE "${CMAKE_MATCH_3}.${couchbase_cxx_encryption_BUILD_NUMBER}")
    endif()
    if(CMAKE_MATCH_5 AND CMAKE_MATCH_5 GREATER 0) # number_of_commits.build_number.sha1
        set(COUCHBASE_CXX_ENCRYPTION_NUMBER_OF_COMMITS ${CMAKE_MATCH_5})
        set(COUCHBASE_CXX_ENCRYPTION_SEMVER
                "${COUCHBASE_CXX_ENCRYPTION_SEMVER}+${CMAKE_MATCH_5}.${couchbase_cxx_encryption_BUILD_NUMBER}.${CMAKE_MATCH_6}")
        if(CMAKE_MATCH_3) # pre-release
            set(COUCHBASE_CXX_ENCRYPTION_PACKAGE_RELEASE
                    "${CMAKE_MATCH_3}.${CMAKE_MATCH_5}.${couchbase_cxx_encryption_BUILD_NUMBER}.${CMAKE_MATCH_6}")
        else()
            set(COUCHBASE_CXX_ENCRYPTION_PACKAGE_RELEASE "${CMAKE_MATCH_5}.${couchbase_cxx_encryption_BUILD_NUMBER}.${CMAKE_MATCH_6}")
        endif()
    endif()
endif()

configure_file(${PROJECT_SOURCE_DIR}/cmake/build_version.hxx.in
        ${PROJECT_BINARY_DIR}/generated/couchbase_encryption/build_version.hxx @ONLY)
