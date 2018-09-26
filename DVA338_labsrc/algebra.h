#ifndef _ALGEBRA_H_
#define _ALGEBRA_H_


#pragma once
typedef struct { float x, y, z; } Vector;
typedef struct { float x, y, z, w; } HomVector;

/* Column-major order are used for the matrices here to be compatible with OpenGL.
** The indices used to access elements in the matrices are shown below.
**  _                _
** |                  |
** |   0   4   8  12  |
** |                  |
** |   1   5   9  13  |
** |                  |
** |   2   6  10  14  |
** |                  |
** |   3   7  11  15  |
** |_                _|
*/
typedef struct matrix { float e[16]; } Matrix;

Vector Add(Vector a, Vector b);
Vector Subtract(Vector a, Vector b);
Vector CrossProduct(Vector a, Vector b);
float DotProduct(Vector a, Vector b);
float Length(Vector a);
Vector Normalize(Vector a);
Vector ScalarVecMul(float t, Vector a);
HomVector MatVecMul(Matrix a, Vector b);
Vector Homogenize(HomVector a);
Matrix MatMatMul(Matrix a, Matrix b);
void PrintMatrix(char *name, Matrix m);
void PrintVector(char *name, Vector v);
void PrintHomVector(char *name, HomVector h);

void rotationX(Matrix& V, float rotX);
void rotationY(Matrix& V, float rotY);
void rotationZ(Matrix& V, float rotZ);

void projectionParallel(Matrix& V, Vector *v, int nv, float near, float far);
void projectionPerspective(Matrix& V, float near, float far, float fov, int screenWidht, int screenHight);

Matrix rotate(Matrix& R, double rotX, double rotY, double rotZ);

Matrix scaling(Matrix& S, float scalX, float scalY, float scalZ);

Matrix translation(Matrix& T, float tranX, float tranY, float tranZ);

#endif

