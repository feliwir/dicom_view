#pragma once
#include <gdcmImage.h>

int dicom_view_render_gl_upload(unsigned int name, gdcm::Image &image);

int dicom_view_render_gl_create_program(unsigned int *program);

int dicom_view_render_gl_draw(unsigned int program, unsigned int name,
                              unsigned int width, unsigned int height);