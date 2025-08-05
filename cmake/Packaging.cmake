include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/couchbase_encryption DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES LICENSE.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

set(COUCHBASE_CXX_ENCRYPTION_PKGCONFIG_VERSION "${COUCHBASE_CXX_ENCRYPTION_SEMVER}" CACHE STRING "The version to use in couchbase_cxx_encryption.pc")
configure_file(${PROJECT_SOURCE_DIR}/cmake/couchbase_cxx_encryption.pc.in
               ${PROJECT_BINARY_DIR}/packaging/couchbase_cxx_encryption.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/packaging/couchbase_cxx_encryption.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/couchbase_cxx_encryption-config.cmake.in
  ${PROJECT_BINARY_DIR}/couchbase_cxx_encryption-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/couchbase_cxx_encryption)

write_basic_package_version_file(
  couchbase_cxx_encryption-version.cmake
  VERSION ${couchbase_cxx_encryption_VERSION}
  COMPATIBILITY SameMinorVersion)

install(
  FILES
  ${PROJECT_BINARY_DIR}/couchbase_cxx_encryption-version.cmake
  ${PROJECT_BINARY_DIR}/couchbase_cxx_encryption-config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/couchbase_cxx_encryption)

install(
  TARGETS couchbase_cxx_encryption
  EXPORT couchbase_cxx_encryption-targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  EXPORT couchbase_cxx_encryption-targets
  NAMESPACE couchbase_cxx_encryption::
  FILE couchbase_cxx_encryption-targets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/couchbase_cxx_encryption)

set(COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME "couchbase-cxx-encryption-${COUCHBASE_CXX_ENCRYPTION_SEMVER}")
set(COUCHBASE_CXX_ENCRYPTION_TARBALL "${PROJECT_BINARY_DIR}/packaging/${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}.tar.gz")
set(COUCHBASE_CXX_ENCRYPTION_MANIFEST "${PROJECT_BINARY_DIR}/packaging/MANIFEST")

if(APPLE)
  find_program(TAR gtar)
  find_program(SED gsed)
  find_program(XARGS gxargs)
  find_program(CP gcp)
else()
  find_program(TAR tar)
  find_program(SED sed)
  find_program(XARGS xargs)
  find_program(CP cp)
endif()

add_custom_command(
  OUTPUT ${COUCHBASE_CXX_ENCRYPTION_MANIFEST}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND git ls-files --recurse-submodules | LC_ALL=C sort > ${COUCHBASE_CXX_ENCRYPTION_MANIFEST})

if(COUCHBASE_CXX_ENCRYPTION_RECORD_BUILD_INFO_FOR_TARBALL)
  file(
    WRITE "${CMAKE_SOURCE_DIR}/cmake/TarballRelease.cmake"
    "
set(COUCHBASE_CXX_ENCRYPTION_GIT_REVISION \"${COUCHBASE_CXX_ENCRYPTION_GIT_REVISION}\")
set(COUCHBASE_CXX_ENCRYPTION_GIT_DESCRIBE \"${COUCHBASE_CXX_ENCRYPTION_GIT_DESCRIBE}\")
set(COUCHBASE_CXX_ENCRYPTION_BUILD_TIMESTAMP \"${COUCHBASE_CXX_ENCRYPTION_BUILD_TIMESTAMP}\")
message(STATUS \"Building from Tarball: ${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}.tar.gz\")
")
endif()

add_custom_command(
  OUTPUT ${COUCHBASE_CXX_ENCRYPTION_TARBALL}
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/packaging"
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}"
  COMMAND ${TAR} -cf - -C ${PROJECT_SOURCE_DIR} -T ${COUCHBASE_CXX_ENCRYPTION_MANIFEST} | ${TAR} xf - -C
          "${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}"
  COMMAND
    ${CMAKE_COMMAND} -S "${PROJECT_BINARY_DIR}/packaging/${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}" -B
    "${PROJECT_BINARY_DIR}/packaging/${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}/tmp/build"
    -DCOUCHBASE_CXX_ENCRYPTION_BUILD_TESTS=OFF -DCOUCHBASE_CXX_ENCRYPTION_BUILD_DOCS=OFF -DCOUCHBASE_CXX_ENCRYPTION_INSTALL=ON
    -DCOUCHBASE_CXX_ENCRYPTION_RECORD_BUILD_INFO_FOR_TARBALL=ON
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}/tmp"
  # https://reproducible-builds.org/docs/archives/
  COMMAND ${TAR} --sort=name --mtime="${COUCHBASE_CXX_ENCRYPTION_BUILD_TIMESTAMP}Z" --owner=0 --group=0 --numeric-owner -czf
          "${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}.tar.gz" "${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}"
  COMMAND ${CMAKE_COMMAND} -E rm -rf "${COUCHBASE_CXX_ENCRYPTION_TARBALL_NAME}"
  DEPENDS ${COUCHBASE_CXX_ENCRYPTION_MANIFEST})

add_custom_target(couchbase_cxx_encryption_packaging_tarball DEPENDS ${COUCHBASE_CXX_ENCRYPTION_TARBALL})
