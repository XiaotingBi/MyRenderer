/* CS580 Homework 6 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI 3.1415926
#define DegreeToRadian(x) (x * PI / 180.0) 

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	float theta = (float)DegreeToRadian(degree);
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = cos(theta);
	mat[1][2] = -sin(theta);
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = sin(theta);
	mat[2][2] = cos(theta);
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	float theta = (float)DegreeToRadian(degree);
	mat[0][0] = cos(theta);
	mat[0][1] = 0;
	mat[0][2] = sin(theta);
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = -sin(theta);
	mat[2][1] = 0;
	mat[2][2] = cos(theta);
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	float theta = (float)DegreeToRadian(degree);
	mat[0][0] = cos(theta);
	mat[0][1] = -sin(theta);
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = sin(theta);
	mat[1][1] = cos(theta);
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = translate[X];

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = translate[Y];

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = translate[Z];

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	return GZ_SUCCESS;
}

int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	mat[0][0] = scale[X];
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = scale[Y];
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = scale[Z];
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}

//----------------------------------------------------------
// Begin main functions

/* NOT part of API - just for general assistance */

short ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

bool inside_tri(float alpha[3][3], int x, int y){
	int tot = 0;

	for (int k = 0; k < 3; k++){
		float ans = alpha[k][0] * x + alpha[k][1] * y + alpha[k][2];
		if (ans < 0){
			tot -= 1;
		}
		else if (ans > 0){
			tot += 1;
		}
		else{
			return true;
		}
	}

	if (tot == 3 || tot == -3)
		return true;
	else
		return false;
}

bool sort_tri_vertex(float * minX, float * maxX, float * minY, float * maxY, const GzCoord * vertexList, int list[3]){
	// sort for x 
	for (int i = 0; i < 2; i++){
		for (int j = i + 1; j < 3; j++){
			if (vertexList[list[i]][X] < vertexList[list[j]][X]){
				int tmp = list[i];
				list[i] = list[j];
				list[j] = tmp;
			}
		}
	}
	*minX = vertexList[list[2]][X];
	*maxX = vertexList[list[0]][X];
	// sort for y
	for (int i = 0; i < 2; i++){
		for (int j = i + 1; j < 3; j++){
			if (vertexList[list[i]][Y] < vertexList[list[j]][Y]){
				int tmp = list[i];
				list[i] = list[j];
				list[j] = tmp;
			}
		}
	}
	*minY = vertexList[list[2]][Y];
	*maxY = vertexList[list[0]][Y];

	return true;
}

bool compute_coefficient(float alpha[3][3], const GzCoord	* vertexList, int * list){
	for (int i = 0; i < 3; i++)
	{
		int next = (i + 1) % 3;
		// Ax + By + C = 0
		// A = dY
		// B = -dX
		// C = dXY - dYX
		alpha[i][0] = vertexList[list[next]][Y] - vertexList[list[i]][Y];
		alpha[i][1] = vertexList[list[i]][X] - vertexList[list[next]][X];
		alpha[i][2] = vertexList[list[next]][X] * vertexList[list[i]][Y]
			- vertexList[list[i]][X] * vertexList[list[next]][Y];
	}
	return true;
}

bool cross_prod(GzCoord cprod, const GzCoord * vertexList, int * orderList)
{
	if (!cprod)
		return false;
	//(X,Y,Z)0 X (X,Y,Z)1 = (A,B,C) 
	// Ax + By + Cz + D = 0
	GzCoord u, v;
	u[X] = vertexList[orderList[1]][X] - vertexList[orderList[0]][X];
	u[Y] = vertexList[orderList[1]][Y] - vertexList[orderList[0]][Y];
	u[Z] = vertexList[orderList[1]][Z] - vertexList[orderList[0]][Z];

	v[X] = vertexList[orderList[2]][X] - vertexList[orderList[1]][X];
	v[Y] = vertexList[orderList[2]][Y] - vertexList[orderList[1]][Y];
	v[Z] = vertexList[orderList[2]][Z] - vertexList[orderList[1]][Z];

	cprod[X] = u[Y] * v[Z] - u[Z] * v[Y];
	cprod[Y] = u[Z] * v[X] - u[X] * v[Z];
	cprod[Z] = u[X] * v[Y] - u[Y] * v[X];
	return true;
}

void vectors_production(const GzCoord A, const GzCoord B, GzCoord &C){
	for (int i = 0; i < 3; i++)
	{
		C[i] = A[i] * B[i];
	}
	return;
}

void cross_production(const GzCoord A, const GzCoord B, GzCoord &C){
	C[X] = A[Y] * B[Z] - A[Z] * B[Y];
	C[Y] = A[Z] * B[X] - A[X] * B[Z];
	C[Z] = A[X] * B[Y] - A[Y] * B[X];
}

float dot_production(const GzCoord A, const GzCoord B){
	return A[X] * B[X] + A[Y] * B[Y] + A[Z] * B[Z];
}

void scale_vector(const GzCoord A, float scale, GzCoord &B){
	B[X] = A[X] * scale;
	B[Y] = A[Y] * scale;
	B[Z] = A[Z] * scale;
	return;
}

void add_vector(const GzCoord A, const GzCoord B, GzCoord C, int factor){
	C[X] = A[X] + B[X] * factor;
	C[Y] = A[Y] + B[Y] * factor;
	C[Z] = A[Z] + B[Z] * factor;
	return;
}

void normalization(GzCoord &A){
	float norms = sqrt(A[X] * A[X] + A[Y] * A[Y] + A[Z] * A[Z]);
	A[X] = A[X] / norms;
	A[Y] = A[Y] / norms;
	A[Z] = A[Z] / norms;
}

void MatMultiply(const GzMatrix mat1, const GzMatrix mat2, GzMatrix &mat3){
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++){
			mat3[i][j] = 0;
			for (int k = 0; k < 4; k++){
				mat3[i][j] += mat1[i][k] * mat2[k][j];
			}
		}
		return;
}

void TransformVertex(const GzMatrix trans_matrix, const GzCoord vertexList, GzCoord &new_vertexList){
	float expand_vector[4];
	float result_vector[4];
	expand_vector[X] = vertexList[X];
	expand_vector[Y] = vertexList[Y];
	expand_vector[Z] = vertexList[Z];
	expand_vector[3] = 1;
	for (int i = 0; i < 4; i++){
		result_vector[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			result_vector[i] += trans_matrix[i][j] * expand_vector[j];
		}
	}
	new_vertexList[X] = result_vector[X] / result_vector[3];
	new_vertexList[Y] = result_vector[Y] / result_vector[3];
	new_vertexList[Z] = result_vector[Z] / result_vector[3];

	return;
}

int BuildXspMat(GzRender *render){
	// Check the validity for render
	if (!render || !render->display)
		return GZ_FAILURE;

	// Xsp matrix
	// xs/2		0		0		xs/2
	// 0	-ys/2		0		ys/2
	// 0		0		Zmax/d	0
	// 0		0		0		1
	// 1 / d = tan(FOV / 2)
	render->Xsp[0][0] = (float)render->display->xres / (2.0);
	render->Xsp[0][1] = (float)0;
	render->Xsp[0][2] = (float)0;
	render->Xsp[0][3] = (float)render->display->xres / (2.0);

	render->Xsp[1][0] = (float)0;
	render->Xsp[1][1] = (float)-render->display->yres / 2.0;
	render->Xsp[1][2] = (float)0;
	render->Xsp[1][3] = (float)render->display->yres / 2.0;

	render->Xsp[2][0] = 0;
	render->Xsp[2][1] = 0;
	render->Xsp[2][2] = (float)INT_MAX / (1 / tan(DegreeToRadian(render->camera.FOV) / 2));
	render->Xsp[2][3] = 0;

	render->Xsp[3][0] = 0;
	render->Xsp[3][1] = 0;
	render->Xsp[3][2] = 0;
	render->Xsp[3][3] = 1;

	return GZ_SUCCESS;
}

int BuildXpiMat(GzRender *render){
	// Check the validity for render
	if (!render || !render->display)
		return GZ_FAILURE;

	// Matrix Xpi
	// 1	0	0	0
	// 0	1	0	0
	// 0	0	1	0
	// 0	0	1/d	1
	render->camera.Xpi[0][0] = 1;
	render->camera.Xpi[0][1] = 0;
	render->camera.Xpi[0][2] = 0;
	render->camera.Xpi[0][3] = 0;

	render->camera.Xpi[1][0] = 0;
	render->camera.Xpi[1][1] = 1;
	render->camera.Xpi[1][2] = 0;
	render->camera.Xpi[1][3] = 0;

	render->camera.Xpi[2][0] = 0;
	render->camera.Xpi[2][1] = 0;
	render->camera.Xpi[2][2] = 1;
	render->camera.Xpi[2][3] = 0;

	render->camera.Xpi[3][0] = 0;
	render->camera.Xpi[3][1] = 0;
	render->camera.Xpi[3][2] = (float)1 / (1 / tan(DegreeToRadian(render->camera.FOV) / 2));
	render->camera.Xpi[3][3] = 1;

	return GZ_SUCCESS;
}

int BuildXiwMat(GzRender * render){
	// Check the validity for render
	if (!render || !render->display)
		return GZ_FAILURE;
	GzCoord xvec, yvec, zvec;

	// Z = lookat_coord - camera_coord
	zvec[X] = render->camera.lookat[X] - render->camera.position[X];
	zvec[Y] = render->camera.lookat[Y] - render->camera.position[Y];
	zvec[Z] = render->camera.lookat[Z] - render->camera.position[Z];
	normalization(zvec);

	// Y = up - (up ¡¤ Z)Z
	float dot_prod = dot_production(render->camera.worldup, zvec);
	yvec[X] = render->camera.worldup[X] - dot_prod * zvec[X];
	yvec[Y] = render->camera.worldup[Y] - dot_prod * zvec[Y];
	yvec[Z] = render->camera.worldup[Z] - dot_prod * zvec[Z];
	normalization(yvec);

	// X = (Y x Z) = i (Yy Zz - Yz Zy) + j (Yz Zx - Yx Zz) + k (Yx Zy - Yy Zx) 
	xvec[X] = yvec[Y] * zvec[Z] - yvec[Z] * zvec[Y];
	xvec[Y] = yvec[Z] * zvec[X] - yvec[X] * zvec[Z];
	xvec[Z] = yvec[X] * zvec[Y] - yvec[Y] * zvec[X];
	normalization(xvec);

	// Xiw
	// Xx	Xy	Xz	-C*X
	// Yx	Yy	Yz	-C*Y
	// Zx	Zy	Zz	-C*Z
	// 0	0	0	1
	render->camera.Xiw[0][0] = xvec[X];
	render->camera.Xiw[0][1] = xvec[Y];
	render->camera.Xiw[0][2] = xvec[Z];
	render->camera.Xiw[0][3] = -dot_production(render->camera.position, xvec);

	render->camera.Xiw[1][0] = yvec[X];
	render->camera.Xiw[1][1] = yvec[Y];
	render->camera.Xiw[1][2] = yvec[Z];
	render->camera.Xiw[1][3] = -dot_production(render->camera.position, yvec);

	render->camera.Xiw[2][0] = zvec[X];
	render->camera.Xiw[2][1] = zvec[Y];
	render->camera.Xiw[2][2] = zvec[Z];
	render->camera.Xiw[2][3] = -dot_production(render->camera.position, zvec);

	render->camera.Xiw[3][0] = 0;
	render->camera.Xiw[3][1] = 0;
	render->camera.Xiw[3][2] = 0;
	render->camera.Xiw[3][3] = 1;

	return GZ_SUCCESS;
}

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
	/*  
	- malloc a renderer struct 
	- keep closed until all inits are done 
	- setup Xsp and anything only done once 
	- span interpolator needs pointer to display 
	- check for legal class GZ_Z_BUFFER_RENDER 
	- init default camera 
	*/ 
	if (!render || !display)
		return GZ_FAILURE;
	if (renderClass != GZ_Z_BUFFER_RENDER)
		return GZ_FAILURE;

	(*render) = new GzRender;
	if (!(*render))
		return GZ_FAILURE;

	(*render)->camera.FOV = DEFAULT_FOV;
	(*render)->camera.position[X] = DEFAULT_IM_X;
	(*render)->camera.position[Y] = DEFAULT_IM_Y;
	(*render)->camera.position[Z] = DEFAULT_IM_Z;
	(*render)->camera.lookat[X] = 0;
	(*render)->camera.lookat[Y] = 0;
	(*render)->camera.lookat[Z] = 0;
	(*render)->camera.worldup[X] = 0;
	(*render)->camera.worldup[Y] = 1;
	(*render)->camera.worldup[Z] = 0;

	(*render)->display = display;
	(*render)->renderClass = renderClass;
	(*render)->open = 0;
	(*render)->matlevel = EMPTY_MAT;

	(*render)->ambientlight.color[RED] = 0.0f;
	(*render)->ambientlight.color[GREEN] = 0.0f;
	(*render)->ambientlight.color[BLUE] = 0.0f;
	(*render)->numlights = 0;
	GzColor Ka = DEFAULT_AMBIENT, Kd = DEFAULT_DIFFUSE, Ks = DEFAULT_SPECULAR;
	(*render)->Ka[RED] = Ka[RED];
	(*render)->Ka[GREEN] = Ka[GREEN];
	(*render)->Ka[BLUE] = Ka[BLUE];
	(*render)->Kd[RED] = Kd[RED];
	(*render)->Kd[GREEN] = Kd[GREEN];
	(*render)->Kd[BLUE] = Kd[BLUE];
	(*render)->Ks[RED] = Ks[RED];
	(*render)->Ks[GREEN] = Ks[GREEN];
	(*render)->Ks[BLUE] = Ks[BLUE];

	(*render)->spec = DEFAULT_SPEC;
	(*render)->interp_mode = GZ_RGB_COLOR;
	return GZ_SUCCESS;
}

int GzFreeRender(GzRender *render)
{
	/* 
	-free all renderer resources
	*/
	if (!render)
		return GZ_FAILURE;

	render->display = NULL;
	delete render;
	render = NULL;

	return GZ_SUCCESS;
}

int GzBeginRender(GzRender *render)
{
	/*  
	- set up for start of each frame - clear frame buffer 
	- compute Xiw and projection xform Xpi from camera definition 
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
	- now stack contains Xsw and app can push model Xforms if it want to. 
	*/ 
	if (!render)
		return GZ_FAILURE;

	render->open = 1;
	render->flatcolor[RED] = 0.7;
	render->flatcolor[GREEN] = 0.3;
	render->flatcolor[BLUE] = 0.4;

	if (BuildXspMat(render) == GZ_FAILURE)
		return GZ_FAILURE;
	if (BuildXpiMat(render) == GZ_FAILURE)
		return GZ_FAILURE;
	if (BuildXiwMat(render) == GZ_FAILURE)
		return GZ_FAILURE;

	render->matlevel = EMPTY_MAT;
	if (GzPushMatrix(render, render->Xsp) == GZ_FAILURE)
		return GZ_FAILURE;
	if (GzPushMatrix(render, render->camera.Xpi) == GZ_FAILURE)
		return GZ_FAILURE;
	if (GzPushMatrix(render, render->camera.Xiw) == GZ_FAILURE)
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
	/*
	- overwrite renderer camera structure with new camera definition
	*/
	if (!render || !camera){
		return GZ_FAILURE;
	}

	memcpy(&(render->camera), camera, sizeof(GzCamera));

	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
	/*
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	// check if the stack is full
	if (!render || render->matlevel >= MATLEVELS)
		return GZ_FAILURE;

	// if stack is empty, just push the new matrix on it
	if (render->matlevel == EMPTY_MAT){
		memcpy(render->Ximage[render->matlevel], matrix, sizeof(GzMatrix));
	}
	else{
		// Multiply with the top of the stack
		GzMatrix new_top;
		MatMultiply(render->Ximage[render->matlevel-1], matrix, new_top);
		memcpy(render->Ximage[render->matlevel], new_top, sizeof(GzMatrix));
	}

	// we don't push Xsp or Xpi matrix to the norm stack since we only do it in the image space.
	if (render->matlevel == 0 || render->matlevel == 1)
	{
		GzMatrix identityMatrix;
		// row 0
		identityMatrix[0][0] = 1;
		identityMatrix[0][1] = 0;
		identityMatrix[0][2] = 0;
		identityMatrix[0][3] = 0;
		// row 1
		identityMatrix[1][0] = 0;
		identityMatrix[1][1] = 1;
		identityMatrix[1][2] = 0;
		identityMatrix[1][3] = 0;
		// row 2
		identityMatrix[2][0] = 0;
		identityMatrix[2][1] = 0;
		identityMatrix[2][2] = 1;
		identityMatrix[2][3] = 0;
		// row 3
		identityMatrix[3][0] = 0;
		identityMatrix[3][1] = 0;
		identityMatrix[3][2] = 0;
		identityMatrix[3][3] = 1;

		// copy the identity matrix into the normals transform stack
		memcpy(render->Xnorm[render->matlevel], identityMatrix, sizeof(GzMatrix));
	}
	else{
		GzMatrix processedMatrix;
		processedMatrix[0][0] = matrix[0][0];
		processedMatrix[0][1] = matrix[0][1];
		processedMatrix[0][2] = matrix[0][2];
		processedMatrix[0][3] = 0;
		processedMatrix[1][0] = matrix[1][0];
		processedMatrix[1][1] = matrix[1][1];
		processedMatrix[1][2] = matrix[1][2];
		processedMatrix[1][3] = 0;
		processedMatrix[2][0] = matrix[2][0];
		processedMatrix[2][1] = matrix[2][1];
		processedMatrix[2][2] = matrix[2][2];
		processedMatrix[2][3] = 0;
		processedMatrix[3][0] = matrix[3][0];
		processedMatrix[3][1] = matrix[3][1];
		processedMatrix[3][2] = matrix[3][2];
		processedMatrix[3][3] = 1;

		// Normalize matrix: K = 1 / sqrt(a^2 + b^2 + c^2)
		float normFactor = 1 / sqrt(matrix[0][0] * matrix[0][0] + matrix[1][0] * matrix[1][0] + matrix[2][0] * matrix[2][0]);
		for (int row = 0; row < 3; row++)
			for (int col = 0; col < 3; col++)
				// normalize this component of upper 3x3 matrix
				processedMatrix[row][col] *= normFactor;

		// the matrix we push onto the geometry stack should be the top of the stack multiplied by the new transform (on the right)
		GzMatrix xformProduct;
		MatMultiply(render->Xnorm[render->matlevel - 1], processedMatrix, xformProduct);

		// copy the matrix into the geometry stack
		memcpy(render->Xnorm[render->matlevel], xformProduct, sizeof(GzMatrix));
	}

	// increaset the stack level
	render->matlevel++;
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
	/*
	- pop a matrix off the Ximage stack
	- check for stack underflow
	*/
	// check if the stack is empty
	if (!render || render->matlevel <= EMPTY_MAT)
		return GZ_FAILURE;

	render->matlevel--;

	return GZ_SUCCESS;
}

int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
	/*
	- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
	- later set shaders, interpolaters, texture maps, and lights
	*/
	// check the validility for the input field
	if (!render || !nameList || !valueList)
		return GZ_FAILURE;

	for (int i = 0; i < numAttributes; i++){
		if (nameList[i] == GZ_RGB_COLOR){
			GzColor * colorPtr = (GzColor *)(valueList[i]);
			render->flatcolor[RED] = (*colorPtr)[RED];
			render->flatcolor[GREEN] = (*colorPtr)[GREEN];
			render->flatcolor[BLUE] = (*colorPtr)[BLUE];
		}
		else if (nameList[i] == GZ_INTERPOLATE){
			int * interpModePtr;
			interpModePtr = (int *)(valueList[i]);
			render->interp_mode = *interpModePtr;
		}
		else if (nameList[i] == GZ_AMBIENT_LIGHT){
			GzLight *lptr = (GzLight *)(valueList[i]);
			memcpy(render->ambientlight.color, lptr->color, sizeof(GzColor));
			memcpy(render->ambientlight.direction, lptr->direction, sizeof(GzCoord));
		}
		else if (nameList[i] == GZ_DIRECTIONAL_LIGHT){
			if (render->numlights < MAX_LIGHTS){
				GzLight *lptr = (GzLight *)(valueList[i]);
				// copy light color
				memcpy(render->lights[render->numlights].color, lptr->color, sizeof(GzColor));
				// copy light direction
				memcpy(render->lights[render->numlights].direction, lptr->direction, sizeof(GzCoord));
				render->numlights++;
			}
		}
		else if (nameList[i] == GZ_AMBIENT_COEFFICIENT){
			GzColor * aLight = (GzColor *)valueList[i];
			render->Ka[RED] = (*aLight)[RED];
			render->Ka[GREEN] = (*aLight)[GREEN];
			render->Ka[BLUE] = (*aLight)[BLUE];
		}
		else if (nameList[i] == GZ_DIFFUSE_COEFFICIENT){
			GzColor * dLight = (GzColor *)valueList[i];
			render->Kd[RED] = (*dLight)[RED];
			render->Kd[GREEN] = (*dLight)[GREEN];
			render->Kd[BLUE] = (*dLight)[BLUE];
		}
		else if (nameList[i] == GZ_SPECULAR_COEFFICIENT){
			GzColor * sLight = (GzColor *)valueList[i];
			render->Ks[RED] = (*sLight)[RED];
			render->Ks[GREEN] = (*sLight)[GREEN];
			render->Ks[BLUE] = (*sLight)[BLUE];
		}
		else if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT){
			float * power = (float *)valueList[i];
			render->spec = *power;
		}
		else if (nameList[i] == GZ_TEXTURE_MAP){
			render->tex_fun = (GzTexture)(valueList[i]);
		} 
		else if (nameList[i] == GZ_AASHIFTX)
		{
			float * x = (float *) valueList[i];
			render->shift_x = *x;
		}
		else if (nameList[i] == GZ_AASHIFTY)
		{
			float * y = (float *) valueList[i];
			render->shift_y = *y;
		}

	}

	return GZ_SUCCESS;
}

bool inside_plane(GzRender *render, GzCoord * new_vertexList){
	// Check parameters
	if (!render || !render->display || !new_vertexList)
		return false;

	// all Y coords are < 0
	if( new_vertexList[0][Y] < 0 && new_vertexList[1][Y] < 0 && new_vertexList[2][Y] < 0 )
		return false;
	// all Y coords are > yres
	else if( new_vertexList[0][Y] > render->display->yres && new_vertexList[1][Y] > render->display->yres && new_vertexList[2][Y] > render->display->yres )
		return false;
	// all X coords are < 0
	else if( new_vertexList[0][X] < 0 && new_vertexList[1][X] < 0 && new_vertexList[2][X] < 0 )
		return false;
	// all X coords are > xres
	else if( new_vertexList[0][X] > render->display->xres && new_vertexList[1][X] > render->display->xres && new_vertexList[2][X] > render->display->xres )
		return false;
	else
		return true;
}

bool compute_color(GzRender * render, const GzCoord imageNormal, GzColor vertexColor, GzTextureIndex uvCoord){
	if (render == NULL) return false;
	GzColor KsColor, KdColor, KaColor;
	KsColor[RED] = KsColor[GREEN] = KsColor[BLUE] = KdColor[RED] = KdColor[GREEN] = KdColor[BLUE] = KaColor[RED] = KaColor[GREEN] = KaColor[BLUE] = 0;
	// Shading equation: 
	// Color = (Ks * sumOverLights[ lightIntensity ( R dot E )^s ] ) + (Kd * sumOverLights[lightIntensity (N dot L)] ) + ( Ka Ia ) 
	// Eye direction (0,0,-1)
	GzCoord eyeDir;
	eyeDir[X] = eyeDir[Y] = 0;
	eyeDir[Z] = -1;

	for (int i = 0; i < render->numlights; i++)
	{
		// sign of N dot L and N dot E
		// N dot Eye
		float NdotE = dot_production(imageNormal, eyeDir);
		// N dot L 
		float NdotL = dot_production(imageNormal, render->lights[i].direction);
		GzCoord newimageNormals;
		if (NdotE > 0 && NdotL > 0){
			newimageNormals[X] = imageNormal[X];
			newimageNormals[Y] = imageNormal[Y];
			newimageNormals[Z] = imageNormal[Z];
		}
		else if (NdotL < 0 && NdotE < 0){
			// flip the Normals
			scale_vector(imageNormal, -1, newimageNormals);
			NdotE *= -1;
			NdotL *= -1;
		}
		else continue;
		// R = 2(N dot L)N - L
		GzCoord tmp, tmp1, rRay;
		scale_vector(newimageNormals, 2 * NdotL, tmp);
		add_vector(tmp, render->lights[i].direction, rRay, -1);
		normalization(rRay);

		float RdotE = dot_production(rRay, eyeDir);
		if (RdotE < 0) RdotE = 0;
		// Ks component
		scale_vector(render->lights[i].color, pow(RdotE, render->spec), tmp);
		// Kd component
		scale_vector(render->lights[i].color, NdotL, tmp1);

		add_vector(KsColor, tmp, KsColor, 1);
		add_vector(KdColor, tmp1, KdColor, 1);
	}

	if (render->tex_fun){
		// Gouraud mode
		if (render->interp_mode == GZ_COLOR){
			add_vector(KsColor, KdColor, vertexColor, 1);
			add_vector(KaColor, vertexColor, vertexColor, 1);
		}
		else if (render->interp_mode == GZ_NORMALS){
			GzColor uvColor;
			render->tex_fun(uvCoord[U], uvCoord[V], uvColor);
			// Color = (Ks * sumOverLights[ lightIntensity ( R dot E )^s ] ) + (Kt * sumOverLights[lightIntensity (N dot L)] ) + ( Kt Ia )
			vectors_production(render->Ks, KsColor, KsColor);
			vectors_production(uvColor, KdColor, KdColor);
			vectors_production(uvColor, render->ambientlight.color, KaColor);

			add_vector(KsColor, KdColor, vertexColor, 1);
			add_vector(KaColor, vertexColor, vertexColor, 1);
		}

	}
	else{
		// Color = (Ks * sumOverLights[ lightIntensity ( R dot E )^s ] ) + (Kd * sumOverLights[lightIntensity (N dot L)] ) + ( Ka Ia )
		vectors_production(render->Ks, KsColor, KsColor);
		vectors_production(render->Kd, KdColor, KdColor);
		vectors_production(render->Ka, render->ambientlight.color, KaColor);

		add_vector(KsColor, KdColor, vertexColor, 1);
		add_vector(KaColor, vertexColor, vertexColor, 1);
	}
	return true;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
				  GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane 
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
*/ 
	if (!render || !nameList || !valueList)
		return GZ_FAILURE;

	// prepare for pointers to 3 vertices and 3 normals
	GzCoord * transformTri = (GzCoord *)malloc(3 * sizeof(GzCoord));;
	GzCoord * normalTri = (GzCoord *)malloc(3 * sizeof(GzCoord));
	GzCoord * normalImage = (GzCoord *)malloc(3 * sizeof(GzCoord));
	GzCoord * modelVerts = 0, *modelNormals = 0;
	GzTextureIndex * uvCoords = 0;

	for (int i = 0; i < numParts; i++){
		if (nameList[i] == GZ_NULL_TOKEN){
			continue;
		}
		else if (nameList[i] == GZ_POSITION){
			modelVerts = (GzCoord *)(valueList[i]);
			bool isSeen = true;
			// Transform the 3 vertexes
			for (int i = 0; i < 3; i++)
			{
				TransformVertex(render->Ximage[render->matlevel - 1], modelVerts[i], transformTri[i]);

				// check if it is behind the view plane
				if (transformTri[i][Z] < 0){
					isSeen = false;
					break;
				}
				transformTri[i][0] += render->shift_x;
				transformTri[i][1] += render->shift_y;
			}
			if (!isSeen || !inside_plane(render, transformTri)) continue;
		}
		else if (nameList[i] == GZ_NORMAL){
			modelNormals = (GzCoord *)(valueList[i]);
		}
		else if (nameList[i] == GZ_TEXTURE_INDEX){
			uvCoords = (GzTextureIndex *)(valueList[i]);
		}
	}

	if (!modelVerts || !modelNormals)
	{
		AfxMessageBox("Error: no vertices and/or normals found in GzPutTriangle!!!\n");
		return GZ_FAILURE;
	}
	// now transform the model space vertices and normals into image space (using normals transformation stack) for rasterization use
	for (int i = 0; i < 3; i++){
		TransformVertex(render->Xnorm[render->matlevel - 1], modelNormals[i], normalTri[i]);
		TransformVertex(render->Xnorm[render->matlevel - 1], modelVerts[i], normalImage[i]);
	}

	int list[3] = { 0, 1, 2 };
	// Sort vertex
	float minX, maxX, minY, maxY;
	if (!sort_tri_vertex(&minX, &maxX, &minY, &maxY, transformTri, list))
		return false;

	// Clockwise
	// Sort edges
	float hy = transformTri[list[0]][1];
	float hx = transformTri[list[0]][0];
	float my = transformTri[list[1]][1];
	float mx = transformTri[list[1]][0];
	float ly = transformTri[list[2]][1];
	float lx = transformTri[list[2]][0];
	float xx = ((lx * hy - hx * ly) - my * (hx - lx)) / (ly - hy);
	if (xx > mx){
		int tmp = list[1];
		list[1] = list[2];
		list[2] = tmp;
	}

	// Calculate coefficents
	float alpha[3][3];
	if (!compute_coefficient(alpha, transformTri, list))
		return false;


	// Interpolate Z
	GzCoord cprod;
	if (!cross_prod(cprod, transformTri, list))
		return false;

	float cA, cB, cC, cD;
	cA = cprod[X];
	cB = cprod[Y];
	cC = cprod[Z];
	cD = -cA * transformTri[0][X] - cB * transformTri[0][Y] - cC * transformTri[0][Z];

	// interpolate values for Gouraud shading:
	GzColor colorA, colorB, colorC, colorD;
	// interpolate values for Phong shading:
	GzCoord normA, normB, normC, normD;
	// Color for NONE shading
	GzColor NColor;

	// interpolate values for texture mapping
	GzTextureIndex texture_A, texture_B, texture_C, texture_D;

	// interpolate intermediates
	GzCoord interp1, interp2, interp3;
	// The same approach as Z interpolate
	interp1[X] = transformTri[list[1]][X] - transformTri[list[0]][X];
	interp1[Y] = transformTri[list[1]][Y] - transformTri[list[0]][Y];
	interp2[X] = transformTri[list[2]][X] - transformTri[list[1]][X];
	interp2[Y] = transformTri[list[2]][Y] - transformTri[list[1]][Y];

	// interpolate values for texures
	GzTextureIndex perCoords[3];
	for (int i = 0; i < 3; i++)
	{
		float z = -(cA * transformTri[i][X] + cB * transformTri[i][Y] + cD) / cC;

		// Perspective correction
		perCoords[i][U] = uvCoords[i][U] * (INT_MAX - z) / INT_MAX;
		perCoords[i][V] = uvCoords[i][V] * (INT_MAX - z) / INT_MAX;
	}
	for (int k = 0; k < 2; k++){
		interp1[Z] = perCoords[list[1]][k] - perCoords[list[0]][k];
		interp2[Z] = perCoords[list[2]][k] - perCoords[list[1]][k];
		cross_production(interp1, interp2, interp3);
		texture_A[k] = interp3[X];
		texture_B[k] = interp3[Y];
		texture_C[k] = interp3[Z];
		texture_D[k] = -(texture_A[k] * transformTri[list[0]][X] + texture_B[k] * transformTri[list[0]][Y] + texture_C[k] * perCoords[list[0]][k]);
	}

	switch (render->interp_mode){
		// Ground mode
		// We regard colorPlane as index z, and do the same interpolation for the colorPlane
	case GZ_COLOR:
		GzColor vertexColors[3];
		for (int i = 0; i < 3; i++)
			compute_color(render, normalTri[i], vertexColors[i], NULL);
		// interpolate coefficients for each color component
		// (X,Y,Z)0 X (X,Y,Z)1 = (A,B,C) 
		// Ax + By + Cz + D = 0  <==> Ax + By + Cz + D = 0 => z = -( Ax + By + D ) / C
		for (int i = 0; i < 3; i++)
		{
			interp1[Z] = vertexColors[list[1]][i] - vertexColors[list[0]][i];
			interp2[Z] = vertexColors[list[2]][i] - vertexColors[list[1]][i];
			cross_production(interp1, interp2, interp3);
			colorA[i] = interp3[X];
			colorB[i] = interp3[Y];
			colorC[i] = interp3[Z];
			colorD[i] = -(colorA[i] * transformTri[list[0]][X] + colorB[i] * transformTri[list[0]][Y] + colorC[i] * vertexColors[list[0]][i]);
		}
		break;
		// Phong mode
	case GZ_NORMALS:
		// interpolate coefficients for vertex normals
		for (int i = 0; i < 3; i++){
			interp1[Z] = normalTri[list[1]][i] - normalTri[list[0]][i];
			interp2[Z] = normalTri[list[2]][i] - normalTri[list[1]][i];
			cross_production(interp1, interp2, interp3);
			normA[i] = interp3[X];
			normB[i] = interp3[Y];
			normC[i] = interp3[Z];
			normD[i] = -(normA[i] * transformTri[list[0]][X] + normB[i] * transformTri[list[0]][Y] + normC[i] * normalTri[list[0]][i]);
		}
		break;
		// None mode
	case GZ_NONE:
		GzCoord nNorms;
		cross_prod(nNorms, normalImage, list);
		normalization(nNorms);
		compute_color(render, nNorms, NColor, NULL);
		break;
	}
	
	// LEE Rasterization Method
	for (int x = max(int(minX), 0); x <= min(int(maxX), render->display->xres - 1); x++)
	{
		for (int y = max(int(minY), 0); y <= min(int(maxY), render->display->yres - 1); y++){
			if (inside_tri(alpha, x, y)){
				bool onEdge = false;
				// Check if it is on the edge of triangle: if so do not shade it
				// Ax + By + C = 0
				for (int i = 0; i < 3; i++)
					if (alpha[i][0] * x + alpha[i][1] * y + alpha[i][2] == 0){
						onEdge = true;
						break;
					}

					// Ax + By + Cz + D = 0 => z = -( Ax + By + D ) / C
					float zval = -(cA * x + cB * y + cD) / cC;

					// index z is behind the horizon or the pixel is on one edge
					if (zval < 0 || onEdge)
						continue;

					GzIntensity r, g, b, a;
					GzDepth	z;
					GzGetDisplay(render->display, x, y, &r, &g, &b, &a, &z);

					if (zval <= z){
						GzColor color;
						switch (render->interp_mode){
							// Ground shading
						case GZ_COLOR:
							for (int i = 0; i < 3; i++)
								color[i] = -(colorA[i] * x + colorB[i] * y + colorD[i]) / colorC[i];

							if (render->tex_fun){
								GzColor uvColor;
								GzTextureIndex texture;
								for (int k = 0; k < 2; k++)
								{
									texture[k] = -(texture_A[k] * x + texture_B[k] * y + texture_D[k]) / texture_C[k];

									// Perspective correction
									texture[k] /= (INT_MAX - zval) / INT_MAX;
								}

								render->tex_fun(texture[U], texture[V], uvColor);
								vectors_production(color, uvColor, color);
							}
							break;
							// Phong shading
						case GZ_NORMALS:
							GzCoord interpNorm;
							for (int i = 0; i < 3; i++)
								interpNorm[i] = -(normA[i] * x + normB[i] * y + normD[i]) / normC[i];
							normalization(interpNorm);
							GzTextureIndex texture;
							for (int k = 0; k < 2; k++)
							{
								texture[k] = -(texture_A[k] * x + texture_B[k] * y + texture_D[k]) / texture_C[k];

								// Perspective correction
								texture[k] /= (INT_MAX - zval) / INT_MAX;
							}
							compute_color(render, interpNorm, color, texture);
							break;
							// NONE shading
						case GZ_NONE:
							color[RED] = NColor[RED];
							color[GREEN] = NColor[GREEN];
							color[BLUE] = NColor[BLUE];
							break;
						default:
							color[RED] = render->flatcolor[RED];
							color[GREEN] = render->flatcolor[GREEN];
							color[BLUE] = render->flatcolor[BLUE];
							break;
						}
						GzPutDisplay(render->display, x, y, ctoi(color[RED]), ctoi(color[GREEN]), ctoi(color[BLUE]), a, (GzDepth)zval);
					}
			}
		}
	}

	return GZ_SUCCESS;
}
