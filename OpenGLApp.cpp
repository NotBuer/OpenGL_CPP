#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Windows Dimensions.
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader;

// Vertex shader.
static const char* vShader = "                                              \n\
#version 330                                                                \n\
                                                                            \n\
layout(location = 0) in vec3 pos;                                           \n\
                                                                            \n\
void main() {                                                               \n\
      gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);                         \n\
}                                                                           \n\
";

// Fragment shader.
static const char* fShader = "                  \n\
#version 330                                    \n\
                                                \n\
out vec4 color;                                 \n\
                                                \n\
void main() {                                   \n\
      color = vec4(1.0, 0.0, 0.0, 1.0);         \n\
}                                               \n\
";

void CreateTriangle() {
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, // Left corner
        1.0f, -1.0f, 0.0f, // Right corner
        0.0f, 1.0f, 0.0f // Top corner
    };

    // Binding VAO.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Binding VBO.
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Unbind VBO.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind VAO.
    glBindVertexArray(0);
}

void AddShader(GLuint program, const char* shaderCode, GLenum type) {
    GLuint shader = glCreateShader(type);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(shader, 1, theCode, codeLength);
    glCompileShader(shader);

    GLint result = 0;
    GLchar log[1024] = { 0 };

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        printf("Error compiling the %d shader: '%s'\n", type, log);
        return;
    }

    glAttachShader(program, shader);
}

void CompileShaders() {
    shader = glCreateProgram();

    if (!shader) {
        printf("Error creating shader program!\n");
        return;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar log[1024] = { 0 };

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(log), NULL, log);
        printf("Error linking program: '%s'\n", log);
        return;
    }

    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(log), NULL, log);
        printf("Error validating program: '%s'\n", log);
        return;
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        printf("GLFW initialization failed!");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties.
    // OpenGL Version.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core profile = No backwards compatibility.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatibility.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the window.
    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL App", NULL, NULL);
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

    if (glewInit() != GLEW_OK) {
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
    while (!glfwWindowShouldClose(mainWindow)) {

        // Get and Handle input events.
        glfwPollEvents();

        // Clear window.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}