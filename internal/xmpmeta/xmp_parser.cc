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

#include <algorithm>
#include <cstring>
#include <stack>

#include "glog/logging.h"
#include "strings/case.h"
#include "strings/numbers.h"
#include "strings/util.h"
#include "xmpmeta/base64.h"
#include "xmpmeta/jpeg_io.h"
#include "xmpmeta/xmp_const.h"
#include "xmpmeta/xml/const.h"
#include "xmpmeta/xml/deserializer_impl.h"
#include "xmpmeta/xml/search.h"
#include "xmpmeta/xml/utils.h"

using std::pair;
using std::stack;
using xmpmeta::xml::DepthFirstSearch;
using xmpmeta::xml::DeserializerImpl;
using xmpmeta::xml::FromXmlChar;
using xmpmeta::xml::GetFirstDescriptionElement;
using xmpmeta::xml::XmlConst;

namespace xmpmeta {
namespace {

const char kJpgExtension[] = "jpg";
const char kJpegExtension[] = "jpeg";

bool BoolStringToBool(const string& bool_str, bool* value) {
  if (StringCaseEqual(bool_str, "true")) {
    *value = true;
    return true;
  }
  if (StringCaseEqual(bool_str, "false")) {
    *value = false;
    return true;
  }
  return false;
}

// Converts string_property to the type T.
template <typename T>
bool ConvertStringPropertyToType(const string& string_property, T* value);

// Gets the end of the XMP meta content. If there is no packet wrapper, returns
// data.length, otherwise returns 1 + the position of last '>' without '?'
// before it. Usually the packet wrapper end is "<?xpacket end="w"?>.
int GetXmpContentEnd(const string& data) {
  for (int i = static_cast<int>(data.size() - 1); i >= 1; --i) {
    if (data[i] == '>') {
      if (data[i - 1] != '?') {
        return i + 1;
      }
    }
  }
  // It should not reach here for a valid XMP meta.
  return static_cast<int>(data.size());
}

// Parses the first valid XMP section. Any other valid XMP section will be
// ignored.
bool ParseFirstValidXMPSection(const std::vector<Section>& sections,
                               XmpData* xmp) {
  for (const Section& section : sections) {
    if (HasPrefixString(section.data, XmpConst::Header())) {
      const int end = GetXmpContentEnd(section.data);
      // Increment header length by 1 for the null termination.
      const int header_length =
          static_cast<int>(strlen(XmpConst::Header()) + 1);
      const int content_length = static_cast<int>(end - header_length);
      if (content_length <= 0) {
        LOG(ERROR) << "Invalid content length: " << content_length;
        return false;
      }
      const char* content_start = &section.data[header_length];
      *xmp->MutableStandardSection() = xmlReadMemory(
          content_start, content_length, nullptr, nullptr, 0);
      if (xmp->StandardSection() == nullptr) {
        LOG(WARNING) << "Failed to parse standard section.";
        return false;
      }
      return true;
    }
  }
  return false;
}

// Collects the extended XMP sections with the given name into a string. Other
// sections will be ignored.
string GetExtendedXmpSections(const std::vector<Section>& sections,
                              const string& section_name) {
  string extended_header = XmpConst::ExtensionHeader();
  extended_header += '\0' + section_name;
  const size_t section_start_offset =
      extended_header.size() + XmpConst::ExtensionHeaderOffset();

  // Compute the size of the buffer to parse the extended sections.
  std::vector<const Section*> xmp_sections;
  std::vector<size_t> xmp_end_offsets;
  size_t buffer_size = 0;
  for (const Section& section : sections) {
    if (extended_header.empty() ||
        HasPrefixString(section.data, extended_header)) {
      const size_t end_offset = section.data.size();
      const size_t section_size = end_offset - section_start_offset;
      if (end_offset < section_start_offset ||
          section_size > SIZE_MAX - buffer_size) {
        return "";
      }
      buffer_size += section_size;
      xmp_sections.push_back(&section);
      xmp_end_offsets.push_back(end_offset);
    }
  }

  // Copy all the relevant sections' data into a buffer.
  string buffer(buffer_size, '\0');
  if (buffer.size() != buffer_size) {
    return "";
  }
  size_t offset = 0;
  for (int i = 0; i < xmp_sections.size(); ++i) {
    const Section* section = xmp_sections[i];
    const size_t length = xmp_end_offsets[i] - section_start_offset;
    std::copy_n(&section->data[section_start_offset], length, &buffer[offset]);
    offset += length;
  }
  return buffer;
}

// Parses the extended XMP sections with the given name. All other sections
// will be ignored.
bool ParseExtendedXmpSections(const std::vector<Section>& sections,
                              const string& section_name, XmpData* xmp_data) {
  const string extended_sections =
      GetExtendedXmpSections(sections, section_name);
  *xmp_data->MutableExtendedSection() =
      xmlReadMemory(extended_sections.data(),
                    static_cast<int>(extended_sections.size()), nullptr,
                    nullptr, XML_PARSE_HUGE);
  if (xmp_data->ExtendedSection() == nullptr) {
    LOG(WARNING) << "Failed to parse extended sections.";
    return false;
  }
  return true;
}

// Extracts a XmpData from a JPEG image stream.
bool ExtractXmpMeta(const bool skip_extended, std::istream* file,
                    XmpData* xmp_data) {
  CHECK_NOTNULL(xmp_data)->Reset();

  const string header(skip_extended ? XmpConst::Header() : "");
  const std::vector<Section> sections = Parse(file, true, header);
  if (sections.empty()) {
    LOG(WARNING) << "No sections found.";
    return false;
  }

  if (!ParseFirstValidXMPSection(sections, xmp_data)) {
    LOG(WARNING) << "Could not parse first section.";
    return false;
  }
  if (skip_extended) {
    return true;
  }
  string extension_name;
  DeserializerImpl deserializer(XmlConst::RdfDescription(),
                                GetFirstDescriptionElement(
                                    xmp_data->StandardSection()));
  if (!deserializer.ParseString(XmpConst::HasExtensionPrefix(),
                                XmpConst::HasExtension(), &extension_name) ||
      !ParseExtendedXmpSections(sections, extension_name, xmp_data)) {
    LOG(WARNING) << "Could not parse extended sections.";
    return false;
  }
  return true;
}

// Extracts the specified string attribute.
bool GetStringProperty(const xmlNodePtr node, const char* prefix,
                       const char* property, string* value) {
  const xmlDocPtr doc = node->doc;
  for (const _xmlAttr* attribute = node->properties;
       attribute != nullptr; attribute = attribute->next) {
    if (attribute->ns
        && strcmp(FromXmlChar(attribute->ns->prefix), prefix) == 0
        && strcmp(FromXmlChar(attribute->name), property) == 0) {
      xmlChar* attribute_string =
          xmlNodeListGetString(doc, attribute->children, 1);
      *value = FromXmlChar(attribute_string);
      xmlFree(attribute_string);
      return true;
    }
  }
  LOG(WARNING) << "Could not find string attribute: " << property;
  return false;
}

// Reads the contents of a node.
// E.g. <prefix:node_name>Contents Here</prefix:node_name>
bool ReadNodeContent(const xmlNodePtr node, const char* prefix,
                     const char* node_name, string* value) {
  auto* element = DepthFirstSearch(node, node_name);
  if (element == nullptr) {
    return false;
  }
  if (prefix != nullptr &&
      (element->ns == nullptr ||
       element->ns->prefix == nullptr ||
       strcmp(FromXmlChar(element->ns->prefix), prefix) != 0)) {
    return false;
  }
  xmlChar* node_content = xmlNodeGetContent(element);
  *value = FromXmlChar(node_content);
  free(node_content);
  return true;
}



template <typename T>
bool ConvertStringPropertyToType(const string& string_property, T* value) {
  QCHECK(value) << "Cannot call this method on a generic type";
  return false;
}

template <>
bool ConvertStringPropertyToType<bool>(const string& string_property,
                                       bool* value) {
  return BoolStringToBool(string_property, value);
}

template <>
bool ConvertStringPropertyToType<double>(const string& string_property,
                                         double* value) {
  *value = std::stod(string_property);
  return true;
}

template <>
bool ConvertStringPropertyToType<int>(const string& string_property,
                                      int* value) {
  return SimpleAtoi(string_property, value);
}

template <>
bool ConvertStringPropertyToType<int64>(const string& string_property,
                                       int64* value) {
  *value = std::stol(string_property);
  return true;
}

}  // namespace

bool ReadXmpHeader(const string& filename, const bool skip_extended,
                   XmpData* xmp_data) {
  const string& lower_filename = strings::ToLower(filename);
  if (!HasSuffixString(lower_filename, kJpgExtension) &&
      !HasSuffixString(lower_filename, kJpegExtension)) {
    LOG(WARNING) << "XMP parse: only JPEG file is supported";
    return false;
  }

  std::ifstream file(filename.c_str(), std::ios::binary);
  if (!file.is_open()) {
    LOG(WARNING) << " Could not read file: " << filename;
    return false;
  }
  return ExtractXmpMeta(skip_extended, &file, xmp_data);
}

bool ReadXmpHeader(std::istream* input_stream, bool skip_extended,
                   XmpData* xmp_data) {
  return ExtractXmpMeta(skip_extended, input_stream, xmp_data);
}

}  // namespace xmpmeta
