
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defines.h"

#include <GL/gl.h>


int glextd_is_supported(char *extension) {

  const GLubyte *e = NULL;
  const GLubyte *s;
  GLubyte *w, *t;


  if (extension == NULL || extension[0] == 0)
    return NO;

  /* extension names should not have spaces */
  w = (GLubyte *)strchr(extension, ' ');
  if (w != NULL)
    return NO;

  e = glGetString(GL_EXTENSIONS);
  s = e;
  while (1) {
    w = (GLubyte *)strstr((char *)s, extension);
    if (w == NULL)
      break;

    t = w + strlen(extension);

    if (w == s || *(w - 1) == ' ') {
      if (*t == ' ' || *t == 0)
	return YES;
    }
    s = t;
  }

  return NO;
}
