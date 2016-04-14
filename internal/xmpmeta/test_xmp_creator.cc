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

#include "test_xmp_creator.h"

#include <sstream>

#include "xmpmeta/file.h"

namespace xmpmeta {
namespace {
// XMP standard test data.
const char* kXmpHeader = "http://ns.adobe.com/xap/1.0/";

// XMP extension test data.
const char* kXmpExtensionHeaderPart1 = "http://ns.adobe.com/xmp/extension/";

// JPEG boilerplate.
const char* kJPEGFileStart = "\xff\xd8";
const char* kJPEGSectionStart = "\xff\xe1";
const char* kJPEGRemainder =
    "\xff\xdaJpegPixelData\xff\xd9";  // SOS, data, EOI.

}  // namespace

string TestXmpCreator::CreateStandardXmpString(string xmp_body) {
  string xmp_string = kXmpHeader;
  xmp_string.push_back(0);
  xmp_string.append(xmp_body);
  return xmp_string;
}

std::vector<string>
TestXmpCreator::CreateExtensionXmpStrings(const int num_sections,
                                          const char* extension_header_part_2,
                                          const char* extension_body) {
  string body = extension_body;
  const int length = body.size();
  std::vector<string> sections;
  for (int i = 0; i < num_sections; ++i) {
    const int start = length * i / num_sections;
    const int end = length * (i + 1) / num_sections;
    string xmp_string = kXmpExtensionHeaderPart1;
    xmp_string.push_back(0);
    xmp_string.append(extension_header_part_2);
    xmp_string.append(body.substr(start, end - start));
    sections.emplace_back(xmp_string);
  }
  return sections;
}

string TestXmpCreator::GetFakeJpegPayload() {
  return kJPEGRemainder;
}

string TestXmpCreator::MakeJPEGFileContents(
    const std::vector<string>& xmp_sections) {
  // Inefficient way to construct a string, but not important here. Just being
  // careful to avoid string construction options that don't play nicely with
  // \0s in the middle.
  string file_contents = kJPEGFileStart;
  for (const string& section : xmp_sections) {
    const int length = section.size() + 2;
    const char lh = static_cast<char>(length >> 8);
    const char ll = static_cast<char>(length & 0xff);
    file_contents.append(kJPEGSectionStart);
    file_contents.push_back(lh);
    file_contents.push_back(ll);
    file_contents.append(section);
  }
  file_contents.append(kJPEGRemainder);
  return file_contents;
}

void TestXmpCreator::WriteJPEGFile(const string& filename,
                                   const std::vector<string>& xmp_sections) {
  string file_contents = MakeJPEGFileContents(xmp_sections);
  WriteStringToFileOrDie(file_contents, filename);
}

}  // namespace xmpmeta
