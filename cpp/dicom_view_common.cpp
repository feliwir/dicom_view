#include "dicom_view_common.h"
#include "dicom_view_render_gl.h"
#include <epoxy/gl.h>
#include <flutter_linux/flutter_linux.h>
#include <gdcmImageReader.h>

struct _DicomViewCommon {
  // OpenGL
  unsigned int name = 0;
  unsigned int program = 0;
};

DicomViewCommon *dicom_view_common_new() { return new DicomViewCommon(); }

int dicom_view_common_dispose(DicomViewCommon *handle) {
  if (handle->name != 0) {
    glDeleteTextures(1, &handle->name);
    handle->name = 0;
  }
  delete handle;
  return 0;
}

int dicom_view_common_load_file(DicomViewCommon *handle,
                                const char *file_path) {
  gdcm::ImageReader reader;
  reader.SetFileName(file_path);
  if (!reader.Read()) {
    return 1;
  }

  // Load the image
  gdcm::Image &image = reader.GetImage();

  // OpenGL
  glGenTextures(1, &handle->name);
  glBindTexture(GL_TEXTURE_2D, handle->name);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  dicom_view_render_gl_upload(handle->name, image);

  return 0;
}

int dicom_view_common_render_gl(DicomViewCommon *handle, unsigned int width,
                                unsigned int height) {
  if (handle->name == 0) {
    return 1;
  }

  // Create our program
  dicom_view_render_gl_create_program(&handle->program);
  // Render our image
  dicom_view_render_gl_draw(handle->program, handle->name, width, height);

  return 0;
}