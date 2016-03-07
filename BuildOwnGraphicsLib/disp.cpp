/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* create a framebuffer:
 -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
 -- pass back pointer 
*/
	if (!framebuffer)
		return GZ_FAILURE;
	// bounds detection
	if (width <= 0 || height <= 0 || width > MAXXRES || height > MAXYRES)
		return GZ_FAILURE;

	*framebuffer = new char[width * height * sizeof(GzPixel)];

	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{

/* create a display:
  -- allocate memory for indicated class and resolution
  -- pass back pointer to GzDisplay object in display
*/
	if (!display)
		return GZ_FAILURE;

	if (xRes <= 0 || xRes > MAXXRES || yRes <= 0 || yRes > MAXYRES)
		return GZ_FAILURE;

	*display = new GzDisplay();	
	(*display)->xres = xRes;
	(*display)->yres = yRes;
	(*display)->dispClass = dispClass;
	(*display)->open = 0;
	(*display)->fbuf = new GzPixel[xRes * yRes];

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* clean up, free memory */
	if (!display)
		return GZ_FAILURE;

	delete display->fbuf;
	display->fbuf = NULL;
	delete display;
	display = NULL;

	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
/* pass back values for an open display */
	if (!xRes || !yRes || !dispClass || !display)
		return GZ_FAILURE;

	*xRes = display->xres;
	*yRes = display->yres;
	*dispClass = display->dispClass;

	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* set everything to some default values - start a new frame */
	if (!display)
		return GZ_FAILURE;

	for (int i = 0; i < display->xres * display->yres; i ++)
	{
		//initial color b,g,b: 128,112,96
		display->fbuf[i].red = 128*16;
		display->fbuf[i].green = 112*16;
		display->fbuf[i].blue = 96*16;
		display->fbuf[i].alpha = 1;
		display->fbuf[i].z = 2147483647; // boundary of signed int: -2147483648 to 2147483647
	}
	display->open = 1;
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* write pixel values into the display */
	if (!display)
		return GZ_FAILURE;
	// boundary detection
	if (i < 0 || j < 0 || i >= display->xres || j >= display->yres)
		return GZ_FAILURE;

	if (r > 4095)
		r = 4095;
	if (r < 0)
		r = 0;
	if (g > 4095)
		g = 4095;
	if (g < 0)
		g = 0;
	if (b > 4095)
		b = 4095;
	if (b < 0)
		b = 0;

	display->fbuf[ARRAY(i,j)].red = r;
	display->fbuf[ARRAY(i,j)].green = g;
	display->fbuf[ARRAY(i,j)].blue = b;
	display->fbuf[ARRAY(i,j)].alpha = a;
	display->fbuf[ARRAY(i,j)].z = z;

	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	/* pass back pixel value in the display */
	/* check display class to see what vars are valid */
	if (!display)
		return GZ_FAILURE;

	if (i < 0 || j < 0 || i >= display->xres || j >= display->yres)
		return GZ_FAILURE;

	*r = display->fbuf[ARRAY(i,j)].red;
	*g = display->fbuf[ARRAY(i,j)].green;
	*b = display->fbuf[ARRAY(i,j)].blue;
	*a = display->fbuf[ARRAY(i,j)].alpha;
	*z = display->fbuf[ARRAY(i,j)].z;
	
	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

	/* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
	if (!display)
		return GZ_FAILURE;
	// size of the image: xres X yres
	fprintf(outfile, "P6 %d %d 255\r", display->xres, display->yres);
	for (int i = 0; i < display->xres * display->yres; i ++)
	{
		// transfer 4096 to 255
		fprintf(outfile, "%c%c%c", display->fbuf[i].red >> 4, display->fbuf[i].green >> 4, display->fbuf[i].blue >> 4);
	}

	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	/* write pixels to framebuffer: 
		- Put the pixels into the frame buffer
		- Caution: store the pixel to the frame buffer as the order of blue, green, and red 
		- Not red, green, and blue !!!
	*/
	if (!display)
		return GZ_FAILURE;

	for (int i = 0; i < display->xres * display->yres; i ++)
	{
		framebuffer[i*3] = display->fbuf[i].blue >> 4;
		framebuffer[i*3+1] = display->fbuf[i].green >> 4;		
		framebuffer[i*3+2] = display->fbuf[i].red >> 4;
	}

	return GZ_SUCCESS;
}