#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);
	}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	//Todas las funciones necesarias
	GLfloat getrotay() { return rotay; }
	GLfloat getrotax() { return rotax; }
	GLfloat getrotaz() { return rotaz; }
	//Aqui se agregan las funciones de rotación 
	GLfloat getarticulacion1() { return articulacion1; }
	GLfloat getarticulacion2() { return articulacion2; }
	GLfloat getarticulacion3() { return articulacion3; }
	GLfloat getarticulacion4() { return articulacion4; }
	GLfloat getarticulacion5() { return articulacion5; }
	GLfloat getarticulacion6() { return articulacion6; }
	GLfloat getarticulacion7() { return articulacion7; }
	GLfloat getarticulacion8() { return articulacion8; }
	GLfloat getarticulacion9() { return articulacion9; }
	GLfloat getarticulacion10() { return articulacion10; }
	GLfloat getarticulacion11() { return articulacion11; }
	GLfloat getarticulacion12() { return articulacion12; }
	GLfloat getarticulacion13() { return articulacion13; }
	GLfloat getarticulacion14() { return articulacion14; }
	GLfloat getarticulacion15() { return articulacion15; }
	//movimiento
	GLfloat getmovX() { return movX; }
	GLfloat getmovY() { return movY; }
	GLfloat getmovZ() { return movZ; }
	//luces: el color del faro del avión cicla con la tecla 1; las teclas 2 y 3 prenden/apagan las luces puntuales
	GLint getColorFaroAvion() { return colorFaroAvion; }
	bool getLuzHolocron() { return luzHolocron; }
	bool getLuzPinza() { return luzPinza; }
	//luces de la nave (puntual + spotlight) prendidas / apagadas
	bool getLuzNave() { return luzNave; }
	//Práctica 8: fuego + luz azul de cada motor, prendidos / apagados con las teclas 2 (motor 1) y 3 (motor 2)
	bool getMotor1() { return motor1; }
	bool getMotor2() { return motor2; }
	//[E09] giro de las 6 llantas del rover en grados (tecla M hacia adelante, Shift + M hacia atrás)
	GLfloat getGiroLlantas() { return giroLlantas; }


	~Window();
private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLfloat rotax, rotay, rotaz, articulacion1, articulacion2, articulacion3, articulacion4, articulacion5, articulacion6, articulacion7, articulacion8, articulacion9, articulacion10, articulacion11, articulacion12, articulacion13, articulacion14, articulacion15, movX, movY, movZ;
	GLint colorFaroAvion;	//0 rojo, 1 amarillo, 2 verde, 3 cian, 4 azul, 5 magenta
	bool luzHolocron;		//luz puntual del holocrón prendida / apagada
	bool luzPinza;			//luz puntual de la pinza del rover prendida / apagada
	bool luzNave;			//luz puntual y spotlight de la nave prendidas / apagadas
	bool motor1;			//fuego y luz azul del motor 1 prendidos / apagados
	bool motor2;			//fuego y luz azul del motor 2 prendidos / apagados
	GLfloat giroLlantas;	//[E09] ángulo en grados de las 6 llantas del rover (sin límite, para que den vueltas completas)
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};