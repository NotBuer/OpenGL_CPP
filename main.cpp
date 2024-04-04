#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLint WIDTH = 800, HEIGHT = 600;

int main() {

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
	if (!mainWindow) {
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
	if (glewInit() != GLEW_OK) {
		printf("GLEW initialization failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup viewport size.
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Loop until window closed.
	while (!glfwWindowShouldClose(mainWindow)) 
	{
		// Get & Handle input events.
		glfwPollEvents();

		// Clear window.
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}