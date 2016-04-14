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
#          alexs.mac@gmail.com (Alex Stewart)

# Extract xmpmeta version from <XMPMETA_SOURCE_ROOT>/include/xmpmeta/version.h
# so that we only have a single definition of the xmpmeta version, not two
# one in the source and one in CMakeLists.txt.
macro(read_xmpmeta_version_from_source XMPMETA_SOURCE_ROOT)
  set(XMPMETA_VERSION_FILE ${XMPMETA_SOURCE_ROOT}/includes/xmpmeta/version.h)
  if (NOT EXISTS ${XMPMETA_VERSION_FILE})
    message(FATAL_ERROR "Cannot find xmpmeta version.h file in specified "
      "xmpmeta source directory: ${XMPMETA_SOURCE_ROOT}, it is not here: "
      "${XMPMETA_VERSION_FILE}")
  endif()

  file(READ ${XMPMETA_VERSION_FILE} XMPMETA_VERSION_FILE_CONTENTS)

  string(REGEX MATCH "#define XMPMETA_VERSION_MAJOR [0-9]+"
    XMPMETA_VERSION_MAJOR "${XMPMETA_VERSION_FILE_CONTENTS}")
  string(REGEX REPLACE "#define XMPMETA_VERSION_MAJOR ([0-9]+)" "\\1"
    XMPMETA_VERSION_MAJOR "${XMPMETA_VERSION_MAJOR}")
  # NOTE: if (VAR) is FALSE if VAR is numeric and <= 0, as such we cannot use
  #       it for testing version numbers, which might well be zero, at least
  #       for the patch version, hence check for empty string explicitly.
  if ("${XMPMETA_VERSION_MAJOR}" STREQUAL "")
    message(FATAL_ERROR "Failed to extract xmpmeta major version from "
      "${XMPMETA_VERSION_FILE}")
  endif()

  string(REGEX MATCH "#define XMPMETA_VERSION_MINOR [0-9]+"
    XMPMETA_VERSION_MINOR "${XMPMETA_VERSION_FILE_CONTENTS}")
  string(REGEX REPLACE "#define XMPMETA_VERSION_MINOR ([0-9]+)" "\\1"
    XMPMETA_VERSION_MINOR "${XMPMETA_VERSION_MINOR}")
  if ("${XMPMETA_VERSION_MINOR}" STREQUAL "")
    message(FATAL_ERROR "Failed to extract xmpmeta minor version from "
      "${XMPMETA_VERSION_FILE}")
  endif()

  string(REGEX MATCH "#define XMPMETA_VERSION_REVISION [0-9]+"
    XMPMETA_VERSION_PATCH "${XMPMETA_VERSION_FILE_CONTENTS}")
  string(REGEX REPLACE "#define XMPMETA_VERSION_REVISION ([0-9]+)" "\\1"
    XMPMETA_VERSION_PATCH "${XMPMETA_VERSION_PATCH}")
  if ("${XMPMETA_VERSION_PATCH}" STREQUAL "")
    message(FATAL_ERROR "Failed to extract xmpmeta patch version from "
      "${XMPMETA_VERSION_FILE}")
  endif()

  # This is on a single line s/t CMake does not interpret it as a list of
  # elements and insert ';' separators which would result in 3.;2.;0 nonsense.
  set(XMPMETA_VERSION "${XMPMETA_VERSION_MAJOR}.${XMPMETA_VERSION_MINOR}.${XMPMETA_VERSION_PATCH}")

  message(STATUS "Detected xmpmeta version: ${XMPMETA_VERSION} from "
    "${XMPMETA_VERSION_FILE}")
endmacro()
