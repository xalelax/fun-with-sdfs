#include "meshes.hpp"
#include "sdf.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 vertexPosition;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main(){
        gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    }
)";

const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main(){
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)";

int main() {
  int width{1200}, height{800};

  // Initialize GLFW
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  // Set OpenGL version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create window
  GLFWwindow *window = glfwCreateWindow(width, height, "Preview", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    std::cout << "Error initializing GLEW" << std::endl;
    return -1;
  }

  // Build and compile shaders
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // Create shader program
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Set up Mesh
  Mesh icosphere = createIcosphere(4).scale(3.);

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, icosphere.vertices.size() * sizeof(glm::vec3),
               icosphere.vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               icosphere.triangles.size() * 3 * sizeof(unsigned int),
               icosphere.triangles.data(), GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    // Calculate model matrix with rotation around Z-axis
    glm::mat4 model = glm::mat4(1.0f);
    unsigned int modelLocation = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

    // Calculate view and projection matrices
    float rotationAngle = 0.7 * glfwGetTime();
    float radius = 8.0f;
    glm::vec3 initialEye(0.0f, 2.0f, -radius);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotationAngle, up);
    glm::vec3 eye = glm::vec3(rotation * glm::vec4(initialEye, 1.0f));

    // Point the camera is looking at (origin)
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), up);
    unsigned int viewLocation = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
    unsigned int projectionLocation =
        glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    // Morph
    icosphere = morphTowardsBox(icosphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    icosphere.vertices.size() * sizeof(glm::vec3),
                    icosphere.vertices.data());
    // Render
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader program
    glUseProgram(shaderProgram);

    // Draw triangle
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, icosphere.triangles.size() * 3,
                   GL_UNSIGNED_INT, 0);

    // Swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up resources
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  // Terminate GLFW
  glfwTerminate();
  return 0;
}
