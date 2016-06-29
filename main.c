#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  600
#define WINDOW_TITLE   "Clouds"
#define WINDOW_COLOR   0, 0.597, .797, 1.0

#define WIREFRAME_MODE 0

GLfloat vertices[] = {
  -0.5,  0.5, 0.0, // top left
   0.5,  0.5, 0.0, // top right
  -0.5, -0.5, 0.0, // bottom left
   0.5, -0.5, 0.0, // bottom right
};

GLuint indices[] = {
  0, 1, 2,
  1, 2, 3,
};

const GLchar *vertexShaderSource = "#version 330 core\n\
\n\
layout (location = 0) in vec3 position;\n\
\n\
void main() {\n\
  gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
}";

const GLchar *fragmentShaderSource = "#version 330 core\n\
\n\
out vec4 color;\n\
\n\
void main() {\n\
  color = vec4(1.0, 0.5, 0.2, 1.0);\n\
}";

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    }
  }
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyCallback);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW\n");
    return -1;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  if (WIREFRAME_MODE)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // compile vertex shader
  GLuint vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  {
    // check if source compiled alright
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      fprintf(stderr, "error: vertex shader compilation failed\n%s", infoLog);
    }
  }

  // compile fragment shader
  GLuint fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  {
    // check if source compiled alright
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      fprintf(stderr, "error: fragment shader compilation failed\n%s", infoLog);
    }
  }

  // build shader program
  GLuint shaderProgram;
  shaderProgram = glCreateProgram();
  {
    // check if glCreateProgram worked
    if (!shaderProgram)
      fprintf(stderr, "error: glCreateProgram failed\n");
  }
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  {
    // check if linking worked
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
      fprintf(stderr, "error: shader program linking failed\n%s", infoLog);
    }
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // set up VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
    // create VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);
    // copy our vertices array into VBO for OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // hook up VBO to vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(0);
    // generate EBO
    GLuint EBO;
    glGenBuffers(1, &EBO);
    // copy our indices array into the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glBindVertexArray(0);

  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(WINDOW_COLOR);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
