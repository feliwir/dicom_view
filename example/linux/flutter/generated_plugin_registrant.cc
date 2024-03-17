//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <dicom_view/dicom_view_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) dicom_view_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "DicomViewPlugin");
  dicom_view_plugin_register_with_registrar(dicom_view_registrar);
}
