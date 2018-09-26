#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"

typedef struct _Triangle {
	int vInds[3]; //vertex indices
} Triangle;

typedef struct _Mesh { 
	int nv;				
	Vector *vertices;
	Vector *vnorms;
	int nt;				
	Triangle *triangles;
	struct _Mesh *next;

	//new attributes for 1.5 : scene composition
	double rotX=0.0;
	double rotY=0.0;
	double rotZ=0.0;
	double tranX=0.0;
	double tranY=0.0;
	double tranZ=0.0;
	double scalX=1.0f;
	double scalY=1.0f;
	double scalZ=1.0f;
	Vector scaling = { 1.0,1.0,1.0 };
	Vector translation = { 0.0,0.0,0.0 };
	Vector rotation = { 0.0,0.0,0.0 };
		
	unsigned int vbo, ibo, vao; // OpenGL handles for rendering
} Mesh;

typedef struct _Camera {
	Vector position;
	Vector rotation;
	double fov; 
	double nearPlane; 
	double farPlane; 
} Camera;

void insertModel(Mesh ** objlist, int nv, float * vArr, int nt, int * tArr, float scale = 1.0);

#endif
