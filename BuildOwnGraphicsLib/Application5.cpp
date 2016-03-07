// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #6
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application5.h"
#include "Gz.h"
#include "disp.h"
#include "rend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INFILE  "ppot.asc"
#define OUTFILE "output.ppm"

// 1 for Phong Shading, 2 for Gourad Shading, otherwise NONE Shading
#define SHADING_MODE 1
// 1 for tex_fun, 2 for ptex_fun
#define TEXTURE_MODE 1

extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */

void shade(GzCoord norm, GzCoord color);

float AAFilter[AAKERNEL_SIZE][3] = { /* X, Y, coef */
	-0.52, 0.38, 0.128,
	0.41, 0.56, 0.119,
	0.27, 0.08, 0.294,
	-0.17, -0.29, 0.249,
	0.58, -0.55, 0.104,
	-0.31, -0.71, 0.106
};

/* Translation matrix */
GzMatrix	scale = 
{ 
	3.25,	0.0,	0.0,	0.0, 
	0.0,	3.25,	0.0,	-3.25, 
	0.0,	0.0,	3.25,	3.5, 
	0.0,	0.0,	0.0,	1.0 
}; 

GzMatrix	rotateX = 
{ 
	1.0,	0.0,	0.0,	0.0, 
	0.0,	.7071,	.7071,	0.0, 
	0.0,	-.7071,	.7071,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 

GzMatrix	rotateY = 
{ 
	.866,	0.0,	-0.5,	0.0, 
	0.0,	1.0,	0.0,	0.0, 
	0.5,	0.0,	.866,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application5::Application5()
{

}

Application5::~Application5()
{
	
}

int Application5::Initialize()
{
	GzCamera	camera;  
	int		    xRes, yRes, dispClass;	/* display parameters */ 

	GzToken		nameListShader[9]; 	    /* shader attribute names */
	GzPointer   valueListShader[9];		/* shader attribute pointers */
	GzToken     nameListLights[10];		/* light info */
	GzPointer   valueListLights[10];
	GzToken     nameListAntialise[2];		/* Antialiasing names */
	GzPointer   valueListAntialise[2];		/* Antialiasing pointers */
	int			shaderType, interpStyle;
	float		specpower;
	int		status; 
 

	status = 0; 

	/* 
	 * Allocate memory for user input
	 */
	m_pUserInput = new GzInput;

	/* 
	 * initialize the display and the renderer 
	 */ 
 	m_nWidth = 256;		// frame buffer and display width
	m_nHeight = 256;    // frame buffer and display height

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);

	status |= GzNewDisplay(&m_pCombDisplay, GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);

	status |= GzGetDisplayParams(m_pCombDisplay, &xRes, &yRes, &dispClass); 
	 
	status |= GzInitDisplay(m_pCombDisplay); 
 
	//status |= GzNewRender(&m_pRender, GZ_Z_BUFFER_RENDER, m_pDisplay); 
	m_pRender = (GzRender**) malloc(AAKERNEL_SIZE * sizeof(GzRender*));
	m_pDisplay = (GzDisplay**) malloc(AAKERNEL_SIZE * sizeof(GzDisplay*));

	for (int i = 0; i < AAKERNEL_SIZE; i ++)
	{
		status |= GzNewDisplay(&m_pDisplay[i], GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight);
		status |= GzGetDisplayParams(m_pDisplay[i], &xRes, &yRes, &dispClass);
		status |= GzInitDisplay(m_pDisplay[i]);
		status |= GzNewRender(&m_pRender[i], GZ_Z_BUFFER_RENDER, m_pDisplay[i]);

#if 1 	/* set up app-defined camera if desired, else use camera defaults */
		camera.position[X] = -3;
		camera.position[Y] = -25;
		camera.position[Z] = -4;

		camera.lookat[X] = 7.8;
		camera.lookat[Y] = 0.7;
		camera.lookat[Z] = 6.5;

		camera.worldup[X] = -0.2;
		camera.worldup[Y] = 1.0;
		camera.worldup[Z] = 0.0;

		camera.FOV = 63.7;              /* degrees *              /* degrees */

		status |= GzPutCamera(m_pRender[i], &camera); 
#endif 
		/* Start Renderer */
		status |= GzBeginRender(m_pRender[i]);

		/* Light */
		GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
		GzLight	light2 = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
		GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };
		GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };

		/* Material property */
		GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
		GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
		GzColor diffuseCoefficient = {0.7, 0.7, 0.7};


/* 
  renderer is ready for frame --- define lights and shader at start of frame 
*/
		 /*
         * Tokens associated with light parameters
         */
        nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[0] = (GzPointer)&light1;
        nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[1] = (GzPointer)&light2;
        nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[2] = (GzPointer)&light3;
        status |= GzPutAttribute(m_pRender[i], 3, nameListLights, valueListLights);

        nameListLights[0] = GZ_AMBIENT_LIGHT;
        valueListLights[0] = (GzPointer)&ambientlight;
        status |= GzPutAttribute(m_pRender[i], 1, nameListLights, valueListLights);


        /*
         * Tokens associated with shading 
         */
        nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
        valueListShader[0] = (GzPointer)diffuseCoefficient;
	/* 
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode  
	*/
        nameListShader[1]  = GZ_INTERPOLATE;


#if SHADING_MODE == 1
		interpStyle = GZ_NORMALS; /* Phong shading */
#elif SHADING_MODE == 2
		interpStyle = GZ_COLOR; /* Gouround shading */
#else
		interpStyle = GZ_NONE;
#endif
		valueListShader[1] = (GzPointer)&interpStyle;

		nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
		valueListShader[2] = (GzPointer)ambientCoefficient;
		nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
		valueListShader[3] = (GzPointer)specularCoefficient;
		nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
		specpower = 32;
		valueListShader[4] = (GzPointer)&specpower;

		nameListShader[5]  = GZ_TEXTURE_MAP;

#if TEXTURE_MODE == 0   /* set up null texture function or valid pointer */
		valueListShader[5] = (GzPointer)0;
#elif TEXTURE_MODE == 1
		valueListShader[5] = (GzPointer)(tex_fun);	/* or use ptex_fun */
#elif TEXTURE_MODE == 2
		valueListShader[5] = (GzPointer)(ptex_fun);
#endif
		status |= GzPutAttribute(m_pRender[i], 6, nameListShader, valueListShader);

		nameListAntialise[0] = GZ_AASHIFTX;
		valueListAntialise[0] = (GzPointer)(&AAFilter[i][0]);

		nameListAntialise[1] = GZ_AASHIFTY;
		valueListAntialise[1] = (GzPointer)(&AAFilter[i][1]);

		status |= GzPutAttribute(m_pRender[i], 2, nameListAntialise, valueListAntialise);
		status |= GzPushMatrix(m_pRender[i], scale);  
		status |= GzPushMatrix(m_pRender[i], rotateY); 
		status |= GzPushMatrix(m_pRender[i], rotateX); 

		if (status) exit(GZ_FAILURE); 
	
	}
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 	
}

int Application5::Render() 
{
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 	/* vertex attribute pointers */
	GzCoord		vertexList[3];	/* vertex position coordinates */ 
	GzCoord		normalList[3];	/* vertex normals */ 
	GzTextureIndex  	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int			status; 

	for (int i = 0; i < AAKERNEL_SIZE; i ++)
	{
		/* Initialize Display */
		status |= GzInitDisplay(m_pDisplay[i]); 
	}	
	
	/* 
	* Tokens associated with triangle vertex values 
	*/ 
	nameListTriangle[0] = GZ_POSITION; 
	nameListTriangle[1] = GZ_NORMAL; 
	nameListTriangle[2] = GZ_TEXTURE_INDEX;  

	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
		 return GZ_FAILURE;
	}

	/* 
	* Walk through the list of triangles, set color 
	* and render each triangle 
	*/ 
	while( fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[0][0]), &(vertexList[0][1]),  
		&(vertexList[0][2]), 
		&(normalList[0][0]), &(normalList[0][1]), 	
		&(normalList[0][2]), 
		&(uvList[0][0]), &(uvList[0][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[1][0]), &(vertexList[1][1]), 	
		&(vertexList[1][2]), 
		&(normalList[1][0]), &(normalList[1][1]), 	
		&(normalList[1][2]), 
		&(uvList[1][0]), &(uvList[1][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[2][0]), &(vertexList[2][1]), 	
		&(vertexList[2][2]), 
		&(normalList[2][0]), &(normalList[2][1]), 	
		&(normalList[2][2]), 
		&(uvList[2][0]), &(uvList[2][1]) ); 

	    /* 
	     * Set the value pointers to the first vertex of the 	
	     * triangle, then feed it to the renderer 
	     * NOTE: this sequence matches the nameList token sequence
	     */ 
	     valueListTriangle[0] = (GzPointer)vertexList; 
		 valueListTriangle[1] = (GzPointer)normalList; 
		 valueListTriangle[2] = (GzPointer)uvList; 
		 for (int i = 0; i < AAKERNEL_SIZE; i ++)
		 {
			 GzPutTriangle(m_pRender[i], 3, nameListTriangle, valueListTriangle); 
		 }
	} 
	AntiCombine();

	GzFlushDisplay2File(outfile, m_pCombDisplay); 	/* write out or update display to file*/
	GzFlushDisplay2FrameBuffer(m_pFrameBuffer, m_pCombDisplay);	// write out or update display to frame buffer

	/* 
	 * Close file
	 */ 

	if( fclose( infile ) )
      AfxMessageBox( "The input file was not closed\n" );

	if( fclose( outfile ) )
      AfxMessageBox( "The output file was not closed\n" );
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Clean()
{
	/* 
	 * Clean up and exit 
	 */ 
	int	status = 0; 

	for (int i = 0; i < AAKERNEL_SIZE; i ++)
	{
		status |= GzFreeRender(m_pRender[i]); 
		status |= GzFreeDisplay(m_pDisplay[i]);
	}	
	status |= GzFreeDisplay(m_pCombDisplay);
	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}

int Application5::AntiCombine()
{
	int status = 0;

	for (int i = 0; i < m_pCombDisplay->xres; i ++)
	{
		for (int j = 0; j < m_pCombDisplay->yres; j ++)
		{
			int x = i + j * m_pCombDisplay->xres;
			m_pCombDisplay->fbuf[x].alpha = 0;
			m_pCombDisplay->fbuf[x].red = 0;
			m_pCombDisplay->fbuf[x].green = 0;
			m_pCombDisplay->fbuf[x].blue = 0;
			m_pCombDisplay->fbuf[x].z = 0;

			for (int t = 0; t < AAKERNEL_SIZE; t ++)
			{
				m_pCombDisplay->fbuf[x].red += m_pDisplay[t]->fbuf[x].red * AAFilter[t][2];
				m_pCombDisplay->fbuf[x].green += m_pDisplay[t]->fbuf[x].green * AAFilter[t][2];
				m_pCombDisplay->fbuf[x].blue += m_pDisplay[t]->fbuf[x].blue * AAFilter[t][2];
				m_pCombDisplay->fbuf[x].z += m_pDisplay[t]->fbuf[x].z * AAFilter[t][2];
			}
		}
	}
	return GZ_SUCCESS;
}


