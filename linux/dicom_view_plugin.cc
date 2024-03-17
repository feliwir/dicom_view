#include "include/dicom_view/dicom_view_plugin.h"
#include "include/dicom_view/dicom_view.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>

#include "dicom_view_plugin_private.h"

#define DICOM_VIEW_PLUGIN(obj)                                                 \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), dicom_view_plugin_get_type(),             \
                              DicomViewPlugin))

struct _DicomViewPlugin {
  GObject parent_instance;
  FlTextureRegistrar *texture_registrar;
  FlPluginRegistrar *plugin_registrar;
};

G_DEFINE_TYPE(DicomViewPlugin, dicom_view_plugin, g_object_get_type())

// Called when a method call is received from Flutter.
static void dicom_view_plugin_handle_method_call(DicomViewPlugin *self,
                                                 FlMethodCall *method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar *method = fl_method_call_get_name(method_call);
  g_message("dicom_view_plugin_handle_method_call: %s", method);

  if (strcmp(method, "getPlatformVersion") == 0) {
    response = get_platform_version();
  } else if (strcmp(method, "createView") == 0) {
    response = create_view(self->texture_registrar, self->plugin_registrar);
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

FlMethodResponse *get_platform_version() {
  struct utsname uname_data = {};
  uname(&uname_data);
  g_autofree gchar *version = g_strdup_printf("Linux %s", uname_data.version);
  g_autoptr(FlValue) result = fl_value_new_string(version);
  return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

FlMethodResponse *create_view(FlTextureRegistrar *texture_registrar,
                              FlPluginRegistrar *plugin_registrar) {
  DicomView *view = dicom_view_new(texture_registrar, plugin_registrar);

  g_autoptr(FlValue) result = fl_value_new_int(dicom_view_get_id(view));
  return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

static void dicom_view_plugin_dispose(GObject *object) {
  G_OBJECT_CLASS(dicom_view_plugin_parent_class)->dispose(object);
}

static void dicom_view_plugin_class_init(DicomViewPluginClass *klass) {
  G_OBJECT_CLASS(klass)->dispose = dicom_view_plugin_dispose;
}

static void dicom_view_plugin_init(DicomViewPlugin *self) {}

static void method_call_cb(FlMethodChannel *channel, FlMethodCall *method_call,
                           gpointer user_data) {
  DicomViewPlugin *plugin = DICOM_VIEW_PLUGIN(user_data);
  dicom_view_plugin_handle_method_call(plugin, method_call);
}

void dicom_view_plugin_register_with_registrar(FlPluginRegistrar *registrar) {
  g_message("dicom_view_plugin_register_with_registrar");
  DicomViewPlugin *plugin =
      DICOM_VIEW_PLUGIN(g_object_new(dicom_view_plugin_get_type(), nullptr));

  FlTextureRegistrar *texture_registrar =
      fl_plugin_registrar_get_texture_registrar(registrar);
  plugin->texture_registrar = texture_registrar;
  plugin->plugin_registrar = registrar;
  g_object_ref(registrar);

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "dicom_view", FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(
      channel, method_call_cb, g_object_ref(plugin), g_object_unref);

  g_object_unref(plugin);
}
