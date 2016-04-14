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


// Serialization implementation for XML nodes.
#ifndef XMPMETA_XML_SERIALIZER_IMPL_H_
#define XMPMETA_XML_SERIALIZER_IMPL_H_

#include <string>
#include <unordered_map>

#include <libxml/tree.h>

#include "xmpmeta/xml/serializer.h"

namespace xmpmeta {
namespace xml {

// Writes properties, lists, and child nodes into an XML structure.
//
// Usage example:
//  std::unordered_map<string, xmlNsPtr> namespaces;
//  std::unordered_map<string, xmlNsPtr> prefixes;
//  string device_name("Device");
//  string cameras_name("Cameras");
//  string camera_name("Camera");
//  string audio_name("Audio");
//  string image_name("Image");
//  PopulateNamespaces(&namespaces);
//  PopulatePrefixes(&prefixes);
//  DoSerialization();
//
//  // Serialization example.
//  void DoSerialization() {
//    xmlNodePtr device_node = xmlNewNode(nullptr, device_name);
//    Serializer device_serializer(namespaces, prefixes,
//                                 device_name, device_node);
//
//    std::unique_ptr<Serializer> cameras_serializer =
//        serializer->CreateListSerializer(cameras_name);
//    for (XdmCamera *camera : camera_list_) {
//      std::unique_ptr<Serializer> camera_serializer =
//          cameras_serializer->CreateItemSerializer(camera_name);
//      success &= camera->Serialize(camera_serializer.get());
//
//      // Serialize Audio.
//      std::unique_ptr<Serializer> audio_serializer =
//          camera_serializer->CreateSerializer(audio_name);
//      audio_serializer->WriteProperty("Data", audio_data);
//      audio_serializer->WriteProperty("Mime", "audio/mp4");
//
//      // Serialize Image.
//      std::unique_ptr<Serializer> image_serializer =
//          camera_serializer->CreateSerializer(image_name);
//      image_serializer->WriteProperty("Data", image_data);
//      image_serializer->WriteProperty("Mime", "image/jpeg");
//
//      // Serialize ImagingModel.
//      std::unique_ptr<Serializer> imaging_model_serializer =
//          camera_serializer->CreateSerializer("ImagingModel", true);
//      std::unique_ptr<Serializer> equirect_model_serializer =
//          imaging_model_serializer->CreateSerializer("EquirectModel");
//      // Serializer equirect model fields here.
//    }
//  }
//
// Resulting XML structure:
// /*
//  * <Device>
//  *   <Device:Cameras>
//  *     <rdf:Seq>
//  *       <rdf:li>
//  *         <Device:Camera>
//  *             <Camera:Audio Audio:Mime="audio/mp4" Audio:Data="DataValue"/>
//  *             <Camera:Image Image:Mime="image/jpeg" Image:Data="DataValue"/>
//  *             <Camera:ImagingModel>
//  *               <EquirectModel ...properties/>
//  *             </Camera:ImagingModel>
//  *         </Device:Camera>
//  *       </rdf:li>
//  *     </rdf:Seq>
//  *   </Device:Cameras>
//  * </Device>
//  */
//
// // Namespace population example.
// void PopulateNamespaces(std::unordered_map<string, xmlNsPtr>* namespaces) {
//   xmlNsPtr device_ns =
//       xmlNewNs(nullptr, ToXmlChar("http://ns.xdm.org/photos/1.0/device")
//                ToXmlChar(device_name.data()));
//   xmlNsPtr camera_ns =
//       xmlNewNs(nullptr, ToXmlChar("http://ns.xdm.org/photos/1.0/camera")
//                ToXmlChar(camera_name.data()));
//   xmlNsPtr audio_ns =
//       xmlNewNs(nullptr, ToXmlChar("http://ns.xdm.org/photos/1.0/audio")
//                ToXmlChar(audio_name.data()));
//   xmlNsPtr image_ns =
//       xmlNewNs(nullptr, ToXmlChar("http://ns.xdm.org/photos/1.0/image")
//                ToXmlChar(image_name.data()));
//   namespaces->insert(device_name, device_ns);
//   namespaces->insert(camera_name, camera_ns);
//   namespaces->insert(audio_name, audio_ns);
//   namespaces->insert(image_name, image_ns);
// }
//
// // Prefix population example.
// void PopulatePrefixes(std::unordered_map<string, xmlNsPtr>* prefixes) {
//   xmlNsPtr device_prefix_ns =
//       xmlNewNs(nullptr, nullptr, ToXmlChar(device_name.data()));
//   xmlNsPtr camera_prefix_ns =
//       xmlNewNs(nullptr, nullptr, ToXmlChar(camera_name.data()));
//   prefixes->insert(device_name, device_prefix_ns);
//   prefixes->insert(camera_name, camera_prefix_ns);
// }

class SerializerImpl : public Serializer {
 public:
  // Constructor.
  // The prefix map is required if one of the CreateSerializer methods will be
  // called on this object. In particular, the RDF namespace must be present in
  // the prefix map if CreateItemSerializer or CreateListSerializer will be
  // called.
  // The namespaces map serves to keep XML namespace creation out of this
  // Serializer, to simplify memory management issues. Note that the libxml
  // xmlDocPtr will own all namespace and node pointers.
  // The prefixes map allows new child XML nodes to be created on the parent
  // namespace, and to be printed in the format ParentNodeName:ChildNodeName.
  // The parent namespace from the namespaces map cannot be used for child node
  // creation, beacuse it cauess the child node to lose the prefix. The use of
  // the prefixes map for node creation is purely for XML document formatting
  // and clarity purposes.
  // @param namespaces A map of node names to full namespaces.
  // @param prefixes A map of node names to their prefix namespaces.
  // @param node_prefix The name of the calling node.
  // @param node The caller node. This will be the node in which serialization
  // takes place in WriteProperties.
  SerializerImpl(const std::unordered_map<string, xmlNsPtr>& namespaces,
                 const std::unordered_map<string, xmlNsPtr>& prefixes,
                 const string& node_name, xmlNodePtr node);

  // Returns a new Serializer for an object that is part of an rdf:Seq list
  // of objects.
  // The parent serializer must be created with CreateListSerializer.
  std::unique_ptr<Serializer>
      CreateItemSerializer(const string& item_name) const override;

  // Returns a new Serializer for a list of objects that correspond to an
  // rdf:Seq XML node, where each object is to be serialized as a child node of
  // every rdf:li node in the list.
  // The serializer is created on an rdf:Seq node, which is the child of a
  // newly created XML node with the name list_name.
  std::unique_ptr<Serializer>
      CreateListSerializer(const string& list_name) const override;

  // Creates a serializer from the current serializer.
  // @param node_name The name of the caller node. This will be the parent of
  // any new nodes or properties set by this serializer.
  std::unique_ptr<Serializer>
      CreateSerializer(const string& node_name) const override;

  // Writes the property into the current node, prefixed with the
  // current node's name.
  bool WriteBoolProperty(const string& name, bool value) const override;
  bool WriteProperty(const string& name, const string& value) const override;

  // Writes the property into the current node, prefixed with prefix if it
  // has a corresponding namespace href in namespaces_, fails otherwise.
  // Returns true if serialization is successful.
  bool WritePropertyWithPrefix(const string& prefix, const string& name,
                               const string& value) const override;

  // Writes the collection of numbers into a child rdf:Seq node.
  bool WriteIntArray(const string& array_name,
                     const std::vector<int>& values) const override;
  bool WriteDoubleArray(const string& array_name,
                        const std::vector<double>& values) const override;

  // Class-specific methods.
  // Constructs a serializer object and writes the xmlNsPtr objects in
  // namespaces_ to node_.
  static std::unique_ptr<SerializerImpl> FromDataAndSerializeNamespaces(
      const std::unordered_map<string, xmlNsPtr>& namespaces,
      const std::unordered_map<string, xmlNsPtr>& prefixes,
      const string& node_name, xmlNodePtr node);

  // Disallow copying.
  SerializerImpl(const SerializerImpl&) = delete;
  void operator=(const SerializerImpl&) = delete;

 private:
  // Creates a child serializer. Underlying implementation for both of the
  // public CreateSerializer methods.
  std::unique_ptr<Serializer>
      CreateSerializer(const string& node_name, bool has_prefix) const;

  // Writes the xmlNsPtr objects in namespaces_ to node_.
  // Modifies namespaces_ by setting each xmlNsPtr's next pointer to the
  // subsequent entry in the collection.
  bool SerializeNamespaces();

  string node_name_;
  xmlNodePtr node_;
  std::unordered_map<string, xmlNsPtr> namespaces_;
  std::unordered_map<string, xmlNsPtr> prefixes_;
};

}  // namespace xml
}  // namespace xmpmeta

#endif  // XMPMETA_XML_SERIALIZER_IMPL_H_
