
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "defines.h"
#include "s3tc.h"



int s3tc_convert_texture(unsigned char *d, int dx, int dy, int type, int level, unsigned char **o, int **size, int *otype, int build_mipmaps, int max_mipmap, int *number_mipmaps) {

  GLuint txtid;
  GLint comp;
  int ol, x, i, p;


  if (d == NULL || o == NULL || size == NULL)
    return FAILED;

  glGenTextures(1, &txtid);
  if (txtid == 0) {
    fprintf(stderr, "S3TC_CONVERT_TEXTURE: glGenTextures() failed!\n");
    return FAILED;
  }

  /* check that the level is ok */
  if (type == GL_RGB && level > 1) {
    fprintf(stderr, "S3TC_CONVERT_TEXTURE: Only DTX1 works for RGB textures. Using DTX1 instead of DTX%d.\n", level);
    level = 1;
  }

  /* convert level to opengl level */
  if (level == 1 && type == GL_RGB)
    ol = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
  else if (level == 1 && type == GL_RGBA)
    ol = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
  else if (level == 3)
    ol = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
  else if (level == 5)
    ol = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
  else {
    fprintf(stderr, "S3TC_CONVERT_TEXTURE: Level %d is out of range.\n", level);
    glDeleteTextures(1, &txtid);
    return FAILED;
  }

  /* compress it */
  glBindTexture(GL_TEXTURE_2D, txtid);

  if (build_mipmaps == NO)
    glTexImage2D(GL_TEXTURE_2D, 0, ol, dx, dy, 0, type, GL_UNSIGNED_BYTE, d);
  else
    gluBuild2DMipmaps(GL_TEXTURE_2D, ol, dx, dy, type, GL_UNSIGNED_BYTE, d);

  x = dx;
  i = 0;
  *o = NULL;
  *number_mipmaps = 1;
  p = 0;

  while (1) {
    /* check that the conversion worked */
    glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_ARB, &comp);
    if (comp == GL_FALSE) {
      fprintf(stderr, "S3TC_CONVERT_TEXTURE: Could not convert to DTX%d.\n", level);
      glDeleteTextures(1, &txtid);
      return FAILED;
    }

    /* check that the format is still the same */
    glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_INTERNAL_FORMAT, &comp);
    *otype = comp;

    if (comp != ol) {
      fprintf(stderr, "S3TC_CONVERT_TEXTURE: Didn't get what we wanted.\n");
      glDeleteTextures(1, &txtid);
      return FAILED;
    }

    /* fetch the compressed data size */
    *size = realloc(*size, sizeof(int)*(i+1));
    if (*size == NULL) {
      fprintf(stderr, "S3TC_CONVERT_TEXTURE: Out of memory error.\n");
      glDeleteTextures(1, &txtid);
      return FAILED;
    }

    glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, &comp);
    (*size)[i] = comp;

    *o = realloc(*o, p + (*size)[i]);
    if (*o == NULL) {
      fprintf(stderr, "S3TC_CONVERT_TEXTURE: Out of memory error.\n");
      glDeleteTextures(1, &txtid);
      return FAILED;
    }

    /* fetch the data */
    glGetCompressedTexImageARB(GL_TEXTURE_2D, i, *o + p);

    if (x == 1 || build_mipmaps == NO || max_mipmap == *number_mipmaps)
      break;

    x /= 2;
    p += (*size)[i];
    i++;
    (*number_mipmaps)++;
  }

  glDeleteTextures(1, &txtid);

  return SUCCEEDED;
}
