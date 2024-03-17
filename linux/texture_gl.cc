#include "include/dicom_view/texture_gl.h"

#include <epoxy/gl.h>

struct _TextureGL {
  FlTextureGL parent_instance;
  guint32 target;
  guint32 name;
  guint32 width;
  guint32 height;
};

G_DEFINE_TYPE(TextureGL, texture_gl, fl_texture_gl_get_type())

static void texture_gl_init(TextureGL *self) {
  // Initialize self
  self->name = 0;
  self->width = 0;
  self->height = 0;
}

static void texture_gl_dispose(GObject *object) {
  TextureGL* self = TEXTURE_GL(object);
  if (self->name != 0) {
    glDeleteTextures(1, &self->name);
    self->name = 0;
  }
  self->width = 0;
  self->height = 0;

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

gboolean texture_gl_populate(FlTextureGL *texture, guint32 *target,
                             guint32 *name, guint32 *width, guint32 *height,
                             GError **error) {
  g_message("texture_gl_populate: %d %d %d %d", *target, *name, *width, *height);
  TextureGL* self = TEXTURE_GL(texture);

  if (self->name == 0) {
    glGenTextures(1, &self->name);
    glBindTexture(GL_TEXTURE_2D, self->name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    guint8 pixel[] = {0, 0, 0, 128};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1,
          0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    self->width = 1;
    self->height = 1;
  }

  *target = GL_TEXTURE_2D;
  *name = self->name;
  *width = self->width;
  *height = self->height;

  // Populate texture with DICOM image
  return TRUE;
}
