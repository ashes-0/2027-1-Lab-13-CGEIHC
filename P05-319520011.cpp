/*
Práctica 5: Optimización y Carga de Modelos
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
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;
//Lista de Modelos a importar
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

//Holocron
Model HoloBase_M;
Model HoloEsq_M;


//Satélite
Model SatBase_M;
Model SatTail_M;
Model P1_M;
Model P2_M;


//Lista de Skybox a crear
Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";


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

	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);
	//Cargar modelos
	//Rover_M = Model();
	//Rover_M.LoadModel("Models/RoverModificado.obj");
	Cuerpo_M = Model();
	Cuerpo_M.LoadModel("Models/cuerpo.fbx");
	//Piezas del brazo
	BB_M = Model();
	BB_M.LoadModel("Models/BB.fbx");
	BM_M = Model();
	BM_M.LoadModel("Models/BM.fbx");
	BMA_M = Model();
	BMA_M.LoadModel("Models/BMA.fbx");
	BP_M = Model();
	BP_M.LoadModel("Models/BP.fbx");
	//Patas delantera
	PataDelanteraDer_M = Model();
	PataDelanteraDer_M.LoadModel("Models/PFD.fbx");
	PataDelanteraIzq_M = Model();
	PataDelanteraIzq_M.LoadModel("Models/PFI.fbx");
	//Patas Medias
	PataMediaDer_M = Model();
	PataMediaDer_M.LoadModel("Models/PMD.fbx");
	PataMediaIzq_M = Model();
	PataMediaIzq_M.LoadModel("Models/PMI.fbx");
	//Patas Traseras
	PataTraseraDer_M = Model();
	PataTraseraDer_M.LoadModel("Models/PTD.fbx");
	PataTraseraIzq_M = Model();
	PataTraseraIzq_M.LoadModel("Models/PTI.fbx");

	//Modelo de Holocron
	//Modelo de base del holocron
	HoloBase_M = Model();
	HoloBase_M.LoadModel("Models/HoloBase.fbx");
	//Modelo de esquina de del holocron
	HoloEsq_M = Model();
	HoloEsq_M.LoadModel("Models/HoloEsq.fbx");

	//Modelo de Satélite
	//Modelo de base 
	SatBase_M = Model();
	SatBase_M.LoadModel("Models/SatBase.fbx");
	//Modelo de la cola
	SatTail_M = Model();
	SatTail_M.LoadModel("Models/SatTail.fbx");
	//Modelo de Panel 1 y 2
	P1_M = Model();
	P1_M.LoadModel("Models/P1.fbx");
	P2_M = Model();
	P2_M.LoadModel("Models/P2.fbx");



	//Crear Skybox con sus 6 texturas
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformColor = 0;
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
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[2]->RenderMeshModel();

		//------------*INICIA DIBUJO DE NUESTROS DEMÁS OBJETOS-------------------*
		//Modelo Inicial
		color = glm::vec3(1.0f, 0.0f, 1.0f); //modelo de color azul
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		modelaux = model;
		modelaux2 = model;
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

		
		//------------*INICIA DIBUJO DEl HOLOCRON-------------------*
		//Modelo HOLOCRON BASE
		color = glm::vec3(1.0f, 1.0f, 1.0f); //modelo de bkanco.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 5.0f, 0.0f));
		modelaux = model;
		modelaux2 = model;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		HoloBase_M.RenderModel();

		//ESQUINA SUPERIOR FRONTAL DERECHA
		color = glm::vec3(0.5f, 1.0f, 0.0f);
		model = glm::translate(model, glm::vec3(4.0f, 4.0, 4.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario F
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
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
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario G
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
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
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario H
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto p
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
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario J
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
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
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion11()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario Y 
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
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
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion12()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario U
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
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
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion13()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario I 
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
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
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion14()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en solitario O
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion15()), glm::vec3(0.0f, 0.0f, 1.0f)); // Giro en conjunto P
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		HoloEsq_M.RenderModel();

		//------------*INICIA DIBUJO DEl SATÉLITE-------------------*
		//Modelo SATBASE

		//Modelo SATBASE
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, 10.0f, 0.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getmovX(), mainWindow.getmovY(), mainWindow.getmovZ())); // movimiento con flechas
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));// orientación del modelo
		modelaux = model;
		modelaux2 = model;// los hijos parten de aquí y heredan todo lo anterior
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SatBase_M.RenderModel();

		//SATTAIL
		model = modelaux;
		model = glm::translate(model, glm::vec3(00.0f, 5.0f, 0.0f)); // punto de unión con el cuerpo
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(0.0f, 1.0f, 0.0f)); // E: giro en solitario
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // corrección fija (reemplaza los 180° en X y Z)
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		SatTail_M.RenderModel();

		
		//P1
		model = modelaux2;
		model = glm::translate(model, glm::vec3(15.5f, -8.0f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(1.0f, 0.0f, 0.0f)); // Giro en solitario R 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 1.0f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		P1_M.RenderModel();

		
		//P2
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-15.0f, -8.0f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(1.0f, 0.0f, 0.0f)); // Giro en solitario T
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.5f, 0.5f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		P2_M.RenderModel();
		

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
