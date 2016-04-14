// xdmlib. A fast XDM parsing and writing library.
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

#ifndef XMPMETA_XDM_DEVICE_H_
#define XMPMETA_XDM_DEVICE_H_

#include <libxml/tree.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "xdmlib/cameras.h"
#include "xdmlib/device_pose.h"
#include "xdmlib/profiles.h"
#include "xmpmeta/xmp_data.h"

namespace xmpmeta {
namespace xdm {

// Implements a Device from the XDM specification, with serialization and
// deserialization.
// Does not implement the Element interface because Device is at the top level
// in the XDM tree.
class Device {
 public:
  ~Device();

  // Creates this object from the given XDM elements.
  static std::unique_ptr<Device> FromData(
      const string& revision,
      std::unique_ptr<DevicePose> device_pose,
      std::unique_ptr<Profiles> profiles,
      std::unique_ptr<Cameras> cameras);

  // Creates a Device from pre-extracted XMP metadata. Returns null if
  // parsing fails. Both the standard and extended XMP sections are required.
  static std::unique_ptr<Device> FromXmp(const XmpData& xmp);

  // Creates a Device by extracting XMP metadata from a JPEG and parsing it.
  // If using XMP for other things as well, FromXmp() should be used instead to
  // prevent redundant extraction of XMP from the JPEG.
  static std::unique_ptr<Device> FromJpegFile(const string& filename);

  // Getters.
  const string& GetRevision() const;
  const Cameras* GetCameras() const;
  const DevicePose* GetDevicePose() const;
  const Profiles* GetProfiles() const;

  // Not const for XML memory management reasons. More info in source comments.
  bool SerializeToXmp(XmpData* xmp);

  // Disallow copying.
  Device(const Device&) = delete;
  void operator=(const Device&) = delete;

 private:
  Device();

  // Retrieves the namespaces of all child elements.
  void GetNamespaces(
      std::unordered_map<string, string>* ns_name_href_map) const;
  // Parses Device fields and XDM children elements.
  bool ParseFields(const XmpData& xmp);
  // Gathers all the namespaces and prefixes of child elements.
  void PopulateNamespacesAndPrefixes();

  // Keep a reference to the namespaces, so they can be freed on destruction.
  std::unordered_map<string, xmlNsPtr> namespaces_;
  std::unordered_map<string, xmlNsPtr> prefixes_;

  // XDM fields and elements.
  string revision_;
  std::unique_ptr<DevicePose> device_pose_;
  std::unique_ptr<Profiles> profiles_;
  std::unique_ptr<Cameras> cameras_;
};

}  // namespace xdm
}  // namespace xmpmeta

#endif  // XMPMETA_XDM_DEVICE_H_
