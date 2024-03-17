#include <flutter_linux/flutter_linux.h>

#include "include/dicom_view/dicom_view_plugin.h"

// Handles the getPlatformVersion method call.
FlMethodResponse *get_platform_version();

// Handles the createCiew method call.
FlMethodResponse *create_view(FlTextureRegistrar *texture_registrar,
                              FlPluginRegistrar *plugin_registrar);