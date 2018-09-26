
#include <stdio.h>
#include <glew.h>
#include <freeglut.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
// Simple vertex shader treating vertex normals as RGB colors
static const char * vs_n2c_src[] = {
	"#version 420 core                                                 \n"
	"                                                                  \n"
	" in vec3 vPos;                                                    \n"
	" in vec3 vNorm;                                                   \n"
	" out vec4 color;                                                  \n"
	" uniform mat4 PV;                                                 \n"
	"                                                                 \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"    color = abs(vec4(vNorm, 1.0));                                \n"
	"    gl_Position = PV * vec4(vPos, 1.0f);                          \n"
	"}                                                                 \n"
};

// Simple fragment shader for color interpolation
static const char * fs_ci_src[] = {
	"#version 420 core                                                 \n"
	"                                                                  \n"
	"in vec4 color;                                                    \n"
	"out vec4 fColor;                                                  \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"    fColor = color;                                               \n"
	"}                                                                 \n"
};



// The source code for additional shaders can be added here
// ...

/*Varying variables provide an interface between Vertex and Fragment Shader. Vertex Shaders compute values per vertex and fragment shaders compute values per fragment.
If you define a varying variable in a vertex shader, its value will be interpolated(perspective - correct) over the primitive being rendered and you can access the 
interpolated value in the fragment shader.*/

//Cartoon shading
static const char * vs_n2c_src_toon[] = {
	"#version 420 core                                                 \n" //u vertex shader se odnosi samo na jednu tacku i njenu normalu, i to radimo za svaku tacku posebno
	"                                                                  \n"
	"                                                                  \n"
	" uniform mat4 PV;                                                 \n" //kako gledas na njega
	" uniform mat4 V;                                                  \n" //pomjeranje i rotacija svijeta
	" uniform mat4 VM;                                                 \n" //view i model matrica
	"                                                                  \n"
	" in vec3 vPos;                                                    \n" //vertex position, koordinate tacke
	" in vec3 vNorm;                                                   \n" //normala na tu tacku
	"                                                                  \n"
	" out vec4 color;                                                  \n" //da bi dodijelio fragmentu kako da boji
	"                                                                  \n"
	" varying vec3 vNormCam ;                                          \n" //normalu iz koord. pocetka hocu da prebacim gdje mi stoji tacka tj. objekat
	" varying vec3 vertexPos ;                                         \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"   															   \n"
	"   vertexPos = mat3(PV) * vPos;		                           \n" //nije nigdje iskoristeno
	"   vNormCam = normalize(transpose(inverse(mat3(VM)))* vNorm);	   \n"
	"   															   \n"
	"   gl_Position = PV * vec4(vPos, 1.0f);                           \n"
	"}                                                                 \n"
};
static const char * fs_ci_src_toon[] = {
	"#version 420 core                                                 \n"
	"                                                                  \n"
	" uniform mat4 V;                                                  \n"
	" uniform mat4 VM;                                                 \n"
	"                                                                  \n"
	" struct Light {                                                   \n"
	"     vec3 position;                                               \n"
	"     vec3 ambient;                                                \n"
	"     vec3 diffuse;                                                \n"
	"     vec3 specular;                                               \n"
	" };                                                               \n"
	"                                                                  \n"
	" #define NUM_OF_LIGHTS 2                                          \n"
	" uniform Light lights[NUM_OF_LIGHTS];                             \n"
	"                                                                  \n"
	" out vec4 fColor;                                                 \n"
	"                                                                  \n"
	" varying vec3 vNormCam ;                                          \n"
	" varying vec3 vertexPos ;                                         \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"   vec4 color; vec4 c;                                            \n" //c je ukupan color koji ce na kraju da bude u tom fragmentu, za sve lightove
	"   float intensity;                                               \n"
	"                                                                  \n"
	"   for(int i = 0; i < NUM_OF_LIGHTS; i++)						   \n"
	"   {															   \n"
	"	  vec3 lightPosCam = mat3(VM) * lights[i].position;            \n"
	"	  vec3 lightDir = normalize(lightPosCam - vertexPos);   \n" 
	"                                                                  \n"
	"	  intensity  = dot(lightDir, vNormCam);                         \n"
	"                                                                  \n"
	"     if (intensity > 0.95)                                        \n"
	"            color = vec4(0.5,0.5,1.0,1.0);                        \n" //ako je blizu normale na tacku bice najsvjetlije
	"     else if (intensity > 0.5)                                    \n"
	"            color = vec4(0.3,0.3,0.6,1.0);                        \n" //malo tamnije
	"     else if (intensity > 0.25)                                   \n"
	"            color =vec4(0.2,0.2,0.4,1.0);                         \n"
	"     else                                                         \n"
	"            color = vec4(0.1,0.1,0.2,1.0);                        \n" //najtamnije
	"     c += color;                                                  \n" //za sve lightove koje smo odradili u for petlji
	"   }                                                              \n"
	"    fColor = c ;                                                  \n"
	"}                                                                 \n"
};

//Gouraud shading.
static const char * vs_n2c_src_gouraud[] = {
	"#version 420 core                                                 \n"
	"                                                                  \n"
	" struct Material {                                                \n"
	"     vec3 ambient;                                                \n"
	"     vec3 diffuse;                                                \n"
	"     vec3 specular;                                               \n"
	"     float shininess;                                             \n"
	" };                                                               \n"
	"                                                                  \n"
	" struct Light {                                                   \n"
	"     vec3 position;                                               \n"
	"																   \n"
	"	  float constant;    										   \n" //da opada svjetlost, kako se udaljvam od kruga gdje pada svjetlost smanjuje intenzitet
	"	  float linear;												   \n"
	"	  float quadratic;											   \n"
	"																   \n"
	"     vec3 ambient;                                                \n"
	"     vec3 diffuse;                                                \n"
	"     vec3 specular;                                               \n"
	" };                                                               \n"
	"                                                                  \n"
	" #define NUM_OF_LIGHTS 2                                          \n"
	" uniform Material material;                                       \n"
	" uniform Light lights[NUM_OF_LIGHTS];                             \n"
	"                                                                  \n"
	" uniform vec3 viewPos;                                            \n"
	" uniform mat4 PV;                                                 \n"
	" uniform mat4 V;                                                  \n"
	" uniform mat4 VM;                                                 \n"
	"                                                                  \n"
	" in vec3 vPos;                                                    \n"
	" in vec3 vNorm;                                                   \n"
	"                                                                  \n"
	" varying vec3 LightingColor;                                      \n"
	"                                                                  \n"
	" vec3 CalcLight(Light light, vec3 normal, vec3 vertexPos, vec3 viewDir);  \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"   vec3 vertexPos = mat3(VM) * vPos;		                       \n"
	"   vec3 vNormCam = normalize(transpose(inverse(mat3(VM)))* vNorm);  \n"
	"   vec3 viewDir = normalize(viewPos - vertexPos);                   \n"
	"                                                                  \n"
	"	for(int i = 0; i < NUM_OF_LIGHTS; i++)						   \n"
	"        LightingColor += CalcLight(lights[i], vNormCam, vertexPos, viewDir);  \n"
	"                                                                  \n"
	"   gl_Position = PV * vec4(vPos, 1.0f);                           \n"
	"}                                                                 \n"
	"                                                                  \n"
	//Function for caculating the light 
	" vec3 CalcLight(Light light, vec3 normal, vec3 vertexPos, vec3 viewDir)   \n"
	"{                                                                 \n"
	"   vec3 lightPosCam = mat3(VM) * light.position;                  \n"
	"   vec3 lightDir = normalize(light.position - vertexPos);         \n"
	"                                                                  \n"
	//for diffuse
	"   float diff = clamp(dot(normal, lightDir), 0.0, 1.0);           \n" //range od 0.0 do 1.0 min i max vrijednost za intensity
	"                                                                  \n"
	//for specular
	"   vec3 reflectDir = reflect(-lightDir, normal);                  \n"
	"   float spec = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), material.shininess*128);  \n"
	"                                                                  \n"
	//for attenuation
	"   float distance = length(light.position - vertexPos);           \n"
	"   float attenuation = 1.f / (light.constant + light.linear * distance +      \n"
	"                                   light.quadratic * (distance * distance));  \n"
	"                                                                  \n"
	//for combination
	"   vec3 ambient = light.ambient * material.ambient;               \n"
	"   vec3 diffuse = light.diffuse * diff * material.diffuse;        \n"
	"   vec3 specular = light.specular * spec * material.specular;     \n"
	"                                                                  \n"
	"   ambient *= attenuation;                                        \n"
	"   diffuse *= attenuation;                                        \n"
	"   specular *= attenuation;                                       \n"
	"                                                                  \n"
	"   return vec3(clamp( diffuse + ambient + specular, 0.0, 1.0));   \n" //clamp-ako boja predje preko jedinice to ce biti mnogo svijetlo, clamp trimuje intenzitet 
	"}                                                                 \n" //boje(valjda svjetlosti) da ne bi bilo vise od 1
};
static const char * fs_ci_src_gouraud[] = {
	"#version 420 core                                                 \n"
	"                                                                  \n"
	" in vec3 LightingColor;                                           \n" //out parametar iz vertex shadera
	"																   \n"
	" out vec4 fColor;                                                 \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"    fColor = vec4(LightingColor, 1.0);                            \n"
	"}                                                                 \n"
};

//Phong shading
static const char * vs_n2c_src_phong[] = {
	"#version 420 core                                                 \n"
	"                                                                  \n"
	" in vec3 vPos;                                                    \n"
	" in vec3 vNorm;                                                   \n"
	"                                                                  \n"
	" uniform mat4 PV;                                                 \n"
	" uniform mat4 V;                                                  \n"
	" uniform mat4 VM;                                                 \n"
	"                                                                  \n"
	" out vec3 Normal;                                                 \n"
	" out vec3 vertexPos;                                              \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"   vertexPos = mat3(VM) * vPos;		                           \n"
	"   Normal = vNorm;                                                \n"
	"   gl_Position = PV * vec4(vPos, 1.0f);                           \n"
	"}                                                                 \n"
};
static const char * fs_ci_src_phong[] = {
	"#version 420 core                                                 \n" //isti kao gouraud samo sto se color racuna u fragment shaderu umjesto u vertex shaderu
	"                                                                  \n" //Gouraud: per-vertex color computation
	" struct Material {                                                \n" //Phong: per-fragment color computation
	"     vec3 ambient;                                                \n"
	"     vec3 diffuse;                                                \n"
	"     vec3 specular;                                               \n"
	"     float shininess;                                             \n"
	" };                                                               \n"
	"                                                                  \n"
	" struct Light {                                                   \n"
	"     vec3 position;                                               \n"
	"																   \n"
	"	  float constant;    										   \n"
	"	  float linear;												   \n"
	"	  float quadratic;											   \n"
	"																   \n"
	"     vec3 ambient;                                                \n"
	"     vec3 diffuse;                                                \n"
	"     vec3 specular;                                               \n"
	" };                                                               \n"
	"                                                                  \n"
	" #define NUM_OF_LIGHTS 2                                          \n"
	" uniform Material material;                                       \n"
	" uniform Light lights[NUM_OF_LIGHTS];                             \n"
	"                                                                  \n"
	" uniform vec3 viewPos;                                            \n"
	" uniform mat4 PV;                                                 \n"
	" uniform mat4 V;                                                  \n"
	" uniform mat4 VM;                                                 \n"
	"                                                                  \n"
	" in vec3 Normal;                                                  \n"
	" in vec3 vertexPos;                                               \n"
	"																   \n"
	" out vec4 fColor;                                                 \n"
	"                                                                  \n"
	" vec3 CalcLight(Light light, vec3 normal, vec3 vertexPos, vec3 viewDir); \n"
	"                                                                  \n"
	"void main(void)                                                   \n"
	"{                                                                 \n"
	"   vec3 vNormCam = normalize(transpose(inverse(mat3(VM)))* Normal);  \n"
	"   vec3 viewDir = normalize(viewPos - vertexPos);                    \n"
	"   vec3 LightingColor;                                            \n"
	"                                                                  \n"
	"	for(int i = 0; i < NUM_OF_LIGHTS; i++)						   \n"
	"        LightingColor += CalcLight(lights[i], vNormCam, vertexPos, viewDir);   \n"
	"                                                                  \n"
	"    fColor = vec4(LightingColor, 1.0);                            \n"
	" }                                                                \n"
	"                                                                  \n"
	//Function for caculating the light 
	" vec3 CalcLight(Light light, vec3 normal, vec3 vertexPos, vec3 viewDir) \n"
	"{                                                                 \n"
	"   vec3 lightPosCam = mat3(VM) * light.position;                  \n"
	"   vec3 lightDir = normalize(light.position - vertexPos);         \n"
	"                                                                  \n"
	//for diffuse
	"   float diff = clamp(dot(normal, lightDir), 0.0, 1.0);           \n"
	"                                                                  \n"
	//for specular
	"   vec3 reflectDir = reflect(-lightDir, normal);                  \n"
	"   float spec = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), material.shininess*128);  \n"
	"                                                                  \n"
	//for attenuation
	"   float distance = length(light.position - vertexPos);           \n"
	"   float attenuation = 1.f / (light.constant + light.linear * distance +      \n"
	"                                   light.quadratic * (distance * distance));  \n"
	"                                                                  \n"
	//for combination
	"   vec3 ambient = light.ambient * material.ambient;               \n"
	"   vec3 diffuse = light.diffuse * diff * material.diffuse;        \n"
	"   vec3 specular = light.specular * spec * material.specular;     \n"
	"                                                                  \n"
	"   ambient *= attenuation;                                        \n"
	"   diffuse *= attenuation;                                        \n"
	"   specular *= attenuation;                                       \n"
	"                                                                  \n"
	"   return vec3(clamp( diffuse + ambient + specular, 0.0, 1.0));   \n"
	"}                                                                 \n"
};


