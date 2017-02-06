#include "Header.h"
#include "ShaderBuilder.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>

float totalEnergy = 0.f,
        maxHeight = 0.f, // max height of track. Top of chain. currently 10m
        zoom = 10.f;

bool cartCamera = false;

double mouse_old_x, 
    mouse_old_y;
float rotate_x = 0.0,
    rotate_y = 0.0;

const GLfloat clearColor[] = { 0.3f, 0.6f, 1.f };
glm::vec3 up(0.f, 1.f, 0.f),
          cam(0.f, 0.5f, 2.f),
          center(0.f, 5.f, 0.f);


GLuint trackVertexArray = 0, groundVertexArray = 0, trackProgram, cartProgram, groundProgram;
std::vector<glm::vec3> vertices, tangents, normals;


void errorCallback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void motion(GLFWwindow* window, double x, double y);
void printOpenGLVersion();

void generateTrackBuffer()
{
    GLuint vertexBuffer = 0, tangentBuffer = 0, normalBuffer = 0;

    glGenVertexArrays(1, &trackVertexArray);
    glBindVertexArray(trackVertexArray);

    maxHeight = generateTrackCurve(vertices);
    generateTrackTangents(vertices, tangents);
    generateTrackNormals(vertices, tangents, normals, maxHeight);

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

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void generateGroundBuffer()
{
    GLuint vertexBuffer = 0;

    glGenVertexArrays(1, &groundVertexArray);
    glBindVertexArray(groundVertexArray);

    std::vector<glm::vec3> groundVerts;

    groundVerts.push_back(glm::vec3(-200.f, -0.1f, -200.f));
    groundVerts.push_back(glm::vec3(-200.f, -0.1f, 200.f));
    groundVerts.push_back(glm::vec3(200.f, -0.1f, -200.f));
    groundVerts.push_back(glm::vec3(200.f, -0.1f, 200.f));

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVerts[0]) * groundVerts.size(), &groundVerts[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void generateShaders()
{
    trackProgram = generateProgram( "shaders/track.vert",
                                    "shaders/track.geom",
                                    "shaders/track.frag");
    cartProgram = generateProgram(  "shaders/cart.vert",
                                    "shaders/cart.geom",
                                    "shaders/cart.frag");
    groundProgram = generateProgram("shaders/general.vert",
                                    "shaders/general.frag");
}

void passBasicUniforms(GLuint program)
{
    glm::mat4   modelview = glm::lookAt(cam * zoom, center, up),
        projection = glm::perspective(45.0f, ASPECT_RATIO, 0.01f, 100.0f);

    if (!cartCamera)
    {
        glm::mat4   rotationX = rotate(identity, rotate_x  * PI / 180.0f, glm::vec3(1.f, 0.f, 0.f)),
            rotationY = rotate(rotationX, rotate_y  * PI / 180.0f, glm::vec3(0.f, 1.f, 0.f));

        modelview *= rotationY;
    }

    glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, GL_FALSE, value_ptr(modelview));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, value_ptr(projection));
}

void renderTrack(GLuint program, GLuint vertexArray, int numVertiecs)
{

    glBindVertexArray(vertexArray);
    glUseProgram(program);

    glLineWidth(3);

    passBasicUniforms(program);

    glDrawArrays(GL_LINE_LOOP, 0, numVertiecs);

    glBindVertexArray(0);
}

void renderCart(GLuint program, GLuint vertexArray, int position)
{
    glBindVertexArray(vertexArray);
    glUseProgram(program);

    passBasicUniforms(program);

    for (int i = 0; i < numOfCarts; i++)
    {
        int pos = (position - (8 * i)) % (int)(vertices.size());
        if (pos >= 0)
            glDrawArrays(GL_POINTS, pos, 1);
        else
            glDrawArrays(GL_POINTS, ((int)vertices.size()) + pos, 1);
    }

    glBindVertexArray(0);
}

void renderGround(GLuint program)
{
    glBindVertexArray(groundVertexArray);
    glUseProgram(program);

    passBasicUniforms(program);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
}

float calculateSpeed(float height)
{
    float eK = max(minKineticEnergy, totalEnergy - (cartMass * gravity * height));   // total energy - eP = eK
    totalEnergy = eK + (cartMass * gravity * height);
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
    generateGroundBuffer();

    glfwSwapInterval(3);

    totalEnergy = minKineticEnergy;   // initial eK + max eP
    float position = 0.f;

	while (!glfwWindowShouldClose(window))
	{
        glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 0, clearColor);


        if (cartCamera)
        {
            float personHeight = 0.5f;
            zoom = 1.f;
            cam = vertices[position] + personHeight * normals[position];
            up = normals[position];
            center = vertices[position] + tangents[position] + personHeight * normals[position];

        }


        renderGround(groundProgram);

        renderTrack(trackProgram, trackVertexArray, vertices.size());
        renderCart(cartProgram, trackVertexArray, position);

        glDisable(GL_DEPTH_TEST);
		glfwSwapBuffers(window);
		glfwPollEvents();

        position = (int)(position + calculateSpeed(vertices[position].y)) % (vertices.size() - 1);

        totalEnergy -= energyLoss;
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
		case(GLFW_KEY_N):
			std::cout << "Recompiling Shaders... ";
            generateShaders();
            std::cout << "Done" << std::endl;
			break;
        case(GLFW_KEY_M):
            std::cout << "Recalculating Track... ";
            generateTrackBuffer();
            std::cout << "Done" << std::endl;
            break;
        case(GLFW_KEY_A):
            center.x += 1.f;
            break;
        case(GLFW_KEY_D):
            center.x -= 1.f;
            break;
        case(GLFW_KEY_S):
            center.y -= 1.f;
            break;
        case(GLFW_KEY_W):
            center.y += 1.f;
            break;
        case(GLFW_KEY_C):
            cartCamera = !cartCamera;
            if (!cartCamera)
            {
                zoom = defaultZoom;
                cam = defaultCam;
                up = defaultUp;
                center = defaultCenter;
            }
            break;
		default:
			break;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!cartCamera)
    {
        if (yoffset < 0)
            zoom += 0.1f;
        else if (yoffset > 0)
            zoom -= 0.1f;
    }
}

void motion(GLFWwindow* window, double x, double y)
{
    if (!cartCamera) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
        {
            rotate_x += (float)((y - mouse_old_y) * 0.5f);
            rotate_y += (float)((x - mouse_old_x) * 0.5f);
        }
        mouse_old_x = x;
        mouse_old_y = y;
    }
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
