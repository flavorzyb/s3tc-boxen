
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>

#include "defines.h"


#define _READ_INT { \
  c = (unsigned char *)&i; \
  fscanf(f, "%c%c%c%c", &c[0], &c[1], &c[2], &c[3]); \
}

#define _READ_WORD { \
  c = (unsigned char *)&i; \
  fscanf(f, "%c%c", &c[0], &c[1]); \
  c[2] = c[3] = 0; \
}


int bmp_load(char *name, int *dx, int *dy, int *bpp, unsigned char **o) {

  unsigned char *c, tmp[4];
  int i, os;
  FILE *f;


  if (name == NULL || dx == NULL || dy == NULL || bpp == NULL || o == NULL)
    return FAILED;

  *o = NULL;
  *bpp = GL_RGB;

  f = fopen(name, "rb");
  if (f == NULL) {
    fprintf(stderr, "BMP_LOAD: Could not open file \"%s\" for reading.\n", name);
    return FAILED;
  }

  fscanf(f, "%c%c", &tmp[0], &tmp[1]);
  if (tmp[0] != 'B' || tmp[1] != 'M') {
    fprintf(stderr, "BMP_LOAD: File \"%s\" is not a BMP image.\n", name);
    fclose(f);
    return FAILED;
  }

  /* skip until the offset to image data */
  fscanf(f, "%*c%*c%*c%*c%*c%*c%*c%*c");

  /* offset to image data */
  _READ_INT;
  os = i - (2+8+4) - (3*4+2*2);

  /* skip header size */
  fscanf(f, "%*c%*c%*c%*c");

  /* width */
  _READ_INT;
  *dx = i;

  /* height */
  _READ_INT;
  *dy = i;

  /* skip planes */
  fscanf(f, "%*c%*c");

  /* bits per pixel */
  _READ_WORD;
  if (i != 24) {
    fprintf(stderr, "BMP_LOAD: Only 24bit images are supported. File \"%s\" has %d bits per pixel.\n", name, i);
    fclose(f);
    return FAILED;
  }

  /* skip to the image data */
  while (os > 0) {
    fscanf(f, "%*c");
    os--;
  }

  *o = malloc((*dx)*(*dy)*3);
  if (*o == NULL) {
    fprintf(stderr, "BMP_LOAD: Out of memory error.\n");
    fclose(f);
    return FAILED;
  }

  /* read the data */
  for (i = 0; i < (*dx)*(*dy)*3; i += 3) {
    /* read BGR */
    fscanf(f, "%c%c%c", &tmp[0], &tmp[1], &tmp[2]);
    (*o)[i+0] = tmp[2];
    (*o)[i+1] = tmp[1];
    (*o)[i+2] = tmp[0];
  }

  fclose(f);

  return SUCCEEDED;
}
