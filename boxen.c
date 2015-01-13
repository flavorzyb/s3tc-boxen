
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "boxen.h"



static void _write_word(FILE *f, int i) {

  fprintf(f, "%c", i & 0xff);
  fprintf(f, "%c", (i>>8) & 0xff);
}


static void _read_word(FILE *f, int *i) {

  char c, d;


  fscanf(f, "%c%c", &c, &d);
  *i = ((int)c) | (((int)d)<<8);
}


static void _write_int(FILE *f, int i) {

  fprintf(f, "%c", i & 0xff);
  fprintf(f, "%c", (i>>8) & 0xff);
  fprintf(f, "%c", (i>>16) & 0xff);
  fprintf(f, "%c", (i>>24) & 0xff);
}


static void _read_int(FILE *f, int *i) {

  char *c;
  int n;


  c = (char *)i;
  for (n = 0; n < sizeof(int); n++, c++)
    fscanf(f, "%c", c);
}


int boxen_write(char *name, unsigned char *d, int dx, int dy, int *size, int type, int number_mipmaps) {

  FILE *f;
  int i, n;


  if (name == NULL || d == NULL || dx <= 0 || dy <= 0)
    return FAILED;

  f = fopen(name, "wb");
  if (f == NULL) {
    fprintf(stderr, "BOXEN_WRITE: Could not open file \"%s\" for writing.\n", name);
    return FAILED;
  }

  /* write the header */
  fprintf(f, "BOXEM");

  /* write the type */
  _write_int(f, type);

  /* write dx and dy */
  _write_word(f, dx);
  _write_word(f, dy);

  /* write the number of mipmaps */
  _write_int(f, number_mipmaps);

  /* write the image sizes */
  for (i = 0; i < number_mipmaps; i++)
    _write_int(f, size[i]);

  /* write the data */
  for (n = 0, i = 0; i < number_mipmaps; i++) {
    fwrite(d + n, 1, size[i], f);
    n += size[i];
  }

  fclose(f);

  return SUCCEEDED;
}


int boxen_read(char *name, unsigned char **d, int *dx, int *dy, int **size, int *type, int *number_mipmaps) {

  char tmp[8];
  FILE *f;
  int i, n, p;


  if (name == NULL || d == NULL || dx == NULL || dy == NULL || size == NULL || type == NULL)
    return FAILED;

  f = fopen(name, "rb");
  if (f == NULL) {
    fprintf(stderr, "BOXEN_READ: Could not open file \"%s\" for reading.\n", name);
    return FAILED;
  }

  /* read the header */
  fscanf(f, "%c%c%c%c%c", &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4]);
  tmp[5] = 0;
  if (strcmp(tmp, "BOXEM") != 0) {
    fprintf(stderr, "BOXEN_READ: File \"%s\" is not a S3TC-BOXEN file.\n", name);
    fclose(f);
    return FAILED;
  }

  /* read the type */
  _read_int(f, type);

  /* read dx and dy */
  _read_word(f, dx);
  _read_word(f, dy);

  /* read the number of mipmaps */
  _read_int(f, number_mipmaps);

  *size = malloc(sizeof(int)*(*number_mipmaps));
  if (*size == NULL) {
    fprintf(stderr, "BOXEN_READ: Out of memory error.\n");
    fclose(f);
    return FAILED;
  }

  /* read the image sizes */
  for (p = 0, i = 0; i < *number_mipmaps; i++) {
    _read_int(f, &n);
    (*size)[i] = n;
    p += n;
  }

  *d = malloc(p);
  if (*d == NULL) {
    fprintf(stderr, "BOXEN_READ: Out of memory error.\n");
    fclose(f);
    return FAILED;
  }

  /* read the data */
  fread(*d, 1, p, f);

  fclose(f);

  return SUCCEEDED;
}
