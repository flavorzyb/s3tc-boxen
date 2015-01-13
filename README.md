# s3tc-boxen
S3 Texture Compression Tool which was written by ville helin &lt;vhelin@iki.fi> in 2003. See it on http://www.villehelin.com/README.boxen 

-----------------------------------------------------------------------------
-                               s3tc-boxen                                  -
-              written by ville helin <vhelin@iki.fi> in 2003               -
-----------------------------------------------------------------------------

1. S3TC-BOXEN?

s3tc-boxen is a small tool that s3tc-compresses your bmp/jpg/png textures.
inside you'll find also boxen.c that handles the writing and loading
of boxen-files. boxen-files include few interesting bits and the
s3tc-compressed data.


2. COMPILING?

make


3. USAGE?

USAGE: ./s3tc-boxen -<1|3|5> [-buildmipmaps <max level>] <INPUT FILE> <OUTPUT FILE>

-1, -3 and -5 select the DXT-level you want to use to compress your texture.
note that DXT1 is the only one that can compress RGB-data. DXT3 and DXT5
handle only RGBA-data (DXT1 can do that as well).

-buildmipmaps forces s3tc-boxen to build mipmaps from the texture image.
it is better to build the mipmaps here, from the original data, than
from the lossy, s3tc-compressed data. you must also give the number of mipmaps
you want ("max level"). here 0 means that you get all the mipmaps, the final
mipmap being an 1x1 image. 1 means that you'll get one image (the original),
2 means two images (the original plus one mipmap), and so forth.


4. HOW DOES IT DO IT?

s3tc-boxen simply loads the texture into your video card's ram and
tells the video card to s3tc-compress it.


5. REQUIREMENTS

currently s3tc-boxen requires:
* linux (hasn't been tested elsewhere)
* SDL (for the OpenGL part, this is ugly)
* libjpeg, libpng and zlib (for reading the textures)
* a video card that supports S3TC-compression


6. MOTIVATION?

i wrote s3tc-boxen, because i want to keep all the textures in one, simple
format. as i would s3tc-compress my textures anyway s3tc-boxen is a nice
way to get rid of all that libjpeg- & libpng-code from my 3D projects.

just s3tc-boxen your textures and include boxen.c into your project.


7. LICENCE

s3tc-boxen is LGPL software. read the LICENSE file for more information.


8. THE AUTHOR?

i'm ville helin <vhelin@iki.fi>. if s3tc-boxen does something bad to
your computer, don't blame me, because i didn't do it. ;) you take full
responsibility for using s3tc-boxen.

and if you spot any bugs or come up with ideas for enhancements, let
me know!


9. BOXEN-FILEFORMAT

  bytes
000 - 004: "BOXEM"
005 - 008: type of the texture (int)
009 - 010: dx (word)
011 - 012: dy (word)
013 - 016: number n of the textures (>1 if mipmaps)
017 - ...: size of the texture 0 (int)
... - ...: size of the texture 1 (int)
...
... - ...: size of the texture n-1 (int)
... - ...: the texture 0
... - ...: the texture 1
...
... - ...: the texture n-1


10. HOW TO LOAD A BOXEN FILE IN MY OWN PROJECT AND USE IT?

that's simple:

i.   call boxen_read()
ii.  call glBindTexture()
iii. call glCompressedTexImage2DARB() for each of the textures.

here is an example:

  ...
  glBindTexture(GL_TEXTURE_2D, t->opengl_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (t->mipmaps > 1) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, t->mipmaps-1);
  }
  else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  for (p = 0, i = 0, div = 1; i < t->mipmaps; i++) {
    glCompressedTexImage2DARB(GL_TEXTURE_2D, i, t->type, t->x/div, t->y/div, 0, t->size[i], t->txt + p);
    p += t->size[i];
    div *= 2;
  }
  ...


11. HISTORY

v1.3 (03-Dec-2003): * input file errors are detected better and handled
                      with more care.
                    * s3tc-boxen loads jpg-files ending to ".jpeg".
                    * fixed possible crashes in png and jpg loaders.
                    * s3tc-boxen loads now bmp-files, too.
                    * it is possible to define the number of mipmaps
                      to be generated.

v1.2 (19-Oct-2003): * s3tc-boxen can now handle mipmaps as well. the boxen-
                      fileformat has changed.

v1.1 (27-Feb-2003): * added texture size checking. if dx or dy isn't a power of two
                      then a warning is displayed.
                    * s3tc-boxen checks now if the video card has support for
                      s3tc-compression.

v1.0 (27-Feb-2003): * initial release.
