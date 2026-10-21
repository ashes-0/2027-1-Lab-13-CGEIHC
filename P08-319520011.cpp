/*
Práctica 8: Iluminación 2
*/

/*
RESUMEN DE LO QUE SE ELIMINÓ (limpieza del código de la Práctica 7)

Includes
- #include "Sphere.h" (no se usaba) y el include comentado de assimp/Importer.hpp.

Texturas
- brickTexture, dirtTexture, plainTexture, AgaveTexture y holocronTexture
  (declaración, carga y uso). Solo se conserva pisoTexture.

Modelos
- Blackhawk_M, Cuerpo_M, BB_M, BM_M, BMA_M, BP_M, las 6 patas del rover
  (PataDelanteraDer_M, PataDelanteraIzq_M, PataMediaDer_M, PataMediaIzq_M,
  PataTraseraDer_M, PataTraseraIzq_M) y Avion_M (declaración y carga).
- Se agregó Nave_M, que carga Models/Wing-Y.fbx.

Geometría
- Las dos pirámides (obj1 y obj2), la vegetación (obj4), CrearDado(),
  CrearHolocron() y calcAverageNormals().
- CreateObjects() ahora solo crea el piso, por lo que el piso pasó de ser
  meshListModel[2] a meshListModel[0]; el fuego de los motores quedó como meshListModel[1].

Luces
- Puntuales: la roja, la del holocrón y la de la pinza.
- Spotlights: la linterna de la cámara, el faro del rover y las dos luces del avión.
- Los arreglos pointLights y spotLights siguen declarados y sus contadores
  (pointLightCount, spotLightCount) quedan en 0 para las luces de la nave.

Variables
- modelaux, modelaux2, posHolocron, puntaPinza, coloresFaroAvion,
  pointLightsActivas y pointLightCountActivas.

Ciclo principal (while)
- Todo el rover: baseRover, faro, cuerpo, patas, brazo y luz de la pinza.
- Todo el avión: baseAvion y sus dos luces.
- El holocrón y los bloques que ya estaban comentados (Blackhawk, Rover, dado y agave).
- La linterna ligada a la cámara (spotLights[0].SetFlash).
- Ya no se leen desde main las teclas de Window: getmovX, getmovY, getmovZ,
  getarticulacion1 a getarticulacion10, getColorFaroAvion, getLuzHolocron y
  getLuzPinza (de Window solo se agregó luzNave/getLuzNave() con la tecla 1 y motor1/motor2 con getMotor1()/getMotor2() y las teclas 2 y 3; lo demás no se modificó).
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
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
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

//---Texturas----------
Texture pisoTexture;
//Textura de color base (albedo) de la nave. Se carga a mano como respaldo por si Model no la encuentra sola
Texture naveTexture;
//Textura de plasma azul que simula el fuego de los motores
Texture fuegoTexture;


//------Modelos--------
//Modelo de la nave (Models/Wing-Y.fbx)
Model Nave_M;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


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


void CreateObjects()
{
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

	//Piso: es la única malla que queda, por eso ahora es meshListModel[0]
	MeshModel* piso = new MeshModel();
	piso->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(piso);

	//Fuego de los motores: dos planos cruzados (uno en XY y otro en XZ) para que se vea desde cualquier ángulo.
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

	MeshModel* fuego = new MeshModel();
	fuego->CreateMeshModel(fuegoVertices, fuegoIndices, 64, 12);
	meshListModel.push_back(fuego);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();
	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	//Textura albedo de la nave (Textures/DefaultMaterial_albedo.jpg).
	//Es .jpg (sin canal alfa), por eso se usa LoadTexture() y no LoadTextureA()
	naveTexture = Texture("Textures/DefaultMaterial_albedo.jpg");
	naveTexture.LoadTexture();

	//Textura del fuego (Textures/fuego_azul.png).
	fuegoTexture = Texture("Textures/fuego_azul.png");
	fuegoTexture.LoadTextureA();

	//Cargar modelos
	//Nave: Models/Wing-Y.fbx. Las demás texturas (AO, emissive, metallic, normal, roughness) no se usan en esta práctica
	Nave_M = Model();
	Nave_M.LoadModel("Models/Wing-Y.fbx");


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
	//contador de luces puntuales (en 0 hasta que se agreguen las de la nave)
	unsigned int pointLightCount = 0;

	//contador de spotlights (en 0 hasta que se agreguen las de la nave)
	unsigned int spotLightCount = 0;

	//Luz puntual de la NAVE: naranja, va debajo del fuselaje. La posición de relleno se sobreescribe en cada frame dentro del while
	pointLights[0] = PointLight(1.0f, 0.5f, 0.0f,	//color RGB: naranja
		0.0f, 1.0f,									//intensidad ambiental, intensidad difusa
		0.0f, 0.0f, 0.0f,							//posición inicial (se sobreescribe en el while)
		1.0f, 0.045f, 0.0075f);						//atenuación: constante, lineal, exponencial
	pointLightCount++;

	//Spotlight de la NAVE: faro en la nariz, apunta al frente y hacia abajo. Posición y dirección se sobreescriben en cada frame dentro del while
	spotLights[0] = SpotLight(1.0f, 1.0f, 0.8f,		//color RGB: luz tipo faro
		0.2f, 1.0f,									//intensidad ambiental, intensidad difusa
		0.0f, 0.0f, 0.0f,							//posición inicial (se sobreescribe en el while)
		0.0f, -1.0f, 0.0f,							//dirección inicial (se sobreescribe en el while)
		1.0f, 0.014f, 0.0007f,						//atenuación: constante, lineal, exponencial
		20.0f);										//edge: ángulo en grados del centro al borde del cono
	spotLightCount++;

	//Luces puntuales azules de los MOTORES: pointLights[1] es la del motor 1 (y positiva) y pointLights[2] la del motor 2 (y negativa).
	//Las posiciones de relleno se sobreescriben en cada frame dentro del while
	pointLights[1] = PointLight(0.2f, 0.5f, 1.0f,	//color RGB: azul
		0.6f, 1.0f,									//intensidad ambiental
		0.0f, 0.0f, 0.0f,							//posición inicial 
		1.0f, 0.09f, 0.032f);						//atenuación
	pointLightCount++;
	pointLights[2] = PointLight(0.2f, 0.5f, 1.0f,
		0.6f, 1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
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

		//Luces de la NAVE
		//baseNave: posición y rotación de la nave (SIN escala). Se usa la MISMA matriz para dibujarla y para sus luces, así siempre van juntas.
		//Los valores del translate y los rotate son los de la nave; se cambian aquí.
		glm::mat4 baseNave(1.0);
		baseNave = glm::translate(baseNave, glm::vec3(0.0f, 10.0f, 0.0f));
		baseNave = glm::rotate(baseNave, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		baseNave = glm::rotate(baseNave, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		//Faro (spotlight): los vec4 están en ejes del MODELO (X = hacia la nariz, Z = arriba).
		//Posición con w = 1: (8, 0, 0) = 8 unidades al frente del origen del modelo, viaja con la nave.
		//Dirección con w = 0: (1, 0, -0.6) = al frente y hacia abajo para que el cono alcance a pegar en el piso.
		spotLights[0].SetFlash(glm::vec3(baseNave * glm::vec4(8.0f, 0.0f, 0.0f, 1.0f)),
			glm::normalize(glm::vec3(baseNave * glm::vec4(1.0f, 0.0f, -0.6f, 0.0f))));

		//Luz puntual: (0, 0, -3) = 3 unidades debajo del origen del modelo. Se vuelve a crear cada frame con la posición nueva.
		glm::vec3 posLuzPuntualNave = glm::vec3(baseNave * glm::vec4(0.0f, 0.0f, -3.0f, 1.0f));
		pointLights[0] = PointLight(1.0f, 0.5f, 0.0f,
			0.0f, 1.0f,
			posLuzPuntualNave.x, posLuzPuntualNave.y, posLuzPuntualNave.z,
			1.0f, 0.045f, 0.0075f);

		//Luces azules de los motores.
		//Se vuelven a crear cada frame con la posición nueva, igual que la luz puntual de la nave.
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

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		//shaderList[0].SetPointLights(pointLights, pointLightCount);
		//shaderList[0].SetSpotLights(spotLights, spotLightCount);
		//Solo se mandan al shader las luces que estén prendidas. Las de la nave dependen de la tecla 1 (leída desde Window); más adelante aquí también entran las de los motores
		PointLight pointLightsActivas[MAX_POINT_LIGHTS];
		unsigned int pointLightCountActivas = 0;
		SpotLight spotLightsActivas[MAX_SPOT_LIGHTS];
		unsigned int spotLightCountActivas = 0;
		if (mainWindow.getLuzNave()) pointLightsActivas[pointLightCountActivas++] = pointLights[0];	//luz puntual de la nave
		if (mainWindow.getLuzNave()) spotLightsActivas[spotLightCountActivas++] = spotLights[0];		//faro (spotlight) de la nave
		//Luces azules de los motores: cada una depende de su tecla (2 = motor 1, 3 = motor 2, leídas desde Window)
		if (mainWindow.getMotor1()) pointLightsActivas[pointLightCountActivas++] = pointLights[1];	//motor 1
		if (mainWindow.getMotor2()) pointLightsActivas[pointLightCountActivas++] = pointLights[2];	//motor 2
		shaderList[0].SetPointLights(pointLightsActivas, pointLightCountActivas);
		shaderList[0].SetSpotLights(spotLightsActivas, spotLightCountActivas);


		//Piso
		color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[0]->RenderMeshModel();

		//Nave Wing-Y flotando
		color = glm::vec3(1.0f, 1.0f, 1.0f);	//blanco para que no tiña la textura
		//La posición y la rotación ya vienen en baseNave (arriba, junto a las luces); aquí solo se escala
		model = baseNave;
		model = glm::scale(model, glm::vec3(1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//Se deja puesta la textura albedo: si Model carga la suya, la usa; si no, queda esta
		naveTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		//Fuego de los motores: se dibuja DESPUÉS de la nave y con blending (transparencia), igual que el agave.
		//glDepthMask(GL_FALSE) evita que las partes transparentes de un plano tapen al plano cruzado.
		//Cada fuego usa la matriz de la nave (baseNave), así que viaja con ella. Se coloca en la tobera con el translate (ejes del modelo)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		fuegoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		//Fuego del motor 1 (y positiva): tobera en (-17, 6.41, -0.58)
		model = baseNave;
		model = glm::translate(model, glm::vec3(-17.0f, 6.41f, -0.58f));
		model = glm::scale(model, glm::vec3(15.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		if (mainWindow.getMotor1()) meshListModel[1]->RenderMeshModel();	//solo se dibuja si el motor 1 está prendido (tecla 2)

		//Fuego del motor 2 (y negativa): tobera en (-17, -6.41, -0.58)
		model = baseNave;
		model = glm::translate(model, glm::vec3(-17.0f, -6.41f, -0.58f));
		model = glm::scale(model, glm::vec3(15.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		if (mainWindow.getMotor2()) meshListModel[1]->RenderMeshModel();	//solo se dibuja si el motor 2 está prendido (tecla 3)

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}