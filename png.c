
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <png.h>

#include "defines.h"



int png_load(char *name, int *dx, int *dy, int *type, unsigned char **o) {

  png_structp png_ptr;
  png_infop info_ptr, end_info;
  png_bytep *row_pointers;
  png_uint_32 width, height;
  int i, depth, colortype, bpp;
  FILE *f;


  if (name == NULL || dx == NULL || dy == NULL || type == NULL || o == NULL)
    return FAILED;

  *o = NULL;

  f = fopen(name, "rb");
  if (f == NULL) {
    fprintf(stderr, "PNG_LOAD: Can't open file \"%s\"\n", name);
    return FAILED;
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(f);
    return FAILED;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    fclose(f);
    return FAILED;
  }

  end_info = png_create_info_struct(png_ptr);
  if (end_info == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(f);
    return FAILED;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(f);
    return FAILED;
  }

  png_init_io(png_ptr, f);
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &depth, &colortype, NULL, NULL, NULL);

  /* set transformations to ARGB */
  if ((colortype == PNG_COLOR_TYPE_PALETTE && depth <= 8) ||
      (colortype == PNG_COLOR_TYPE_GRAY && depth < 8) ||
      (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)))
    png_set_expand(png_ptr);

  if (colortype == PNG_COLOR_TYPE_GRAY) {
    png_set_gray_to_rgb(png_ptr);
    colortype = PNG_COLOR_TYPE_RGB;
  }

  if (depth == 16)
    png_set_strip_16(png_ptr);

  /* update transformations */
  png_read_update_info(png_ptr, info_ptr);

  /* fill in texture information */
  *dx = width;
  *dy = height;
  if (colortype == PNG_COLOR_TYPE_RGB) {
    *type = GL_RGB;
    bpp = 3;
  }
  else {
    *type = GL_RGBA;
    bpp = 4;
  }

  *o = malloc(bpp*width*height);
  if (*o == NULL) {
    fprintf(stderr, "PNG_LOAD: Out of memory error.\n");
    fclose(f);
    return FAILED;
  }

  /* allocate row pointers and fill them in */
  row_pointers = malloc(height*sizeof(png_bytep));
  if (row_pointers == NULL) {
    fprintf(stderr, "PNG_LOAD: Out of memory error.\n");
    fclose(f);
    free(*o);
    return FAILED;
  }

  for (i = 0; i < height; i++)
    row_pointers[i] = *o + i*width*bpp;

  /* read the image */
  png_read_image(png_ptr, row_pointers);

  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  free(row_pointers);

  fclose(f);

  return SUCCEEDED;
}
