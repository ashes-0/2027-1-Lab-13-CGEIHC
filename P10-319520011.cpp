/*
Animación por keyframes
*/
//para que Visual Studio deje usar fopen y fscanf al leer y escribir keyframescohete.txt
#define _CRT_SECURE_NO_WARNINGS
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

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;


Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;



Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
//Model Cohete_M; //cohete que se anima con keyframes
//el cohete ahora se carga en 4 piezas para poder separarlo en fases
Model BoosterIzq_M;
Model BoosterDer_M;
Model EtapaCentral_M;
Model ParteSuperior_M;

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
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
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
	meshList.push_back(obj1);

	MeshModel* obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXavion = 2.0, posYavion = -2.0, posZavion = -3.0; //posYavion antes 5.0, ahora el cohete parte del piso (y = -2)
//piezas del cohete: 0 booster izquierdo, 1 booster derecho, 2 etapa central (naranja), 3 parte superior (blanca, es la que queda orbitando)
#define NUM_PIEZAS 4
float	movAvion_x[NUM_PIEZAS] = { 0.0f }, movAvion_y[NUM_PIEZAS] = { 0.0f }; //antes un solo valor, ahora uno por cada pieza
float movAvion_z[NUM_PIEZAS] = { 0.0f }; //movimiento en Z de cada pieza
float giroAvion[NUM_PIEZAS] = { 0.0f }; //antes un solo giro, ahora uno por cada pieza (giro en Z)
float giroAvionY[NUM_PIEZAS] = { 0.0f }; //giro en Y de cada pieza alrededor del punto de despegue, con este se hace la orbita
//nombre de cada pieza para los mensajes en consola
const char* nombrePieza[NUM_PIEZAS] = { "booster izquierdo", "booster derecho", "etapa central", "parte superior" };
//pieza que se mueve con el teclado, -1 mueve la parte superior y todo lo que sigue unido a ella
int piezaActual = -1;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 6;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x[NUM_PIEZAS];		//Variable para PosicionX
	float movAvion_y[NUM_PIEZAS];		//Variable para PosicionY
	float movAvion_xInc[NUM_PIEZAS];		//Variable para IncrementoX
	float movAvion_yInc[NUM_PIEZAS];		//Variable para IncrementoY
	float giroAvion[NUM_PIEZAS];
	float giroAvionInc[NUM_PIEZAS];
	//posicion en Z y giro en Y de cada pieza con sus incrementos
	float movAvion_z[NUM_PIEZAS];
	float movAvion_zInc[NUM_PIEZAS];
	float giroAvionY[NUM_PIEZAS];
	float giroAvionYInc[NUM_PIEZAS];
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos //antes 6, ahora todos los keyframes se crean por teclado
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{

	//no deja guardar mas keyframes de los que caben en el arreglo
	if (FrameIndex >= MAX_FRAMES)
	{
		printf("Ya no caben mas keyframes (maximo %d)\n", MAX_FRAMES);
		return;
	}

	printf("frameindex %d\n", FrameIndex);


	//se guarda la posicion y el giro de cada pieza
	for (int i = 0; i < NUM_PIEZAS; i++)
	{
		KeyFrame[FrameIndex].movAvion_x[i] = movAvion_x[i];
		KeyFrame[FrameIndex].movAvion_y[i] = movAvion_y[i];
		KeyFrame[FrameIndex].movAvion_z[i] = movAvion_z[i];
		KeyFrame[FrameIndex].giroAvion[i] = giroAvion[i];//completar //ya se completo: ahora tambien guarda el giro
		KeyFrame[FrameIndex].giroAvionY[i] = giroAvionY[i];
		//no volatil, agregar una forma de escribir a un archivo para guardar los frames
		//imprime el keyframe guardado para la captura de evidencia
		printf("KeyFrame[%d] guardado, %s: x = %f, y = %f, z = %f, giroZ = %f, giroY = %f\n", FrameIndex, nombrePieza[i],
			movAvion_x[i], movAvion_y[i], movAvion_z[i], giroAvion[i], giroAvionY[i]);
	}
	FrameIndex++;
}

//escribe (o sobrescribe) todos los keyframes guardados en keyframescohete.txt (tecla 7)
void escribirArchivoKeyframes(void)
{
	//con "w" el archivo se crea si no existe y se borra lo que tenia si ya existe
	FILE* archivo = fopen("keyframescohete.txt", "w");
	if (archivo == NULL)
	{
		printf("No se pudo abrir keyframescohete.txt para escribir\n");
		return;
	}
	//primera linea: cuantos keyframes hay
	fprintf(archivo, "%d\n", FrameIndex);
	//por cada keyframe van 4 lineas, una por pieza: x y z giroZ giroY
	for (int i = 0; i < FrameIndex; i++)
	{
		for (int j = 0; j < NUM_PIEZAS; j++)
		{
			fprintf(archivo, "%f %f %f %f %f\n", KeyFrame[i].movAvion_x[j], KeyFrame[i].movAvion_y[j], KeyFrame[i].movAvion_z[j],
				KeyFrame[i].giroAvion[j], KeyFrame[i].giroAvionY[j]);
		}
	}
	fclose(archivo);
	printf("Se escribieron %d keyframes en keyframescohete.txt\n", FrameIndex);
}

//lee los keyframes de keyframescohete.txt y los pone en el arreglo KeyFrame (tecla 8)
void leerArchivoKeyframes(void)
{
	FILE* archivo = fopen("keyframescohete.txt", "r");
	if (archivo == NULL)
	{
		printf("No se encontro keyframescohete.txt\n");
		return;
	}
	int total = 0;
	if (fscanf(archivo, "%d", &total) != 1 || total < 0 || total > MAX_FRAMES)
	{
		printf("keyframescohete.txt no tiene el formato correcto\n");
		fclose(archivo);
		return;
	}
	int leidos = 0;
	bool completo = true;
	while (leidos < total && completo)
	{
		//cada keyframe trae una linea por pieza
		for (int j = 0; j < NUM_PIEZAS && completo; j++)
		{
			if (fscanf(archivo, "%f %f %f %f %f", &KeyFrame[leidos].movAvion_x[j], &KeyFrame[leidos].movAvion_y[j],
				&KeyFrame[leidos].movAvion_z[j], &KeyFrame[leidos].giroAvion[j], &KeyFrame[leidos].giroAvionY[j]) != 5)
			{
				completo = false;
			}
		}
		if (completo)
		{
			for (int j = 0; j < NUM_PIEZAS; j++)
			{
				printf("KeyFrame[%d] leido, %s: x = %f, y = %f, z = %f, giroZ = %f, giroY = %f\n", leidos, nombrePieza[j],
					KeyFrame[leidos].movAvion_x[j], KeyFrame[leidos].movAvion_y[j], KeyFrame[leidos].movAvion_z[j],
					KeyFrame[leidos].giroAvion[j], KeyFrame[leidos].giroAvionY[j]);
			}
			leidos++;
		}
	}
	fclose(archivo);
	FrameIndex = leidos;
	printf("Se leyeron %d keyframes de keyframescohete.txt, presiona barra espaciadora para reproducir\n", FrameIndex);
}

void resetElements(void) //Tecla 0
{

	for (int i = 0; i < NUM_PIEZAS; i++) //cada pieza regresa a su primer keyframe
	{
		movAvion_x[i] = KeyFrame[0].movAvion_x[i];
		movAvion_y[i] = KeyFrame[0].movAvion_y[i];
		movAvion_z[i] = KeyFrame[0].movAvion_z[i];
		giroAvion[i] = KeyFrame[0].giroAvion[i];
		giroAvionY[i] = KeyFrame[0].giroAvionY[i];
	}
}

void interpolation(void)
{
	for (int i = 0; i < NUM_PIEZAS; i++) //incrementos de cada pieza
	{
		KeyFrame[playIndex].movAvion_xInc[i] = (KeyFrame[playIndex + 1].movAvion_x[i] - KeyFrame[playIndex].movAvion_x[i]) / i_max_steps;
		KeyFrame[playIndex].movAvion_yInc[i] = (KeyFrame[playIndex + 1].movAvion_y[i] - KeyFrame[playIndex].movAvion_y[i]) / i_max_steps;
		KeyFrame[playIndex].movAvion_zInc[i] = (KeyFrame[playIndex + 1].movAvion_z[i] - KeyFrame[playIndex].movAvion_z[i]) / i_max_steps;
		KeyFrame[playIndex].giroAvionInc[i] = (KeyFrame[playIndex + 1].giroAvion[i] - KeyFrame[playIndex].giroAvion[i]) / i_max_steps;
		KeyFrame[playIndex].giroAvionYInc[i] = (KeyFrame[playIndex + 1].giroAvionY[i] - KeyFrame[playIndex].giroAvionY[i]) / i_max_steps;
	}

}


//para que la animacion no dependa de los FPS
float tiempoAnimacion = 0.0f;		//tiempo real acumulado que falta por avanzar
double ultimoTiempoAnimacion = 0.0;	//momento del cuadro anterior
const float pasoAnimacion = 1.0f / 60.0f;	//se avanza un paso cada 1/60 s, asi 90 pasos duran 1.5 s

void animate(void)
{
	//tiempo real que paso desde el cuadro anterior
	double ahora = glfwGetTime();
	tiempoAnimacion += (float)(ahora - ultimoTiempoAnimacion);
	ultimoTiempoAnimacion = ahora;
	if (!play)
		tiempoAnimacion = 0.0f;

	//Movimiento del objeto con barra espaciadora
	//se dan los pasos que tocan segun el tiempo real, no uno por cada cuadro dibujado
	while (play && tiempoAnimacion >= pasoAnimacion)
	{
		tiempoAnimacion -= pasoAnimacion;
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolación del próximo cuadro
			{

				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//Dibujar Animación
			for (int i = 0; i < NUM_PIEZAS; i++) //avanza cada pieza
			{
				movAvion_x[i] += KeyFrame[playIndex].movAvion_xInc[i];
				movAvion_y[i] += KeyFrame[playIndex].movAvion_yInc[i];
				movAvion_z[i] += KeyFrame[playIndex].movAvion_zInc[i];
				giroAvion[i] += KeyFrame[playIndex].giroAvionInc[i];
				giroAvionY[i] += KeyFrame[playIndex].giroAvionYInc[i];
			}
			i_curr_steps++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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



	//Blackhawk_M = Model();
	//Blackhawk_M.LoadModel("Models/uh60.obj");
	//el cohete reemplaza al helicoptero (eje Y hacia arriba, base en y = 0, 40 unidades de alto)
	//Cohete_M = Model();
	//Cohete_M.LoadModel("Models/Artemis2.fbx");
	//las 4 piezas del cohete, cada modelo esta centrado en su propio centro para que gire sobre el
	BoosterIzq_M = Model();
	BoosterIzq_M.LoadModel("Models/Artemis_booster_izq.obj");
	BoosterDer_M = Model();
	BoosterDer_M.LoadModel("Models/Artemis_booster_der.obj");
	EtapaCentral_M = Model();
	EtapaCentral_M.LoadModel("Models/Artemis_etapa_central.obj");
	ParteSuperior_M = Model();
	ParteSuperior_M.LoadModel("Models/Artemis_parte_superior.obj");



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
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);



	//printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
	//printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X");
	  //teclas nuevas para crear los keyframes del cohete
	printf("\nTeclas para uso de Keyframes del cohete:\n");
	printf("TAB: cambiar la pieza que se mueve (cohete unido, boosters, etapa central, parte superior)\n");
	printf("1 / 2: mover en X (+ / -)\n");
	printf("3 / 4: mover en Y (+ / -)\n");
	printf("F / G: mover en Z (+ / -)\n");
	printf("5 / 6: girar (5 inclina hacia X positivo, 6 regresa)\n");
	printf("H / J: girar en Y alrededor del punto de despegue (+ / -), sirve para orbitar\n");
	printf("M: cambiar el paso de movimiento y giro entre 10 y 1\n");
	printf("L: guardar keyframe con la posicion actual\n");
	printf("7: escribir/sobrescribir keyframescohete.txt\n");
	printf("8: leer keyframescohete.txt\n");
	printf("9: borrar los keyframes en memoria para grabar de nuevo\n");
	printf("Barra espaciadora: reproducir animacion\n");
	printf("0: volver a habilitar la reproduccion\n\n");


	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
		}
		rotllanta += rotllantaOffset * deltaTime;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

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



		//Cohete animado con keyframes
		//movAvion_x, movAvion_y y giroAvion ahora mueven al cohete
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(posXavion + movAvion_x, posYavion + movAvion_y, posZavion));
		////gira alrededor de su centro: sube a la mitad de su altura (2 / 2 = 1), gira en Z y regresa
		//model = glm::translate(model, glm::vec3(0.0f, 12.0f, 0.0f)); 
		//model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::translate(model, glm::vec3(0.0f, -12.0f, 0.0f)); 
		//model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f)); //ahora mide 24 de alto
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Cohete_M.RenderModel();

		//cada pieza se dibuja con su propio movimiento y giro
		//primero se va al punto de despegue y ahi gira en Y: como ese giro va antes del movimiento,
		//una pieza que esta lejos del punto de despegue da vueltas en circulo alrededor de el (orbita)
		//despues el translate lleva a la pieza a su lugar en el cohete armado (ya con la escala de 0.6) mas lo que se ha movido
		//y al final gira en Z sobre su propio centro (inclinacion)
		//booster izquierdo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXavion, posYavion, posZavion));
		model = glm::rotate(model, giroAvionY[0] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(movAvion_x[0] - 1.55f, movAvion_y[0] + 6.63f, movAvion_z[0]));
		model = glm::rotate(model, giroAvion[0] * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BoosterIzq_M.RenderModel();

		//booster derecho
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXavion, posYavion, posZavion));
		model = glm::rotate(model, giroAvionY[1] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(movAvion_x[1] + 1.55f, movAvion_y[1] + 6.63f, movAvion_z[1]));
		model = glm::rotate(model, giroAvion[1] * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BoosterDer_M.RenderModel();

		//etapa central: tanque naranja, motores y tuberias
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXavion, posYavion, posZavion));
		model = glm::rotate(model, giroAvionY[2] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(movAvion_x[2], movAvion_y[2] + 8.97f, movAvion_z[2]));
		model = glm::rotate(model, giroAvion[2] * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		EtapaCentral_M.RenderModel();

		//parte superior blanca, la que queda orbitando
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posXavion, posYavion, posZavion));
		model = glm::rotate(model, giroAvionY[3] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(movAvion_x[3], movAvion_y[3] + 20.92f, movAvion_z[3] - 0.01f));
		model = glm::rotate(model, giroAvion[3] * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ParteSuperior_M.RenderModel();




		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMeshModel();

		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetflechau += 0.001;
		toffsetflechav += 0.0;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
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

		//plano con todos los números
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

		//número 1
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

		for (int i = 1; i < 4; i++)
		{
			//números 2-4
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

		for (int j = 1; j < 5; j++)
		{
			//números 5-8
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


		//número cambiante 
		/*
		¿Cómo hacer para que sea a una velocidad visible?
		*/

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

		//cambiar automáticamente entre textura número 1 y número 2
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





		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}


//cuanto se mueve o gira el cohete con cada pulsacion
float pasoMovimiento = 10.0f; //antes 0.5f
float pasoGiro = 10.0f;

//regresa true solo en el cuadro en que la tecla se acaba de presionar,
//asi una pulsacion mueve una sola vez aunque se dibujen muchos cuadros
bool teclaAntes[1024] = { false };
bool pulsoTecla(bool* keys, int tecla)
{
	bool pulso = keys[tecla] && !teclaAntes[tecla];
	teclaAntes[tecla] = keys[tecla];
	return pulso;
}

//regresa true si la pieza i se mueve con la seleccion actual
bool piezaSeleccionada(int i)
{
	if (piezaActual >= 0)
		return i == piezaActual;
	//con el cohete unido se mueven la parte superior (pieza 3) y las piezas que siguen en su misma posicion,
	//las que ya se separaron se quedan donde estan
	return fabs(movAvion_x[i] - movAvion_x[3]) < 0.01f && fabs(movAvion_y[i] - movAvion_y[3]) < 0.01f &&
		fabs(movAvion_z[i] - movAvion_z[3]) < 0.01f && fabs(giroAvion[i] - giroAvion[3]) < 0.01f &&
		fabs(giroAvionY[i] - giroAvionY[3]) < 0.01f;
}

//mueve o gira las piezas seleccionadas e imprime como quedaron
void moverSeleccion(float dx, float dy, float dz, float dgiroZ, float dgiroY)
{
	//con el cohete unido no se gira en Z: cada pieza gira sobre su propio centro y el cohete se desarmaria
	//el giro en Y si se puede porque todas giran alrededor del mismo punto
	if (piezaActual == -1 && dgiroZ != 0.0f)
	{
		printf("El giro en Z es por pieza, elige una con TAB\n");
		return;
	}
	//primero se revisa que piezas se mueven, antes de cambiar la posicion de la parte superior
	bool seMueve[NUM_PIEZAS];
	for (int i = 0; i < NUM_PIEZAS; i++)
	{
		seMueve[i] = piezaSeleccionada(i);
	}
	for (int i = 0; i < NUM_PIEZAS; i++)
	{
		if (seMueve[i])
		{
			movAvion_x[i] += dx;
			movAvion_y[i] += dy;
			movAvion_z[i] += dz;
			giroAvion[i] += dgiroZ;
			giroAvionY[i] += dgiroY;
			printf("%s: x = %f, y = %f, z = %f, giroZ = %f, giroY = %f\n", nombrePieza[i],
				movAvion_x[i], movAvion_y[i], movAvion_z[i], giroAvion[i], giroAvionY[i]);
		}
	}
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;
			habilitaranimacion++; //para que el mensaje salga una sola vez mientras se mantiene la tecla

		}
	}



	//teclas nuevas: cada tecla actua una sola vez por pulsacion
	//no se mueve ni se guarda nada mientras se reproduce la animacion
	bool libre = !play;

	//cambiar la pieza que se mueve con el teclado
	if (pulsoTecla(keys, GLFW_KEY_TAB) && libre)
	{
		piezaActual++;
		if (piezaActual >= NUM_PIEZAS)
			piezaActual = -1;
		if (piezaActual == -1)
			printf("Pieza seleccionada: cohete unido (la parte superior y lo que sigue pegado a ella)\n");
		else
			printf("Pieza seleccionada: %s\n", nombrePieza[piezaActual]);
	}

	//cambiar el paso entre 10 y 1 para acomodar con mas detalle
	if (pulsoTecla(keys, GLFW_KEY_M) && libre)
	{
		if (pasoMovimiento == 10.0f)
		{
			pasoMovimiento = 1.0f;
			pasoGiro = 1.0f;
		}
		else
		{
			pasoMovimiento = 10.0f;
			pasoGiro = 10.0f;
		}
		printf("Paso de movimiento y giro: %f\n", pasoMovimiento);
	}

	//guardar keyframe con la posicion actual
	if (pulsoTecla(keys, GLFW_KEY_L) && libre)
	{
		saveFrame();
	}

	//mover en X
	if (pulsoTecla(keys, GLFW_KEY_1) && libre)
	{
		moverSeleccion(pasoMovimiento, 0.0f, 0.0f, 0.0f, 0.0f); //antes movAvion_x += pasoMovimiento, ahora mueve las piezas seleccionadas
		//printf("x = %f, y = %f, giro = %f\n", movAvion_x, movAvion_y, giroAvion);
	}
	if (pulsoTecla(keys, GLFW_KEY_2) && libre)
	{
		moverSeleccion(-pasoMovimiento, 0.0f, 0.0f, 0.0f, 0.0f); //antes movAvion_x -= pasoMovimiento
		//printf("x = %f, y = %f, giro = %f\n", movAvion_x, movAvion_y, giroAvion);
	}

	//mover en Y
	if (pulsoTecla(keys, GLFW_KEY_3) && libre)
	{
		moverSeleccion(0.0f, pasoMovimiento, 0.0f, 0.0f, 0.0f); //antes movAvion_y += pasoMovimiento
		//printf("x = %f, y = %f, giro = %f\n", movAvion_x, movAvion_y, giroAvion);
	}
	if (pulsoTecla(keys, GLFW_KEY_4) && libre)
	{
		moverSeleccion(0.0f, -pasoMovimiento, 0.0f, 0.0f, 0.0f); //antes movAvion_y -= pasoMovimiento
		//printf("x = %f, y = %f, giro = %f\n", movAvion_x, movAvion_y, giroAvion);
	}

	//mover en Z
	if (pulsoTecla(keys, GLFW_KEY_F) && libre)
	{
		moverSeleccion(0.0f, 0.0f, pasoMovimiento, 0.0f, 0.0f);
	}
	if (pulsoTecla(keys, GLFW_KEY_G) && libre)
	{
		moverSeleccion(0.0f, 0.0f, -pasoMovimiento, 0.0f, 0.0f);
	}

	//girar en Z: el angulo negativo inclina la punta hacia X positivo
	if (pulsoTecla(keys, GLFW_KEY_5) && libre)
	{
		moverSeleccion(0.0f, 0.0f, 0.0f, -pasoGiro, 0.0f); //antes giroAvion -= pasoGiro
		//printf("x = %f, y = %f, giro = %f\n", movAvion_x, movAvion_y, giroAvion);
	}
	if (pulsoTecla(keys, GLFW_KEY_6) && libre)
	{
		moverSeleccion(0.0f, 0.0f, 0.0f, pasoGiro, 0.0f); //antes giroAvion += pasoGiro
		//printf("x = %f, y = %f, giro = %f\n", movAvion_x, movAvion_y, giroAvion);
	}

	//girar en Y
	if (pulsoTecla(keys, GLFW_KEY_H) && libre)
	{
		moverSeleccion(0.0f, 0.0f, 0.0f, 0.0f, pasoGiro);
	}
	if (pulsoTecla(keys, GLFW_KEY_J) && libre)
	{
		moverSeleccion(0.0f, 0.0f, 0.0f, 0.0f, -pasoGiro);
	}

	//escribir/sobrescribir el archivo
	if (pulsoTecla(keys, GLFW_KEY_7) && libre)
	{
		escribirArchivoKeyframes();
	}

	//leer el archivo
	if (pulsoTecla(keys, GLFW_KEY_8) && libre)
	{
		leerArchivoKeyframes();
	}

	//borrar los keyframes en memoria y regresar el cohete al inicio para grabar de nuevo
	if (pulsoTecla(keys, GLFW_KEY_9) && libre)
	{
		FrameIndex = 0;
		for (int i = 0; i < NUM_PIEZAS; i++) //todas las piezas regresan al cohete armado
		{
			movAvion_x[i] = 0.0f;
			movAvion_y[i] = 0.0f;
			movAvion_z[i] = 0.0f;
			giroAvion[i] = 0.0f;
			giroAvionY[i] = 0.0f;
		}
		piezaActual = -1;
		printf("Se borraron los keyframes en memoria, el archivo no se toca\n");
	}

}