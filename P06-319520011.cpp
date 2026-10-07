/*
Práctica 6: Texturizado
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

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz

std::vector<Shader> shaderList;

Camera camera;

Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture holocronTexture;

Model Kitt_M;
Model Llanta_M;
Model Dado_M;
Model Holocron_M;
Model Avion_M; /// modelo avión



Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";





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


	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearDado()
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

	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	-1.0f,	//3
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
	// Holocron texturizado por codigo (coordenadas del profesor limpiadas a multiplos de 1.25).
	// 14 caras planas, cada una con sus propios vertices
	// (se repiten las esquinas para que cada cara tenga sus propias coordenadas S,T).
	// La textura es un atlas de 4x4 celdas: la cara N ocupa la celda N (fila de arriba = caras 0-3).
	// PASO 4: S,T calculados. Normales todavia en 0 (se agregan en el paso 5).
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
		0.00f,	2.50f,	-2.50f,	0.1250f,	0.9750f,	0.0f,	0.0f,	0.0f,	//0
		0.00f,	2.50f,	2.50f,	0.1250f,	0.7750f,	0.0f,	0.0f,	0.0f,	//1
		2.50f,	2.50f,	0.00f,	0.2250f,	0.8750f,	0.0f,	0.0f,	0.0f,	//2
		-2.50f,	2.50f,	0.00f,	0.0250f,	0.8750f,	0.0f,	0.0f,	0.0f,	//3
		// Cara 1: Lado frontal (+Z)
		0.00f,	2.50f,	2.50f,	0.3750f,	0.9750f,	0.0f,	0.0f,	0.0f,	//4
		-2.50f,	0.00f,	2.50f,	0.2750f,	0.8750f,	0.0f,	0.0f,	0.0f,	//5
		2.50f,	0.00f,	2.50f,	0.4750f,	0.8750f,	0.0f,	0.0f,	0.0f,	//6
		0.00f,	-2.50f,	2.50f,	0.3750f,	0.7750f,	0.0f,	0.0f,	0.0f,	//7
		// Cara 2: Lado derecho (+X)
		2.50f,	0.00f,	2.50f,	0.5250f,	0.8750f,	0.0f,	0.0f,	0.0f,	//8
		2.50f,	0.00f,	-2.50f,	0.7250f,	0.8750f,	0.0f,	0.0f,	0.0f,	//9
		2.50f,	-2.50f,	0.00f,	0.6250f,	0.7750f,	0.0f,	0.0f,	0.0f,	//10
		2.50f,	2.50f,	0.00f,	0.6250f,	0.9750f,	0.0f,	0.0f,	0.0f,	//11
		// Cara 3: Lado trasero (-Z)
		0.00f,	2.50f,	-2.50f,	0.8750f,	0.9750f,	0.0f,	0.0f,	0.0f,	//12
		2.50f,	0.00f,	-2.50f,	0.7750f,	0.8750f,	0.0f,	0.0f,	0.0f,	//13
		0.00f,	-2.50f,	-2.50f,	0.8750f,	0.7750f,	0.0f,	0.0f,	0.0f,	//14
		-2.50f,	0.00f,	-2.50f,	0.9750f,	0.8750f,	0.0f,	0.0f,	0.0f,	//15
		// Cara 4: Lado izquierdo (-X)
		-2.50f,	-2.50f,	0.00f,	0.1250f,	0.5250f,	0.0f,	0.0f,	0.0f,	//16
		-2.50f,	0.00f,	2.50f,	0.2250f,	0.6250f,	0.0f,	0.0f,	0.0f,	//17
		-2.50f,	0.00f,	-2.50f,	0.0250f,	0.6250f,	0.0f,	0.0f,	0.0f,	//18
		-2.50f,	2.50f,	0.00f,	0.1250f,	0.7250f,	0.0f,	0.0f,	0.0f,	//19
		// Cara 5: Esquina superior frontal-derecha
		0.00f,	2.50f,	2.50f,	0.2750f,	0.7116f,	0.0f,	0.0f,	0.0f,	//20
		2.50f,	0.00f,	2.50f,	0.3750f,	0.5384f,	0.0f,	0.0f,	0.0f,	//21
		2.50f,	2.50f,	0.00f,	0.4750f,	0.7116f,	0.0f,	0.0f,	0.0f,	//22
		// Cara 6: Esquina superior trasera-derecha
		0.00f,	2.50f,	-2.50f,	0.7250f,	0.7116f,	0.0f,	0.0f,	0.0f,	//23
		2.50f,	0.00f,	-2.50f,	0.6250f,	0.5384f,	0.0f,	0.0f,	0.0f,	//24
		2.50f,	2.50f,	0.00f,	0.5250f,	0.7116f,	0.0f,	0.0f,	0.0f,	//25
		// Cara 7: Esquina superior trasera-izquierda
		0.00f,	2.50f,	-2.50f,	0.7750f,	0.7116f,	0.0f,	0.0f,	0.0f,	//26
		-2.50f,	0.00f,	-2.50f,	0.8750f,	0.5384f,	0.0f,	0.0f,	0.0f,	//27
		-2.50f,	2.50f,	0.00f,	0.9750f,	0.7116f,	0.0f,	0.0f,	0.0f,	//28
		// Cara 8: Esquina superior frontal-izquierda
		0.00f,	2.50f,	2.50f,	0.2250f,	0.4616f,	0.0f,	0.0f,	0.0f,	//29
		-2.50f,	0.00f,	2.50f,	0.1250f,	0.2884f,	0.0f,	0.0f,	0.0f,	//30
		-2.50f,	2.50f,	0.00f,	0.0250f,	0.4616f,	0.0f,	0.0f,	0.0f,	//31
		// Cara 9: Esquina inferior frontal-derecha
		2.50f,	0.00f,	2.50f,	0.3750f,	0.4616f,	0.0f,	0.0f,	0.0f,	//32
		1.25f,	-2.50f,	1.25f,	0.3750f,	0.2884f,	0.0f,	0.0f,	0.0f,	//33
		0.00f,	-2.50f,	2.50f,	0.2750f,	0.2884f,	0.0f,	0.0f,	0.0f,	//34
		2.50f,	-2.50f,	0.00f,	0.4750f,	0.2884f,	0.0f,	0.0f,	0.0f,	//35
		// Cara 10: Esquina inferior trasera-derecha
		2.50f,	0.00f,	-2.50f,	0.6250f,	0.4616f,	0.0f,	0.0f,	0.0f,	//36
		1.25f,	-2.50f,	-1.25f,	0.6250f,	0.2884f,	0.0f,	0.0f,	0.0f,	//37
		0.00f,	-2.50f,	-2.50f,	0.7250f,	0.2884f,	0.0f,	0.0f,	0.0f,	//38
		2.50f,	-2.50f,	0.00f,	0.5250f,	0.2884f,	0.0f,	0.0f,	0.0f,	//39
		// Cara 11: Esquina inferior trasera-izquierda
		-2.50f,	-2.50f,	0.00f,	0.9750f,	0.2884f,	0.0f,	0.0f,	0.0f,	//40
		-1.25f,	-2.50f,	-1.25f,	0.8750f,	0.2884f,	0.0f,	0.0f,	0.0f,	//41
		0.00f,	-2.50f,	-2.50f,	0.7750f,	0.2884f,	0.0f,	0.0f,	0.0f,	//42
		-2.50f,	0.00f,	-2.50f,	0.8750f,	0.4616f,	0.0f,	0.0f,	0.0f,	//43
		// Cara 12: Esquina inferior frontal-izquierda
		-2.50f,	-2.50f,	0.00f,	0.0250f,	0.0384f,	0.0f,	0.0f,	0.0f,	//44
		-2.50f,	0.00f,	2.50f,	0.1250f,	0.2116f,	0.0f,	0.0f,	0.0f,	//45
		-1.25f,	-2.50f,	1.25f,	0.1250f,	0.0384f,	0.0f,	0.0f,	0.0f,	//46
		0.00f,	-2.50f,	2.50f,	0.2250f,	0.0384f,	0.0f,	0.0f,	0.0f,	//47
		// Cara 13: Base inferior (rombo)
		-2.50f,	-2.50f,	0.00f,	0.4750f,	0.1250f,	0.0f,	0.0f,	0.0f,	//48
		-1.25f,	-2.50f,	-1.25f,	0.4250f,	0.1750f,	0.0f,	0.0f,	0.0f,	//49
		-1.25f,	-2.50f,	1.25f,	0.4250f,	0.0750f,	0.0f,	0.0f,	0.0f,	//50
		1.25f,	-2.50f,	1.25f,	0.3250f,	0.0750f,	0.0f,	0.0f,	0.0f,	//51
		0.00f,	-2.50f,	2.50f,	0.3750f,	0.0250f,	0.0f,	0.0f,	0.0f,	//52
		1.25f,	-2.50f,	-1.25f,	0.3250f,	0.1750f,	0.0f,	0.0f,	0.0f,	//53
		0.00f,	-2.50f,	-2.50f,	0.3750f,	0.2250f,	0.0f,	0.0f,	0.0f,	//54
		2.50f,	-2.50f,	0.00f,	0.2750f,	0.1250f,	0.0f,	0.0f,	0.0f,	//55
	};

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(holocron_vertices, holocron_indices,
		sizeof(holocron_vertices) / sizeof(GLfloat),		// total de floats (vertices x 8)
		sizeof(holocron_indices) / sizeof(unsigned int));	// total de indices
	meshListModel.push_back(holocron);
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado-de-numeros.png");
	dadoTexture.LoadTextureA();
	holocronTexture = Texture("Textures/holocron_codigo_1.png");
	holocronTexture.LoadTextureA();
	
	Holocron_M = Model();
	Holocron_M.LoadModel("Models/holoblend.obj");

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

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
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
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshListModel[2]->RenderMeshModel();


		

		//Dado de Opengl
		//Ejercicio 1: Texturizar su dado con la imagen ya optimizada por ustedes con logos de star wars
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshListModel[4]->RenderMeshModel();
		
		//Ejercicio 2:Importar el cubo texturizado en el programa de modelado con 
		//la imagen ya optimizada por ustedes
		
		/*
		//Dado importado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 3.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado_M.RenderModel();
		*/
	



		/*Reporte de práctica :
		
		Ejercicio 1: Crear o modificar el holocron y texturizarlo por medio de código
		Ejercicio 2: Importar el modelo del holocron texturizardo en el programa de modelado
		Ejercicio 3: Importar un modelo de avión con con la textura de la cara del personaje de la imagen del previo:
		Vidrio fonrtal: OJOS
		Frente del avión: Nariz y Sonrisa
		Alas: Logos del universo del personaje
		
		*/


		//Holocrones
		color = glm::vec3(1.0f, 1.0f, 1.0f);//color que multiplica a la información de color de la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.5f, 4.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocronTexture.UseTexture();
		meshListModel[5]->RenderMeshModel();
		

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.5f, 2.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Holocron_M.RenderModel();

		//Avion
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Avion_M.RenderModel();

	
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/