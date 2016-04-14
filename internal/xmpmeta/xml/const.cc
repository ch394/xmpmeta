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

#include "xmpmeta/xml/const.h"

namespace xmpmeta {
namespace xml {

const char* XmlConst::EncodingStr() {
  return "UTF-8";
}

// RDF metadata constants.
const char* XmlConst::RdfAbout() {
  return "about";
}

const char* XmlConst::RdfDescription() {
  return "Description";
}

const char* XmlConst::RdfNodeName() {
  return "RDF";
}

const char* XmlConst::RdfNodeNs() {
  return "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
}

const char* XmlConst::RdfPrefix() {
  return "rdf";
}

const char* XmlConst::RdfSeq() {
  return "Seq";
}

const char* XmlConst::RdfLi() {
  return "li";
}

// XML metadata constants.
const char* XmlConst::NsAttrName() {
  return "xmlns";
}

const char* XmlConst::Separator() {
  return ":";
}

const char* XmlConst::Version() {
  return "1.0";
}

}  // namespace xml
}  // namespace xmpmeta
