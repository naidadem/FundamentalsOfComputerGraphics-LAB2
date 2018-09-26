#include <stdlib.h>
#include "mesh.h"
#include <stdio.h>

float rnd() {
	return 2.0f * float(rand()) / float(RAND_MAX) - 1.0f;
}


void insertModel(Mesh **list, int nv, float * vArr, int nt, int * tArr, float scale) {
	Mesh * mesh = (Mesh *) malloc(sizeof(Mesh));
	mesh->nv = nv;
	mesh->nt = nt;	
	mesh->vertices = (Vector *) malloc(nv * sizeof(Vector));
	mesh->vnorms = (Vector *)malloc(nv * sizeof(Vector));
	mesh->triangles = (Triangle *) malloc(nt * sizeof(Triangle));
	Vector *normals = (Vector *)malloc(nt * sizeof(Vector));
	
	// set mesh vertices
	for (int i = 0; i < nv; i++) {
		mesh->vertices[i].x = vArr[i*3] * scale;
		mesh->vertices[i].y = vArr[i*3+1] * scale;
		mesh->vertices[i].z = vArr[i*3+2] * scale;

		mesh->vnorms[i].x = 0;
		mesh->vnorms[i].y = 0;
		mesh->vnorms[i].z = 0;
	
	}

	// set mesh triangles

	
	Vector u,v;

	for (int i = 0; i < nt; i++) {
		mesh->triangles[i].vInds[0] = tArr[i * 3];
		mesh->triangles[i].vInds[1] = tArr[i * 3 + 1];
		mesh->triangles[i].vInds[2] = tArr[i * 3 + 2];
	}

	for (int i = 0; i < nt; i++)
	{
		normals[i] = { 0,0,0 };
		u.x = mesh->vertices[mesh->triangles[i].vInds[1]].x - mesh->vertices[mesh->triangles[i].vInds[0]].x;
		u.y = mesh->vertices[mesh->triangles[i].vInds[1]].y - mesh->vertices[mesh->triangles[i].vInds[0]].y;
		u.z = mesh->vertices[mesh->triangles[i].vInds[1]].z - mesh->vertices[mesh->triangles[i].vInds[0]].z;

		v.x = mesh->vertices[mesh->triangles[i].vInds[2]].x - mesh->vertices[mesh->triangles[i].vInds[0]].x;
		v.y = mesh->vertices[mesh->triangles[i].vInds[2]].y - mesh->vertices[mesh->triangles[i].vInds[0]].y;
		v.z = mesh->vertices[mesh->triangles[i].vInds[2]].z - mesh->vertices[mesh->triangles[i].vInds[0]].z;


		normals[i] = Normalize(CrossProduct(u, v));
	}

	for (int i = 0; i < nv; i++)
	{
		Vector nulti = { 0.0,0.0,0.0 };
		for (int j = 0; j < nt; j++)
		{
			if (i == mesh->triangles[j].vInds[0] || i == mesh->triangles[j].vInds[1] || i == mesh->triangles[j].vInds[2])
				nulti = Add(nulti, normals[j]);
		}
		mesh->vnorms[i] = Normalize(nulti);
	}

	mesh->scaling.x = 1.0;
	mesh->scaling.y = 1.0;
	mesh->scaling.z= 1.0;

	// Assignment 1: 
	// Calculate and store suitable vertex normals for the mesh here.
	// Replace the code below that simply sets some arbitrary normal values	
	//for (int i = 0; i < nv; i++) {


	//	mesh->vnorms[i].x = rnd();
	//	mesh->vnorms[i].y = rnd();
	//	mesh->vnorms[i].z = rnd();
	//}

	mesh->next = *list;
	*list = mesh;	
}
