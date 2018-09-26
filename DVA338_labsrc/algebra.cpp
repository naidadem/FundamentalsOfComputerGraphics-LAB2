#define _USE_MATH_DEFINES // To get M_PI defined
#include <math.h>
#include <stdio.h>
#include "algebra.h"
#include "mesh.h"
#include <cmath>

Vector CrossProduct(Vector a, Vector b) {
	Vector v = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
	return v;
}

float DotProduct(Vector a, Vector b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector Subtract(Vector a, Vector b) {
	Vector v = { a.x-b.x, a.y-b.y, a.z-b.z };
	return v;
}    

Vector Add(Vector a, Vector b) {
	Vector v = { a.x+b.x, a.y+b.y, a.z+b.z };
	return v;
}    

float Length(Vector a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

Vector Normalize(Vector a) {
	float len = Length(a);
	Vector v = { a.x/len, a.y/len, a.z/len };
	return v;
}

Vector ScalarVecMul(float t, Vector a) {
	Vector b = { t*a.x, t*a.y, t*a.z };
	return b;
}

HomVector MatVecMul(Matrix a, Vector b) {
	HomVector h;
	h.x = b.x*a.e[0] + b.y*a.e[4] + b.z*a.e[8] + a.e[12];
	h.y = b.x*a.e[1] + b.y*a.e[5] + b.z*a.e[9] + a.e[13];
	h.z = b.x*a.e[2] + b.y*a.e[6] + b.z*a.e[10] + a.e[14];
	h.w = b.x*a.e[3] + b.y*a.e[7] + b.z*a.e[11] + a.e[15];
	return h;
}

Vector Homogenize(HomVector h) {
	Vector a;
	if (h.w == 0.0) {
		fprintf(stderr, "Homogenize: w = 0\n");
		a.x = a.y = a.z = 9999999;
		return a;
	}
	a.x = h.x / h.w;
	a.y = h.y / h.w;
	a.z = h.z / h.w;
	return a;
}

Matrix MatMatMul(Matrix a, Matrix b) {
	Matrix c;
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			c.e[j*4+i] = 0.0;
			for (k = 0; k < 4; k++)
				c.e[j*4+i] += a.e[k*4+i] * b.e[j*4+k];
		}
	}
	return c;
}

void PrintVector(char *name, Vector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z);
}

void PrintHomVector(char *name, HomVector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z, a.w);
}

void PrintMatrix(char *name, Matrix a) { 
	int i,j;

	printf("%s:\n", name);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%6.5lf ", a.e[j*4+i]);
		}
		printf("\n");
	}
}

void rotationX(Matrix& V, float rotX){
	Matrix R;
	R.e[0] = 1; R.e[4] = 0; R.e[8] = 0; R.e[12] = 0;
	R.e[1] = 0; R.e[5] = cos(rotX); R.e[9] = -sin(rotX); R.e[13] = 0;
	R.e[2] = 0; R.e[6] = sin(rotX); R.e[10] = cos(rotX); R.e[14] = 0;
	R.e[3] = 0; R.e[7] = 0; R.e[11] = 0; R.e[15] = 1;

	V = MatMatMul(V, R);
}

void rotationY(Matrix& V, float rotY) {
	Matrix R;
	R.e[0] = cos(rotY); R.e[4] = 0; R.e[8] = sin(rotY); R.e[12] = 0;
	R.e[1] = 0; R.e[5] = 1; R.e[9] = 0; R.e[13] = 0;
	R.e[2] = -sin(rotY); R.e[6] = 0; R.e[10] = cos(rotY); R.e[14] = 0;
	R.e[3] = 0; R.e[7] = 0; R.e[11] = 0; R.e[15] = 1;

	V = MatMatMul(V, R);
}

void rotationZ(Matrix& V, float rotZ) {
	Matrix R;
	R.e[0] = cos(rotZ); R.e[4] = -sin(rotZ); R.e[8] = 0; R.e[12] = 0;
	R.e[1] = sin(rotZ); R.e[5] = cos(rotZ); R.e[9] = 0; R.e[13] = 0;
	R.e[2] = 0; R.e[6] = 0; R.e[10] = 1; R.e[14] = 0;
	R.e[3] = 0; R.e[7] = 0; R.e[11] = 0; R.e[15] = 1;

	V = MatMatMul(V, R);
}

Matrix rotate(Matrix& W,double rotX, double rotY, double rotZ) {
	Matrix R1, R2, R3, R;
	for (int i = 0; i < 16; i++)
	{
		R.e[i] = 0.0f;
	}
	R1.e[0] = 1; R1.e[4] = 0; R1.e[8] = 0; R1.e[12] = 0;
	R1.e[1] = 0; R1.e[5] = cos(rotX); R1.e[9] = -sin(rotX); R1.e[13] = 0;
	R1.e[2] = 0; R1.e[6] = sin(rotX); R1.e[10] = cos(rotX); R1.e[14] = 0;
	R1.e[3] = 0; R1.e[7] = 0; R1.e[11] = 0; R1.e[15] = 1;

	R2.e[0] = cos(rotY); R2.e[4] = 0; R2.e[8] = sin(rotY); R2.e[12] = 0;
	R2.e[1] = 0; R2.e[5] = 1; R2.e[9] = 0; R2.e[13] = 0;
	R2.e[2] = -sin(rotY); R2.e[6] = 0; R2.e[10] = cos(rotY); R2.e[14] = 0;
	R2.e[3] = 0; R2.e[7] = 0; R2.e[11] = 0; R2.e[15] = 1;

	R3.e[0] = cos(rotZ); R3.e[4] = -sin(rotZ); R3.e[8] = 0; R3.e[12] = 0;
	R3.e[1] = sin(rotZ); R3.e[5] = cos(rotZ); R3.e[9] = 0; R3.e[13] = 0;
	R3.e[2] = 0; R3.e[6] = 0; R3.e[10] = 1; R3.e[14] = 0;
	R3.e[3] = 0; R3.e[7] = 0; R3.e[11] = 0; R3.e[15] = 1;

	R = MatMatMul(R3, MatMatMul(R2, R1));
	return MatMatMul(R, W);
}

Matrix scaling(Matrix& W, float scalX, float scalY, float scalZ) {
	Matrix S;

	S.e[0] = scalX; S.e[4] = 0; S.e[8] = 0; S.e[12] = 0;
	S.e[1] = 0; S.e[5] = scalY; S.e[9] = 0; S.e[13] = 0;
	S.e[2] = 0; S.e[6] = 0; S.e[10] = scalZ; S.e[14] = 0;
	S.e[3] = 0; S.e[7] = 0; S.e[11] = 0; S.e[15] = 1;

	return S;
}

Matrix translation(Matrix& V, float tranX, float tranY, float tranZ) {
	Matrix T;
	T.e[0] = 1; T.e[4] = 0; T.e[8] = 0; T.e[12] = tranX;
	T.e[1] = 0; T.e[5] = 1; T.e[9] = 0; T.e[13] = tranY;
	T.e[2] = 0; T.e[6] = 0; T.e[10] = 1; T.e[14] = tranZ;
	T.e[3] = 0; T.e[7] = 0; T.e[11] = 0; T.e[15] = 1;

	V = MatMatMul(T, V);
	return V;
}


void projectionParallel(Matrix& P, Vector *v, int nv, float near, float far) {
	float left = v[0].x; 
	float right = v[0].x;
	float bottom = v[0].y;
	float top = v[0].y;

	for (int i = 0; i < nv; i++)
	{
		if (v[i].x < left)
			left = v[i].x;
		if (v[i].x > right)
			right = v[i].x;
		if (v[i].y < bottom)
			bottom = v[i].y;
		if (v[i].y > top)
			top =v[i].y;
	}


	P.e[0] = 2/(right-left); P.e[4] = 0; P.e[8] = 0; P.e[12] = -((right+left)/(right-left));
	P.e[1] = 0; P.e[5] = 2/(top-bottom); P.e[9] = 0; P.e[13] = -((top+bottom)/(top-bottom));
	P.e[2] = 0; P.e[6] = 0; P.e[10] = 2/(near-far); P.e[14] = -((far+near)/(far-near));
	P.e[3] = 0; P.e[7] = 0; P.e[11] = 0; P.e[15] = 1;
}

void projectionPerspective(Matrix& P, float near, float far, float fov, int screenWidth, int screenHeight) {
	float aspect = (float)screenWidth / screenHeight;

	P.e[0] = (1.0/tan(fov/2/180*M_PI))/aspect; P.e[4] = 0; P.e[8] = 0; P.e[12] = 0;
	P.e[1] = 0; P.e[5] = 1.0 / tan(fov/2/180*M_PI); P.e[9] = 0; P.e[13] = 0;
	P.e[2] = 0; P.e[6] = 0; P.e[10] = (far+near)/(near-far); P.e[14] = (2*far*near)/(near-far);
	P.e[3] = 0; P.e[7] = 0; P.e[11] = -1; P.e[15] = 0;
}