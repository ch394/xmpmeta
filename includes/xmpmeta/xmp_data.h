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

#ifndef XMPMETA_XMP_DATA_H_
#define XMPMETA_XMP_DATA_H_

#include <libxml/tree.h>

namespace xmpmeta {

// XmpData contains the standard, and optionally extended, XMP metadata from a
// JPEG file. See xmp_parser for reading XmpData from a JPEG or reading
// attributes from XmpData.
class XmpData {
 public:
  XmpData();
  ~XmpData();

  // Frees any allocated resources and resets the xmlDocPtrs to null.
  void Reset();

  // The standard XMP section.
  const xmlDocPtr StandardSection() const;
  xmlDocPtr* MutableStandardSection();

  // The extended XMP section.
  const xmlDocPtr ExtendedSection() const;
  xmlDocPtr* MutableExtendedSection();

 private:
  xmlDocPtr xmp_;
  xmlDocPtr xmp_extended_;
};

}  // namespace xmpmeta

#endif  // XMPMETA_XMP_DATA_H_
