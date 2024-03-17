#include "include/dicom_view/dicom_view_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "dicom_view_plugin.h"

void DicomViewPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  dicom_view::DicomViewPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
