#include "dicom_view_common.h"
#include <gdcmImageReader.h>
#include <epoxy/gl.h>

struct _DicomViewCommon {
  gdcm::ImageReader reader;
};

DicomViewCommon* dicom_view_common_new() {
  return new DicomViewCommon();
}


int dicom_view_common_dispose(DicomViewCommon* handle) {
  delete handle;
  return 0;
}

int dicom_view_common_load_file(DicomViewCommon* handle, const char* file_path) {
  handle->reader.SetFileName(file_path);
  if (!handle->reader.Read()) {
    return 1;
  }
  return 0;
}

int dicom_view_common_upload_gl_texture(DicomViewCommon* handle, unsigned int* target, unsigned int* name, unsigned int* width, unsigned int* height) {
  gdcm::Image& image = handle->reader.GetImage();
  *width = image.GetDimension(0);
  *height = image.GetDimension(1);
  *target = GL_TEXTURE_2D;
  glGenTextures(1, name);
  glBindTexture(GL_TEXTURE_2D, *name);
  char* buffer = new char[image.GetBufferLength()];
  if(image.GetBuffer(buffer)) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, *width, *height, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else
  {
    return 1;
  }
  return 0;
}