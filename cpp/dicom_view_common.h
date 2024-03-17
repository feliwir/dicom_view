#pragma once

#if __cplusplus
extern "C"
{
#endif

    struct _DicomViewCommon;
    typedef struct _DicomViewCommon DicomViewCommon;

    DicomViewCommon *dicom_view_common_handle_new();
    int dicom_view_common_dispose(DicomViewCommon *handle);

    int dicom_view_common_load_file(DicomViewCommon *handle, const char *file_path);

    int dicom_view_common_upload_gl_texture(DicomViewCommon *handle, unsigned int *target, unsigned int *name, unsigned int *width, unsigned int *height);

#if __cplusplus
}
#endif
