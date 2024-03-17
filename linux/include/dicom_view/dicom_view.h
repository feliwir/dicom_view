#ifndef FLUTTER_PLUGIN_DICOM_VIEW_H_
#define FLUTTER_PLUGIN_DICOM_VIEW_H_

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#define DICOM_VIEW_TYPE (dicom_view_get_type())
G_DECLARE_FINAL_TYPE(DicomView, dicom_view, DICOM_VIEW, DICOM_VIEW, GObject)

DicomView *dicom_view_new(FlTextureRegistrar *texture_registrar,
                          FlPluginRegistrar *plugin_registrar);

gint64 dicom_view_get_texture_id(DicomView *self);
gint64 dicom_view_get_id(DicomView *self);
gboolean dicom_view_set_dicom_path(DicomView *self, const gchar *dicom_file);

#define DICOM_VIEW(obj)                                                        \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), dicom_view_get_type(), DicomView))

G_END_DECLS

#endif // FLUTTER_PLUGIN_DICOM_VIEW_H_