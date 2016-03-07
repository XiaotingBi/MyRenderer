/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include	"stdint.h"
#include <cmath>

GzColor	*image;
int xs, ys;
int reset = 1;

inline int Index(int x, int y){
	return x + y * xs;
}
// #define TEXFILE "colors.ppm"
#define TEXFILE "texture"
/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
	fd = fopen(TEXFILE, "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs * ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */
  if (u > 1) u = 1;
  if (u < 0) u = 0;
  if (v > 1) v = 1;
  if (v < 0) v = 0;
  float pixelX = u * (xs - 1), pixelY = v * (ys - 1);
  int minX = floor(pixelX), maxX = ceil(pixelX);
  int minY = floor(pixelY), maxY = ceil(pixelY);
  // (minX, minY)  -  (maxX, minY)
  //       |               |
  // (minX, maxY)  -  (maxX, maxY)
  GzColor A, B, C, D;
  memcpy(A, image[Index(minX, minY)], sizeof(GzColor));
  memcpy(B, image[Index(maxX, minY)], sizeof(GzColor));
  memcpy(C, image[Index(maxX, maxY)], sizeof(GzColor));
  memcpy(D, image[Index(minX, maxY)], sizeof(GzColor));
  // Bilinear Interpolation
  // Color(p) = s t C + (1-s) t D + s (1-t) B + (1-s) (1-t) A
  float t = pixelY - minY, s = pixelX - minX;
  for (int i = 0; i < 3; i++)
	  color[i] = s * t * C[i] + (1 - s) * t * D[i] + s * (1 - t) * B[i] + (1 - s) * (1 - t) * A[i];
  return GZ_SUCCESS;
}

#define DIM 256
float noise[DIM][DIM];
void generateNoise(float noise[DIM][DIM]);
double turbulence(double x, double y, double size);
double smoothNoise(double x, double y);
bool flag = true;
int ptex_fun(float u, float v, GzColor color)
{
	if (flag){
		generateNoise(noise);
		flag = false;
	}
	//xPeriod and yPeriod together define the angle of the lines
	//xPeriod and yPeriod both 0 ==> it becomes a normal clouds or turbulence pattern
	double xPeriod = 5.0; //defines repetition of marble lines in x direction
	double yPeriod = 10.0; //defines repetition of marble lines in y direction
	//turbPower = 0 ==> it becomes a normal sine pattern
	double turbPower = 5.0; //makes twists
	double turbSize = 32.0; //initial size of the turbulence
	u *= 128; v *= 128;
	double xyValue = u * xPeriod / DIM + v * yPeriod / DIM + turbPower * turbulence(u, v, turbSize) / 512.0;
	double sineValue = fabs(sin(xyValue * 3.14159));
	color[RED] = color[GREEN] = color[BLUE] = sineValue;

	return GZ_SUCCESS;
}
void generateNoise(float noise[DIM][DIM])
{
	for (int x = 0; x < DIM; x++)
		for (int y = 0; y < DIM; y++)
			noise[x][y] = (rand() % 256) / 256.0;
}
double turbulence(double x, double y, double size)
{
	double value = 0.0, initialSize = size;

	while (size >= 1)
	{
		value += smoothNoise(x / size, y / size) * size;
		size /= 2.0;
	}
	return(128.0 * value / initialSize);
}
double smoothNoise(double x, double y)
{
	//get fractional part of x and y
	double fractX = x - int(x);
	double fractY = y - int(y);

	//wrap around
	int x1 = (int(x) + DIM) % DIM;
	int y1 = (int(y) + DIM) % DIM;

	//neighbor values
	int x2 = (x1 + DIM - 1) % DIM;
	int y2 = (y1 + DIM - 1) % DIM;

	//smooth the noise with bilinear interpolation
	double value = 0.0;
	value += fractX       * fractY       * noise[x1][y1];
	value += fractX       * (1 - fractY) * noise[x1][y2];
	value += (1 - fractX) * fractY       * noise[x2][y1];
	value += (1 - fractX) * (1 - fractY) * noise[x2][y2];

	return value;
}
