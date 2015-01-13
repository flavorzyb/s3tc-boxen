
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_video.h>
#include <SDL/SDL_keysym.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "defines.h"
#include "main.h"
#include "jpeg.h"
#include "png.h"
#include "s3tc.h"
#include "boxen.h"
#include "glextd.h"
#include "bmp.h"


/* S3TC */
int s3tc_level = 0;

/* input file */
unsigned char *d = NULL;
int dx = 0, dy = 0, type = 0;

/* output data */
unsigned char *e = NULL;
int *size = NULL, otype = 0;

/* mipmap */
int build_mipmaps = NO, number_mipmaps = 1;


int _powerof2(int i) {

  if (i == 2 || i == 4 || i == 8 || i == 16 || i == 32 || i == 64 || i == 128 || i == 256)
    return YES;
  if (i == 512 || i == 1024 || i == 2048 || i == 4096 || i == 8192)
    return YES;

  return NO;
}


int main(int argc, char *argv[]) {

  const SDL_VideoInfo *sdl_video_info = NULL;
  char *iname, *oname;
  int l, i, max;


  if (!(argc == 4 || argc == 6)) {
    fprintf(stderr, "USAGE: %s -<1|3|5> [-buildmipmaps <max level>] <INPUT FILE> <OUTPUT FILE>\n", argv[0]);
    return 1;
  }

  /* check the wanted s3tc level */
  if (strcmp(argv[1], "-1") == 0)
    s3tc_level = 1;
  else if (strcmp(argv[1], "-3") == 0)
    s3tc_level = 3;
  else if (strcmp(argv[1], "-5") == 0)
    s3tc_level = 5;
  else {
    fprintf(stderr, "MAIN: The second argument should be the S3TC-level used for compression (DXT1, DXT3 or DXT5).\n");
    return 1;
  }

  if (strcmp(argv[2], "-buildmipmaps") == 0) {
    build_mipmaps = YES;
    max = atoi(argv[3]);
    iname = argv[4];
    oname = argv[5];
  }
  else {
    build_mipmaps = NO;
    max = 0;
    iname = argv[2];
    oname = argv[3];
  }

  /* read the input file */
  l = strlen(iname);
  for (i = 0; i < l; i++) {
    if ((iname)[i] == '.')
      break;
  }

  if (l == i) {
    fprintf(stderr, "MAIN: The input file must be either a JPEG or a PNG image.\n");
    return 1;
  }

  if (strcasecmp(iname+i, ".jpg") == 0 || strcasecmp(iname+i, ".jpeg") == 0)
    l = jpeg_load(iname, &dx, &dy, &type, &d);
  else if (strcasecmp(iname+i, ".png") == 0)
    l = png_load(iname, &dx, &dy, &type, &d);
  else if (strcasecmp(iname+i, ".bmp") == 0)
    l = bmp_load(iname, &dx, &dy, &type, &d);
  else {
    fprintf(stderr, "MAIN: The input file must be either a JPEG or a PNG image.\n");
    return 1;
  }

  if (l == FAILED || d == NULL) {
    fprintf(stderr, "MAIN: Error loading \"%s\".\n", iname);
    return 1;
  }

  /* check that dx and dy are powers of 2 */
  if (_powerof2(dx) == NO || _powerof2(dy) == NO) {
    fprintf(stderr, "MAIN: The size of the picture (%dx%d) isn't powers of two. This might cause problems.\n", dx, dy);
  }

  /* init SDL & opengl */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
    fprintf(stderr, "OPENGL_INIT: Video initialization failed: %s\n", SDL_GetError());
    return FAILED;
  }

  sdl_video_info = SDL_GetVideoInfo();
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_SetVideoMode(320, 240, sdl_video_info->vfmt->BitsPerPixel, SDL_OPENGL);

  /* check if the texture compressing is supported */
  if (glextd_is_supported("GL_EXT_texture_compression_s3tc") == NO) {
    fprintf(stderr, "MAIN: \"GL_EXT_texture_compression_s3tc\" is not supported by your video card.\n");
    SDL_Quit();
    return 1;
  }

  /* convert the texture to s3tc format */
  if (s3tc_convert_texture(d, dx, dy, type, s3tc_level, &e, &size, &otype, build_mipmaps, max, &number_mipmaps) == FAILED) {
    SDL_Quit();
    return 1;
  }

  /* write it to the disk */
  boxen_write(oname, e, dx, dy, size, otype, number_mipmaps);

  SDL_Quit();

  return 0;
}
