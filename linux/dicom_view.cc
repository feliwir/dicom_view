#include "include/dicom_view/dicom_view.h"
#include "../cpp/dicom_view_common.h"
#include "include/dicom_view/texture_gl.h"

struct _DicomView {
  GObject parent_instance;
  FlTextureRegistrar *texture_registrar;
  TextureGL *texture;
  DicomViewCommon *common;
  gint64 id;
};

G_DEFINE_TYPE(DicomView, dicom_view, G_TYPE_OBJECT)

static gint64 dicom_view_id = 0;

// Forward declares
FlMethodResponse *dicom_view_get_texture_id_fl(DicomView *view);
FlMethodResponse *dicom_view_set_dicom_file_fl(DicomView *view,
                                               const gchar *file_path);

static void dicom_view_class_init(DicomViewClass *klass) {}

static void dicom_view_init(DicomView *self) {
  self->common = dicom_view_common_new();
  self->texture = texture_gl_new();
  texture_gl_set_dicom_common(self->texture, self->common);
}

// Called when a method call is received from Flutter.
static void dicom_view_handle_method_call(DicomView *self,
                                          FlMethodCall *method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar *method = fl_method_call_get_name(method_call);
  g_message("dicom_view_handle_method_call: %s", method);

  FlValue *args = fl_method_call_get_args(method_call);

  if (strcmp(method, "getTextureId") == 0) {
    response = dicom_view_get_texture_id_fl(self);
  } else if (strcmp(method, "setDicomFile") == 0) {
    const gchar *file_path =
        fl_value_get_string(fl_value_lookup_string(args, "file"));
    g_message("dicom_view_handle_method_call: setFile: %s", file_path);
    response = dicom_view_set_dicom_file_fl(self, file_path);
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

static void method_call_cb(FlMethodChannel *channel, FlMethodCall *method_call,
                           gpointer user_data) {
  DicomView *view = DICOM_VIEW(user_data);
  dicom_view_handle_method_call(view, method_call);
}

DicomView *dicom_view_new(FlTextureRegistrar *texture_registrar,
                          FlPluginRegistrar *plugin_registrar) {
  g_message("dicom_view_new");
  DicomView *self = DICOM_VIEW(g_object_new(DICOM_VIEW_TYPE, NULL));
  self->id = dicom_view_id++;
  self->texture_registrar = texture_registrar;
  fl_texture_registrar_register_texture(self->texture_registrar,
                                        FL_TEXTURE(self->texture));

  fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                    FL_TEXTURE(self->texture));

  // Method channel for this view
  gchar *channel_str = g_strdup_printf("dicom_view_%ld", self->id);
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(plugin_registrar),
                            channel_str, FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(self), g_object_unref);

  return self;
}

gint64 dicom_view_get_id(DicomView *self) {
  g_message("dicom_view_get_id");
  return (gint64)self->id;
}

gint64 dicom_view_get_texture_id(DicomView *self) {
  g_message("dicom_view_get_texture_id: %lu", (gint64)self->texture);
  return (gint64)self->texture;
}

gboolean dicom_view_set_dicom_path(DicomView *self, const gchar *dicom_file) {
  g_message("dicom_view_set_dicom_path: %s", dicom_file);
  int result = dicom_view_common_load_file(self->common, dicom_file);
  return result == 0;
}

FlMethodResponse *dicom_view_get_texture_id_fl(DicomView *self) {
  FlValue *result = fl_value_new_int(dicom_view_get_texture_id(self));
  return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

FlMethodResponse *dicom_view_set_dicom_file_fl(DicomView *self,
                                               const gchar *dicom_file) {
  g_autoptr(FlValue) result =
      fl_value_new_bool(dicom_view_set_dicom_path(self, dicom_file));
  return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}