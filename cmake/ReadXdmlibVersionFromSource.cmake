# xdmlib. A fast XDM parsing and writing library.
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
#          alexs.mac@gmail.com (Alex Stewart)

# Extract xdmlib version from <XDMLIB_SOURCE_ROOT>/include/xdmlib/version.h
# so that we only have a single definition of the xdmlib version, not two
# one in the source and one in CMakeLists.txt.
macro(read_xdmlib_version_from_source XDMLIB_SOURCE_ROOT)
  set(XDMLIB_VERSION_FILE ${XDMLIB_SOURCE_ROOT}/includes/xdmlib/version.h)
  if (NOT EXISTS ${XDMLIB_VERSION_FILE})
    message(FATAL_ERROR "Cannot find xdmlib version.h file in specified "
      "xdmlib source directory: ${XDMLIB_SOURCE_ROOT}, it is not here: "
      "${XDMLIB_VERSION_FILE}")
  endif()

  file(READ ${XDMLIB_VERSION_FILE} XDMLIB_VERSION_FILE_CONTENTS)

  string(REGEX MATCH "#define XDMLIB_VERSION_MAJOR [0-9]+"
    XDMLIB_VERSION_MAJOR "${XDMLIB_VERSION_FILE_CONTENTS}")
  string(REGEX REPLACE "#define XDMLIB_VERSION_MAJOR ([0-9]+)" "\\1"
    XDMLIB_VERSION_MAJOR "${XDMLIB_VERSION_MAJOR}")
  # NOTE: if (VAR) is FALSE if VAR is numeric and <= 0, as such we cannot use
  #       it for testing version numbers, which might well be zero, at least
  #       for the patch version, hence check for empty string explicitly.
  if ("${XDMLIB_VERSION_MAJOR}" STREQUAL "")
    message(FATAL_ERROR "Failed to extract xdmlib major version from "
      "${XDMLIB_VERSION_FILE}")
  endif()

  string(REGEX MATCH "#define XDMLIB_VERSION_MINOR [0-9]+"
    XDMLIB_VERSION_MINOR "${XDMLIB_VERSION_FILE_CONTENTS}")
  string(REGEX REPLACE "#define XDMLIB_VERSION_MINOR ([0-9]+)" "\\1"
    XDMLIB_VERSION_MINOR "${XDMLIB_VERSION_MINOR}")
  if ("${XDMLIB_VERSION_MINOR}" STREQUAL "")
    message(FATAL_ERROR "Failed to extract xdmlib minor version from "
      "${XDMLIB_VERSION_FILE}")
  endif()

  string(REGEX MATCH "#define XDMLIB_VERSION_REVISION [0-9]+"
    XDMLIB_VERSION_PATCH "${XDMLIB_VERSION_FILE_CONTENTS}")
  string(REGEX REPLACE "#define XDMLIB_VERSION_REVISION ([0-9]+)" "\\1"
    XDMLIB_VERSION_PATCH "${XDMLIB_VERSION_PATCH}")
  if ("${XDMLIB_VERSION_PATCH}" STREQUAL "")
    message(FATAL_ERROR "Failed to extract xdmlib patch version from "
      "${XDMLIB_VERSION_FILE}")
  endif()

  # This is on a single line s/t CMake does not interpret it as a list of
  # elements and insert ';' separators which would result in 3.;2.;0 nonsense.
  set(XDMLIB_VERSION "${XDMLIB_VERSION_MAJOR}.${XDMLIB_VERSION_MINOR}.${XDMLIB_VERSION_PATCH}")

  message(STATUS "Detected xdmlib version: ${XDMLIB_VERSION} from "
    "${XDMLIB_VERSION_FILE}")
endmacro()
