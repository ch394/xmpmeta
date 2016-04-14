# xmpmeta. A fast XMP metadata parsing and writing library.
# Copyright 2016 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
# * Neither the name of Google Inc. nor the names of its contributors may be
#   used to endorse or promote products derived from this software without
#   specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Authors: miraleung@google.com (Mira Leung)
#          keir@google.com (Keir Mierle)

set(XMPMETA_INTERNAL_SRC
    base64.cc
    file.cc
    gaudio.cc
    gimage.cc
    gpano.cc
    jpeg_io.cc
    md5.cc
    vr_photo_writer.cc
    xmp_const.cc
    xmp_data.cc
    xmp_parser.cc
    xmp_writer.cc
    external/strings/ascii_ctype.cc
    external/strings/case.cc
    external/strings/escaping.cc
    external/strings/numbers.cc
    xml/const.cc
    xml/deserializer_impl.cc
    xml/search.cc
    xml/serializer.h
    xml/serializer_impl.cc
    xml/utils.cc
)

# Heuristic for determining LIB_SUFFIX. FHS recommends that 64-bit systems
# install native libraries to lib64 rather than lib. Most distros seem to
# follow this convention with a couple notable exceptions (Debian-based and
# Arch-based distros) which we try to detect here.
if (CMAKE_SYSTEM_NAME MATCHES "Linux" AND
    NOT DEFINED LIB_SUFFIX AND
    NOT CMAKE_CROSSCOMPILING AND
    CMAKE_SIZEOF_VOID_P EQUAL "8" AND
    NOT EXISTS "/etc/debian_version" AND
    NOT EXISTS "/etc/arch-release")
  set(LIB_SUFFIX "64")
endif ()

# Also depend on the header files so that they appear in IDEs.
file(GLOB XMPMETA_INTERNAL_HDRS *.h)

# Build the list of dependencies for xmpmeta based on the current configuration.
if (NOT MINIGLOG AND GLOG_FOUND)
  list(APPEND XMPMETA_LIBRARY_PUBLIC_DEPENDENCIES ${GLOG_LIBRARIES})
endif (NOT MINIGLOG AND GLOG_FOUND)

set(XMPMETA_LIBRARY_SOURCE
    ${XMPMETA_INTERNAL_SRC}
    ${XMPMETA_INTERNAL_HDRS})

# Primarily for Android, but optionally for others, compile the minimal
# glog implementation into xmpmeta.
if (MINIGLOG)
  list(APPEND XMPMETA_LIBRARY_SOURCE miniglog/glog/logging.cc)
endif (MINIGLOG)

add_library(xmpmeta ${XMPMETA_LIBRARY_SOURCE})
set_target_properties(xmpmeta PROPERTIES
  VERSION ${XMPMETA_VERSION}
  SOVERSION ${XMPMETA_VERSION_MAJOR})

include(AppendTargetProperty)
# Always build position-independent code (PIC), even when building xmpmeta as a
# static library so that shared libraries can link against it, not just
# executables (PIC does not apply on Windows).
if (NOT WIN32 AND NOT BUILD_SHARED_LIBS)
  # Use the explicit POSITION_INDEPENDENT_CODE target property on CMake versions
  # that support it (>= 2.8.9).  Otherwise, manually add the -fPIC flag as an
  # additional compile definitions for the target.
  if (CMAKE_VERSION VERSION_LESS "2.8.9")
    append_target_property(xmpmeta COMPILE_FLAGS "-fPIC")
  else()
    # Use set_target_properties() not append_target_property() here as
    # POSITION_INDEPENDENT_CODE is a binary ON/OFF switch.
    set_target_properties(xmpmeta PROPERTIES POSITION_INDEPENDENT_CODE ON)
  endif()
endif()

if (CMAKE_VERSION VERSION_LESS "2.8.12")
  # CMake version < 2.8.12 does not support target_compile_options(), warn
  # user that they will have to add compile flags to their own projects
  # manually if required.
  if (CXX11 AND COMPILER_HAS_CXX11_FLAG)
    message("-- Warning: Detected CMake version: ${CMAKE_VERSION} < 2.8.12, "
      "which is the minimum required for compile options to be included in an "
      "exported CMake target, but XMPMETA_USE_CXX11 is enabled and the detected. "
      "compiler requires -std=c++11. The client is responsible for adding "
      "-std=c++11 when using xmpmeta.")
  endif()
else()
  # CMake version >= 2.8.12 supports target_compile_options().
  if (CXX11 AND COMPILER_HAS_CXX11_FLAG)
    # If xmpmeta is compiled using C++11, and the compiler requires -std=c++11
    # to be set, then ensure that this requirement is rolled into the exported
    # CMake target, such that client code which uses xmpmeta will inherit it (if
    # the CMake version supports it), iff they are NOT compiling for C.  We
    # check for not C, rather than C++ as LINKER_LANGUAGE is often NOTFOUND and
    # then uses the default (C++).
    target_compile_options(xmpmeta PUBLIC
      $<$<NOT:$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,C>>:-std=c++11>)
  endif()
endif()

if (BUILD_SHARED_LIBS)
  # When building a shared library, mark all external libraries as
  # PRIVATE so they don't show up as a dependency.
  target_link_libraries(xmpmeta
        LINK_PUBLIC ${XMPMETA_LIBRARY_PUBLIC_DEPENDENCIES}
        LINK_PRIVATE ${XMPMETA_LIBRARY_PRIVATE_DEPENDENCIES})
else (BUILD_SHARED_LIBS)
  # When building a static library, all external libraries are
  # PUBLIC(default) since the user needs to link to them.
  # They will be listed in xmpmetaTargets.cmake.
  set(XMPMETA_LIBRARY_DEPENDENCIES
        ${XMPMETA_LIBRARY_PUBLIC_DEPENDENCIES}
        ${XMPMETA_LIBRARY_PRIVATE_DEPENDENCIES})
  target_link_libraries(xmpmeta ${XMPMETA_LIBRARY_DEPENDENCIES})
endif (BUILD_SHARED_LIBS)

install(TARGETS xmpmeta
        EXPORT  XmpmetaExport
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX})

if (BUILD_TESTING AND GFLAGS)
  # The XMPMETA_GFLAGS_NAMESPACE compile definition is NOT stored in
  # XMPMETA_COMPILE_OPTIONS (and thus config.h) as xmpmeta itself does not
  # require gflags, only the tests and examples do.
  add_definitions(-DXMPMETA_GFLAGS_NAMESPACE=${GFLAGS_NAMESPACE})

  add_library(gtest gmock_gtest_all.cc gmock_main.cc)
  if (BUILD_SHARED_LIBS)
    # Define gtest-specific shared library flags for compilation.
    append_target_property(gtest COMPILE_DEFINITIONS
      GTEST_CREATE_SHARED_LIBRARY)
  endif()

  add_library(test_util
              test_xmp_creator.cc
              test_util.cc)

  if (MINIGLOG)
    # When using miniglog, it is compiled into xmpmeta, thus xmpmeta becomes
    # the library against which other libraries should link for logging.
    target_link_libraries(gtest ${GFLAGS_LIBRARIES} xmpmeta)
    target_link_libraries(test_util xmpmeta gtest)
  else (MINIGLOG)
    target_link_libraries(gtest ${GFLAGS_LIBRARIES} ${GLOG_LIBRARIES})
    target_link_libraries(test_util xmpmeta gtest ${GLOG_LIBRARIES})
  endif (MINIGLOG)

  macro (XMPMETA_TEST NAME)
    add_executable(${NAME}_test ${NAME}_test.cc)
    target_link_libraries(${NAME}_test test_util xmpmeta gtest)
    if (BUILD_SHARED_LIBS)
      # Define gtest-specific shared library flags for linking.
      append_target_property(${NAME}_test COMPILE_DEFINITIONS
        GTEST_LINKED_AS_SHARED_LIBRARY)
    endif()
    add_test(NAME ${NAME}_test
             COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}_test
             --test_srcdir
             ${CMAKE_SOURCE_DIR}/testdata)
  endmacro (XMPMETA_TEST)

  macro (XML_TEST NAME)
    add_executable(${NAME}_test xml/${NAME}_test.cc)
    target_link_libraries(${NAME}_test test_util xmpmeta gtest)
    if (BUILD_SHARED_LIBX)
      # Define gtest-specific shared library flags for linking.
      append_target_property(${NAME}_test COMPILE_DEFINITIONS
        GTEST_LINKED_AS_SHARED_LIBRARY)
    endif()
    add_test(NAME ${NAME}_test
             COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${NAME}_test
             --test_srcdir
             ${CMAKE_SOURCE_DIR}/testdata)
  endmacro(XML_TEST)

  xmpmeta_test(base64)
  xmpmeta_test(vr_photo_writer)
  xmpmeta_test(gaudio)
  xmpmeta_test(gimage)
  xmpmeta_test(gpano)
  xmpmeta_test(jpeg_io)
  xmpmeta_test(md5)
  xmpmeta_test(xmp_parser)
  xmpmeta_test(xmp_writer)
  xml_test(deserializer_impl)
  xml_test(search)
  xml_test(serializer_impl)
  xml_test(utils)

endif (BUILD_TESTING AND GFLAGS)
