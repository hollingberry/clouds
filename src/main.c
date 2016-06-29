#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define WINDOW_COLOR   0, 0.597, .797, 1.0

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

void onKey(GLFWwindow *window, int key, int scancode, int action, int mode) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    }
  }
}

GLFWwindow *createWindow() {
  GLFWwindow *window;

  if (!glfwInit())
    return NULL;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  window = glfwCreateWindow(800, 600, "Clouds", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, onKey);

  return window;
}

void initGlew() {
  glewExperimental = GL_TRUE;
  if (!glewInit())
    fprintf(stderr, "failed to initialize glew\n");
}

void setViewport(GLFWwindow *window) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
}

GLuint compileShader(GLenum shaderType, const GLchar **source, const GLint *length) {
  GLuint shader;
  GLint success;
  GLchar infoLog[512];

  shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, source, length);
  glCompileShader(shader);
  
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr, "error: shader compilation vailed\n%s", infoLog);
    return -1;
  }

  return shader;
}


int main() {
  GLFWwindow *window = createWindow();
  initGlew();
  setViewport(window);

  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, &vertexShaderSource, NULL);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, &fragmentShaderSource, NULL);

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
