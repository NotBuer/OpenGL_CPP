#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Windows Dimensions.
const GLint WIDTH = 800, HEIGHT = 600;

const float TO_RADIANS = 3.14159265f / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

bool goingRight = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.0005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex shader.
static const char* vShader = "                                  \n\
#version 330                                                    \n\
                                                                \n\
layout (location = 0) in vec3 pos;                              \n\
                                                                \n\
uniform mat4 model;                                             \n\
                                                                \n\
void main() {                                                   \n\
    gl_Position = model * vec4(pos, 1.0);       \n\
}                                                               \n\
";

// Fragment shader.
static const char* fShader = "                  \n\
#version 330                                    \n\
                                                \n\
out vec4 color;                                 \n\
                                                \n\
void main() {                                   \n\
    color = vec4(1.0, 0.0, 0.0, 1.0);           \n\
}                                               \n\
";

void CreateTriangle() {
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, // Left corner
        1.0f, -1.0f, 0.0f, // Right corner
        0.0f, 1.0f, 0.0f, // Top corner
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

    uniformModel = glGetUniformLocation(shader, "model");
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

        if (goingRight) {
            triOffset += triIncrement;
        }
        else {
            triOffset -= triIncrement;
        }

        if (abs(triOffset) >= triMaxoffset) {
            goingRight = !goingRight;
        }

        curAngle += 0.01f;
        if (curAngle >= 360.0f) {
            curAngle -= 360.0f;
        }

        if (goingRight) {
            curSize += 0.0001f;
        }
        else {
            curSize -= 0.0001f;
        }

        if (curSize >= maxSize || curSize <= minSize) {
            sizeDirection = !sizeDirection;
        }

        // Clear window.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader); 

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
        model = glm::rotate(model, curAngle * TO_RADIANS, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(curSize, 0.4f, 1.0f));

        glUniform1f(uniformModel, triOffset);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}