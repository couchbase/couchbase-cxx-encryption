find_package(Doxygen)
find_program(DOT dot)
if(DOXYGEN_FOUND AND DOT)
    message(STATUS "Using doxygen: ${DOXYGEN_VERSION} (with ${DOT})")
    find_package(Java COMPONENTS Runtime)
    if(Java_Runtime_FOUND)
        include(UseJava)
        find_jar(PLANTUML_JAR_PATH NAMES plantuml)
        message(STATUS "Found plantuml: ${PLANTUML_JAR_PATH}")
    endif()
    file(
            GLOB_RECURSE
            COUCHBASE_CXX_ENCRYPTION_PUBLIC_HEADERS
            ${PROJECT_SOURCE_DIR}/couchbase_encryption/*.hxx
            ${PROJECT_SOURCE_DIR}/docs/*.hxx
            ${PROJECT_SOURCE_DIR}/docs/*.md)

    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/couchbase_encryption)
    set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/couchbase-cxx-encryption-${COUCHBASE_CXX_ENCRYPTION_SEMVER})
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
    set(DOXYGEN_CONFIG_TEMPLATE ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_CONFIG ${PROJECT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_CONFIG_TEMPLATE} ${DOXYGEN_CONFIG})
    add_custom_command(
            OUTPUT ${DOXYGEN_INDEX_FILE}
            DEPENDS ${COUCHBASE_CXX_ENCRYPTION_PUBLIC_HEADERS}
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG}
            WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIR}
            MAIN_DEPENDENCY ${DOXYGEN_CONFIG}
            ${DOXYGEN_CONFIG_TEMPLATE}
            COMMENT "Generating documentation with Doxygen: ${DOXYGEN_INDEX_FILE}")
    add_custom_target(doxygen DEPENDS ${DOXYGEN_INDEX_FILE} ${COUCHBASE_CXX_ENCRYPTION_PUBLIC_HEADERS})
    message(STATUS "Doxygen public headers: ${COUCHBASE_CXX_ENCRYPTION_PUBLIC_HEADERS}")
else()
    message(STATUS "Could not find doxygen executable. Documentation generation will be disabled.")
endif()
