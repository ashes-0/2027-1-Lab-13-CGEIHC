#include "Window.h"

// Limita el angulo de las patas de las ruedas a +-45 grados
//Traemos la función desde la práctica 05 para que el Rover siga rotando
static GLfloat limita45(GLfloat angulo)
{
	if (angulo > 45.0f)  return 45.0f;
	if (angulo < -45.0f) return -45.0f;
	return angulo;
}

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	rotax = 0.0f;
	rotay = 0.0f;
	rotaz = 0.0f;
	articulacion1 = 0.0f;
	articulacion2 = 0.0f;
	articulacion3 = 0.0f;
	articulacion4 = 0.0f;
	articulacion5 = 0.0f; //rueda 1: delantera derecha
	articulacion6 = 0.0f; //rueda 2: delantera izquierda
	articulacion7 = 0.0f; //rueda 3: media derecha
	articulacion8 = 0.0f; //rueda 4: media izquierda
	articulacion9 = 0.0f; //rueda 5: trasera derecha
	articulacion10 = 0.0f; //rueda 6: trasera izquierda
	articulacion11 = 0.0f;
	articulacion12 = 0.0f;
	articulacion13 = 0.0f;
	articulacion14 = 0.0f;
	articulacion15 = 0.0f;
	movX = 0.0f; //movimiento
	movY = 0.0f;
	movZ = 0.0f;
	colorFaroAvion = 0;		//el faro del avión empieza en rojo
	luzHolocron = true;		//las luces puntuales empiezan prendidas
	luzPinza = true;
	luzNave = true;			//las luces de la nave empiezan prendidas
	motor1 = false;			//los motores empiezan apagados (se prenden con las teclas 2 y 3)
	motor2 = false;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "PracticaXX:Nombre de la practica", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
	// Asignar valores de la ventana y coordenadas

	//Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	// Movimiento del satelite sobre X, Y, Z
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_RIGHT) { theWindow->movX += 1.0f; }
		if (key == GLFW_KEY_LEFT) { theWindow->movX -= 1.0f; }
		if (key == GLFW_KEY_UP) { theWindow->movZ -= 1.0f; }
		if (key == GLFW_KEY_DOWN) { theWindow->movZ += 1.0f; }
		if (key == GLFW_KEY_PAGE_UP) { theWindow->movY += 1.0f; }
		if (key == GLFW_KEY_PAGE_DOWN) { theWindow->movY -= 1.0f; }
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow->muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow->muevex -= 1.0;
	}
	if (key == GLFW_KEY_E)
	{
		theWindow->rotax += 10.0;
	}
	if (key == GLFW_KEY_R)
	{
		theWindow->rotay += 10.0; //rotar sobre el eje y 10 grados
	}
	if (key == GLFW_KEY_T)
	{
		theWindow->rotaz += 10.0;
	}
	if (key == GLFW_KEY_F)
	{
		theWindow->articulacion1 += 10.0;
	}

	if (key == GLFW_KEY_G)
	{
		theWindow->articulacion2 += 10.0;
	}
	if (key == GLFW_KEY_H)
	{
		theWindow->articulacion3 += 10.0;
	}
	if (key == GLFW_KEY_J)
	{
		theWindow->articulacion4 += 10.0;
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow->articulacion11 += 10.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow->articulacion12 += 10.0;
	}
	if (key == GLFW_KEY_I)
	{
		theWindow->articulacion13 += 10.0;
	}
	if (key == GLFW_KEY_O)
	{
		theWindow->articulacion14 += 10.0;
	}
	if (key == GLFW_KEY_P)
	{
		theWindow->articulacion15 += 10.0;
	}
	/*
	Rotacion independiente de las patas de las ruedas (maximo 45 grados en cada sentido)
	  Tecla sola         -> gira hacia adelante
	  Shift + tecla      -> gira hacia atras
	  K: delantera derecha   L: delantera izquierda
	  C: media derecha       V: media izquierda
	  B: trasera derecha     N: trasera izquierda
	Solo se procesa al presionar o mantener la tecla (no al soltarla)
	*/
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		GLfloat paso = (mode & GLFW_MOD_SHIFT) ? -5.0f : 5.0f;

		if (key == GLFW_KEY_K)
		{
			theWindow->articulacion5 = limita45(theWindow->articulacion5 + paso);
		}
		if (key == GLFW_KEY_L)
		{
			theWindow->articulacion6 = limita45(theWindow->articulacion6 + paso);
		}
		if (key == GLFW_KEY_C)
		{
			theWindow->articulacion7 = limita45(theWindow->articulacion7 + paso);
		}
		if (key == GLFW_KEY_V)
		{
			theWindow->articulacion8 = limita45(theWindow->articulacion8 + paso);
		}
		if (key == GLFW_KEY_B)
		{
			theWindow->articulacion9 = limita45(theWindow->articulacion9 + paso);
		}
		if (key == GLFW_KEY_N)
		{
			theWindow->articulacion10 = limita45(theWindow->articulacion10 + paso);
		}
	}

	//Luces de la /
	//1: siguiente color del faro del avión que apunta al suelo (rojo, amarillo, verde, cian, azul, magenta)
	//if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	//{
	//	theWindow->colorFaroAvion = (theWindow->colorFaroAvion + 1) % 6;
	//}
	//1 (Práctica 8): prender / apagar al mismo tiempo la luz puntual y el spotlight de la nave
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		theWindow->luzNave = !theWindow->luzNave;
	}
	//2: prender / apagar la luz puntual del holocrón
	//if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	//{
	//	theWindow->luzHolocron = !theWindow->luzHolocron;
	//}
	//3: prender / apagar la luz puntual de la pinza del rover
	//if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	//{
	//	theWindow->luzPinza = !theWindow->luzPinza;
	//}
	//2 (Práctica 8): prender / apagar el fuego y la luz azul del motor 1
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		theWindow->motor1 = !theWindow->motor1;
	}
	//3 (Práctica 8): prender / apagar el fuego y la luz azul del motor 2
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		theWindow->motor2 = !theWindow->motor2;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		const char* key_name = glfwGetKeyName(GLFW_KEY_D, 0);
		//printf("se presiono la tecla: %s\n",key_name);
	}



	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}