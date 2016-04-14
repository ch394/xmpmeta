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

#include "xmpmeta/xmp_const.h"

namespace xmpmeta {

// XMP namespace constants.
const char* XmpConst::Namespace() {
  return "adobe:ns:meta/";
}

const char* XmpConst::NamespacePrefix() {
  return "x";
}

const char* XmpConst::NodeName() {
  return "xmpmeta";
}

const char* XmpConst::AdobePropName() {
  return "xmptk";
}

const char* XmpConst::AdobePropValue() {
  return "Adobe XMP";
}

const char* XmpConst::NoteNamespace() {
  return "http://ns.adobe.com/xmp/note/";
}

// XMP headers.
const char* XmpConst::Header() {
  return "http://ns.adobe.com/xap/1.0/";
}

const char* XmpConst::ExtensionHeader() {
  return "http://ns.adobe.com/xmp/extension/";
}

const char* XmpConst::HasExtensionPrefix() {
  return "xmpNote";
}

const char* XmpConst::HasExtension() {
  return "HasExtendedXMP";
}

// Sizes.
const int XmpConst::ExtensionHeaderOffset() {
  return 8;
}

const int XmpConst::MaxBufferSize() {
  return 65502;
}

const int XmpConst::ExtendedMaxBufferSize() {
  return 65458;
}

}  // namespace xmpmeta
