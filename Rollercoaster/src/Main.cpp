#include "Header.h"
#include "ShaderBuilder.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>

#define constEnergy 100   // min kinetic energy is 100J
#define cartMass 200    // mass of the cart is 200Kg
float totalEnergy = 0.f,
        maxHeight = 0.f, // max height of track. Top of chain. currently 10m
        zoom = 10.f;

double mouse_old_x, 
    mouse_old_y;
float rotate_x = 0.0,
    rotate_y = 0.0;

GLuint vertexArray = 0, trackProgram, cartProgram;
std::vector<glm::vec3> vertices, tangents;

void errorCallback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void printOpenGLVersion();

void motion(GLFWwindow* window, double x, double y)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        rotate_x += (float)((y - mouse_old_y) * 0.5f);
        rotate_y += (float)((x - mouse_old_x) * 0.5f);
    }
    mouse_old_x = x;
    mouse_old_y = y;
}

void generateTrackBuffer()
{
    GLuint vertexBuffer, tangentBuffer;

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    maxHeight = generateTrackCurve(vertices);
    generateTrackTangents(vertices, tangents);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &tangentBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangents[0]) * tangents.size(), &tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void generateShaders()
{
    trackProgram = generateProgram( "shaders/general.vert",
                                    "shaders/general.geom",
                                    "shaders/general.frag");
    cartProgram = generateProgram(  "shaders/cart.vert",
                                    "shaders/cart.frag");
}

void renderTrack(GLuint program, GLuint vertexArray, int numVertiecs)
{
    glm::mat4   modelview = glm::lookAt(cam * zoom, center, up),
                projection = glm::perspective(45.0f, ASPECT_RATIO, 0.01f, 100.0f),
                rotationX = rotate(identity, rotate_x  * PI / 180.0f, glm::vec3(1.f, 0.f, 0.f)),
                rotationY = rotate(rotationX, rotate_y  * PI / 180.0f, glm::vec3(0.f, 1.f, 0.f));

    modelview *= rotationY;

    glBindVertexArray(vertexArray);
    glUseProgram(program);

    glPointSize(1);

    glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, GL_FALSE, value_ptr(modelview));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, value_ptr(projection));

    glDrawArrays(GL_LINE_STRIP, 0, numVertiecs);

    glBindVertexArray(0);
}

void renderCart(GLuint program, GLuint vertexArray, int position)
{
    glm::mat4   modelview = glm::lookAt(cam * zoom, center, up),
                projection = glm::perspective(45.0f, ASPECT_RATIO, 0.01f, 100.0f),
                rotationX = rotate(identity, rotate_x  * PI / 180.0f, glm::vec3(1.f, 0.f, 0.f)),
                rotationY = rotate(rotationX, rotate_y  * PI / 180.0f, glm::vec3(0.f, 1.f, 0.f));

    modelview *= rotationY;

    glBindVertexArray(vertexArray);
    glUseProgram(program);

    glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, GL_FALSE, value_ptr(modelview));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, value_ptr(projection));
    
    glPointSize(10);
    glDrawArrays(GL_POINTS, position, 1);

    glBindVertexArray(0);
}

float calculateSpeed(float height)
{
    float eK = totalEnergy - (cartMass * gravity * height);   // total energy - eP = eK
    return sqrt(eK * 2.f / cartMass); // convert eK into velocity
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_SAMPLES, 32);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rollercoaster", NULL, NULL);

	if (!window) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, motion);
    glfwSetScrollCallback(window, scroll_callback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}
	printOpenGLVersion();

    generateShaders();

    generateTrackBuffer();

    glfwSwapInterval(1);

    totalEnergy = (cartMass * gravity * maxHeight) + constEnergy;   // initial eK + max eP
    int position = 0;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderTrack(trackProgram, vertexArray, vertices.size());
        renderCart(cartProgram, vertexArray, position);

		glfwSwapBuffers(window);
		glfwPollEvents();

        position = (int)(position + calculateSpeed(vertices[position].y)) % vertices.size();
	}


	// Shutdow the program
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case(GLFW_KEY_S):
			std::cout << "Recompiling Shaders... ";
            generateShaders();
            std::cout << "Done" << std::endl;
			break;
		default:
			break;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset < 0)
        zoom += 0.1f;
    else if (yoffset > 0)
        zoom -= 0.1f;
}

void printOpenGLVersion()
{
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("OpenGL on %s %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
	printf("GLSL version supported %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("GL version major, minor: %i.%i\n", major, minor);
}

void errorCallback(int error, const char* description)
{
	std::cout << "GLFW ERROR " << error << ": " << description << std::endl;
}
