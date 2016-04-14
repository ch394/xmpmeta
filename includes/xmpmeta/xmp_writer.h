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

#ifndef XMPMETA_XMP_WRITER_H_
#define XMPMETA_XMP_WRITER_H_

#include <iostream>
#include <string>
#include <memory>

#include "base/port.h"
#include "xmpmeta/xmp_data.h"

namespace xmpmeta {

// Creates a new XmpData object and initializes the boilerplate for the
// standard XMP section.
// The extended section is initialized only if create_extended is true.
std::unique_ptr<XmpData> CreateXmpData(bool create_extended);

// Writes  XMP data to an existing JPEG image file.
// This is equivalent to writeXMPMeta in geo/lightfield/metadata/XmpUtil.java.
// If the extended section is not null, this will modify the given XmpData by
// setting a property in the standard section that links it with the
// extended section.
bool WriteLeftEyeAndXmpMeta(const string& left_data, const string& filename,
                            const XmpData& xmp_data);

// Updates a JPEG input stream with new XMP data and writes it to an
// output stream.
// This is equivalent to writeXMPMeta in geo/lightfield/metadata/XmpUtil.java.
bool AddXmpMetaToJpegStream(std::istream* input_jpeg_stream,
                            const XmpData& xmp_data,
                            std::ostream* output_jpeg_stream);

}  // namespace xmpmeta

#endif  // XMPMETA_XMP_WRITER_H_
