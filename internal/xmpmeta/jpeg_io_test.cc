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

#include "xmpmeta/jpeg_io.h"

#include <fstream>
#include <string>
#include <vector>

#include "base/port.h"
#include "glog/logging.h"
#include "xmpmeta/file.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/test_xmp_creator.h"

namespace xmpmeta {
namespace {

// Test file paths.
const char* kJpegTestDataPath = "left_with_xmp.jpg";

// XMP standard test data.
const char* kXmpBody =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:GImage=\"http://ns.google.com/photos/1.0/image/\"\n"
    "      xmlns:xmpNote=\"http://ns.adobe.com/xmp/note/\"\n"
    "      GImage:Mime=\"image/jpeg\"\n"
    "      xmpNote:HasExtendedXMP=\"123ABC\"/>\n"
    "  </rdf:RDF>\n"
    "</x:xmpmeta>\n";

const char* kXmpMalformedBody =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:GImage=\"http://ns.google.com/photos/1.0/image/\"\n"
    "      xmlns:xmpNote=\"http://ns.adobe.com/xmp/note/\"\n"
    "      GImage:Mime=\"image/jpeg\"\n"
    "      xmpNote:HasExtendedXMP=\"123ABC\"/>\n"
    "</x:xmpmeta>\n";

// XMP extension test data.
const char* kXmpExtensionHeaderPart2 = "123ABCxxxxxxxx";
const char* kXmpExtensionBody =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:GImage=\"http://ns.google.com/photos/1.0/image/\"\n"
    "      GImage:Data=\"9865\"/>\n"
    "  </rdf:RDF>\n"
    "</x:xmpmeta>\n";

TEST(JpegIO, ParseStandardXmp) {
  const string filename = TempFileAbsolutePath("test.jpg");
  std::vector<string> standard_xmp;
  standard_xmp.push_back(TestXmpCreator::CreateStandardXmpString(kXmpBody));
  TestXmpCreator::WriteJPEGFile(filename, standard_xmp);

  std::ifstream file(filename.c_str(), std::ios::binary);
  ASSERT_TRUE(file.is_open());
  const std::vector<Section> sections = Parse(&file, true, "");
  EXPECT_FALSE(sections.empty());
  for (const auto& section : sections) {
    EXPECT_FALSE(section.is_image_section);
  }
}

TEST(JpegIO, ParseMalformedStandardXmp) {
  const string filename = TempFileAbsolutePath("test.jpg");
  std::vector<string> standard_xmp;
  standard_xmp.push_back(
      TestXmpCreator::CreateStandardXmpString(kXmpMalformedBody));
  TestXmpCreator::WriteJPEGFile(filename, standard_xmp);

  std::ifstream file(filename.c_str(), std::ios::binary);
  ASSERT_TRUE(file.is_open());
  const std::vector<Section> sections = Parse(&file, true, "");
  EXPECT_FALSE(sections.empty());
  for (const auto& section : sections) {
    EXPECT_FALSE(section.is_image_section);
  }
}

TEST(JpegIO, ParseExtendedXmp) {
  const string filename = TempFileAbsolutePath("test.jpg");
  std::vector<string> xmp_sections =
      TestXmpCreator::CreateExtensionXmpStrings(2, kXmpExtensionHeaderPart2,
                                                kXmpExtensionBody);
  xmp_sections.insert(xmp_sections.begin(),
                      TestXmpCreator::CreateStandardXmpString(kXmpBody));
  TestXmpCreator::WriteJPEGFile(filename, xmp_sections);

  std::ifstream file(filename.c_str(), std::ios::binary);
  ASSERT_TRUE(file.is_open());
  const std::vector<Section> sections = Parse(&file, true, "");
  EXPECT_FALSE(sections.empty());
  for (const auto& section : sections) {
    EXPECT_FALSE(section.is_image_section);
  }
}

TEST(JpegIO, ParseJpegWithExtendedXmpReadMetaOnly) {
  const string filename = TestFileAbsolutePath(kJpegTestDataPath);
  std::ifstream file(filename.c_str(), std::ios::binary);

  ASSERT_TRUE(file.is_open());
  const std::vector<Section> sections = Parse(&file, true, "");
  EXPECT_FALSE(sections.empty());
  for (const auto& section : sections) {
    EXPECT_FALSE(section.is_image_section);
  }
}

TEST(JpegIO, ParseJpegWithExtendedXmpReadEverything) {
  const string filename = TestFileAbsolutePath(kJpegTestDataPath);
  std::ifstream file(filename.c_str(), std::ios::binary);

  ASSERT_TRUE(file.is_open());
  const std::vector<Section> sections = Parse(&file, false, "");
  EXPECT_FALSE(sections.empty());
  for (int i = 0; i < sections.size() - 1; i++) {
    EXPECT_FALSE(sections.at(i).is_image_section);
  }
  EXPECT_TRUE(sections.at(sections.size() - 1).is_image_section);
}

}  // namespace
}  // namespace xmpmeta
