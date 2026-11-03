/*
Práctica 9-2: Animación Compleja
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
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;	// tiempo de la animación en grados (avión y satélite)
//Se comentan el reloj del avión y el humo
/*
//Reloj del avión: distancia recorrida dentro del tramo actual de su circuito de acrobacias
float avanceAvion = 0.0f;
int tramoAvion = 0;			// tramo en el que va el avión (0 a 5)
//Largo de cada tramo: recta con tonel, curva, recta, giro, recta, curva
float largoTramoAvion[6] = { 160.0f, 157.08f, 50.0f, 125.66f, 110.0f, 157.08f };

//Humo del avión: sistema de partículas. Cada bocanada nace en un motor, se queda en el lugar
//del mundo donde nació (así forma la estela), crece, sube un poco y desaparece a los 2 segundos
struct ParticulaHumo
{
	glm::vec3 pos;	// posición en el mundo
	float edad;		// segundos que lleva viva
	bool viva;
};
const int MAX_HUMO = 400;				// 2 motores x 50 bocanadas por segundo x 2 segundos = 200 vivas a la vez
ParticulaHumo humo[MAX_HUMO] = {};		// todas empiezan muertas
int siguienteHumo = 0;					// siguiente casilla a reusar (arreglo circular)
float acumHumo = 0.0f;					// tiempo acumulado para soltar la siguiente bocanada
*/
//Reloj del holocrón: segundos reales dentro de su ciclo de 10 s
float tiempoHolo = 0.0f;
//Cohete: animación por estados con su propio reloj en segundos reales
int faseCohete = 0;				// 0 en la plataforma, 1 sube recto, 2 giro, 3 regresa y aterriza en la plataforma
float tiempoCohete = 0.0f;		// segundos que lleva en la fase actual
glm::vec3 posCohete(0.0f);		// centro del cohete medido desde la plataforma
float inclinacionCohete = 0.0f;	// ángulo desde la vertical en radianes: la nariz sigue la tangente de la trayectoria

//------------------------- Ventana, cámara y shaders -------------------------
Window mainWindow;
std::vector<MeshModel*> meshList;
//MeshModel* humoMesh;	// plano del humo, aparte de meshList para que su índice no dependa de lo comentado
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
//Texture humoTexture;	// bocanada de humo (PNG con alfa)

//------------------------- Modelos -------------------------
//Se comenta el avión
//Model Avion_M;

//Holocrón de la práctica 5: base y una esquina que se dibuja 8 veces
Model HoloBase_M;
Model HoloEsq_M;

//Cohete Artemis II
Model Cohete_M;


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

	//Se comenta el plano del humo
	/*
	//Humo: plano de 1 x 1 en XY centrado en el origen, con el frente hacia +Z.
	//Al dibujarlo se voltea hacia la cámara para que siempre se vea de frente
	unsigned int humoIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	GLfloat humoVertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, -1.0f
	};

	humoMesh = new MeshModel();
	humoMesh->CreateMeshModel(humoVertices, humoIndices, 32, 6);
	*/
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

	//Se comenta la textura del humo
	/*
	//Textura del humo: .png con canal alfa (bordes transparentes)
	humoTexture = Texture("Textures/humo.png");
	humoTexture.LoadTextureA();
	*/

	//------------------------- Carga de modelos -------------------------
	//Se comenta la carga del avión
	/*
	//Avión
	Avion_M = Model();
	Avion_M.LoadModel("Models/plane.obj");
	*/

	//Holocrón: base y esquina
	HoloBase_M = Model();
	HoloBase_M.LoadModel("Models/HoloBase.fbx");
	HoloEsq_M = Model();
	HoloEsq_M.LoadModel("Models/HoloEsq.fbx");

	//Cohete
	Cohete_M = Model();
	Cohete_M.LoadModel("Models/Artemis2.fbx");


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

	unsigned int pointLightCount = 0;	// sin luces puntuales

	unsigned int spotLightCount = 0;

	//Linterna ligada a la cámara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//Se comentan las luces del avión: solo queda la linterna (spotLightCount = 1)
	/*
	//Luces del avión: pasan a spotLights[1] y [2] (antes [2] y [3]) porque el faro del rover está comentado
	//y el shader solo lee las primeras spotLightCount luces del arreglo
	//Avión: luz hacia abajo (color cambia con la tecla 1)
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,		// color
		0.2f, 1.0f,									// intensidad ambiental, difusa
		0.0f, 0.0f, 0.0f,							// posición
		0.0f, -1.0f, 0.0f,							// dirección
		1.0f, 0.014f, 0.0007f,						// atenuación
		20.0f);										// apertura del cono
	spotLightCount++;

	//Avión: luz hacia adelante
	spotLights[2] = SpotLight(0.6f, 1.0f, 0.6f,		// color
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

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 modelaux2(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	//Se comentan los colores del faro del avión
	/*
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
		GLfloat dtSegundos = now - lastTime;	// duración real del frame en segundos: el humo vive 2 segundos reales
		lastTime = now;

		//Reloj de la animación compleja (avión y satélite), en grados de 0 a 360
		angulovaria += 0.5f * deltaTime;
		if (angulovaria > 360.0f)
			angulovaria -= 360.0f;

		//Se comenta el reloj del avión
		/*
		//Reloj del avión: ~60 unidades por segundo
		avanceAvion += 1.0f * deltaTime;
		//Animación por estados: al terminar un tramo pasa al siguiente y después del último regresa al primero
		if (avanceAvion >= largoTramoAvion[tramoAvion])
		{
			avanceAvion -= largoTramoAvion[tramoAvion];
			tramoAvion = (tramoAvion + 1) % 6;
		}
		*/

		//Reloj del holocrón: avanza con segundos reales y se reinicia cada 10 s
		tiempoHolo += dtSegundos;
		if (tiempoHolo >= 10.0f)
			tiempoHolo -= 10.0f;

		//Fases del holocrón: 0-1 cerrado, 1-2 se giran, 2-3 se separan, 3-7 flotan,
		//7-8 regresan, 8-9 se acomodan y 9-10 cerrado otra vez

		//Separación de las esquinas por su diagonal: sube de 0 a 6 entre 2 y 3 s, se queda en 6 y
		//regresa a 0 entre 7 y 8 s. Con (1 - cos(pi u)) / 2 arranca y frena suave en vez de dar un jalón
		float separaHolo = 0.0f;
		if (tiempoHolo >= 2.0f && tiempoHolo < 3.0f)
			separaHolo = 6.0f * (1.0f - cos(3.14159265f * (tiempoHolo - 2.0f))) / 2.0f;
		else if (tiempoHolo >= 3.0f && tiempoHolo < 7.0f)
			separaHolo = 6.0f;
		else if (tiempoHolo >= 7.0f && tiempoHolo < 8.0f)
			separaHolo = 6.0f * (1.0f + cos(3.14159265f * (tiempoHolo - 7.0f))) / 2.0f;

		//Giro de las esquinas sobre su diagonal. La velocidad de giro sube suave de 0 a velHolo entre 1 y 2 s,
		//se queda en velHolo hasta los 8 s y baja suave a 0 entre 8 y 9 s. El ángulo es la integral de esa velocidad.
		//Con velHolo = 720 / 7 grados por segundo el total del ciclo da 720° (2 vueltas completas),
		//así la esquina termina igual que empezó y el ciclo empata sin brincos
		float velHolo = 720.0f / 7.0f;
		float giroHolo = 0.0f;
		if (tiempoHolo >= 1.0f && tiempoHolo < 2.0f)
		{
			float u = tiempoHolo - 1.0f;	// 0 a 1 dentro de la fase
			giroHolo = velHolo / 2.0f * (u - sin(3.14159265f * u) / 3.14159265f);
		}
		else if (tiempoHolo >= 2.0f && tiempoHolo < 8.0f)
			giroHolo = velHolo / 2.0f + velHolo * (tiempoHolo - 2.0f);
		else if (tiempoHolo >= 8.0f && tiempoHolo < 9.0f)
		{
			float u = tiempoHolo - 8.0f;
			giroHolo = velHolo / 2.0f + 6.0f * velHolo + velHolo / 2.0f * (u + sin(3.14159265f * u) / 3.14159265f);
		}
		else if (tiempoHolo >= 9.0f)
			giroHolo = 7.0f * velHolo;	// = 720°

		//Reloj del cohete: avanza con segundos reales y la fase decide qué ecuación se usa
		tiempoCohete += dtSegundos;
		switch (faseCohete)
		{
		case 0:	//En la plataforma 3 s; en el último segundo vibra por el encendido
			posCohete = glm::vec3(0.0f, 20.0f, 0.0f);	// el centro está a 20 de la base
			if (tiempoCohete > 2.0f)
				posCohete.x = 0.1f * sin(80.0f * tiempoCohete);	// vibración rápida y chiquita
			inclinacionCohete = 0.0f;
			if (tiempoCohete >= 3.0f)
			{
				faseCohete = 1;
				tiempoCohete = 0.0f;
			}
			break;
		case 1:	//Sube recto 4 s con aceleración constante: y = (1/2)(10)t^2. Termina 80 más arriba yendo a 40 por segundo
		{
			float t = tiempoCohete;
			if (t > 4.0f)
				t = 4.0f;
			posCohete = glm::vec3(0.0f, 20.0f + 5.0f * t * t, 0.0f);
			inclinacionCohete = 0.0f;
			if (tiempoCohete >= 4.0f)
			{
				faseCohete = 2;
				tiempoCohete -= 4.0f;
			}
			break;
		}
		case 2:	//giro: círculo vertical de radio 40 con centro en (40, 100), recorrido a la misma velocidad de 40 por segundo.
			//Ángulo = distancia / radio = 40t / 40 = t. Posición: (40 - 40 cos(ang), 100 + 40 sen(ang)).
			//Entra por el lado izquierdo del círculo yendo hacia arriba, da la vuelta completa y regresa al mismo punto
		{
			float ang = tiempoCohete;	// radianes
			if (ang > 2.0f * 3.14159265f)
				ang = 2.0f * 3.14159265f;
			posCohete = glm::vec3(40.0f - 40.0f * cos(ang), 100.0f + 40.0f * sin(ang), 0.0f);
			inclinacionCohete = ang;	// la tangente del círculo, (sen, cos), está girada justo el mismo ángulo desde la vertical
			if (tiempoCohete >= 2.0f * 3.14159265f)
			{
				faseCohete = 3;
				tiempoCohete -= 2.0f * 3.14159265f;
			}
			break;
		}
		case 3:	//Regresa a la plataforma de cola, con la nariz hacia arriba, como los cohetes que aterrizan.
			//Interpolación de Hermite en 8 s: sale de y = 100 subiendo a 40 por segundo (lo que traía del giro)
			//y llega a y = 20 (parado en la plataforma) con velocidad 0 para que aterrice suave.
			//y(s) = (2s^3 - 3s^2 + 1)(100) + (s^3 - 2s^2 + s)(8)(40) + (-2s^3 + 3s^2)(20), con s = t / 8
		{
			float s = tiempoCohete / 8.0f;
			if (s > 1.0f)
				s = 1.0f;
			float yCohete = (2.0f * s * s * s - 3.0f * s * s + 1.0f) * 100.0f
				+ (s * s * s - 2.0f * s * s + s) * 8.0f * 40.0f
				+ (-2.0f * s * s * s + 3.0f * s * s) * 20.0f;
			posCohete = glm::vec3(0.0f, yCohete, 0.0f);
			inclinacionCohete = 0.0f;
			if (tiempoCohete >= 8.0f)
			{
				faseCohete = 0;	// otra vez en la plataforma y el ciclo vuelve a empezar
				tiempoCohete = 0.0f;
			}
			break;
		}
		}

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

		//Se comenta el círculo del avión: ahora sigue el circuito de acrobacias
		/*
		//Matriz base del avión. Animación compleja: círculo de radio 40 a 15 de altura
		glm::mat4 baseAvion(1.0);
		baseAvion = glm::translate(baseAvion, glm::vec3(0.0f, 15.0f, 0.0f));	// centro del círculo
		baseAvion = glm::rotate(baseAvion, glm::radians(-angulovaria), glm::vec3(0.0f, 1.0f, 0.0f));
		baseAvion = glm::translate(baseAvion, glm::vec3(40.0f, 0.0f, 0.0f));	// radio
		*/

		//Se comentan las acrobacias del avión y sus luces
		/*
		//Acrobacias del avión: circuito tipo estadio a 25 de altura con un tonel en una recta y un giro en la otra.
		//tramoAvion elige la fórmula y avanceAvion dice cuánto lleva recorrido de ese tramo.
		//posAvion es el punto sobre la línea del circuito; desfaseAvion es lo que se sale de esa línea
		//en el tonel y en el giro, medido en ejes del avión (X = lado, Y = arriba, Z = nariz)
		glm::vec3 posAvion(0.0f);
		glm::vec3 desfaseAvion(0.0f);
		float giroAvion = 0.0f;			// hacia dónde voltea (eje Y)
		float cabeceoAvion = 0.0f;		// nariz hacia arriba en el giro
		float alabeoAvion = 0.0f;		// giro sobre su eje largo: tonel y ladeo en las curvas
		switch (tramoAvion)
		{
		case 0:	//Recta con tonel: avanza hacia +Z y da 2 vueltas en espiral de radio 6 alrededor de la línea
		{
			float phi = 4.0f * 3.14159265f * avanceAvion / 160.0f;	// 2 vueltas completas en los 160 del tramo
			posAvion = glm::vec3(50.0f, 25.0f, -80.0f + avanceAvion);
			giroAvion = 0.0f;
			desfaseAvion = glm::vec3(6.0f * sin(phi), 6.0f * (1.0f - cos(phi)), 0.0f);	// espiral: siempre con el techo hacia el eje
			alabeoAvion = glm::degrees(phi);
			break;
		}
		case 1:	//Curva 1: media vuelta de radio 50 alrededor de (0, 80); ángulo = distancia / radio
			posAvion = glm::vec3(50.0f * cos(avanceAvion / 50.0f), 25.0f, 80.0f + 50.0f * sin(avanceAvion / 50.0f));
			giroAvion = -glm::degrees(avanceAvion / 50.0f);
			alabeoAvion = 35.0f * sin(avanceAvion / 50.0f);	// 0 al entrar y al salir, 35° a la mitad
			break;
		case 2:	//Recta de regreso antes del giro: avanza hacia -Z
			posAvion = glm::vec3(-50.0f, 25.0f, 80.0f - avanceAvion);
			giroAvion = 180.0f;
			break;
		case 3:	//giro: círculo vertical de radio 20 que empieza y termina en z = 30; ángulo = distancia / radio
			posAvion = glm::vec3(-50.0f, 25.0f, 30.0f);
			giroAvion = 180.0f;
			desfaseAvion = glm::vec3(0.0f, 20.0f * (1.0f - cos(avanceAvion / 20.0f)), 20.0f * sin(avanceAvion / 20.0f));
			cabeceoAvion = glm::degrees(avanceAvion / 20.0f);	// la nariz da la vuelta completa con el círculo
			break;
		case 4:	//Recta después del giro, de z = 30 a z = -80
			posAvion = glm::vec3(-50.0f, 25.0f, 30.0f - avanceAvion);
			giroAvion = 180.0f;
			break;
		case 5:	//Curva 2: media vuelta de radio 50 alrededor de (0, -80)
			posAvion = glm::vec3(-50.0f * cos(avanceAvion / 50.0f), 25.0f, -80.0f - 50.0f * sin(avanceAvion / 50.0f));
			giroAvion = 180.0f - glm::degrees(avanceAvion / 50.0f);
			alabeoAvion = 35.0f * sin(avanceAvion / 50.0f);
			break;
		}

		//Matriz base del avión: la comparten el modelo y sus luces
		glm::mat4 baseAvion(1.0);
		baseAvion = glm::translate(baseAvion, posAvion);	// punto sobre la línea del circuito
		baseAvion = glm::rotate(baseAvion, glm::radians(giroAvion), glm::vec3(0.0f, 1.0f, 0.0f));	// voltea hacia donde avanza
		baseAvion = glm::translate(baseAvion, desfaseAvion);	// se sale de la línea en el tonel y en el giro
		baseAvion = glm::rotate(baseAvion, glm::radians(-cabeceoAvion), glm::vec3(1.0f, 0.0f, 0.0f));	// nariz arriba en el giro
		baseAvion = glm::rotate(baseAvion, glm::radians(alabeoAvion), glm::vec3(0.0f, 0.0f, 1.0f));	// gira sobre su eje largo

		//Luces del avión: salen de baseAvion, así se mueven con él
		glm::vec3 frenteAvion = glm::vec3(0.0f, 0.0f, 1.0f);	// nariz del avión

		//Luz hacia abajo: se vuelve a crear con el color elegido y luego se coloca
		glm::vec3 posLuzAbajoAvion = glm::vec3(baseAvion * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));
		glm::vec3 dirLuzAbajoAvion = glm::normalize(glm::vec3(baseAvion * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)));
		glm::vec3 colorFaroAvion = coloresFaroAvion[mainWindow.getColorFaroAvion() % 6];
		spotLights[1] = SpotLight(colorFaroAvion.r, colorFaroAvion.g, colorFaroAvion.b,	// antes spotLights[2]
			0.2f, 1.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			1.0f, 0.014f, 0.0007f,
			20.0f);
		spotLights[1].SetFlash(posLuzAbajoAvion, dirLuzAbajoAvion);	// antes spotLights[2]

		//Luz hacia adelante: sale de la nariz, inclinada 45° hacia el piso
		glm::vec3 posLuzFrenteAvion = glm::vec3(baseAvion * glm::vec4(frenteAvion * 3.0f, 1.0f));
		glm::vec3 dirLuzFrenteAvion = glm::normalize(glm::vec3(baseAvion * glm::vec4(frenteAvion + glm::vec3(0.0f, -1.0f, 0.0f), 0.0f)));
		spotLights[2].SetFlash(posLuzFrenteAvion, dirLuzFrenteAvion);	// antes spotLights[3]
		*/

		//Envío de luces al shader
		shaderList[0].SetDirectionalLight(&mainLight);
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

		//------------------------- Avión -------------------------
		//Se comenta el avión
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

		//------------------------- Holocrón -------------------------
		//Animación compleja que imita el gif: todo sale de tiempoHolo (ver sección de tiempo).
		//El eje Z de cada esquina, después de sus rotaciones de 45° en Y y -35° en X, apunta por la diagonal
		//del cubo hacia afuera. Por eso un translate en Z la separa y un rotate en Z la tuerce, igual para las 8
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//Modelo HOLOCRON BASE
		color = glm::vec3(1.0f, 1.0f, 1.0f); //modelo de blanco.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 12.0f, 0.0f));	
		model = glm::rotate(model, glm::radians(36.0f * tiempoHolo), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		modelaux2 = model;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloBase_M.RenderModel();

		//ESQUINA SUPERIOR FRONTAL DERECHA
		model = modelaux2;
		model = glm::translate(model, glm::vec3(4.0f, 4.0, 4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario F
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, separaHolo));	// sale por su diagonal
		model = glm::rotate(model, glm::radians(giroHolo), glm::vec3(0.0f, 0.0f, 1.0f));	// gira sobre su diagonal
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//ESQUINA SUPERIOR FRONTAL IZQUIERDA
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-4.0f, 4.0, 4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario G
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, separaHolo));	// sale por su diagonal
		model = glm::rotate(model, glm::radians(giroHolo), glm::vec3(0.0f, 0.0f, 1.0f));	// gira sobre su diagonal
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//ESQUINA SUPERIOR TRASERA IZQUIERDA (la superior derecha de enfrente girada 180° en Y)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-4.0f, 4.0f, -4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 45 + 180
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario H
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, separaHolo));	// sale por su diagonal
		model = glm::rotate(model, glm::radians(giroHolo), glm::vec3(0.0f, 0.0f, 1.0f));	// gira sobre su diagonal
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//ESQUINA SUPERIOR TRASERA DERECHA (la superior izquierda de enfrente girada 180° en Y)
		model = modelaux2;
		model = glm::translate(model, glm::vec3(4.0f, 4.0f, -4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // -45 + 180
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario J
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, separaHolo));	// sale por su diagonal
		model = glm::rotate(model, glm::radians(giroHolo), glm::vec3(0.0f, 0.0f, 1.0f));	// gira sobre su diagonal
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.2f, 0.0f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//ESQUINA INFERIOR FRONTAL IZQUIERDA (la superior frontal derecha girada 180° en Z)
		model = modelaux2;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(4.0f, 4.0f, 4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion11()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario Y
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, separaHolo));	// sale por su diagonal
		model = glm::rotate(model, glm::radians(-giroHolo), glm::vec3(0.0f, 0.0f, 1.0f));	// las de abajo giran al revés que las de arriba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.0f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//ESQUINA INFERIOR FRONTAL DERECHA (la superior frontal izquierda girada 180° en Z)
		model = modelaux2;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-4.0f, 4.0f, 4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion12()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario U
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, separaHolo));	// sale por su diagonal
		model = glm::rotate(model, glm::radians(-giroHolo), glm::vec3(0.0f, 0.0f, 1.0f));	// las de abajo giran al revés que las de arriba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 0.0f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//ESQUINA INFERIOR TRASERA DERECHA (la superior trasera izquierda girada 180° en Z)
		model = modelaux2;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-4.0f, 4.0f, -4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion13()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario I
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, separaHolo));	// sale por su diagonal
		model = glm::rotate(model, glm::radians(-giroHolo), glm::vec3(0.0f, 0.0f, 1.0f));	// las de abajo giran al revés que las de arriba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 1.0f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//ESQUINA INFERIOR TRASERA IZQUIERDA (la superior trasera derecha girada 180° en Z)
		model = modelaux2;
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(4.0f, 4.0f, -4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion14()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario O
		//model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, separaHolo));	// sale por su diagonal
		model = glm::rotate(model, glm::radians(-giroHolo), glm::vec3(0.0f, 0.0f, 1.0f));	// las de abajo giran al revés que las de arriba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//------------------------- Cohete -------------------------
		//Animación compleja: la posición y la inclinación salen de la máquina de estados de la sección de tiempo.
		//La trayectoria mueve el centro del cohete y el modelo gira alrededor de ese centro para que la nariz siga la tangente
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-40.0f, -2.0f, -150.0f));	// plataforma sobre el piso
		model = glm::translate(model, posCohete);	// centro del cohete sobre la trayectoria
		model = glm::rotate(model, -inclinacionCohete, glm::vec3(0.0f, 0.0f, 1.0f));	// se inclina hacia +X
		model = glm::translate(model, glm::vec3(0.0f, -20.0f, 0.0f));	// del centro a la base, que es el origen del modelo
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();	// respaldo para las piezas sin textura propia
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cohete_M.RenderModel();

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

		//------------------------- Humo del avión -------------------------
		//Se comenta todo el humo del avión
		/*
		//Va al final porque es transparente: se dibuja cuando todo lo sólido ya está en pantalla
		//1) Soltar bocanadas: cada 0.02 s nace una en cada motor, en la posición actual del motor
		acumHumo += dtSegundos;
		while (acumHumo >= 0.02f)
		{
			acumHumo -= 0.02f;

			//Motor derecho: punto en ejes del avión (X = lado, Y = arriba, Z = nariz) pasado al mundo con baseAvion
			humo[siguienteHumo].pos = glm::vec3(baseAvion * glm::vec4(3.0f, 0.0f, -1.0f, 1.0f));
			humo[siguienteHumo].edad = 0.0f;
			humo[siguienteHumo].viva = true;
			siguienteHumo = (siguienteHumo + 1) % MAX_HUMO;	// arreglo circular: reusa la casilla más vieja

			//Motor izquierdo
			humo[siguienteHumo].pos = glm::vec3(baseAvion * glm::vec4(-3.0f, 0.0f, -1.0f, 1.0f));
			humo[siguienteHumo].edad = 0.0f;
			humo[siguienteHumo].viva = true;
			siguienteHumo = (siguienteHumo + 1) % MAX_HUMO;
		}

		//2) Hacia dónde ve la cámara: con eso cada bocanada se voltea de frente a ella
		glm::vec3 dirCamara = camera.getCameraDirection();
		float giroHumo = atan2(-dirCamara.x, -dirCamara.z);	// gira en Y para quedar de frente
		float inclinacionHumo = asin(dirCamara.y);			// se inclina en X si la cámara ve hacia arriba o abajo

		//3) Envejecer y dibujar. Mezcla aditiva (GL_ONE): el humo suma luz, así que al bajarle
		//el color hasta negro deja de sumar y se desvanece; tampoco hay que ordenar las bocanadas
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);	// lo transparente no tapa a las bocanadas de atrás
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		humoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		for (int i = 0; i < MAX_HUMO; i++)
		{
			if (!humo[i].viva)
				continue;

			humo[i].edad += dtSegundos;
			if (humo[i].edad >= 2.0f)	// a los 2 segundos desaparece
			{
				humo[i].viva = false;
				continue;
			}
			humo[i].pos.y += 1.5f * dtSegundos;	// sube despacio

			float vidaHumo = humo[i].edad / 2.0f;	// 0 al nacer, 1 al desaparecer

			model = glm::mat4(1.0);
			model = glm::translate(model, humo[i].pos);
			model = glm::rotate(model, giroHumo, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, inclinacionHumo, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f + 4.0f * vidaHumo));	// crece de 1 a 5 mientras se dispersa
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(0.6f * (1.0f - vidaHumo));	// se apaga hasta negro = invisible con mezcla aditiva
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			humoMesh->RenderMeshModel();
		}

		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// regresa a la mezcla normal
		glDisable(GL_BLEND);
		*/

		//------------------------- Fin del frame -------------------------
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}