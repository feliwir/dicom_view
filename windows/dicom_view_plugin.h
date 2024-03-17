#ifndef FLUTTER_PLUGIN_DICOM_VIEW_PLUGIN_H_
#define FLUTTER_PLUGIN_DICOM_VIEW_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace dicom_view {

class DicomViewPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  DicomViewPlugin();

  virtual ~DicomViewPlugin();

  // Disallow copy and assign.
  DicomViewPlugin(const DicomViewPlugin&) = delete;
  DicomViewPlugin& operator=(const DicomViewPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace dicom_view

#endif  // FLUTTER_PLUGIN_DICOM_VIEW_PLUGIN_H_
