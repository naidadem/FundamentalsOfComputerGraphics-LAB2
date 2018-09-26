//#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES // To get M_PI defined

#include <stdio.h>
#include <iostream>
#include "cmath"
#include <math.h>

using namespace std;

#include <glew.h>
#include <freeglut.h>

#include "algebra.h"
#include "shaders.h"
#include "mesh.h"


int screen_width = 1024;
int screen_height = 768;

int total = 0;
int current = 0;
int sFlag = 0; 

Mesh *meshList = NULL; // Global pointer to linked list of triangle meshes
Mesh *lightList = NULL; // for lights

Camera cam = {{0,0,20}, {0,0,0}, 60, 1, 10000}; // Setup the global camera parameters


// For light
#define NUM_OF_LIGHTS 2
static float g_lightPosition[NUM_OF_LIGHTS * 3]; //imamo niz od 6 jer imamo dva lighta
static float g_lightRotation; //sta sam ustvari uradila sa ovim 

GLuint shprg; // Shader program id


// Global transform matrices
// V is the view transform
// P is the projection transform
// PV = P * V is the combined view-projection transform
Matrix V, P, PV;
bool projectionType=false;


void prepareShaderProgram(const char ** vs_src, const char ** fs_src) {
	GLint success = GL_FALSE;

	shprg = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in vertex shader!\n");
	else printf("Vertex shader compiled successfully!\n");

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in fragment shader!\n");
	else printf("Fragment shader compiled successfully!\n");

	glAttachShader(shprg, vs);
	glAttachShader(shprg, fs);
	glLinkProgram(shprg);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(shprg, GL_LINK_STATUS, &isLinked);
	if (!isLinked) printf("Link error in shader program!\n");
	else printf("Shader program linked successfully!\n");
}

void prepareMesh(Mesh *mesh) {
	int sizeVerts = mesh->nv * 3 * sizeof(float);
	int sizeCols  = mesh->nv * 3 * sizeof(float);
	int sizeTris = mesh->nt * 3 * sizeof(int);
	
	// For storage of state and other buffer objects needed for vertex specification
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Allocate VBO and load mesh data (vertices and normals)
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeVerts + sizeCols, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerts, (void *)mesh->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeVerts, sizeCols, (void *)mesh->vnorms);

	// Allocate index buffer and load mesh indices
	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeTris, (void *)mesh->triangles, GL_STATIC_DRAW);

	// Define the format of the vertex data
	GLint vPos = glGetAttribLocation(shprg, "vPos");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Define the format of the vertex data 
	GLint vNorm = glGetAttribLocation(shprg, "vNorm");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)(mesh->nv * 3 *sizeof(float)));

	glBindVertexArray(0);

}

void renderMesh(Mesh *mesh) {
	
	// Assignment 1: Apply the transforms from local mesh coordinates to world coordinates here
	// Combine it with the viewing transform that is passed to the shader below

	Matrix W,M;
	for (int i = 0; i < 16; i++)
	{
		W.e[i] = 0.0f;
	}

	W = scaling(W, mesh->scaling.x, mesh->scaling.y, mesh->scaling.z);
	W=rotate(W, mesh->rotation.x, mesh->rotation.y, mesh->rotation.z);
	W = translation(W, mesh->translation.x, mesh->translation.y, mesh->translation.z);

	M = MatMatMul(PV, W);
	Matrix VM = MatMatMul(V, W);


	// For Shading, Pass the viewing transform to the shader
	GLint loc_V = glGetUniformLocation(shprg, "V"); //salje matrice da se izvrse na grafickoj GPU, sa CPU na GPU
	glUniformMatrix4fv(loc_V, 1, GL_FALSE, V.e);

	GLint loc_VM = glGetUniformLocation(shprg, "VM"); //isto
	glUniformMatrix4fv(loc_VM, 1, GL_FALSE, VM.e);

	// Pass the viewing transform to the shader
	GLint loc_PV = glGetUniformLocation(shprg, "PV");
	glUniformMatrix4fv(loc_PV, 1, GL_FALSE, M.e);


	// Select current resources 
	glBindVertexArray(mesh->vao);
	
	// To accomplish wireframe rendering (can be removed to get filled triangles)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);



	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 


	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);


	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL); 
	glBindVertexArray(0);
}


void transferInfoToShader() { //da bismo poslali vrijednosti u oba shadera(vertex i fragment)
	//for shading, transfer light information to shader (Multiple lights)
	glUniform3f(glGetUniformLocation(shprg, "lights[0].position"), g_lightPosition[0], g_lightPosition[1], g_lightPosition[2]);
	glUniform3f(glGetUniformLocation(shprg, "lights[0].ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(shprg, "lights[0].diffuse"), 1.f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(shprg, "lights[0].specular"), 1.f, 1.f, 1.f);
	glUniform1f(glGetUniformLocation(shprg, "lights[0].constant"), 1.f);
	glUniform1f(glGetUniformLocation(shprg, "lights[0].linear"), 0.009f);
	glUniform1f(glGetUniformLocation(shprg, "lights[0].quadratic"), 0.000032f);

	glUniform3f(glGetUniformLocation(shprg, "lights[1].position"), g_lightPosition[3], g_lightPosition[4], g_lightPosition[5]);
	glUniform3f(glGetUniformLocation(shprg, "lights[1].ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(shprg, "lights[1].diffuse"), 0.0f, 1.0f, 0.0f);
	glUniform3f(glGetUniformLocation(shprg, "lights[1].specular"), 1.f, 1.f, 1.f);
	glUniform1f(glGetUniformLocation(shprg, "lights[1].constant"), 1.f);
	glUniform1f(glGetUniformLocation(shprg, "lights[1].linear"), 0.009f);
	glUniform1f(glGetUniformLocation(shprg, "lights[1].quadratic"), 0.000032f);

	glUniform3f(glGetUniformLocation(shprg, "viewPos"), cam.position.x, cam.position.y, cam.position.z);

	glUniform3f(glGetUniformLocation(shprg, "material.ambient"), 0.0215,0.1745,0.0215); //ove vrijednosti ne bi smjele da budu vece od 1 kad se saberu(u ovom slucaju je zbir 3), ali ne pravi problem jer imamo clamp funkciju
	glUniform3f(glGetUniformLocation(shprg, "material.diffuse"), 0.07568,0.61424,0.07568);
	glUniform3f(glGetUniformLocation(shprg, "material.specular"), 0.633,0.727811,0.633);
	glUniform1f(glGetUniformLocation(shprg, "material.shininess"), 0.6);
}


void display(void) {
	Mesh *mesh;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	// Assignment 1: Calculate the transform to view coordinates yourself 	
	// The matrix V should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 
	V.e[0] = 1.0f; V.e[4] = 0.0f; V.e[ 8] = 0.0f; V.e[12] =  cam.position.x;
	V.e[1] = 0.0f; V.e[5] = 1.0f; V.e[ 9] = 0.0f; V.e[13] =  cam.position.y;
	V.e[2] = 0.0f; V.e[6] = 0.0f; V.e[10] = 1.0f; V.e[14] = -cam.position.z;
	V.e[3] = 0.0f; V.e[7] = 0.0f; V.e[11] = 0.0f; V.e[15] =   1.0f;

	rotationX(V,cam.rotation.x);
	rotationY(V, cam.rotation.y);
	rotationZ(V, cam.rotation.z);


	// Assignment 1: Calculate the projection transform yourself 	
	// The matrix P should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 	
	//P.e[0] = 1.299038f; P.e[4] = 0.000000f; P.e[ 8] =  0.000000f; P.e[12] =  0.0f;
	//P.e[1] = 0.000000f; P.e[5] = 1.732051f; P.e[ 9] =  0.000000f; P.e[13] =  0.0f;
	//P.e[2] = 0.000000f; P.e[6] = 0.000000f; P.e[10] = -1.000200f; P.e[14] = -2.000200f;
	//P.e[3] = 0.000000f; P.e[7] = 0.000000f; P.e[11] = -1.000000f; P.e[15] =  0.0f;


	// Select the shader program to be used during rendering 
	glUseProgram(shprg);

	// Render all meshes in the scene
	mesh = meshList;

	if (projectionType) 
		projectionParallel(P, mesh->vertices, mesh->nv, cam.nearPlane, cam.farPlane);
	else
		projectionPerspective(P, cam.nearPlane, cam.farPlane, cam.fov, screen_width, screen_height);

	// This finds the combined view-projection matrix
	PV = MatMatMul(P, V);

	while (mesh != NULL) {
		renderMesh(mesh);
		mesh = mesh->next;
	}

	transferInfoToShader();
	//for lights.

	glFlush();
}

void changeSize(int w, int h) {
	screen_width = w;
	screen_height = h;
	glViewport(0, 0, screen_width, screen_height);
}

void chooseShader() {
	// Compile and link the given shader program (vertex shader and fragment shader)
	//prepareShaderProgram(vs_n2c_src, fs_ci_src); 
	glDeleteShader(shprg);
	// Compile and link the given shader program (vertex shader and fragment shader)
	if (sFlag == 0) // nothing
		prepareShaderProgram(vs_n2c_src, fs_ci_src);
	else if (sFlag == 1) // Cartoon shading
		prepareShaderProgram(vs_n2c_src_toon, fs_ci_src_toon);
	else if (sFlag == 2) // Gouraud shading
		prepareShaderProgram(vs_n2c_src_gouraud, fs_ci_src_gouraud);
	else if (sFlag == 3) // Phong shading
		prepareShaderProgram(vs_n2c_src_phong, fs_ci_src_phong);
}


void keypress(unsigned char key, int x, int y) {
	int lightFlag = 0;
	Mesh *currentModel=meshList;
	int i = 0;
	while (i != current) {
		currentModel = currentModel->next;
		i++;
	}
	switch(key) {
	case 'x':
		cam.position.x -= 0.5f;
		break;
	case 'X':
		cam.position.x += 0.5f;
		break;
	case 'y':
		cam.position.y -= 0.7f;
		break;
	case 'Y':
		cam.position.y += 0.7f;
		break;
	case 'z':
		cam.position.z -= 0.2f;
		break;
	case 'Z':
		cam.position.z += 0.2f;
		break;
	case 'i':
		cam.rotation.x -= 0.7853981634;
		break;
	case 'I':
		cam.rotation.x += 0.7853981634;
		break;
	case 'j':
		cam.rotation.y -= 0.7853981634;
		break;
	case 'J':
		cam.rotation.y += 0.7853981634;
		break;
	case 'k':
		cam.rotation.z -= 0.7853981634;
		break;
	case 'K':
		cam.rotation.z += 0.7853981634;
		break;
	case 'l':
		projectionType = true;
		break;
	case 'L':
		projectionType = false;
		break;
	case 'Q':
	case 'q':		
		glutLeaveMainLoop();
		break;
	case 'o':
		currentModel->rotation.x -= 0.7853981634;
		break;
	case 'O':
		currentModel->rotation.x += 0.7853981634;
		break;
	case 'p':
		currentModel->rotation.y -= 0.25;
		break;
	case 'P':
		currentModel->rotation.y += 0.25;
		break;
	case 'r':
		currentModel->rotation.z -= 0.25;
		break;
	case 'R':
		currentModel->rotation.z += 0.7853981634;
		break;
	case 's':
		currentModel->translation.x-= 0.5f;
		break;
	case 'S':
		currentModel->translation.x += 0.5f;
		break;
	case 't':
		currentModel->translation.y -= 0.7f;
		break;
	case 'T':
		currentModel->translation.y += 0.7f;
		break;
	case 'u':
		currentModel->translation.z -= 0.2f;
		break;
	case 'U':
		currentModel->translation.z += 0.2f;
		break;
	case 'a':
		currentModel->scaling.x -= 0.1f;
		currentModel->scaling.y -= 0.1f;
		currentModel->scaling.z -= 0.1f;
		break;
	case 'A':
		currentModel->scaling.x += 0.1f;
		currentModel->scaling.y += 0.1f;
		currentModel->scaling.z += 0.1f;
		break;
	case 'd':
		current = (current + 1) % total;
		break;
	case 'e':
		sFlag++;
		sFlag %= 4;
		chooseShader();
		break;
	case 'E':
		sFlag--;
		if (sFlag < 0)
			sFlag = 3;
		chooseShader();
		break;

	}
	glutPostRedisplay();
}

void init(void) {
	// for light, do the first cycle to initialize positions
	chooseShader();
	g_lightRotation = 0.0f;

	for (int i = 0; i < NUM_OF_LIGHTS; i++) {
		g_lightPosition[i * 3 + 0] = cam.position.x;
		g_lightPosition[i * 3 + 1] = cam.position.y;
		g_lightPosition[i * 3 + 2] = cam.position.z;
	}

	// Setup OpenGL buffers for rendering of the meshes
	Mesh * mesh = meshList;
	while (mesh != NULL) {
		total++;
		prepareMesh(mesh);
		mesh = mesh->next;
	}	

}

void cleanUp(void) {	
	printf("Running cleanUp function... ");
	// Free openGL resources
	// ...

	// Free meshes
	// ...
	printf("Done!\n\n");
}

// Include data for some triangle meshes (hard coded in struct variables)
#include "./models/mesh_bunny.h"
#include "./models/mesh_cow.h"
#include "./models/mesh_cube.h"
#include "./models/mesh_frog.h"
#include "./models/mesh_knot.h"
#include "./models/mesh_sphere.h"
#include "./models/mesh_teapot.h"
#include "./models/mesh_triceratops.h"


int main(int argc, char **argv) {

	// Setup freeGLUT	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("DVA338 Programming Assignments");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Specify your preferred OpenGL version and profile
	glutInitContextVersion(4, 5);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);	
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Uses GLEW as OpenGL Loading Library to get access to modern core features as well as extensions
	GLenum err = glewInit();
	if (GLEW_OK != err) { fprintf(stdout, "Error: %s\n", glewGetErrorString(err)); return 1; }

	// Output OpenGL version info
	fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "OpenGL version: %s\n", (const char *)glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n\n", glGetString(GL_VENDOR));


	// Insert the 3D models you want in your scene here in a linked list of meshes
	// Note that "meshList" is a pointer to the first mesh and new meshes are added to the front of the list	
	//insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	//insertModel(&meshList, triceratops.nov, triceratops.verts, triceratops.nof, triceratops.faces, 3.0);
	//insertModel(&meshList, bunny.nov, bunny.verts, bunny.nof, bunny.faces, 60.0);	
	//insertModel(&meshList, cube.nov, cube.verts, cube.nof, cube.faces, 5.0);
	//insertModel(&meshList, frog.nov, frog.verts, frog.nof, frog.faces, 2.5);
	insertModel(&meshList, knot.nov, knot.verts, knot.nof, knot.faces, 0.5);
	//insertModel(&meshList, sphere.nov, sphere.verts, sphere.nof, sphere.faces, 12.0);
	//insertModel(&meshList, teapot.nov, teapot.verts, teapot.nof, teapot.faces, 3.0);
	
	
	init();
	//glTranslatef(0.0, 0.0, -100);
	glutMainLoop();

	cleanUp();	
	return 0;
}
