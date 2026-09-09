#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"

const float toRadians = 3.14159265f / 180.0; 
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";
//shaders nuevos para el ejercicio 2
static const char* vShaderBase = "shaders/shaderv.vert";
static const char* fShaderVerde = "shaders/shaderverde.frag";
static const char* fShaderAzul = "shaders/shaderazul.frag";
static const char* fShaderCafe = "shaders/shadercafe.frag";
static const char* fShaderMagenta = "shaders/shadermagenta.frag";
static const char* fShaderAmarillo = "shaders/shaderamarillo.frag";
static const char* fShaderRojo = "shaders/shaderrojo.frag";
static const char* fShaderNegro = "shaders/shadernegro.frag";

//float angulo = 0.0f;

//color café/marrón en RGB : 0.478, 0.255, 0.067

using std::vector;

//Pirámide triangular
void CreaPiramide()
{
	unsigned int indices[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3

	};
	GLfloat vertices[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* piramidetriangular = new Mesh();
	piramidetriangular->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(piramidetriangular);
}

//función para crear pirámide cuadrangular
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}

//Vértices de un cubo
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

//comentado para no interferir con el ejercicio 02
/*
void CrearLetrasyFiguras()
{
	GLfloat vertices_letras[] = {
		//	x			Y		Z		R		G		B
		//Letra M
		-0.264f,	-0.857f,	0.0f,	0.7f,	0.4f,	0.9f,
		-0.462f,	-0.857f,	0.0f,	0.7f,	0.4f,	0.9f,
		-0.264f,	0.857f,		0.0f,	0.7f,	0.4f,	0.9f,

		-0.462f,	0.857f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.462f,	-0.857f,	0.0f,	0.7f,	0.4f,	0.9f,
		-0.264f,	0.857f,		0.0f,	0.7f,	0.4f,	0.9f,

		-0.462f,	0.857f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.462f,	0.285f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.6f,		0.428f,		0.0f,	0.7f,	0.4f,	0.9f,

		-0.6f,		-0.142f,	0.0f,	0.7f,	0.4f,	0.9f,
		-0.462f,	0.285f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.6f,		0.428f,		0.0f,	0.7f,	0.4f,	0.9f,

		-0.6f,		-0.142f,	0.0f,	0.7f,	0.4f,	0.9f,
		-0.733f,	0.285f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.6f,		0.428f,		0.0f,	0.7f,	0.4f,	0.9f,

		-0.733f,	0.857f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.733f,	0.285f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.6f,		0.428f,		0.0f,	0.7f,	0.4f,	0.9f,

		-0.733f,	0.857f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.733f,	-0.857f,	0.0f,	0.7f,	0.4f,	0.9f,
		-0.933f,	-0.857f,	0.0f,	0.7f,	0.4f,	0.9f,

		-0.733f,	0.857f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.933f,	0.857f,		0.0f,	0.7f,	0.4f,	0.9f,
		-0.933f,	-0.857f,	0.0f,	0.7f,	0.4f,	0.9f,


		// Letra A
		-0.2f,		-0.857f,	0.0f,	1.0f,	0.5f,	0.0f,
		-0.0f,		-0.857f,	0.0f,	1.0f,	0.5f,	0.0f,
		0.066f,		-0.142f,	0.0f,	1.0f,	0.5f,	0.0f,

		0.1f,		-0.071f,	0.0f,	1.0f,	0.5f,	0.0f,
		0.133f,		-0.142f,	0.0f,	1.0f,	0.5f,	0.0f,
		0.066f,		-0.142f,	0.0f,	1.0f,	0.5f,	0.0f,

		0.2f,		-0.857f,	0.0f,	1.0f,	0.5f,	0.0f,
		0.133f,		-0.142f,	0.0f,	1.0f,	0.5f,	0.0f,
		0.4f,		-0.857f,	0.0f,	1.0f,	0.5f,	0.0f,

		0.0f,		0.142f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.266f,		0.142f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.4f,		-0.857f,	0.0f,	1.0f,	0.5f,	0.0f,

		0.2f,		0.142f,		0.0f,	1.0f,	0.5f,	0.0f,
		-0.066f,	0.142f,		0.0f,	1.0f,	0.5f,	0.0f,
		-0.2f,		-0.857f,	0.0f,	1.0f,	0.5f,	0.0f,

		0.0f,		0.857f,		0.0f,	1.0f,	0.5f,	0.0f,
		-0.066f,	0.142f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.1f,		0.571f,		0.0f,	1.0f,	0.5f,	0.0f,

		0.2f,		0.857f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.266f,		0.142f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.1f,		0.571f,		0.0f,	1.0f,	0.5f,	0.0f,

		-0.03f,		0.285f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.23f,		0.285f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.1f,		0.571f,		0.0f,	1.0f,	0.5f,	0.0f,

		0.2f,		0.571f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.266f,		0.142f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.2f,		0.142f,		0.0f,	1.0f,	0.5f,	0.0f,

		0.0f,		0.571f,		0.0f,	1.0f,	0.5f,	0.0f,
		-0.066f,	0.142f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.0f,		0.142f,		0.0f,	1.0f,	0.5f,	0.0f,

		0.0f,		0.857f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.1f,		0.571f,		0.0f,	1.0f,	0.5f,	0.0f,
		0.2f,		0.857f,		0.0f,	1.0f,	0.5f,	0.0f,



		//Letra L
		0.466f,		-0.857f,	0.0f,	0.0f,	0.7f,	0.7f,
		0.666f,		-0.857f,	0.0f,	0.0f,	0.7f,	0.7f,
		0.666f,		0.857f,		0.0f,	0.0f,	0.7f,	0.7f,

		0.466f,		-0.857f,	0.0f,	0.0f,	0.7f,	0.7f,
		0.466f,		0.857f,		0.0f,	0.0f,	0.7f,	0.7f,
		0.666f,		0.857f,		0.0f,	0.0f,	0.7f,	0.7f,

		0.866f,		-0.285f,	0.0f,	0.0f,	0.7f,	0.7f,
		0.666f,		-0.285f,	0.0f,	0.0f,	0.7f,	0.7f,
		0.666f,		-0.857f,	0.0f,	0.0f,	0.7f,	0.7f,

		0.866f,		-0.285f,	0.0f,	0.0f,	0.7f,	0.7f,
		0.866f,		-0.857f,	0.0f,	0.0f,	0.7f,	0.7f,
		0.666f,		-0.857f,	0.0f,	0.0f,	0.7f,	0.7f,
	};

	MeshColor* letras = new MeshColor();
	letras->CreateMeshColor(vertices_letras, 414);
	meshColorList.push_back(letras);
}
*/


void CreateShaders()
{

	Shader* shader1 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();//shader para usar color como parte del VAO: letras 
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2);

	//shaders para el ejercicio 2
	Shader* shaderVerde = new Shader();
	shaderVerde->CreateFromFiles(vShaderBase, fShaderVerde);
	shaderList.push_back(*shaderVerde);

	Shader* shaderAzul = new Shader();
	shaderAzul->CreateFromFiles(vShaderBase, fShaderAzul);
	shaderList.push_back(*shaderAzul);

	Shader* shaderCafe = new Shader();
	shaderCafe->CreateFromFiles(vShaderBase, fShaderCafe);
	shaderList.push_back(*shaderCafe);

	Shader* shaderMagenta = new Shader();
	shaderMagenta->CreateFromFiles(vShaderBase, fShaderMagenta);
	shaderList.push_back(*shaderMagenta);

	Shader* shaderAmarillo = new Shader();
	shaderAmarillo->CreateFromFiles(vShaderBase, fShaderAmarillo);
	shaderList.push_back(*shaderAmarillo);

	Shader* shaderRojo = new Shader();
	shaderRojo->CreateFromFiles(vShaderBase, fShaderRojo);
	shaderList.push_back(*shaderRojo);

	Shader* shaderNegro = new Shader();
	shaderNegro->CreateFromFiles(vShaderBase, fShaderNegro);
	shaderList.push_back(*shaderNegro);
}


int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	CreaPiramide(); //índice 0 en MeshList
	CrearCubo();//índice 1 en MeshList
	CrearPiramideCuadrangular(); //índice 2 en MeshList
	//CrearLetrasyFiguras(); //comentado para no interferir con el ejercicio 02
	CreateShaders();
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	//Projection: Matriz de Dimensión 4x4 para indicar si vemos en 2D( orthogonal) o en 3D) perspectiva
	//Se activa ortogonal para el ejercicio de las 3 figuras (portería, cuadrado con rombos, triforce)
	//Se amplía el volumen de -1,1 a -8,8 (simétrico en X y Y para no distorsionar, la ventana es cuadrada de 800x800)
	glm::mat4 projection = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, 0.1f, 100.0f);
	//glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	//Model: Matriz de Dimensión 4x4 en la cual se almacena la multiplicación de las transformaciones geométricas.
	glm::mat4 model(1.0); //fuera del while se usa para inicializar la matriz con una identidad

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		//Recibir eventos del usuario
		glfwPollEvents();
		//Limpiar la ventana
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad

		//comentado para no interferir con el ejercicio 02
		/*
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshColorList[0]->RenderMeshColor(); //letras de colores
		*/


		// Grupo cohete
		float g1x = -6.0f, g1z = -5.0f;

		//Poste izquierdo, cubo café escalado
		shaderList[4].useShader();
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g1x - 1.0f, 1.3f, g1z));
		model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); //cubo café

		//Poste derecho, cubo café
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g1x + 1.0f, 1.3f, g1z));
		model = glm::scale(model, glm::vec3(0.3f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh(); //cubo café

		//Pirámide amarilla invertida, arriba 
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g1x, 2.7f, g1z));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); //pirámide amarilla

		//Pirámide roja, medio 
		shaderList[7].useShader();
		uniformModel = shaderList[7].getModelLocation();
		uniformProjection = shaderList[7].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g1x, 1.2f, g1z));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); //pirámide roja

		//Pirámide verde, bajo 
		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g1x, -0.3f, g1z));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); //pirámide verde


		// Grupo cuadrado
		float g2x = 0.0f, g2z = -5.0f;

		//Esquina amarilla, ahora pirámide
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g2x - 1.0f, 1.1f, g2z));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[2]->RenderMesh(); //pirámide amarilla

		//Esquina roja, arriba-derecha, pirámide
		shaderList[7].useShader();
		uniformModel = shaderList[7].getModelLocation();
		uniformProjection = shaderList[7].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g2x + 1.0f, 1.1f, g2z));
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[2]->RenderMesh(); //pirámide roja

		//Esquina verde, abajo-derecha, pirámide
		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g2x + 1.0f, -0.9f, g2z));
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[2]->RenderMesh(); //pirámide verde

		//Esquina magenta, abajo-izquierda, pirámide
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g2x - 1.0f, -0.9f, g2z));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[2]->RenderMesh(); //pirámide magenta

		//cubo azul rotado 45 en Z (antes cuadradoazul)
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g2x, 0.1f, g2z + 0.01f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); //cubo azul

		//cubo café rotado 45 en Z (antes cuadradocafe)
		shaderList[4].useShader();
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g2x, 0.1f, g2z + 0.02f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); //cubo café


		// Grupo 3 la No Trifuerza

		float g3x = 6.0f, g3z = -5.0f;

		//Pirámide magenta (antes triángulo magenta)
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g3x - 1.0f, 1.8f, g3z));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); //pirámide magenta

		//Pirámide verde (antes triángulo verde)
		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g3x - 2.0f, -0.2f, g3z));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); //pirámide verde

		//Pirámide roja
		shaderList[7].useShader();
		uniformModel = shaderList[7].getModelLocation();
		uniformProjection = shaderList[7].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g3x + 0.0f, -0.2f, g3z));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); //pirámide roja

		//Pirámide amarilla (antes triángulo amarillo)
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g3x - 1.0f, -0.2f, g3z + 0.01f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh(); //pirámide amarilla

		float g4x = 0.0f, g4z = -5.0f;
		//Piso, ahora cubo negro (antes cuadrado negro)
		shaderList[8].useShader();
		uniformModel = shaderList[8].getModelLocation();
		uniformProjection = shaderList[8].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(g4x - 0.0f, -1.5f, g4z));
		model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(15.0f, 0.5f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh(); //cubo negro

		glUseProgram(0);
		mainWindow.swapBuffers();

	}
	return 0;
}
// inicializar matriz: glm::mat4 model(1.0);
// reestablecer matriz: model = glm::mat4(1.0);
//Traslación
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
//////////////// ROTACIÓN //////////////////
//model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
////////////////  ESCALA ////////////////
//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
///////////////////// T+R////////////////
/*model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
*/
/////////////R+T//////////
/*model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
*/