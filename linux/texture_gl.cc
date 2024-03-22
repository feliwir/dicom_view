#include "include/dicom_view/texture_gl.h"
#include "../cpp/dicom_view_common.h"
#include <epoxy/gl.h>

struct _TextureGL {
  FlTextureGL parent_instance;
  guint32 target;
  guint32 fbo;
  guint32 name;
  guint32 width;
  guint32 height;
  DicomViewCommon *common;
};

G_DEFINE_TYPE(TextureGL, texture_gl, fl_texture_gl_get_type())

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  g_message("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
            severity, message);
}

static void texture_gl_init(TextureGL *self) {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(MessageCallback, 0);

  // Initialize self
  self->name = 0;
  self->width = 0;
  self->height = 0;
  self->common = NULL;
}

static void texture_gl_dispose(GObject *object) {
  TextureGL *self = TEXTURE_GL(object);
  if (self->name != 0) {
    glDeleteTextures(1, &self->name);
    self->name = 0;
  }
  self->width = 0;
  self->height = 0;
  self->common = NULL;

  G_OBJECT_CLASS(texture_gl_parent_class)->dispose(object);
}

static void texture_gl_class_init(TextureGLClass *klass) {
  FL_TEXTURE_GL_CLASS(klass)->populate = texture_gl_populate;
  G_OBJECT_CLASS(klass)->dispose = texture_gl_dispose;
}

TextureGL *texture_gl_new() {
  g_message("texture_gl_new");

  TextureGL *self = TEXTURE_GL(g_object_new(TEXTURE_GL_TYPE, NULL));
  return self;
}

void texture_gl_set_dicom_common(TextureGL *self, DicomViewCommon *common) {
  g_message("texture_gl_set_dicom_common");
  self->common = common;
}

gboolean texture_gl_populate(FlTextureGL *texture, guint32 *target,
                             guint32 *name, guint32 *width, guint32 *height,
                             GError **error) {
  g_message("texture_gl_populate: %d %d %d %d", *target, *name, *width,
            *height);
  TextureGL *self = TEXTURE_GL(texture);

  self->width = *width;
  self->height = *height;

  // Recreate texture & FBI
  if (self->name == 0 && self->fbo == 0) {
    // Create FBO
    glGenFramebuffers(1, &self->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, self->fbo);

    // Create texture
    glGenTextures(1, &self->name);
    glBindTexture(GL_TEXTURE_2D, self->name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, self->width, self->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Attach texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           self->name, 0);
  } else {
    // Rebind texture & FBO
    glBindTexture(GL_TEXTURE_2D, self->name);
    glBindFramebuffer(GL_FRAMEBUFFER, self->fbo);
  }

  if (self->common) {
    // Populate texture with DICOM image
    g_message("dicom_view_common_render_gl");
    int result =
        dicom_view_common_render_gl(self->common, self->width, self->height);
    if (result != 0) {
      g_message("failed to render texture");
    }
  }

  *target = GL_TEXTURE_2D;
  *name = self->name;
  *width = self->width;
  *height = self->height;

  return TRUE;
}
