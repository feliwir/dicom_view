#include "dicom_view_common.h"
#include "dicom_view_render_gl.h"
// GLM
#include <glm/glm.hpp>
// OpenGL
#include <epoxy/gl.h>
#include <flutter_linux/flutter_linux.h>
// GDCM
#include <gdcmImageReader.h>

struct _DicomViewCommon {
  // View parameters
  glm::vec2 view_offset = glm::vec2(0.0f, 0.0f);
  glm::vec2 view_scale = glm::vec2(1.0f, 1.0f);

  // Image parameters
  glm::ivec2 img_size = glm::ivec2(0, 0);
  glm::dvec3 img_spacing = glm::dvec3(1.0, 1.0, 1.0);
  int img_min = 0;
  int img_max = 0;

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
  handle->img_size.x = image.GetDimension(0);
  handle->img_size.y = image.GetDimension(1);
  // Spacing for image voxels
  handle->img_spacing.x = image.GetSpacing(0);
  handle->img_spacing.y = image.GetSpacing(1);
  handle->img_spacing.z = image.GetSpacing(2);
  // Min-Max for level window
  auto &format = image.GetPixelFormat();
  handle->img_min = format.GetMin();
  handle->img_max = format.GetMax();

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
  dicom_view_render_gl_draw(handle->program, handle->name,
                            glm::ivec2(width, height), handle->img_size,
                            handle->img_spacing, handle->view_offset,
                            handle->view_scale);

  return 0;
}