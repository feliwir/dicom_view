#pragma once
#include <gdcmImage.h>
#include <glm/glm.hpp>

int dicom_view_render_gl_upload(unsigned int name, gdcm::Image &image);

int dicom_view_render_gl_create_program(unsigned int *program);

int dicom_view_render_gl_draw(unsigned int program, unsigned int name,
                              const glm::ivec2 &vp_size,
                              const glm::ivec2 &img_size,
                              const glm::dvec3 &img_spacing,
                              const glm::vec2 &view_offset,
                              const glm::vec2 &view_scale);