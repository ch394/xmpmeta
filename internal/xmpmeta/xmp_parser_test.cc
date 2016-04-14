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

#include "xmpmeta/xmp_parser.h"

#include <string>
#include <vector>

#include "base/port.h"
#include "gtest/gtest.h"
#include "xmpmeta/test_util.h"
#include "xmpmeta/test_xmp_creator.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/utils.h"

using xmpmeta::xml::DeserializerImpl;
using xmpmeta::xml::GetFirstDescriptionElement;
using xmpmeta::xml::XmlConst;

namespace xmpmeta {
namespace {

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

// XMP alternatively formatted test data.
const char* kAltXmpBody =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:GImage=\"http://ns.google.com/photos/1.0/image/\"\n"
    "      xmlns:xmpNote=\"http://ns.adobe.com/xmp/note/\">\n"
    "      <GImage:Mime>image/jpeg</GImage:Mime>\n"
    "      <xmpNote:HasExtendedXMP>123ABC</xmpNote:HasExtendedXMP>\n"
    "    </rdf:Description>"
    "  </rdf:RDF>\n"
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

const char* kXmpMalformedBody =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:GImage=\"http://ns.google.com/photos/1.0/image/\"\n"
    "      xmlns:xmpNote=\"http://ns.adobe.com/xmp/note/\"\n"
    "      GImage:Mime=\"image/jpeg\"\n"
    "      xmpNote:HasExtendedXMP=\"123ABC\"/>\n"
    "</x:xmpmeta>\n";

const char* kXmpBodyWithTrueBooleanCamelCase =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:Prefix=\"http://www.prefix.com/prefix/\"\n"
    "      Prefix:Property=\"TrUe\"/>\n"
    "  </rdf:RDF>\n"
    "</x:xmpmeta>\n";

const char* kXmpBodyWithTrueBooleanUpperCase =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:Prefix=\"http://www.prefix.com/prefix/\"\n"
    "      Prefix:Property=\"TRUE\"/>\n"
    "  </rdf:RDF>\n"
    "</x:xmpmeta>\n";

const char* kXmpBodyWithTrueBooleanLowerCase =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:Prefix=\"http://www.prefix.com/prefix/\"\n"
    "      Prefix:Property=\"true\"/>\n"
    "  </rdf:RDF>\n"
    "</x:xmpmeta>\n";

const char* kXmpBodyWithFalseBooleanCamelCase =
    "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"Adobe XMP\">\n"
    "  <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
    "    <rdf:Description rdf:about=\"\"\n"
    "      xmlns:Prefix=\"http://www.prefix.com/prefix/\"\n"
    "      Prefix:Property=\"fALsE\"/>\n"
    "  </rdf:RDF>\n"
    "</x:xmpmeta>\n";

TEST(XmpParser, ReadValidStandardXmp) {
  const string filename = TempFileAbsolutePath("test.jpg");
  std::vector<string> standard_xmp;
  standard_xmp.push_back(TestXmpCreator::CreateStandardXmpString(kXmpBody));
  TestXmpCreator::WriteJPEGFile(filename, standard_xmp);

  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(filename, true, &xmp_data));

  string value;
  xmlNodePtr node = GetFirstDescriptionElement(xmp_data.StandardSection());
  DeserializerImpl deserializer(XmlConst::RdfDescription(), node);
  ASSERT_TRUE(deserializer.ParseString("GImage", "Mime", &value));
  ASSERT_EQ(string("image/jpeg"), value);
}

TEST(XmpParser, ReadValidStandardXmpFromMemoryStream) {
  std::stringstream mem_stream;
  std::vector<string> standard_xmp;
  standard_xmp.push_back(TestXmpCreator::CreateStandardXmpString(kXmpBody));
  mem_stream << TestXmpCreator::MakeJPEGFileContents(standard_xmp);
  mem_stream.seekg(0, mem_stream.beg);

  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(&mem_stream, true, &xmp_data));

  string value;
  xmlNodePtr node = GetFirstDescriptionElement(xmp_data.StandardSection());
  DeserializerImpl deserializer(XmlConst::RdfDescription(), node);
  ASSERT_TRUE(deserializer.ParseString("GImage", "Mime", &value));
  ASSERT_EQ(string("image/jpeg"), value);
}

TEST(XmpParser, ReadValidAltXmpFromMemoryStream) {
  std::stringstream mem_stream;
  std::vector<string> standard_xmp;
  standard_xmp.push_back(TestXmpCreator::CreateStandardXmpString(kAltXmpBody));
  mem_stream << TestXmpCreator::MakeJPEGFileContents(standard_xmp);
  mem_stream.seekg(0, mem_stream.beg);

  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(&mem_stream, true, &xmp_data));

  string value;
  xmlNodePtr std_node = GetFirstDescriptionElement(xmp_data.StandardSection());
  DeserializerImpl deserializer(XmlConst::RdfDescription(), std_node);
  ASSERT_TRUE(deserializer.ParseString("GImage", "Mime", &value));
  ASSERT_EQ(string("image/jpeg"), value);
}

TEST(XmpParser, ReadMalformedStandardXmp) {
  const string filename = TempFileAbsolutePath("test.jpg");
  std::vector<string> standard_xmp;
  standard_xmp.push_back(
      TestXmpCreator::CreateStandardXmpString(kXmpMalformedBody));
  TestXmpCreator::WriteJPEGFile(filename, standard_xmp);

  XmpData xmp_data;
  ASSERT_FALSE(ReadXmpHeader(filename, true, &xmp_data));
}

TEST(XmpParser, ReadBooleanProperty) {
  const string filename = TempFileAbsolutePath("test.jpg");
  std::vector<string> standard_xmp;
  standard_xmp.push_back(
      TestXmpCreator::CreateStandardXmpString(
          kXmpBodyWithTrueBooleanCamelCase));
  TestXmpCreator::WriteJPEGFile(filename, standard_xmp);

  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(filename, true, &xmp_data));
  bool value;
  string prefix("Prefix");
  string property("Property");
  xmlNodePtr node = GetFirstDescriptionElement(xmp_data.StandardSection());
  // Read camel case.
  DeserializerImpl camel_case_deserializer(XmlConst::RdfDescription(),
                                           GetFirstDescriptionElement(
                                               xmp_data.StandardSection()));
  ASSERT_TRUE(camel_case_deserializer.ParseBoolean(prefix, property, &value));
  ASSERT_TRUE(value);

  // Read uppercase.
  standard_xmp.clear();
  standard_xmp.push_back(
      TestXmpCreator::CreateStandardXmpString(
          kXmpBodyWithTrueBooleanUpperCase));
  TestXmpCreator::WriteJPEGFile(filename, standard_xmp);

  ASSERT_TRUE(ReadXmpHeader(filename, true, &xmp_data));
  node = GetFirstDescriptionElement(xmp_data.StandardSection());
  DeserializerImpl uppercase_deserializer(XmlConst::RdfDescription(),
                                          GetFirstDescriptionElement(
                                              xmp_data.StandardSection()));
  ASSERT_TRUE(uppercase_deserializer.ParseBoolean(prefix, property, &value));
  ASSERT_TRUE(value);

  // Read lowercase.
  standard_xmp.clear();
  standard_xmp.push_back(
      TestXmpCreator::CreateStandardXmpString(
          kXmpBodyWithTrueBooleanLowerCase));
  TestXmpCreator::WriteJPEGFile(filename, standard_xmp);

  ASSERT_TRUE(ReadXmpHeader(filename, true, &xmp_data));
  node = GetFirstDescriptionElement(xmp_data.StandardSection());
  DeserializerImpl lowercase_deserializer(XmlConst::RdfDescription(),
                                          GetFirstDescriptionElement(
                                              xmp_data.StandardSection()));
  ASSERT_TRUE(lowercase_deserializer.ParseBoolean(prefix, property, &value));
  ASSERT_TRUE(value);

  // Read a false value.
  standard_xmp.clear();
  standard_xmp.push_back(
      TestXmpCreator::CreateStandardXmpString(
          kXmpBodyWithFalseBooleanCamelCase));
  TestXmpCreator::WriteJPEGFile(filename, standard_xmp);

  ASSERT_TRUE(ReadXmpHeader(filename, true, &xmp_data));
  node = GetFirstDescriptionElement(xmp_data.StandardSection());

  DeserializerImpl value_deserializer(XmlConst::RdfDescription(), node);
  ASSERT_TRUE(value_deserializer.ParseBoolean(prefix, property, &value));
  ASSERT_FALSE(value);
}

TEST(XmpParser, ReadInvalidStandardXmp) {
  const string filename = TempFileAbsolutePath("test.jpg");
  TestXmpCreator::WriteJPEGFile(filename, {"<x\\<<>q"});

  XmpData xmp_data;
  EXPECT_FALSE(ReadXmpHeader(filename, true, &xmp_data));
}

TEST(XmpParser, ReadExtendedXmp) {
  const string filename = TempFileAbsolutePath("test.jpg");
  std::vector<string> xmp_sections =
      TestXmpCreator::CreateExtensionXmpStrings(2, kXmpExtensionHeaderPart2,
                                                kXmpExtensionBody);
  xmp_sections.insert(xmp_sections.begin(),
                      TestXmpCreator::CreateStandardXmpString(kXmpBody));
  TestXmpCreator::WriteJPEGFile(filename, xmp_sections);

  XmpData xmp_data;
  ASSERT_TRUE(ReadXmpHeader(filename, false, &xmp_data));

  string value;
  DeserializerImpl deserializer(XmlConst::RdfDescription(),
                                GetFirstDescriptionElement(
                                    xmp_data.ExtendedSection()));
  ASSERT_TRUE(deserializer.ParseString("GImage", "Data", &value));
  ASSERT_EQ(string("9865"), value);
}

}  // namespace
}  // namespace xmpmeta
