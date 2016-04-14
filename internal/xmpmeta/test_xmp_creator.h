// xmpmeta. A fast XMP metadata parsing and writing library.
// Copyright 2016 Google Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of Google Inc. nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Author: miraleung@google.com (Mira Leung)

#ifndef XMPMETA_TEST_XMP_CREATOR_H_
#define XMPMETA_TEST_XMP_CREATOR_H_

#include <string>
#include <vector>

#include "base/port.h"

namespace xmpmeta {

// Creates XMP strings and files to aid testing.
class TestXmpCreator {
 public:
  // Combines the xmp header and body to form a string to be used in the jpeg.
  static string CreateStandardXmpString(string xmp_body);

  // Splits up the xmp extension body into multiple strings with headers, each
  // of which is to be used as a section in the jpeg.
  // extension_header_part_2 is the GUID of the extended section's contents.
  static std::vector<string>
      CreateExtensionXmpStrings(const int num_sections,
                                const char* extension_header_part_2,
                                const char* extension_body);

  // Returns a string containing fake JPEG data.
  static string GetFakeJpegPayload();

  // Writes XMP metadata into a new JPEG file.
  static void WriteJPEGFile(const string& filename,
                            const std::vector<string>& xmp_sections);

  // Returns the contents of a JPEG file with the given XMP metadata.
  static string MakeJPEGFileContents(const std::vector<string>& xmp_sections);
};

}  // namespace xmpmeta

#endif  // XMPMETA_TEST_XMP_CREATOR_H_
