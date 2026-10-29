/*
Práctica 09: Animación
Simple o básica: Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
*/

//==================================================================
//                            INCLUDES
//==================================================================
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
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//Iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"


//==================================================================
//  VARIABLES GLOBALES
//==================================================================
const float toRadians = 3.14159265f / 180.0f;

//------------------------- Animación -------------------------
//bool avanza = true;		// dirección del rover: true = hacia su frente
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;	// tiempo de la animación en grados (avión y satélite)
//Reloj de la nave: distancia recorrida dentro del tramo actual de la pista
float avanceNave = 0.0f;
int tramoNave = 0;			// tramo en el que va la nave (0 a 6)
//Largo de cada tramo: recta, rampa de subida, recta elevada, rampa de bajada, curva, recta de regreso, curva
float largoTramo[7] = { 100.0f, 63.25f, 40.0f, 63.25f, 188.5f, 260.0f, 188.5f };
//float movCoche = 0.0f;		// avance del rover
//float movOffset = 0.5f;		// velocidad del rover: ~30 unidades por segundo

//------------------------- Ventana, cámara y shaders -------------------------
Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

//------------------------- Texturas -------------------------
Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture naveTexture;	// albedo de la nave Wing-Y
Texture fuegoTexture;	// plasma azul de los motores (PNG con alfa)
Texture pistaTexture;	// asfalto de la pista de carreras

//------------------------- Modelos -------------------------
//Se comentan rover y avión para que no aparezcan en escena
/*
//Rover con jerarquía
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
Model LlantaDelanteraDer_M;
Model LlantaDelanteraIzq_M;
Model LlantaMediaDer_M;
Model LlantaMediaIzq_M;
Model LlantaTraseraDer_M;
Model LlantaTraseraIzq_M;

//Avión
Model Avion_M;
*/

//Satélite con jerarquía
Model SatBase_M;	// cuerpo (padre)
Model SatTail_M;	// cola
Model P1_M;			// panel solar 1
Model P2_M;			// panel solar 2

//Nave Wing-Y de la práctica anterior
Model Nave_M;

Skybox skybox;

//------------------------- Materiales -------------------------
Material Material_brillante;
Material Material_opaco;

//------------------------- Tiempo ------------------------
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

//------------------------- Luces -------------------------
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];


//==================================================================
//                        FUNCIONES AUXILIARES
//==================================================================

//Promedio de normales para sombreado de Phong
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

//Geometría creada por código: pirámides, piso, vegetación, flecha y números
void CreateObjects()
{
	//Pirámide
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

	//Piso
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

	//Vegetación (dos planos cruzados)
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

	//Flecha
	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	//Plano con todos los números
	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	//Plano de un solo número
	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	MeshModel* obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);	// [0] pirámide

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);	// [1] pirámide

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);	// [2] piso

	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);	// [3] vegetación

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);	// [4] flecha

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);	// [5] todos los números

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);	// [6] solo un número

	//Fuego de los motores: dos planos cruzados (XY y XZ) para que se vea desde cualquier ángulo.
	//Va en ejes del modelo de la nave: la base (tobera) en x = 0 y la punta en x = -1 (hacia atrás).
	//En la textura la base de la llama está abajo, por eso la base lleva v = 1 y la punta v = 0.
	unsigned int fuegoIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7
	};

	GLfloat fuegoVertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		0.0f, -0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		0.0f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		-1.0f, 0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
		-1.0f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.5f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, -0.5f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f
	};

	MeshModel* obj8 = new MeshModel();
	obj8->CreateMeshModel(fuegoVertices, fuegoIndices, 64, 12);
	meshList.push_back(obj8);	// [7] fuego de los motores

	//Tramo recto de pista: cuadro de 1 x 1 que empieza en z = 0 y avanza hacia +Z.
	//Al dibujarlo se escala a 20 de ancho y al largo de cada mosaico
	unsigned int tramoIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat tramoVertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		-0.5f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, 1.0f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 1.0f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f
	};

	MeshModel* obj9 = new MeshModel();
	obj9->CreateMeshModel(tramoVertices, tramoIndices, 32, 6);
	meshList.push_back(obj9);	// [8] tramo recto de pista

	//Curva de pista: media vuelta de radio 60 y 20 de ancho (de radio 50 a 70) hecha con 36 segmentos.
	//Empieza en (-60, 0, 0), pasa por (0, 0, -60) y termina en (60, 0, 0). La textura se repite cada 10 unidades
	GLfloat curvaVertices[37 * 2 * 8];
	unsigned int curvaIndices[36 * 6];
	for (int i = 0; i <= 36; i++)
	{
		float ang = 3.14159265f * i / 36.0f;
		float v = 60.0f * ang / 10.0f;	// distancia recorrida en el centro / 10
		GLfloat interior[] = { -50.0f * cos(ang), 0.0f, -50.0f * sin(ang),	0.0f, v,	0.0f, -1.0f, 0.0f };
		GLfloat exterior[] = { -70.0f * cos(ang), 0.0f, -70.0f * sin(ang),	1.0f, v,	0.0f, -1.0f, 0.0f };
		for (int k = 0; k < 8; k++)
		{
			curvaVertices[i * 16 + k] = interior[k];
			curvaVertices[i * 16 + 8 + k] = exterior[k];
		}
	}
	//Dos triángulos por segmento entre el borde interior y el exterior
	for (int i = 0; i < 36; i++)
	{
		curvaIndices[i * 6 + 0] = i * 2;
		curvaIndices[i * 6 + 1] = i * 2 + 2;
		curvaIndices[i * 6 + 2] = i * 2 + 1;
		curvaIndices[i * 6 + 3] = i * 2 + 1;
		curvaIndices[i * 6 + 4] = i * 2 + 2;
		curvaIndices[i * 6 + 5] = i * 2 + 3;
	}

	MeshModel* obj10 = new MeshModel();
	obj10->CreateMeshModel(curvaVertices, curvaIndices, 37 * 2 * 8, 36 * 6);
	meshList.push_back(obj10);	// [9] curva de pista
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	//==================================================================
	//                         INICIALIZACIÓN
	//==================================================================

	//------------------------- Ventana y cámara -------------------------
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	//------------------------- Carga de texturas -------------------------
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
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();

	//Textura de la nave: es .jpg sin canal alfa, por eso LoadTexture()
	naveTexture = Texture("Textures/DefaultMaterial_albedo.jpg");
	naveTexture.LoadTexture();

	//Textura del fuego: es .png con canal alfa, por eso LoadTextureA()
	fuegoTexture = Texture("Textures/fuego_azul.png");
	fuegoTexture.LoadTextureA();

	//Textura de la pista: .png con canal alfa
	pistaTexture = Texture("Textures/pista.png");
	pistaTexture.LoadTextureA();

	//------------------------- Carga de modelos -------------------------
	//Se comenta la carga del rover y del avión
	/*
	//Rover: cuerpo
	Cuerpo_M = Model();
	Cuerpo_M.LoadModel("Models/cuerpo.fbx");

	//Rover: brazo
	BB_M = Model();
	BB_M.LoadModel("Models/BB.fbx");
	BM_M = Model();
	BM_M.LoadModel("Models/BM.fbx");
	BMA_M = Model();
	BMA_M.LoadModel("Models/BMA.fbx");
	BP_M = Model();
	BP_M.LoadModel("Models/BP.fbx");

	//Rover: patas
	PataDelanteraDer_M = Model();
	PataDelanteraDer_M.LoadModel("Models/PFD_pata.obj");
	PataDelanteraIzq_M = Model();
	PataDelanteraIzq_M.LoadModel("Models/PFI_pata.obj");
	PataMediaDer_M = Model();
	PataMediaDer_M.LoadModel("Models/PMD_pata.obj");
	PataMediaIzq_M = Model();
	PataMediaIzq_M.LoadModel("Models/PMI_pata.obj");
	PataTraseraDer_M = Model();
	PataTraseraDer_M.LoadModel("Models/PTD_pata.obj");
	PataTraseraIzq_M = Model();
	PataTraseraIzq_M.LoadModel("Models/PTI_pata.obj");

	//Rover: llantas
	LlantaDelanteraDer_M = Model();
	LlantaDelanteraDer_M.LoadModel("Models/PFD_llanta.obj");
	LlantaDelanteraIzq_M = Model();
	LlantaDelanteraIzq_M.LoadModel("Models/PFI_llanta.obj");
	LlantaMediaDer_M = Model();
	LlantaMediaDer_M.LoadModel("Models/PMD_llanta.obj");
	LlantaMediaIzq_M = Model();
	LlantaMediaIzq_M.LoadModel("Models/PMI_llanta.obj");
	LlantaTraseraDer_M = Model();
	LlantaTraseraDer_M.LoadModel("Models/PTD_llanta.obj");
	LlantaTraseraIzq_M = Model();
	LlantaTraseraIzq_M.LoadModel("Models/PTI_llanta.obj");

	//Avión
	Avion_M = Model();
	Avion_M.LoadModel("Models/plane.obj");
	*/

	//Satélite
	SatBase_M = Model();
	SatBase_M.LoadModel("Models/SatBase.fbx");
	SatTail_M = Model();
	SatTail_M.LoadModel("Models/SatTail.fbx");
	P1_M = Model();
	P1_M.LoadModel("Models/P1.fbx");
	P2_M = Model();
	P2_M.LoadModel("Models/P2.fbx");

	//Nave Wing-Y
	Nave_M = Model();
	Nave_M.LoadModel("Models/Wing-Y.fbx");

	//------------------------- Skybox y materiales -------------------------
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

	//------------------------- Luces -------------------------
	//Luz direccional: solo 1 y siempre debe existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	unsigned int pointLightCount = 0;

	//Se comenta la luz de la pinza del rover: pointLightCount se queda en 0
	/*
	//Luz puntual verde de la pinza del rover (posición se actualiza en el ciclo)
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,	// color
		0.0f, 1.0f,									// intensidad ambiental, difusa
		0.0f, 0.0f, 0.0f,							// posición
		1.0f, 0.045f, 0.0075f);						// atenuación
	pointLightCount++;
	*/

	//Luz puntual de la nave: naranja, debajo del fuselaje (posición se actualiza en el ciclo)
	pointLights[0] = PointLight(1.0f, 0.5f, 0.0f,	// color naranja
		0.0f, 1.0f,									// intensidad ambiental, difusa
		0.0f, 0.0f, 0.0f,							// posición
		1.0f, 0.045f, 0.0075f);						// atenuación
	pointLightCount++;

	//Luces puntuales azules de los motores: [1] motor 1 (y positiva), [2] motor 2 (y negativa)
	pointLights[1] = PointLight(0.2f, 0.5f, 1.0f,	// color azul
		0.6f, 1.0f,									// intensidad ambiental, difusa
		0.0f, 0.0f, 0.0f,							// posición
		1.0f, 0.09f, 0.032f);						// atenuación
	pointLightCount++;
	pointLights[2] = PointLight(0.2f, 0.5f, 1.0f,
		0.6f, 1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	unsigned int spotLightCount = 0;

	//Linterna ligada a la cámara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//Faro de la nave: en la nariz, apunta al frente y hacia abajo (posición y dirección se actualizan en el ciclo)
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.8f,		// color
		0.2f, 1.0f,									// intensidad ambiental, difusa
		0.0f, 0.0f, 0.0f,							// posición
		0.0f, -1.0f, 0.0f,							// dirección
		1.0f, 0.014f, 0.0007f,						// atenuación
		20.0f);										// apertura del cono
	spotLightCount++;

	//Se comentan el faro del rover y las luces del avión: solo queda la linterna (spotLightCount = 1)
	/*
	//Faro del rover (posición y dirección se actualizan en el ciclo)
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.6f,		// color
		0.2f, 1.0f,									// intensidad ambiental, difusa
		5.0f, 10.0f, 0.0f,							// posición
		0.0f, -5.0f, 0.0f,							// dirección
		1.0f, 0.014f, 0.0007f,						// atenuación
		15.0f);										// apertura del cono
	spotLightCount++;

	//Avión: luz hacia abajo (color cambia con la tecla 1)
	spotLights[2] = SpotLight(1.0f, 0.0f, 0.0f,		// color
		0.2f, 1.0f,									// intensidad ambiental, difusa
		0.0f, 0.0f, 0.0f,							// posición
		0.0f, -1.0f, 0.0f,							// dirección
		1.0f, 0.014f, 0.0007f,						// atenuación
		20.0f);										// apertura del cono
	spotLightCount++;

	//Avión: luz hacia adelante
	spotLights[3] = SpotLight(0.6f, 1.0f, 0.6f,		// color
		0.2f, 1.0f,									// intensidad ambiental, difusa
		0.0f, 0.0f, 0.0f,							// posición
		1.0f, 0.0f, 0.0f,							// dirección
		1.0f, 0.009f, 0.0003f,						// atenuación
		20.0f);										// apertura del cono
	spotLightCount++;
	*/

	//------------------------- Uniforms y matrices -------------------------
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	//movCoche = 0.0f;
	//movOffset = 0.5f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelaux2(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	//Se comentan los datos de la pinza y del faro del avión
	/*
	//Punta de la pinza en coordenadas locales de BP
	glm::vec3 puntaPinza = glm::vec3(1.5f, 0.0f, 0.0f);

	//Colores del faro inferior del avión, en el orden del ciclo
	const glm::vec3 coloresFaroAvion[6] = {
		glm::vec3(1.0f, 0.0f, 0.0f),	// rojo
		glm::vec3(1.0f, 1.0f, 0.0f),	// amarillo
		glm::vec3(0.0f, 1.0f, 0.0f),	// verde
		glm::vec3(0.0f, 1.0f, 1.0f),	// cian
		glm::vec3(0.0f, 0.0f, 1.0f),	// azul
		glm::vec3(1.0f, 0.0f, 1.0f)		// magenta
	};
	*/

	//Empieza en el tiempo actual para que el primer deltaTime no incluya la carga de modelos
	lastTime = glfwGetTime();


	//==================================================================
	//                       CICLO PRINCIPAL
	//==================================================================
	while (!mainWindow.getShouldClose())
	{
		//------------------------- Tiempo y animación -------------------------
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Reloj de la animación compleja (avión y satélite), en grados de 0 a 360
		angulovaria += 0.5f * deltaTime;
		if (angulovaria > 360.0f)
			angulovaria -= 360.0f;

		//Reloj de la nave: mismo deltaTime pero con su propia velocidad (~60 unidades por segundo)
		avanceNave += 1.8f * deltaTime;
		//Animación por estados: al terminar un tramo pasa al siguiente y después del último regresa al primero
		if (avanceNave >= largoTramo[tramoNave])
		{
			avanceNave -= largoTramo[tramoNave];
			tramoNave = (tramoNave + 1) % 7;
		}

		//Se comenta el movimiento del rover
		/*
		//Animación simple del rover: la bandera avanza cambia al llegar a cada orilla del piso
		if (avanza)
		{
			if (movCoche < 292.0f)
				movCoche += movOffset * deltaTime;
			else
				avanza = false;
		}
		else
		{
			if (movCoche > -293.0f)
				movCoche -= movOffset * deltaTime;
			else
				avanza = true;
		}
		*/

		//------------------------- Entrada y cámara -------------------------
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//------------------------- Shader y uniforms -------------------------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();	// textura con movimiento
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//------------------------- Luces dinámicas -------------------------
		//Linterna ligada a la cámara
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//Posición y orientación de la nave según el tramo de la pista en el que va.
		//tramoNave elige la fórmula y avanceNave dice cuánto lleva recorrido de ese tramo
		glm::vec3 posNave(0.0f);
		float giroNave = 0.0f;			// hacia dónde voltea (eje Y)
		float inclinacionNave = 0.0f;	// nariz arriba o abajo en las rampas
		float ladeoNave = 0.0f;			// se ladea hacia adentro en las curvas
		switch (tramoNave)
		{
		case 0:	//Recta de salida: avanza hacia -Z
			posNave = glm::vec3(-60.0f, 2.2f, 120.0f - avanceNave);
			giroNave = 180.0f;
			break;
		case 1:	//Rampa de subida: sube 20 mientras avanza 60
			posNave = glm::vec3(-60.0f, 2.2f + 20.0f * avanceNave / 63.25f, 20.0f - 60.0f * avanceNave / 63.25f);
			giroNave = 180.0f;
			inclinacionNave = 18.43f;	// atan(20 / 60)
			break;
		case 2:	//Recta elevada
			posNave = glm::vec3(-60.0f, 22.2f, -40.0f - avanceNave);
			giroNave = 180.0f;
			break;
		case 3:	//Rampa de bajada: baja 20 mientras avanza 60
			posNave = glm::vec3(-60.0f, 22.2f - 20.0f * avanceNave / 63.25f, -80.0f - 60.0f * avanceNave / 63.25f);
			giroNave = 180.0f;
			inclinacionNave = -18.43f;
			break;
		case 4:	//Curva 1: media vuelta de radio 60 alrededor de (0, -140); ángulo = distancia / radio
			posNave = glm::vec3(-60.0f * cos(avanceNave / 60.0f), 2.2f, -140.0f - 60.0f * sin(avanceNave / 60.0f));
			giroNave = 180.0f - glm::degrees(avanceNave / 60.0f);
			ladeoNave = 20.0f * sin(avanceNave / 60.0f);	// 0 al entrar y al salir, 20° a la mitad
			break;
		case 5:	//Recta de regreso: avanza hacia +Z
			posNave = glm::vec3(60.0f, 2.2f, -140.0f + avanceNave);
			giroNave = 0.0f;
			break;
		case 6:	//Curva 2: media vuelta de radio 60 alrededor de (0, 120)
			posNave = glm::vec3(60.0f * cos(avanceNave / 60.0f), 2.2f, 120.0f + 60.0f * sin(avanceNave / 60.0f));
			giroNave = -glm::degrees(avanceNave / 60.0f);
			ladeoNave = 20.0f * sin(avanceNave / 60.0f);
			break;
		}

		//Matriz base de la nave: la comparten el modelo, el fuego y las luces de los motores
		glm::mat4 baseNave(1.0);
		baseNave = glm::translate(baseNave, posNave);	// antes (20, 5, -35) fija; ahora sigue la pista
		baseNave = glm::rotate(baseNave, glm::radians(giroNave), glm::vec3(0.0f, 1.0f, 0.0f));	// voltea hacia donde avanza
		baseNave = glm::rotate(baseNave, glm::radians(-inclinacionNave), glm::vec3(1.0f, 0.0f, 0.0f));	// nariz arriba o abajo en las rampas
		baseNave = glm::rotate(baseNave, glm::radians(ladeoNave), glm::vec3(0.0f, 0.0f, 1.0f));	// se ladea en las curvas
		baseNave = glm::rotate(baseNave, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// el FBX trae Z hacia arriba
		baseNave = glm::rotate(baseNave, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));	// nariz hacia +Z del mundo

		//Faro de la nave: vec4 en ejes del modelo (X = hacia la nariz, Z = arriba)
		//Posición con w = 1: 8 al frente. Dirección con w = 0: al frente y hacia abajo
		spotLights[1].SetFlash(glm::vec3(baseNave * glm::vec4(8.0f, 0.0f, 0.0f, 1.0f)),
			glm::normalize(glm::vec3(baseNave * glm::vec4(1.0f, 0.0f, -0.6f, 0.0f))));

		//Luz puntual de la nave: 3 debajo del origen del modelo
		//PointLight no tiene función para mover la luz: se vuelve a crear en la posición nueva
		glm::vec3 posLuzPuntualNave = glm::vec3(baseNave * glm::vec4(0.0f, 0.0f, -3.0f, 1.0f));
		pointLights[0] = PointLight(1.0f, 0.5f, 0.0f,
			0.0f, 1.0f,
			posLuzPuntualNave.x, posLuzPuntualNave.y, posLuzPuntualNave.z,
			1.0f, 0.045f, 0.0075f);

		//Luces azules de los motores, dentro de su fuego
		glm::vec3 posLuzMotor1 = glm::vec3(baseNave * glm::vec4(-28.0f, 6.41f, -0.58f, 1.0f));
		pointLights[1] = PointLight(0.2f, 0.5f, 1.0f,
			0.6f, 1.0f,
			posLuzMotor1.x, posLuzMotor1.y, posLuzMotor1.z,
			1.0f, 0.09f, 0.032f);
		glm::vec3 posLuzMotor2 = glm::vec3(baseNave * glm::vec4(-28.0f, -6.41f, -0.58f, 1.0f));
		pointLights[2] = PointLight(0.2f, 0.5f, 1.0f,
			0.6f, 1.0f,
			posLuzMotor2.x, posLuzMotor2.y, posLuzMotor2.z,
			1.0f, 0.09f, 0.032f);

		//Se comentan las matrices base y las luces del rover y del avión
		/*
		//Matriz base del rover: la comparten el cuerpo y el faro
		glm::mat4 baseRover(1.0);
		baseRover = glm::rotate(baseRover, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		baseRover = glm::translate(baseRover, glm::vec3(movCoche, 0.5f, 0.0f));	// avance con movCoche

		//Faro del rover: posición con w = 1, dirección con w = 0
		glm::vec3 posFaroRover = glm::vec3(baseRover * glm::vec4(4.0f, 0.5f, 0.0f, 1.0f));
		glm::vec3 dirFaroRover = glm::normalize(glm::vec3(baseRover * glm::vec4(1.0f, -0.3f, 0.0f, 0.0f)));
		spotLights[1].SetFlash(posFaroRover, dirFaroRover);

		//Matriz base del avión. Animación compleja: círculo de radio 40 a 15 de altura
		glm::mat4 baseAvion(1.0);
		baseAvion = glm::translate(baseAvion, glm::vec3(0.0f, 15.0f, 0.0f));	// centro del círculo
		baseAvion = glm::rotate(baseAvion, glm::radians(-angulovaria), glm::vec3(0.0f, 1.0f, 0.0f));
		baseAvion = glm::translate(baseAvion, glm::vec3(40.0f, 0.0f, 0.0f));	// radio

		//Luces del avión: salen de baseAvion, así se mueven con él
		glm::vec3 frenteAvion = glm::vec3(0.0f, 0.0f, 1.0f);	// nariz del avión

		//Luz hacia abajo: se vuelve a crear con el color elegido y luego se coloca
		glm::vec3 posLuzAbajoAvion = glm::vec3(baseAvion * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));
		glm::vec3 dirLuzAbajoAvion = glm::normalize(glm::vec3(baseAvion * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)));
		glm::vec3 colorFaroAvion = coloresFaroAvion[mainWindow.getColorFaroAvion() % 6];
		spotLights[2] = SpotLight(colorFaroAvion.r, colorFaroAvion.g, colorFaroAvion.b,
			0.2f, 1.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			1.0f, 0.014f, 0.0007f,
			20.0f);
		spotLights[2].SetFlash(posLuzAbajoAvion, dirLuzAbajoAvion);

		//Luz hacia adelante: sale de la nariz, inclinada 45° hacia el piso
		glm::vec3 posLuzFrenteAvion = glm::vec3(baseAvion * glm::vec4(frenteAvion * 3.0f, 1.0f));
		glm::vec3 dirLuzFrenteAvion = glm::normalize(glm::vec3(baseAvion * glm::vec4(frenteAvion + glm::vec3(0.0f, -1.0f, 0.0f), 0.0f)));
		spotLights[3].SetFlash(posLuzFrenteAvion, dirLuzFrenteAvion);
		*/

		//Envío de luces al shader (la pinza solo si está prendida con la tecla 3)
		shaderList[0].SetDirectionalLight(&mainLight);
		//PointLight pointLightsActivas[MAX_POINT_LIGHTS];
		//unsigned int pointLightCountActivas = 0;
		//if (mainWindow.getLuzPinza()) pointLightsActivas[pointLightCountActivas++] = pointLights[1];	// luz de la pinza
		//shaderList[0].SetPointLights(pointLightsActivas, pointLightCountActivas);
		shaderList[0].SetPointLights(pointLights, pointLightCount);	// sin luces puntuales (pointLightCount = 0)
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//------------------------- Piso -------------------------
		//Reinicio de variables en cada ciclo
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);

		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMeshModel();

		//------------------------- Pista de carreras -------------------------
		//Mosaicos de 20 x 10 para rectas y rampas, y dos medias vueltas; va 0.2 arriba del piso (y = -1.8)
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pistaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//Tramo 1: recta de salida, de z = 120 a z = 20
		for (int i = 0; i < 10; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-60.0f, -1.8f, 120.0f - i * 10.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// avanza hacia -Z
			model = glm::scale(model, glm::vec3(20.0f, 1.0f, 10.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[8]->RenderMeshModel();
		}

		//Tramo 2: rampa de subida, sube 20 en 60 (6 mosaicos de 10.54)
		for (int i = 0; i < 6; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-60.0f, -1.8f + i * 3.333f, 20.0f - i * 10.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// avanza hacia -Z
			model = glm::rotate(model, glm::radians(-18.43f), glm::vec3(1.0f, 0.0f, 0.0f));	// pendiente hacia arriba: atan(20 / 60)
			model = glm::scale(model, glm::vec3(20.0f, 1.0f, 10.54f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[8]->RenderMeshModel();
		}

		//Tramo 3: recta elevada a 20 de altura, de z = -40 a z = -80
		for (int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-60.0f, 18.2f, -40.0f - i * 10.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// avanza hacia -Z
			model = glm::scale(model, glm::vec3(20.0f, 1.0f, 10.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[8]->RenderMeshModel();
		}

		//Tramo 4: rampa de bajada, baja 20 en 60
		for (int i = 0; i < 6; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-60.0f, 18.2f - i * 3.333f, -80.0f - i * 10.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// avanza hacia -Z
			model = glm::rotate(model, glm::radians(18.43f), glm::vec3(1.0f, 0.0f, 0.0f));	// pendiente hacia abajo
			model = glm::scale(model, glm::vec3(20.0f, 1.0f, 10.54f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[8]->RenderMeshModel();
		}

		//Tramo 5: curva 1, media vuelta alrededor de (0, -140)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, -140.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[9]->RenderMeshModel();

		//Tramo 6: recta de regreso, de z = -140 a z = 120 (26 mosaicos)
		for (int i = 0; i < 26; i++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(60.0f, -1.8f, -140.0f + i * 10.0f));
			model = glm::scale(model, glm::vec3(20.0f, 1.0f, 10.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[8]->RenderMeshModel();
		}

		//Tramo 7: curva 2, la misma media vuelta girada 180° alrededor de (0, 120)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 120.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[9]->RenderMeshModel();

		//------------------------- Rover -------------------------
		//Se comenta todo el dibujo del rover para que no aparezca en escena
		/*
		//Animación simple: avanza y retrocede con movCoche; las llantas ruedan con el avance
		color = glm::vec3(1.0f, 1.0f, 1.0f);

		//Cuerpo: misma matriz que el faro para que siempre vayan juntos
		model = baseRover;
		modelaux = model;
		modelaux2 = model;	// matriz padre de patas y brazo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuerpo_M.RenderModel();

		//Pata delantera derecha (tecla K)
		model = glm::translate(model, glm::vec3(2.0f, 1.5, -2.8f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		PataDelanteraDer_M.RenderModel();

		//Llanta delantera derecha: gira con la tecla M y con el avance (distancia / radio)
		model = glm::translate(model, glm::vec3(3.899f, -2.829f, 0.0f));	// eje de la llanta
		model = glm::rotate(model, glm::radians(mainWindow.getGiroLlantas()) - movCoche / 1.241f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDelanteraDer_M.RenderModel();

		//Pata delantera izquierda (tecla L)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.0f, 1.5, 3.3f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.50f, 1.0f, 0.5f);//Color verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		PataDelanteraIzq_M.RenderModel();

		//Llanta delantera izquierda
		model = glm::translate(model, glm::vec3(3.901f, -2.829f, 0.0f));	// eje de la llanta
		model = glm::rotate(model, glm::radians(mainWindow.getGiroLlantas()) - movCoche / 1.241f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDelanteraIzq_M.RenderModel();

		//Pata media derecha (tecla C)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.0f, 0.5, -3.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		PataMediaDer_M.RenderModel();

		//Llanta media derecha
		model = glm::translate(model, glm::vec3(2.481f, -1.873f, 0.0f));	// eje de la llanta
		model = glm::rotate(model, glm::radians(mainWindow.getGiroLlantas()) - movCoche / 1.241f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaMediaDer_M.RenderModel();

		//Pata media izquierda (tecla V)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.0f, 0.5, 3.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		PataMediaIzq_M.RenderModel();

		//Llanta media izquierda
		model = glm::translate(model, glm::vec3(2.502f, -2.066f, 0.0f));	// eje de la llanta
		model = glm::rotate(model, glm::radians(mainWindow.getGiroLlantas()) - movCoche / 1.241f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaMediaIzq_M.RenderModel();

		//Pata trasera derecha (tecla B)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.0f, 0.5, -3.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		PataTraseraDer_M.RenderModel();

		//Llanta trasera derecha
		model = glm::translate(model, glm::vec3(-2.488f, -2.092f, 0.0f));	// eje de la llanta
		model = glm::rotate(model, glm::radians(mainWindow.getGiroLlantas()) - movCoche / 1.241f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTraseraDer_M.RenderModel();

		//Pata trasera izquierda (tecla N)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-3.0f, 0.5, 3.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion10()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		PataTraseraIzq_M.RenderModel();

		//Llanta trasera izquierda
		model = glm::translate(model, glm::vec3(-2.505f, -2.066f, 0.0f));	// eje de la llanta
		model = glm::rotate(model, glm::radians(mainWindow.getGiroLlantas()) - movCoche / 1.241f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTraseraIzq_M.RenderModel();

		//Brazo: base
		model = modelaux2;
		model = glm::translate(model, glm::vec3(2.0f, 2.0, -1.2f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		BB_M.RenderModel();

		//Brazo: articulación 1 (tecla F)
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		BM_M.RenderModel();

		//Brazo: articulación 2 (tecla G)
		modelaux = model;
		model = glm::translate(model, glm::vec3(3.0f, 2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		BMA_M.RenderModel();

		//Brazo: pinza, articulación 3 (tecla H)
		modelaux = model;
		model = glm::translate(model, glm::vec3(-3.5f, 3.0f, 0.15));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		BP_M.RenderModel();

		//Luz de la pinza: nodo hijo de BP, su posición sale de la columna 3 de la matriz
		modelaux = model;
		model = glm::translate(model, puntaPinza);
		glm::vec3 posPinza = glm::vec3(model[3]);
		model = modelaux;	// regresa a la matriz de BP
		//PointLight no tiene función para mover la luz: se vuelve a crear en la posición nueva
		pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
			0.0f, 1.0f,
			posPinza.x, posPinza.y, posPinza.z,
			1.0f, 0.045f, 0.0075f);
		//Se vuelven a mandar las luces puntuales ya con la pinza en su lugar
		pointLightCountActivas = 0;
		if (mainWindow.getLuzPinza()) pointLightsActivas[pointLightCountActivas++] = pointLights[1];	// luz de la pinza
		shaderList[0].SetPointLights(pointLightsActivas, pointLightCountActivas);
		*/

		//------------------------- Avión -------------------------
		//Se comenta el dibujo del avión para que no aparezca en escena
		/*
		//Animación compleja: la posición viene de baseAvion (círculo)
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = baseAvion;
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));	// se inclina en la curva
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Avion_M.RenderModel();
		*/

		//------------------------- Satélite -------------------------
		//Animación compleja: vuela un ocho (curva de Lissajous 1:2) con t = angulovaria
		//x = 80 + 90 sen(t),  y = 242 + 4 cos(t),  z = 90 sen(2t)
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);	// superficie metálica

		//Base: trayectoria y orientación
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 200.0f, 0.0f));	// sube el ocho a 200 de altura
		model = glm::translate(model, glm::vec3(80.0f + 90.0f * sin(glm::radians(angulovaria)), 42.0f + 4.0f * cos(glm::radians(angulovaria)), 90.0f * sin(2.0f * glm::radians(angulovaria))));
		//model = glm::translate(model, glm::vec3(mainWindow.getmovX(), mainWindow.getmovY(), mainWindow.getmovZ())); // movimiento con flechas
		//Voltea hacia donde avanza: atan2 de la derivada (dx/dt = 90 cos t, dz/dt = 180 cos 2t)
		model = glm::rotate(model, atan2(-90.0f * cos(glm::radians(angulovaria)), -180.0f * cos(2.0f * glm::radians(angulovaria))), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// orientación del modelo
		modelaux = model;
		modelaux2 = model;	// matriz padre de cola y paneles
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		SatBase_M.RenderModel();

		//Cola: giro continuo, 2 vueltas por cada ocho (antes tecla E)
		model = modelaux;
		model = glm::translate(model, glm::vec3(00.0f, 5.0f, 0.0f));	// punto de unión
		model = glm::rotate(model, glm::radians(angulovaria * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));	// corrección fija
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.0f, 1.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		SatTail_M.RenderModel();

		//Panel 1: se mece entre -45° y 45° sobre su unión (antes tecla R)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(15.5f, -8.0f, 0.0f));	// punto de unión
		modelaux = model;
		model = glm::rotate(model, glm::radians(45.0f * sin(glm::radians(angulovaria * 2.0f))), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.8f, 1.0f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		P1_M.RenderModel();

		//Panel 2: mismo ángulo que el panel 1 (antes tecla T)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-15.0f, -8.0f, 0.0f));	// punto de unión
		modelaux = model;
		model = glm::rotate(model, glm::radians(45.0f * sin(glm::radians(angulovaria * 2.0f))), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(0.5f, 0.5f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		plainTexture.UseTexture();
		P2_M.RenderModel();

		//------------------------- Nave -------------------------
		//Nave Wing-Y de la práctica anterior, por ahora fija en la escena
		model = baseNave;	// misma matriz que el fuego y las luces de los motores
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		naveTexture.UseTexture();	// respaldo por si Model no encuentra la textura sola
		Nave_M.RenderModel();

		//Fuego de los motores: planos con transparencia que salen de cada tobera
		//glDepthMask(GL_FALSE) evita que lo transparente de un plano tape al plano cruzado
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		fuegoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//Motor 1 (y positiva): tobera en (-17, 6.41, -0.58)
		model = baseNave;
		model = glm::translate(model, glm::vec3(-17.0f, 6.41f, -0.58f));	// antes -26, igual que en la P08
		model = glm::scale(model, glm::vec3(15.0f, 4.0f, 4.0f));	// antes 9; 15 de largo hacia atrás, 4 de ancho
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();

		//Motor 2 (y negativa): tobera en (-17, -6.41, -0.58)
		model = baseNave;
		model = glm::translate(model, glm::vec3(-17.0f, -6.41f, -0.58f));	// antes -26
		model = glm::scale(model, glm::vec3(15.0f, 4.0f, 4.0f));	// antes 9
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[7]->RenderMeshModel();

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		//------------------------- Texturas animadas -------------------------
		//Blending para transparencia de la flecha y los números
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Flecha: la textura se desliza sumando un offset en U
		toffsetflechau += 0.001;
		toffsetflechav = 0.000;
		if (toffsetflechau > 1.0)	// evita que se desborde
			toffsetflechau = 0.0;
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FlechaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMeshModel();

		//Plano con todos los números
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		//Número 1
		//toffsetnumerou = 0.0;
		//toffsetnumerov = 0.0;
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 2.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		//glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		//Números 2 a 4: recorren la textura 0.25 en U
		for (int i = 1; i < 4; i++)
		{
			toffsetnumerou += 0.25;
			toffsetnumerov = 0.0;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-10.0f - (i * 3.0), 2.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();
		}

		//Números 5 a 8: segunda fila de la textura (-0.33 en V)
		for (int j = 1; j < 5; j++)
		{
			toffsetnumerou += 0.25;
			toffsetnumerov = -0.33;
			toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-7.0f - (j * 3.0), 5.0f, -6.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			NumerosTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[6]->RenderMeshModel();
		}

		//Número cambiante: avanza un número cada frame
		//¿Cómo hacer para que sea a una velocidad visible?
		toffsetnumerocambiau += 0.25;
		if (toffsetnumerocambiau > 1.0)
			toffsetnumerocambiau = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerocambiau, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		NumerosTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[6]->RenderMeshModel();

		//Cambio automático entre textura número 1 y número 2
		toffsetnumerou = 0.0;
		toffsetnumerov = 0.0;
		toffset = glm::vec2(toffsetnumerou, toffsetnumerov);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.0f, 10.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Numero1Texture.UseTexture();
		//if
		//Numero1Texture.UseTexture();
		//Numero2Texture.UseTexture();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMeshModel();

		glDisable(GL_BLEND);

		//------------------------- Fin del frame -------------------------
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}