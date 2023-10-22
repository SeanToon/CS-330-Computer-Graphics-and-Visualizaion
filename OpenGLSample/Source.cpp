/*
* CS-330 Comp Graphic and Visualizaiton 23EW1
* Final 3D Scene - Toon
* Reference:
* https://learn.snhu.edu/d2l/le/1374817/discussions/posts/95399676/ViewAttachment?fileId=107255141
*
*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ShapeGenerator.h"
#include "ShapeData.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "shader.h"
#include "camera.h"
#include "Sphere.h"

#include <iostream>

#include "cylinder.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
// Initial camera movement speed
float cameraSpeed = 0.1f;

// view toggle
bool isOrtho = false;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//define structure used for shader properties

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 color;
};


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Interactivity in a 3D scene - Toon", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);



	// build and compile our shader zprograms
	// ------------------------------------
	Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");
	Shader lampShader("shaderfiles/2.1.light_cube.vs", "shaderfiles/2.1.light_cube.fs");


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// Front face
		-1.25f, -2.5f, 0.25f, 0.0f, 0.0f,
			1.25f, -2.5f, 0.25f, 0.0f, 0.0f,
			1.25f, 2.5f, 0.25f, 0.0f, 0.0f,
			1.25f, 2.5f, 0.25f, 0.0f, 0.0f,
			-1.25f, 2.5f, 0.25f, 0.0f, 0.0f,
			-1.25f, -2.5f, 0.25f, 0.0f, 0.0f,

			// Back face
			-1.25f, -2.5f, -0.25f, 0.0f, 0.0f,
			1.25f, -2.5f, -0.25f, 1.0f, 0.0f,
			1.25f, 2.5f, -0.25f, 1.0f, 1.0f,
			1.25f, 2.5f, -0.25f, 1.0f, 1.0f,
			-1.25f, 2.5f, -0.25f, 0.0f, 1.0f,
			-1.25f, -2.5f, -0.25f, 0.0f, 0.0f,

			// Top face
			-1.25f, 2.5f, 0.25f, 0.0f, 0.0f,
			1.25f, 2.5f, 0.25f, 0.1f, 0.0f,
			1.25f, 2.5f, -0.25f, 0.1f, 0.1f,
			1.25f, 2.5f, -0.25f,  0.1f, 0.1f,
			-1.25f, 2.5f, -0.25f, 0.0f, 0.1f,
			-1.25f, 2.5f, 0.25f, 0.0f, 0.0f,

			// Bottom face
			-1.25f, -2.5f, 0.25f, 0.0f, 0.0f,
			1.25f, -2.5f, 0.25f, 0.1f, 0.0f,
			1.25f, -2.5f, -0.25f, 0.1f, 0.1f,
			1.25f, -2.5f, -0.25f, 0.1f, 0.1f,
			-1.25f, -2.5f, -0.25f, 0.0f, 0.1f,
			-1.25f, -2.5f, 0.25f, 0.0f, 0.0f,

			//Left side
		   -1.25f, -2.5f, -0.25f, 0.0f, 0.0f,
		   -1.25f, 2.5f, -0.25f, 0.1f, 0.0f,
		   -1.25f, 2.5f, 0.25f, 0.1f, 0.1f,
		   -1.25f, 2.5f, 0.25f, 0.1f, 0.1f,
		   -1.25f, -2.5f, 0.25f, 0.0f, 0.1f,
		   -1.25f, -2.5f, -0.25f, 0.0f, 0.0f,

		   // Right side
		   1.25f, -2.5f, -0.25f, 0.0f, 0.0f,
		   1.25f, 2.5f, -0.25f, 0.1f, 0.0f,
		   1.25f, 2.5f, 0.25f, 0.1f, 0.1f,
		   1.25f, 2.5f, 0.25f, 0.1f, 0.1f,
		   1.25f, -2.5f, 0.25f, 0.0f, 0.1f,
		   1.25f, -2.5f, -0.25f, 0.0f, 0.0f,
	};

	float planeVertices[] = {
		// Positions		   //Normals          // Texture
		-25.0f, 0.0f, -15.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		 25.0f, 0.0f, -15.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		 25.0f, 0.0f,  15.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		-25.0f, 0.0f, -15.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		 25.0f, 0.0f,  15.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		-25.0f, 0.0f,  15.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
	};


	unsigned int VBO, VAO;
	unsigned int VBO2, VAO2;
	unsigned int VBO3, VAO3;
	unsigned int planeVBO, planeVAO;
	unsigned int lightVBO, lightVAO;
	unsigned int VBO4, VAO4;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glBindVertexArray(VAO4);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4);

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	//plane position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//plane texture coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//plane texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // texture does not need to be flipped on the y-axis.
	unsigned char* data = stbi_load("images/transparentGlass.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	stbi_set_flip_vertically_on_load(false); // texture flipped on the y-axis.
	data = stbi_load("images/MyScreen.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// texture 3
// ---------
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data2 = stbi_load("images/container2_black.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data2);

	// texture 4
// ---------
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data3 = stbi_load("images/blackDesk.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data3);

	// texture 5
// ---------
	glGenTextures(1, &texture5);
	glBindTexture(GL_TEXTURE_2D, texture5);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data4 = stbi_load("images/EnergyDrink.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data4);

	// texture 6
// ---------
	glGenTextures(1, &texture6);
	glBindTexture(GL_TEXTURE_2D, texture6);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data5 = stbi_load("images/keyboard.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data5);

	// texture 7
// ---------
	glGenTextures(1, &texture7);
	glBindTexture(GL_TEXTURE_2D, texture7);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data6 = stbi_load("images/side.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data6);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data6);

	// texture 8
// ---------
	glGenTextures(1, &texture8);
	glBindTexture(GL_TEXTURE_2D, texture8);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data7 = stbi_load("images/front2.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data7);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data7);

	// texture 9
// ---------
	glGenTextures(1, &texture9);
	glBindTexture(GL_TEXTURE_2D, texture9);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data8 = stbi_load("images/mouse.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data8);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data8);


	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setInt("texture3", 2);
	ourShader.setInt("texture4", 3);
	ourShader.setInt("texture5", 4);
	ourShader.setInt("texture6", 5);

	// light properties
	lampShader.use();
	Light light;
	light.position = glm::vec3(0.0, 8.0, -25.0);
	light.ambient = glm::vec3(0.05, 0.05, 0.05);
	light.diffuse = glm::vec3(2.0, 2.0, 2.0);
	light.specular = glm::vec3(0.5, 0.5, 0.5);
	light.color = glm::vec3(0.0, 0.255, 0.255);

	//set values in shader using structure vectors
	lampShader.setVec3("light.position", light.position);
	lampShader.setVec3("light.ambient", light.ambient);
	lampShader.setVec3("light.diffuse", light.diffuse);
	lampShader.setVec3("light.specular", light.specular);
	lampShader.setVec3("light.color", light.color);

	glm::mat4 model;
	float xAngle;
	float yAngle;
	float angle;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.5f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// view is initially perspective
		// toggle between orthographic and perspective views when p is pressed
		glm::mat4 projection;

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !isOrtho)
		{
			//orthographic view.
			float scale = 100;
			projection = glm::ortho(-(1200.0f / scale), 1200.0f / scale, -(1000.0f / scale), 1000.0f / scale, 0.1f, 100.0f);
			isOrtho = true;
			//update
			ourShader.setMat4("projection", projection);
		}
		else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
		{
			isOrtho = false;
			// perspective view.
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			// Update 
			ourShader.setMat4("projection", projection);
		}

		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);

		// Render phone box
		// bind textures on phone box
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-12.0f, -2.5f, -4.0f));
		yAngle = 45.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 90.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Render light sphere
		glBindVertexArray(lightVAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, light.position);
		model = glm::scale(model, glm::vec3(0.2f));
		ourShader.setMat4("model", model);

		Sphere S(5.0, 90, 50);
		S.Draw();

		//Inner light part of screen
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing

		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, 6.5f, -19.98f));
		yAngle = 0.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 180.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.5f, 2.5f, 0.5f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Outerborder of screen
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::vec3 scaleFactors(9.9f, 2.6f, 0.5f);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, 6.5f, -20.0f));
		yAngle = 0.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 0.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(9.9f, 2.8f, 0.5f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//back stand of screen - vertical arm
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, -22.5f));
		yAngle = 0.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 0.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 1.5f, 3.8f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//back stand of screen - horizontal arm
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, -22.0f));
		yAngle = 0.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 90.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.6f, 3.8f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//back stand of screen - bottom base
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, -21.0f));
		yAngle = 0.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 90.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 1.0f, 2.5f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Keyboard
		glBindTexture(GL_TEXTURE_2D, texture6);
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, -2.2f, -10.0f));
		yAngle = 0.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 95.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.5f, 1.0f, 1.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Left side of desktop - air filter holes texture
		glBindTexture(GL_TEXTURE_2D, texture7);
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(15.0f, 2.5f, -15.0f));
		yAngle = 90.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 0.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 0.8f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Right side of desktop - blank texture
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(18.0f, 2.5f, -15.0f));
		yAngle = 90.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 0.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 0.8f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//top side of desktop - blank texture
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(16.5f, 7.3f, -15.0f));
		yAngle = 90.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 90.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 0.65f, 1.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Back side of desktop - blank texture
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(16.5f, 2.5f, -21.0f));
		yAngle = 0.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 0.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.3f, 2.0f, 1.0f));  //(5.0f, 0.65f, 1.0f)
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Front face side of desktop - hp desktop texture
		glBindTexture(GL_TEXTURE_2D, texture8);
		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(16.5f, 2.5f, -9.5f));
		yAngle = 0.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		xAngle = 180.0f;
		model = glm::rotate(model, glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.2f, 1.9f, 1.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Computer mouse body
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.0f, -2.5f, 0.0f));
		yAngle = 15.0f;
		model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.2f, 0.95f));
		ourShader.setMat4("model", model);

		Sphere S2(3.0, 90, 50);

		S2.Draw();

		// Render cylinders
		// bind textures on "phone camera" cylinders
		glBindTexture(GL_TEXTURE_2D, texture3);

		// render first cylinder
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-13.95f, -2.28f, -4.87f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C(.18, 40, .1, true, true, true);
		C.render();

		// render second cylinder
		glBindVertexArray(VAO3);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-13.585f, -2.28f, -4.56f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C2(.18, 40, .1, true, true, true);
		C2.render();

		// Render the plane
		glBindTexture(GL_TEXTURE_2D, texture3);
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		glBindVertexArray(planeVAO);
		glm::mat4 planeModel = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		//adjust position, rotation, and scale of the plane
		planeModel = glm::translate(planeModel, glm::vec3(0.0f, -2.5f, -10.0f));
		planeModel = glm::rotate(planeModel, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		planeModel = glm::scale(planeModel, glm::vec3(1.0f, 1.0f, 1.0f));
		lampShader.setMat4("model", planeModel);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Switch back to ourShader
		ourShader.use();

		glBindTexture(GL_TEXTURE_2D, texture5);
		// render energy drink cylinder
		glBindVertexArray(VAO4);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-15.0f, 1.0f, -11.0f));
		angle = 180.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C3(1.2, 30.0, 7.0, true, true, false);
		C3.render();


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	glDeleteVertexArrays(1, &VAO3);
	glDeleteBuffers(1, &VBO3);

	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &lightVBO);


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	//'q' and 'e' keys control camera up and down commands

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	// adjust camera speed based on scroll direction
	if (yoffset > 0)
	{
		cameraSpeed += 0.1f; // increase camera speed when scrolling up
	}
	else
	{
		cameraSpeed -= 0.1f; // decrease camera speed when scrolling down
		if (cameraSpeed < 0.00001f) // minimum camera speed
		{
			cameraSpeed = 0.0000001f;
		}
	}
}
