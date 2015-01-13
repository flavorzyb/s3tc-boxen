
#ifndef _BOXEN_H
#define _BOXEN_H

int boxen_write(char *name, unsigned char *d, int dx, int dy, int *size, int type, int number_mipmaps);
int boxen_read(char *name, unsigned char **d, int *dx, int *dy, int **size, int *type, int *number_mipmaps);

#endif
