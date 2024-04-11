#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformModel;

bool goingRight = true;
float tri_XOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

// Vertex Shader
static const char* vShader = "													\n\
#version 330																	\n\
																				\n\
layout (location = 0) in vec3 pos;												\n\
																				\n\
uniform mat4 model;																\n\
																				\n\
void main() {																	\n\
	gl_Position = model * vec4(0.5f * pos.x, 0.5f * pos.y, 0.5f*pos.z, 1.0);	\n\
}																				\n\
";

// Fragment Shader
static const char* fShader = "									\n\
#version 330													\n\
																\n\
out vec4 color;													\n\
																\n\
																\n\
void main() {													\n\
	color = vec4(1.0, 0.0, 0.0, 1.0);							\n\
}																\n\
";

static void CreateTriangle()
{
	GLfloat vertices[9] = {
		-1.0f,  -1.0f,  0.0f,
		 1.0f,  -1.0f,  0.0f,
		 0.0f,   1.0f,  0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static void AddShader(GLuint program, const char* code, GLenum type)
{
	GLuint shader = glCreateShader(type);

	const GLchar* _code[1];
	_code[0] = code;

	GLint codeLength[1];
	codeLength[0] = strlen(code);

	glShaderSource(shader, 1, _code, codeLength);
	glCompileShader(shader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d program: '%s'\n", type, eLog);
		return;
	}

	glAttachShader(program, shader);
}

static void CompileShaders()
{
	shader = glCreateProgram();

	if (!shader)
	{
		printf("Error creating shader program!\n");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
}

int main()
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		printf("GLFW initialization failed!");
		return 1;
	}

	// Setup GLFW window properties.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // No Backward Compatibility.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Allow Forward Compatibility.

	// Create the window.
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get buffer size information.
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use.
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features.
	glewExperimental = GL_TRUE;

	// Initialize GLEW.
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup viewport size.
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	// Loop until window closed.
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get & Handle input events.
		glfwPollEvents();

		if (goingRight)
			tri_XOffset += triIncrement;
		else
			tri_XOffset -= triIncrement;

		if (abs(tri_XOffset) >= triMaxOffset) 
			goingRight = !goingRight;

		// Enable backface culling.
		glEnable(GL_CULL_FACE);

		// Clear window.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(tri_XOffset, 0.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}