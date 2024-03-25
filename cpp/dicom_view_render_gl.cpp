#include "dicom_view_render_gl.h"
// GLM
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
// OpenGL
#include <epoxy/gl.h>
#include <flutter_linux/flutter_linux.h>

int dicom_view_render_gl_upload(unsigned int name, gdcm::Image &image) {
  auto width = image.GetDimension(0);
  auto height = image.GetDimension(1);
  g_message("dicom_view_render_gl_upload: %d %d %d", name, width, height);
  glBindTexture(GL_TEXTURE_2D, name);
  char *buffer = new char[image.GetBufferLength()];
  if (image.GetBuffer(buffer)) {
    auto &format = image.GetPixelFormat();
    g_message("scalartype: %d bitsAllocated: %i bitsStored: %i",
              format.GetScalarType(), format.GetBitsAllocated(),
              format.GetBitsStored());
    switch (format.GetScalarType()) {
    case gdcm::PixelFormat::INT8:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                   GL_UNSIGNED_BYTE, buffer);
      break;
    case gdcm::PixelFormat::UINT8:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                   GL_UNSIGNED_BYTE, buffer);
      break;
    case gdcm::PixelFormat::INT16:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                   GL_UNSIGNED_SHORT, buffer);
      break;
    case gdcm::PixelFormat::UINT16:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                   GL_UNSIGNED_SHORT, buffer);
      break;
    case gdcm::PixelFormat::INT32:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                   GL_UNSIGNED_INT, buffer);
      break;
    case gdcm::PixelFormat::UINT32:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                   GL_UNSIGNED_INT, buffer);
      break;
    case gdcm::PixelFormat::FLOAT32:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT,
                   buffer);
      break;
    case gdcm::PixelFormat::FLOAT64:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                   GL_DOUBLE, buffer);
    default:
      g_message("Unsupported pixel format");
      return 1;
    }
  } else
    return 1;
  return 0;
}

int dicom_view_render_gl_create_program(unsigned int *program) {
  if (program == nullptr)
    return 1;
  if (*program != 0)
    return 0;

  // Disable depth test and culling
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDisable(GL_CULL_FACE);

  const char *vertex_shader_source = R"(
    #version 100
    attribute vec2 position;
    attribute vec2 texcoord;
    varying vec2 v_texcoord;
    uniform mat4 u_mvp;
    void main() {
      gl_Position = u_mvp * vec4(position, 0.0, 1.0);
      v_texcoord = texcoord;
    }
  )";
  const char *fragment_shader_source = R"(
    #version 100
    precision mediump float;
    varying vec2 v_texcoord;
    uniform sampler2D u_tex;
    void main() {
      gl_FragColor = vec4(texture2D(u_tex, v_texcoord).rrr * 10.0, 1.0);
    }
  )";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  // Check if the shader linked successfully
  GLint compiled;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint info_len = 0;
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &info_len);
    if (info_len > 1) {
      char *info_log = (char *)malloc(sizeof(char) * info_len);
      glGetShaderInfoLog(vertex_shader, info_len, NULL, info_log);
      g_message("Error compiling vertex shader: %s", info_log);
      free(info_log);
    }
    glDeleteShader(vertex_shader);
    return 1;
  }

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  // Check if the shader linked successfully
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint info_len = 0;
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
    if (info_len > 1) {
      char *info_log = (char *)malloc(sizeof(char) * info_len);
      glGetShaderInfoLog(fragment_shader, info_len, NULL, info_log);
      g_message("Error compiling fragment shader: %s", info_log);
      free(info_log);
    }
    glDeleteShader(fragment_shader);
    return 1;
  }

  *program = glCreateProgram();
  glAttachShader(*program, vertex_shader);
  glAttachShader(*program, fragment_shader);
  glLinkProgram(*program);

  // Check if the program linked successfully
  GLint linked;
  glGetProgramiv(*program, GL_LINK_STATUS, &linked);
  if (!linked) {
    GLint info_len = 0;
    glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &info_len);
    if (info_len > 1) {
      char *info_log = (char *)malloc(sizeof(char) * info_len);
      glGetProgramInfoLog(*program, info_len, NULL, info_log);
      g_message("Error linking program: %s", info_log);
      free(info_log);
    }
    glDeleteProgram(*program);
    *program = 0;
    return 1;
  }

  return 0;
}

int dicom_view_render_gl_draw(unsigned int program, unsigned int name,
                              const glm::ivec2 &vp_size,
                              const glm::ivec2 &img_size,
                              const glm::dvec3 &img_spacing,
                              const glm::vec2 &view_offset,
                              const glm::vec2 &view_scale) {
  glViewport(0, 0, vp_size.x, vp_size.y);
  // Use our program
  glUseProgram(program);

  GLuint position_location = glGetAttribLocation(program, "position");
  GLuint texcoord_location = glGetAttribLocation(program, "texcoord");
  GLuint tex_location = glGetUniformLocation(program, "u_tex");
  GLuint mvp_location = glGetUniformLocation(program, "u_mvp");

  auto img_size_with_spacing = glm::vec2(img_size) * glm::vec2(img_spacing);
  g_message(
      "img_size: %d %d img_spacing: %f %f %f img_size_with_spacing: %f %f",
      img_size.x, img_size.y, img_spacing.x, img_spacing.y, img_spacing.z,
      img_size_with_spacing.x, img_size_with_spacing.y);

  // Set up the MVP matrix
  auto projection =
      glm::ortho(0.0f, (float)vp_size.x, (float)vp_size.y, 0.0f, -1.0f,
                 1.0f); // Inverted Y axis
  // Apply the view offset and scale
  auto model = glm::mat4(1.0f);
  // Center the image
  // model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));
  // Apply the image spacing
  model =
      glm::scale(model, glm::vec3(img_spacing.x, img_spacing.y, img_spacing.z));
  // Apply the view offset
  model = glm::translate(model, glm::vec3(view_offset.x, view_offset.y, 0.0f));
  // Apply the view scale
  model = glm::scale(model, glm::vec3(view_scale.x, view_scale.y, 1.0f));
  // Apply the image size
  model = glm::scale(model, glm::vec3(img_size_with_spacing, 1.0f));
  // Apply the ratio of viewport & image size
  auto fit_scale = glm::compMin(glm::vec2(vp_size) / glm::vec2(img_size));
  model = glm::scale(model, glm::vec3(glm::vec2(fit_scale), 1.0f));
  // Identity view
  auto view = glm::mat4(1.0f);

  auto mvp = projection * view * model;

  // 2D Rect
  GLfloat vertices[] = {
      // Positions    // Texture Coords
      1.0f, 1.0f, 1.0f, 1.0f, // Top Right
      1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
      0.0f, 0.0f, 0.0f, 0.0f, // Bottom Left
      0.0f, 1.0f, 0.0f, 1.0f, // Top Left
  };

  GLuint indices[] = {
      0, 1, 2, 0, 2, 3,
  };

  GLuint vbo, ebo;
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE,
                        4 * sizeof(GLfloat), (GLvoid *)0);
  glEnableVertexAttribArray(position_location);

  glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE,
                        4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(texcoord_location);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, name);

  glUniform1i(tex_location, 0);
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  g_message("dicom_view_render_gl_draw: %d %d %d %d %d %d %f %f", program, name,
            vp_size.x, vp_size.y, img_size.x, img_size.y,
            img_size_with_spacing.x, img_size_with_spacing.y);

  return 0;
}