/*
Práctica 7: Iluminación 1
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
// Añadir Texturas del holocron
Texture holocronTexture;


//Modelo BlackHawk
Model Blackhawk_M;
//Sustituir por sus propios Rovers Con jerarquía
// //Lista de Modelos del Rover a importar
Model Cuerpo_M;
Model BB_M;
Model BM_M;
Model BMA_M;
Model BP_M;
Model PataDelanteraDer_M;
Model PataDelanteraIzq_M;
Model PataMediaDer_M;
Model PataMediaIzq_M;
Model PataTraseraDer_M;
Model PataTraseraIzq_M;

//Modelo del avión
Model Avion_M;



Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;

PointLight pointLights[MAX_POINT_LIGHTS];

SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado() //sustituir con su propio dado, para que tenga la textura de la práctica 6
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};	GLfloat cubo_vertices[] = {
			// front
			//x		y		z		S		T			NX		NY		NZ
			-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	1.0f,	//0
			0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	1.0f,	//1
			0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	1.0f,	//2
			-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	1.0f,	//3
			// right
			//x		y		z		S		T
			0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
			0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
			0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
			0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
			// back
			-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
			0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
			0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
			-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

			// left
			//x		y		z		S		T
			-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
			-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
			-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
			-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

			// bottom
			//x		y		z		S		T
			-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
			0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
			 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
			-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

			//UP
			 //x		y		z		S		T
			 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
			  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
			 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);

}


void CrearHolocron()
{

	unsigned int holocron_indices[] = {
		// Cara 0: Tapa superior (rombo)
		0, 1, 2,  0, 3, 1,
		// Cara 1: Lado frontal (+Z)
		7, 4, 5,  7, 6, 4,
		// Cara 2: Lado derecho (+X)
		10, 11, 8,  10, 9, 11,
		// Cara 3: Lado trasero (-Z)
		14, 12, 13,  14, 15, 12,
		// Cara 4: Lado izquierdo (-X)
		18, 17, 19,  18, 16, 17,
		// Cara 5: Esquina superior frontal-derecha
		20, 21, 22,
		// Cara 6: Esquina superior trasera-derecha
		25, 24, 23,
		// Cara 7: Esquina superior trasera-izquierda
		26, 27, 28,
		// Cara 8: Esquina superior frontal-izquierda
		31, 30, 29,
		// Cara 9: Esquina inferior frontal-derecha
		32, 33, 35,  32, 34, 33,
		// Cara 10: Esquina inferior trasera-derecha
		36, 37, 38,  36, 39, 37,
		// Cara 11: Esquina inferior trasera-izquierda
		43, 41, 40,  43, 42, 41,
		// Cara 12: Esquina inferior frontal-izquierda
		45, 46, 47,  45, 44, 46,
		// Cara 13: Base inferior (rombo)
		53, 51, 50,  50, 48, 49,  49, 54, 53,  53, 55, 51,  51, 52, 50,  50, 49, 53,
	};

	GLfloat holocron_vertices[] = {

		//x			y			z			S		T		NX		NY		NZ
		// Cara 0: Tapa superior (rombo)
		0.00f,	2.50f,	-2.50f,	0.1250f,	0.9750f,	0.0f,	-1.0f,	0.0f,	//0
		0.00f,	2.50f,	2.50f,	0.1250f,	0.7750f,	0.0f,	-1.0f,	0.0f,	//1
		2.50f,	2.50f,	0.00f,	0.2250f,	0.8750f,	0.0f,	-1.0f,	0.0f,	//2
		-2.50f,	2.50f,	0.00f,	0.0250f,	0.8750f,	0.0f,	-1.0f,	0.0f,	//3
		// Cara 1: Lado frontal (+Z)
		0.00f,	2.50f,	2.50f,	0.3750f,	0.9750f,	0.0f,	0.0f,	-1.0f,	//4
		-2.50f,	0.00f,	2.50f,	0.2750f,	0.8750f,	0.0f,	0.0f,	-1.0f,	//5
		2.50f,	0.00f,	2.50f,	0.4750f,	0.8750f,	0.0f,	0.0f,	-1.0f,	//6
		0.00f,	-2.50f,	2.50f,	0.3750f,	0.7750f,	0.0f,	0.0f,	-1.0f,	//7
		// Cara 2: Lado derecho (+X)
		2.50f,	0.00f,	2.50f,	0.5250f,	0.8750f,	-1.0f,	0.0f,	0.0f,	//8
		2.50f,	0.00f,	-2.50f,	0.7250f,	0.8750f,	-1.0f,	0.0f,	0.0f,	//9
		2.50f,	-2.50f,	0.00f,	0.6250f,	0.7750f,	-1.0f,	0.0f,	0.0f,	//10
		2.50f,	2.50f,	0.00f,	0.6250f,	0.9750f,	-1.0f,	0.0f,	0.0f,	//11
		// Cara 3: Lado trasero (-Z)
		0.00f,	2.50f,	-2.50f,	0.8750f,	0.9750f,	0.0f,	0.0f,	1.0f,	//12
		2.50f,	0.00f,	-2.50f,	0.7750f,	0.8750f,	0.0f,	0.0f,	1.0f,	//13
		0.00f,	-2.50f,	-2.50f,	0.8750f,	0.7750f,	0.0f,	0.0f,	1.0f,	//14
		-2.50f,	0.00f,	-2.50f,	0.9750f,	0.8750f,	0.0f,	0.0f,	1.0f,	//15
		// Cara 4: Lado izquierdo (-X)
		-2.50f,	-2.50f,	0.00f,	0.1250f,	0.5250f,	1.0f,	0.0f,	0.0f,	//16
		-2.50f,	0.00f,	2.50f,	0.2250f,	0.6250f,	1.0f,	0.0f,	0.0f,	//17
		-2.50f,	0.00f,	-2.50f,	0.0250f,	0.6250f,	1.0f,	0.0f,	0.0f,	//18
		-2.50f,	2.50f,	0.00f,	0.1250f,	0.7250f,	1.0f,	0.0f,	0.0f,	//19
		// Cara 5: Esquina superior frontal-derecha
		0.00f,	2.50f,	2.50f,	0.2750f,	0.7116f,	-0.57735f,	-0.57735f,	-0.57735f,	//20
		2.50f,	0.00f,	2.50f,	0.3750f,	0.5384f,	-0.57735f,	-0.57735f,	-0.57735f,	//21
		2.50f,	2.50f,	0.00f,	0.4750f,	0.7116f,	-0.57735f,	-0.57735f,	-0.57735f,	//22
		// Cara 6: Esquina superior trasera-derecha
		0.00f,	2.50f,	-2.50f,	0.7250f,	0.7116f,	-0.57735f,	-0.57735f,	0.57735f,	//23
		2.50f,	0.00f,	-2.50f,	0.6250f,	0.5384f,	-0.57735f,	-0.57735f,	0.57735f,	//24
		2.50f,	2.50f,	0.00f,	0.5250f,	0.7116f,	-0.57735f,	-0.57735f,	0.57735f,	//25
		// Cara 7: Esquina superior trasera-izquierda
		0.00f,	2.50f,	-2.50f,	0.7750f,	0.7116f,	0.57735f,	-0.57735f,	0.57735f,	//26
		-2.50f,	0.00f,	-2.50f,	0.8750f,	0.5384f,	0.57735f,	-0.57735f,	0.57735f,	//27
		-2.50f,	2.50f,	0.00f,	0.9750f,	0.7116f,	0.57735f,	-0.57735f,	0.57735f,	//28
		// Cara 8: Esquina superior frontal-izquierda
		0.00f,	2.50f,	2.50f,	0.2250f,	0.4616f,	0.57735f,	-0.57735f,	-0.57735f,	//29
		-2.50f,	0.00f,	2.50f,	0.1250f,	0.2884f,	0.57735f,	-0.57735f,	-0.57735f,	//30
		-2.50f,	2.50f,	0.00f,	0.0250f,	0.4616f,	0.57735f,	-0.57735f,	-0.57735f,	//31
		// Cara 9: Esquina inferior frontal-derecha
		2.50f,	0.00f,	2.50f,	0.3750f,	0.4616f,	-0.57735f,	0.57735f,	-0.57735f,	//32
		1.25f,	-2.50f,	1.25f,	0.3750f,	0.2884f,	-0.57735f,	0.57735f,	-0.57735f,	//33
		0.00f,	-2.50f,	2.50f,	0.2750f,	0.2884f,	-0.57735f,	0.57735f,	-0.57735f,	//34
		2.50f,	-2.50f,	0.00f,	0.4750f,	0.2884f,	-0.57735f,	0.57735f,	-0.57735f,	//35
		// Cara 10: Esquina inferior trasera-derecha
		2.50f,	0.00f,	-2.50f,	0.6250f,	0.4616f,	-0.57735f,	0.57735f,	0.57735f,	//36
		1.25f,	-2.50f,	-1.25f,	0.6250f,	0.2884f,	-0.57735f,	0.57735f,	0.57735f,	//37
		0.00f,	-2.50f,	-2.50f,	0.7250f,	0.2884f,	-0.57735f,	0.57735f,	0.57735f,	//38
		2.50f,	-2.50f,	0.00f,	0.5250f,	0.2884f,	-0.57735f,	0.57735f,	0.57735f,	//39
		// Cara 11: Esquina inferior trasera-izquierda
		-2.50f,	-2.50f,	0.00f,	0.9750f,	0.2884f,	0.57735f,	0.57735f,	0.57735f,	//40
		-1.25f,	-2.50f,	-1.25f,	0.8750f,	0.2884f,	0.57735f,	0.57735f,	0.57735f,	//41
		0.00f,	-2.50f,	-2.50f,	0.7750f,	0.2884f,	0.57735f,	0.57735f,	0.57735f,	//42
		-2.50f,	0.00f,	-2.50f,	0.8750f,	0.4616f,	0.57735f,	0.57735f,	0.57735f,	//43
		// Cara 12: Esquina inferior frontal-izquierda
		-2.50f,	-2.50f,	0.00f,	0.0250f,	0.0384f,	0.57735f,	0.57735f,	-0.57735f,	//44
		-2.50f,	0.00f,	2.50f,	0.1250f,	0.2116f,	0.57735f,	0.57735f,	-0.57735f,	//45
		-1.25f,	-2.50f,	1.25f,	0.1250f,	0.0384f,	0.57735f,	0.57735f,	-0.57735f,	//46
		0.00f,	-2.50f,	2.50f,	0.2250f,	0.0384f,	0.57735f,	0.57735f,	-0.57735f,	//47
		// Cara 13: Base inferior (rombo)
		-2.50f,	-2.50f,	0.00f,	0.4750f,	0.1250f,	0.0f,	1.0f,	0.0f,	//48
		-1.25f,	-2.50f,	-1.25f,	0.4250f,	0.1750f,	0.0f,	1.0f,	0.0f,	//49
		-1.25f,	-2.50f,	1.25f,	0.4250f,	0.0750f,	0.0f,	1.0f,	0.0f,	//50
		1.25f,	-2.50f,	1.25f,	0.3250f,	0.0750f,	0.0f,	1.0f,	0.0f,	//51
		0.00f,	-2.50f,	2.50f,	0.3750f,	0.0250f,	0.0f,	1.0f,	0.0f,	//52
		1.25f,	-2.50f,	-1.25f,	0.3250f,	0.1750f,	0.0f,	1.0f,	0.0f,	//53
		0.00f,	-2.50f,	-2.50f,	0.3750f,	0.2250f,	0.0f,	1.0f,	0.0f,	//54
		2.50f,	-2.50f,	0.00f,	0.2750f,	0.1250f,	0.0f,	1.0f,	0.0f,	//55
	};

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(holocron_vertices, holocron_indices,
		sizeof(holocron_vertices) / sizeof(GLfloat),		// total de floats (vertices x 8)
		sizeof(holocron_indices) / sizeof(unsigned int));	// total de indices
	meshListModel.push_back(holocron);
}




int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	//cargar la textura del holocron 
	holocronTexture = Texture("Textures/holocron_codigo_1.png");
	holocronTexture.LoadTextureA();

	//Cargar modelos
	//Rover_M = Model();
	//Rover_M.LoadModel("Models/RoverModificado.obj");
	Cuerpo_M = Model();
	Cuerpo_M.LoadModel("Models/cuerpo.fbx");//1
	//Piezas del brazo
	BB_M = Model();
	BB_M.LoadModel("Models/BB.fbx");//2
	BM_M = Model();
	BM_M.LoadModel("Models/BM.fbx");//3
	BMA_M = Model();
	BMA_M.LoadModel("Models/BMA.fbx");//4
	BP_M = Model();
	BP_M.LoadModel("Models/BP.fbx");//5
	//Patas delantera
	PataDelanteraDer_M = Model();
	PataDelanteraDer_M.LoadModel("Models/PFD.fbx");//6
	PataDelanteraIzq_M = Model();
	PataDelanteraIzq_M.LoadModel("Models/PFI.fbx");//7
	//Patas Medias
	PataMediaDer_M = Model();
	PataMediaDer_M.LoadModel("Models/PMD.fbx");//8
	PataMediaIzq_M = Model();
	PataMediaIzq_M.LoadModel("Models/PMI.fbx");//9
	//Patas Traseras
	PataTraseraDer_M = Model();
	PataTraseraDer_M.LoadModel("Models/PTD.fbx");//10
	PataTraseraIzq_M = Model();
	PataTraseraIzq_M.LoadModel("Models/PTI.fbx");//11

	//Modelo de Blackhawk
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	//Modelo avión
	Avion_M = Model();
	Avion_M.LoadModel("Models/plane.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//contador de spotlights: le dice al shader cuántas luces del arreglo spotLights debe usar
	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	/*luz faro Rover
	Reutilizamos el lugar [1] del arreglo, que antes era la luz fija verde
	Se definen sus propiedades fijas: color, intensidad, atenuación y apertura.
	La posición y dirección son de relleno: se sobreescriben en cada frame
	dentro del while con SetFlash, para que sigan al rover.*/
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.6f,		//color RGB: luz tipo faro
		1.0f, 2.0f,									//intensidad ambiental, intensidad difusa
		5.0f, 10.0f, 0.0f,							//posición inicial (se sobreescribe en el while)
		0.0f, -5.0f, 0.0f,							//dirección inicial (se sobreescribe en el while)
		1.0f, 0.0f, 0.0f,							//atenuación: constante, lineal, exponencial 
		15.0f);										//edge: ángulo en grados del centro al borde del cono
	spotLightCount++;

	/*Luces del Avión 
	usamos los lugares[2] y[3] del arreglo
	Igual que el faro del rover: aquí solo van sus propiedades fijas.
	Posición y dirección se sobreescriben en cada frame*/
	spotLights[2] = SpotLight(0.2f, 0.8f, 1.0f,		//color RGB: azul
		1.0f, 2.0f,									//intensidad ambiental, intensidad difusa
		0.0f, 0.0f, 0.0f,							//posición inicial
		0.0f, -1.0f, 0.0f,							//dirección inicial
		1.0f, 0.0f, 0.0f,							//atenuación
		20.0f);										//edge: cono más abierto porque el avión está alto 
	spotLightCount++;								//3 spotlights: linterna, faro rover y luz de abajo del avión

	//luz del avión hacia ADELANTE
	spotLights[3] = SpotLight(0.6f, 1.0f, 0.6f,		//color RGB: verde claro
		1.0f, 2.0f,									//intensidad ambiental, intensidad difusa
		0.0f, 0.0f, 0.0f,							//posición inicial
		1.0f, 0.0f, 0.0f,							//dirección inicial
		1.0f, 0.0f, 0.0f,							//atenuación
		15.0f);										//edge
	spotLightCount++;								//4 spotlights: linterna, faro rover, avión abajo y avión adelante

	//se crean mas luces puntuales y spotlight 
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelaux2(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		//Luz del ROVER
		//baseRover es la matriz raíz del rover: guarda hacia dónde mira y cuánto ha avanzado.
		glm::mat4 baseRover(1.0);
		baseRover = glm::rotate(baseRover, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		baseRover = glm::translate(baseRover, glm::vec3(-mainWindow.getmovZ(), 1.5f, 0.0f));	
		
		/////////////////////////////////////////////////////////////////////////
		// Faro del rover: rama que sale de la base hacia el frente
		//Funciona igual que una pata, pero no dibujamos

		//Posición: vec4 con w = 1 -> la matriz le aplica rotación Y traslación, así el punto viaja con el rover.
		//(4.0, 0.5, 0.0) = 4 unidades al frente y 0.5 arriba del centro del cuerpo.
		glm::vec3 posFaroRover = glm::vec3(baseRover * glm::vec4(4.0f, 0.5f, 0.0f, 1.0f));

		//Dirección: vec4 con w = 0 -> solo le afecta la rotación.
		//(1.0, -0.3, 0.0) = hacia el frente y un poco hacia abajo para que la luz pegue en el piso.
		glm::vec3 dirFaroRover = glm::normalize(glm::vec3(baseRover * glm::vec4(1.0f, -0.3f, 0.0f, 0.0f)));

		//SetFlash actualiza la posición y dirección del faro
		spotLights[1].SetFlash(posFaroRover, dirFaroRover);

		//Luces del AVIÓN
		//Mismo patrón que el rover matriz (baseAvion)

		//frenteAvion Luz de la nariz
		glm::vec3 frenteAvion = glm::vec3(0.0f, 0.0f, 1.0f);

		//baseAvion
		glm::mat4 baseAvion(1.0);
		baseAvion = glm::translate(baseAvion, glm::vec3(4.0f, 15.0f, 20.0f)
			+ glm::vec3(0.0f, mainWindow.getmovY(), 0.0f)		//subir / bajar
			+ frenteAvion * mainWindow.getmovX());				//avanzar / retroceder

		//Luz de ABAJO: rama que sale de la panza del avión.
		//Posición con w = 1: (0, -1, 0) = una unidad debajo del centro, viaja con el avión.
		glm::vec3 posLuzAbajoAvion = glm::vec3(baseAvion * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));
		//Dirección con w = 0: (0, -1, 0) = recto hacia el piso.
		glm::vec3 dirLuzAbajoAvion = glm::normalize(glm::vec3(baseAvion * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)));
		spotLights[2].SetFlash(posLuzAbajoAvion, dirLuzAbajoAvion);

		//Luz de ADELANTE: rama que sale de la nariz del avión.
		//Posición con w = 1: 3 unidades hacia el frente del centro.
		glm::vec3 posLuzFrenteAvion = glm::vec3(baseAvion * glm::vec4(frenteAvion * 3.0f, 1.0f));
		//Dirección con w = 0: hacia el frente y un poco hacia abajo (-0.3 en Y),
		//para que el cono alcance a pegar en el piso y se note en las capturas.
		glm::vec3 dirLuzFrenteAvion = glm::normalize(glm::vec3(baseAvion * glm::vec4(frenteAvion + glm::vec3(0.0f, -0.3f, 0.0f), 0.0f)));
		spotLights[3].SetFlash(posLuzFrenteAvion, dirLuzFrenteAvion);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());


		//información al shader de fuentes de iluminación
		//Aquí se mandan TODAS las luces al shader (incluido el faro ya actualizado con SetFlash)
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Piso
		//Antes aquí se volvían a declarar model, modelaux y color. Eso creaba variables "gemelas"
		//que tapaban a las de fuera del while. Se comentan y solo se reinicia el color a blanco.
		//glm::mat4 model(1.0);
		//glm::mat4 modelaux(1.0);
		//glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();

		//------------Dibujo jerarquico del Rover-------------------*
		//Modelo Inicial
		color = glm::vec3(1.0f, 0.0f, 1.0f); //modelo de color azul
		//Estas 3 líneas ya no hacen falta porque baseRover ya trae la rotación y el movimiento,
		//model = glm::mat4(1.0);
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(-mainWindow.getmovZ(), 1.5f, 0.0f));// funcion que nos permite mover al rover adelante y atras

		//Se usa la MISMA matriz que el faro para que cuerpo y luz siempre estén juntos.
		model = baseRover;
		modelaux = model;
		modelaux2 = model;	//patas y brazo parten de modelaux2, así que también heredan el movimiento
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();//modificar por el modelo de solo cuerpo del Rover, para que se pueda separar el brazo y las llantas

		//Nueva rama Pata Delantera derecha.
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 1.5, -2.8f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.50f, 1.0f, 0.5f);//Color verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		PataDelanteraDer_M.RenderModel();

		//Nueva rama Pata Delantera izquierda
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.0f, 1.5, 3.3f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.50f, 1.0f, 0.5f);//Color verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		PataDelanteraIzq_M.RenderModel();

		//Nueva rama Pata Media derecha.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.0f, 0.5, -3.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.5f);//Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		PataMediaDer_M.RenderModel();

		//Nueva rama Pata Media IZQ.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.0f, 0.5, 3.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.5f);//Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		PataMediaIzq_M.RenderModel();

		//Nueva rama Pata Trasera DER.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.0f, 0.5, -3.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.5f);//Color rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		PataTraseraDer_M.RenderModel();

		//Nueva rama Pata Trasera IZQ.
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.0f, 0.5, 3.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion10()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.5f);//Color rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		PataTraseraIzq_M.RenderModel();


		//Nueva rama BRAZO
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.0f, 2.0, -1.2f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.5f);//Color rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		BB_M.RenderModel();

		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.7f, 0.5f);//Color naranja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		BM_M.RenderModel();

		modelaux = model;
		model = glm::translate(model, glm::vec3(3.0f, 2.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.2f, 0.9f);//Color rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		BMA_M.RenderModel();

		modelaux = model;
		model = glm::translate(model, glm::vec3(-3.5f, 3.0f, 0.15));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);//Color rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		BP_M.RenderModel();


		//comentamos todo el blackhwk
		/*
		//BLACKHAWK
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();
		*/
		//Rover: Sustituir por su propio Rover con jerarquía
		/*
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Rover_M.RenderModel();
		*/

		//Dado Comentado para que no salga en escena
		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//dadoTexture.UseTexture();
		meshListModel[4]->RenderMeshModel();*/

		//Holocron
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.0f, 7.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color blanco para que no tina la textura
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		holocronTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);	//material brillante
		meshListModel[5]->RenderMeshModel();


		//Añadir modelo del avión
		//Avion
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//[MODIFICADO P7 - Avión] Estas 2 líneas ya no hacen falta: la posición (4, 15, 20)
		//y el movimiento con teclado ya vienen en baseAvion.
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(4.0f, 15.0f, 20.0f));
		//Se usa la MISMA matriz que sus dos luces para que avión y luces siempre estén juntos.
		model = baseAvion;
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Avion_M.RenderModel();


		//Agave ¿qué sucede si lo renderizan antes del helicóptero?
		//Comentado para que no aparezca en escena
		/*
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		*/

		//blending: transparencia o traslucidez
		//También se comenta: con su translate comentado, el agave se dibujaba con la
		//última matriz usada (la del avión) y aparecía pegado a él.
		/*
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[3]->RenderMeshModel();
		glDisable(GL_BLEND);
		*/

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}