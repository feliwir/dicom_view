#pragma once

#if __cplusplus
extern "C" {
#endif

struct _DicomViewCommon;
typedef struct _DicomViewCommon DicomViewCommon;

DicomViewCommon *dicom_view_common_new();
int dicom_view_common_dispose(DicomViewCommon *handle);

int dicom_view_common_load_file(DicomViewCommon *handle, const char *file_path);

enum DicomViewCommonUpdateEvent {
  DICOM_VIEW_COMMON_UPDATE_EVENT_IMAGE = 1,
};

int dicom_view_common_set_update_callback(DicomViewCommon *handle,
                                          void (*update_cb)(int, void *),
                                          void *user_data);

// OpenGL
int dicom_view_common_render_gl(DicomViewCommon *handle, unsigned int width,
                                unsigned int height);

#if __cplusplus
}
#endif
