
#ifndef TEXTURE_GL_H_
#define TEXTURE_GL_H_

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#define TEXTURE_GL_TYPE (texture_gl_get_type())

G_DECLARE_FINAL_TYPE(TextureGL, texture_gl, DICOM_VIEW, TEXTURE_GL, FlTextureGL)

#define TEXTURE_GL(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), texture_gl_get_type(), TextureGL))

TextureGL *texture_gl_new();

/**
 * @brief Populates texture with DICOM image.
 */
gboolean texture_gl_populate(FlTextureGL *texture,
                             guint32 *target,
                             guint32 *name,
                             guint32 *width,
                             guint32 *height,
                             GError **error);
G_END_DECLS

#endif