#include "dicom_view_common.h"
#include "dicom_view_render_gl.h"
// GLM
#include <glm/glm.hpp>
// OpenGL
#include <epoxy/gl.h>
#include <flutter_linux/flutter_linux.h>
// GDCM
#include <gdcmAttribute.h>
#include <gdcmImageReader.h>

struct _DicomViewCommon {
  // View parameters
  glm::vec2 view_offset = glm::vec2(0.0f, 0.0f);
  glm::vec2 view_scale = glm::vec2(1.0f, 1.0f);
  float view_window_center = 0.0f;
  float view_window_width = 0.0f;

  // Image parameters
  glm::ivec2 img_size = glm::ivec2(0, 0);
  glm::dvec3 img_spacing = glm::dvec3(1.0, 1.0, 1.0);
  glm::ivec2 img_minmax = glm::ivec2(0, 255);
  glm::dvec2 img_scalebias = glm::dvec2(1.0, 0.0);

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
  // Load the DICOM file
  gdcm::ImageReader reader;
  reader.SetFileName(file_path);
  if (!reader.Read()) {
    return 1;
  }

  // Get the WindowWidth & WindowCenter tags
  gdcm::File &file = reader.GetFile();
  gdcm::DataSet &ds = file.GetDataSet();
  gdcm::Attribute<0x0028, 0x1050> ww;
  gdcm::Attribute<0x0028, 0x1051> wc;
  ww.SetFromDataSet(ds);
  wc.SetFromDataSet(ds);
  handle->view_window_center = wc.GetValue();
  handle->view_window_width = ww.GetValue();
  g_message("window center: %f window width: %f", handle->view_window_center,
            handle->view_window_width);

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
  handle->img_minmax.x = format.GetMin();
  handle->img_minmax.y = format.GetMax();
  g_message("min max: %d %d", handle->img_minmax.x, handle->img_minmax.y);
  // Scale-bias
  handle->img_scalebias.x = image.GetSlope();
  handle->img_scalebias.y = image.GetIntercept();
  g_message("slope: %f intercept: %f", handle->img_scalebias.x,
            handle->img_scalebias.y);

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
  dicom_view_render_gl_draw(
      handle->program, handle->name, glm::ivec2(width, height),
      handle->img_size, handle->img_spacing, handle->img_scalebias,
      handle->view_offset, handle->view_scale, handle->view_window_center,
      handle->view_window_width);

  return 0;
}