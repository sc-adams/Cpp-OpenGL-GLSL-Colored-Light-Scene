#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "filesystem.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include <iostream>
#include "pen_accent.h"
#include "pen_body.h"
#include "pen_clip.h"
#include "pen_point.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void GetDesktopResolution(float& horizontal, float& vertical);
// settings

float SCR_WIDTH = 800;
float SCR_HEIGHT = 600;
bool redLight = false;
bool blueLight = false;
bool noLight = true;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    GetDesktopResolution(SCR_WIDTH, SCR_HEIGHT);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Classroom Scene", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    Shader lightingShader("specular.vs", "specular.fs");
    Shader sShader("glsl.vs", "green.fs");



    float lightvertices[] = {
        // positions         
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,   // this is a modufied cube z axis is my x axis and it is modified to be less tall
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,  // left
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, // right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // back
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  // right
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  // top
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  //bottom
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float groundVertices[] = {
        // positions          // normals           // texture coords

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  // not
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float blackboard[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.01f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.01f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.01f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.01f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,  // left
        -0.5f,  0.5f, -0.01f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.01f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, // right
         0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.01f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.01f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // back
        -0.5f, -0.5f, -0.01f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.01f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.01f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.01f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  // right
         0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.01f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.01f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  // top
        -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.01f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.01f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.01f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  //bottom
         0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.01f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };


    float compassVertices[] = {
         0.49f, -0.5f, .5f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // back rectangle
         0.5f, -0.5f, .5f,     0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         0.49f, 0.5f, .5f,     0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
         0.5f, 0.5f, .5f,      0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
         0.5f, -0.5f, .5f,     0.0f,  0.0f, -1.0f,    0.0f, 1.0f,

         0.49f, 0.5f, .5f,     0.0f,  0.0f,  1.0f,      1.0f, 0.0f,  // bottom
         0.0f, 0.0f, 1.0f,      0.0f,  0.0f,  1.0f,     0.5f, 0.5f,
         0.49f, -0.5f, .5f,    0.0f,  0.0f,  1.0f,      0.0f, 0.0f,

         0.49f, 0.5f, 0.5f,     0.0f,  0.0f,  1.0f,     1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,      0.0f,  0.0f,  1.0f,      0.5f, 0.5f,
         0.49f, 0.5f, .5f,     -1.0f,  0.0f,  0.0f,       0.0f, 0.0f,

         0.5f, 0.5f, .5f,       -1.0f,  0.0f,  0.0f,      1.0f, 0.0f,     // modified pyramid to shift .1 down (y axis modified after camera transformations)
         0.0f, 0.0f, 1.0f,      -1.0f,  0.0f,  0.0f,      0.5f, 0.5f,
         0.5f, 0.5f, .5f,       -1.0f,  0.0f,  0.0f,      0.0f, 0.0f,
         0.5f, -0.5f, .5f,      -1.0f,  0.0f,  0.0f,      1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,        1.0f,  0.0f,  0.0f,     0.5f, 0.5f,
         0.5f, -0.5f, .5f,       1.0f,  0.0f,  0.0f,      0.0f, 0.0f,
         0.49f, -0.5f, .5f,      1.0f,  0.0f,  0.0f,      1.0f, 0.0f,
    };




    float roof2Vertices[] = {

         0.4f, -0.5f, .5f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f,
         0.5f, -0.5f, .5f,     0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         0.4f, 0.5f, .5f,     0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
         0.5f, 0.5f, .5f,      0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
         0.5f, -0.5f, .5f,     0.0f,  0.0f, -1.0f,    0.0f, 1.0f,

         0.4f, 0.5f, .5f,     0.0f,  0.0f,  1.0f,      1.0f, 0.0f,    // this is modified pyramid that is shorter on z axis (appears as y axis)
         0.0f, 0.0f, 1.0f,      0.0f,  0.0f,  1.0f,     0.5f, 0.5f,   // also it is modified to be shifted up on the z (y) axis to sit on top of the cube
         0.4f, -0.5f, .5f,    0.0f,  0.0f,  1.0f,      0.0f, 0.0f,

         0.4f, 0.5f, 0.5f,     0.0f,  0.0f,  1.0f,     1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,      0.0f,  0.0f,  1.0f,      0.5f, 0.5f,
         0.4f, 0.5f, .5f,     -1.0f,  0.0f,  0.0f,       0.0f, 0.0f,

         0.5f, 0.5f, .5f,       -1.0f,  0.0f,  0.0f,      1.0f, 0.0f,     // modified pyramid to shift .1 down (y axis modified after camera transformations)
         0.0f, 0.0f, 1.0f,      -1.0f,  0.0f,  0.0f,      0.5f, 0.5f,

         0.5f, 0.5f, .5f,       -1.0f,  0.0f,  0.0f,      0.0f, 0.0f,
         0.5f, -0.5f, .5f,      -1.0f,  0.0f,  0.0f,      1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,        1.0f,  0.0f,  0.0f,     0.5f, 0.5f,
         0.5f, -0.5f, .5f,       1.0f,  0.0f,  0.0f,      0.0f, 0.0f,
         0.4f, -0.5f, .5f,      1.0f,  0.0f,  0.0f,      1.0f, 0.0f,
    };


    glm::vec3 pointLightPositions[] = {
        glm::vec3(-3.0f, 3.0f, -3.0f),
        glm::vec3(3.0f, 3.0f, 3.0f),
        glm::vec3(3.0f, 3.0f, -3.0f),
        glm::vec3(-3.0f, 3.0f, 3.0f),
        glm::vec3(0.0f, 3.0f, 0.0f),
        glm::vec3(-3.0f, 3.0f, -3.0f),
        glm::vec3(0.0f, 3.0f, -2.0f),
        glm::vec3(-2.0f, 3.0f, -2.0f),

    };

    unsigned int redTexture = loadTexture("resources/textures/class/red.jpg");
    unsigned int blueTexture = loadTexture("resources/textures/class/blue.jpg");
    unsigned int lightgreyTexture = loadTexture("resources/textures/class/lightgrey.jpg");
    unsigned int compassTexture = loadTexture("resources/textures/class/protractor2.jpg");
    unsigned int skyboxTexture = loadTexture("resources/textures/class/wall.jpg");
    unsigned int blackboardTexture = loadTexture("resources/textures/class/blackboard.jpg");
    unsigned int groundTexture = loadTexture("resources/textures/class/AdobeStock_321846439.png");
    unsigned int greyTexture = loadTexture("resources/textures/class/grey.jpg");
    unsigned int purpleTexture = loadTexture("resources/textures/class/purple.jpg");
    unsigned int metalTexture = loadTexture("resources/textures/class/damkier.png");
    unsigned int deskTexture = loadTexture("resources/textures/class/AdobeStock_372442505.png");
    unsigned int ballpointTexture = loadTexture("resources/textures/class/AdobeStock_372442505.png");

    unsigned int blackboardVBO, blackboardVAO;
    glGenVertexArrays(1, &blackboardVAO);
    glGenBuffers(1, &blackboardVBO);
    glBindBuffer(GL_ARRAY_BUFFER, blackboardVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blackboard), blackboard, GL_STATIC_DRAW);
    glBindVertexArray(blackboardVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int compassVBO, compassVAO;
    glGenVertexArrays(1, &compassVAO);
    glGenBuffers(1, &compassVBO);
    glBindBuffer(GL_ARRAY_BUFFER, compassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(compassVertices), compassVertices, GL_STATIC_DRAW);
    glBindVertexArray(compassVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int bookVBO, bookVAO;
    glGenVertexArrays(1, &bookVAO);
    glGenBuffers(1, &bookVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bookVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(bookVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int skyboxVBO, skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(skyboxVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int groundVBO, groundVAO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glBindVertexArray(groundVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // light boxes

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightvertices), &lightvertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    unsigned int uniformBlockIndexPink = glGetUniformBlockIndex(sShader.ID, "Matrices");
    unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(sShader.ID, "Matrices");
    unsigned int uniformBlockIndexPurple = glGetUniformBlockIndex(sShader.ID, "Matrices");
    glUniformBlockBinding(sShader.ID, uniformBlockIndexPink, 0);
    glUniformBlockBinding(sShader.ID, uniformBlockIndexGreen, 0);
    glUniformBlockBinding(sShader.ID, uniformBlockIndexPurple, 0);
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
    glm::mat4 projection = glm::perspective(95.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        // view/projection 
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        float angle = 0.0;
        float ambient[] = { 0.5f, 0.5f, 0.5f, 1 };
        float diffuse[] = { 0.8f, 0.8f, 0.8f, 1 };
        float specular[] = { 1.0f, 1.0f, 1.0f, 1 };
        float shininess = 128;
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
        glBindVertexArray(compassVAO);
        model = glm::mat4(1.0f);

        model = glm::rotate(model, glm::radians(240.0f), glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::translate(model, glm::vec3(0.14f, -0.2f, .467f)); //(forward .14, left .2, up..467)
        model = glm::scale(model, glm::vec3(1.1f)); // Make it a smaller cube
        model = glm::scale(model, glm::vec3(.75f, .75f, .25f));
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, compassTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
      


        lightingShader.setInt("spriteColor", 3);
        int vertexColorLocation = glGetUniformLocation(lightingShader.ID, "fragColor");

        angle = 0.0;
        if (redLight)
        {
            double  timeValue = glfwGetTime();
            float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
            float blueValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
            float redValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);


            glm::vec4 fragColor = glm::vec4(redValue, 0.0f, blueValue, 1.0f);
            lightingShader.setVec3("spriteColor", fragColor);
        }
        if (blueLight)
        {
            double timeValue = glfwGetTime();
            float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
            float blueValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
            float redValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);


            glm::vec4 fragColor = glm::vec4(0.0f, 0.0f, blueValue, 1.0f);
            lightingShader.setVec3("spriteColor", fragColor);
        }
        if (noLight)
        
        {
            glm::vec4 fragColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            lightingShader.setVec3("spriteColor", fragColor);
        }
        

        glBindVertexArray(blackboardVAO);
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(3.01f, 0.01f, -11.01f));
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, -3.67f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, blackboardTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        glBindVertexArray(bookVAO);
        for (unsigned int i = 0; i < 10; i++)  // desk
        {
            glm::mat4 model = glm::mat4(1.0f);
            if (i == 1)
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(1.8f, 1.2f, 1.5f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, deskTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (i == 2)
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
                model = glm::translate(model, glm::vec3(0.0f, .62f, 0.05f));
                model = glm::scale(model, glm::vec3(.4f, .035f, .6f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, redTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (i == 3)
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, -1.1f, 0.01f));
                model = glm::translate(model, glm::vec3(0.0f, .695f, 0.03f));
                model = glm::scale(model, glm::vec3(.4f, .035f, .4f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, blueTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (i == 4)
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
                model = glm::translate(model, glm::vec3(-.5f, .64f, 0.0285f));
                model = glm::scale(model, glm::vec3(.4f, .07f, .5f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, lightgreyTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (i == 5)
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
                model = glm::translate(model, glm::vec3(-.5f, .605f, 0.03f));
                model = glm::scale(model, glm::vec3(.41f, .01f, .51f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, greyTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (i == 6)
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
                model = glm::translate(model, glm::vec3(-.50f, .678f, 0.03f));
                model = glm::scale(model, glm::vec3(.41f, .01f, .51f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, greyTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (i == 7)
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
                model = glm::translate(model, glm::vec3(-.3f, .639f, 0.03f));
                model = glm::scale(model, glm::vec3(.01f, .07f, .51f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, greyTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (i == 8)
            {
                angle = 7.0f;
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.00f, -.08f, 0.00f));
                model = glm::translate(model, glm::vec3(-.50f, .7f, 0.05));
                model = glm::scale(model, glm::vec3(.31f, .03f, .41f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, greyTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if (i == 9)
            {
                angle = -9;
                glm::mat4 model = glm::mat4(1.0f);

                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.00f, 1.00f, 0.0f));
                model = glm::translate(model, glm::vec3(0.0f, .66f, 0.03f));
                model = glm::scale(model, glm::vec3(.4f, .035f, .6f));
                lightingShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, purpleTexture);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        glBindVertexArray(0);
       
        angle = 0.0f;
        glBindVertexArray(groundVAO);
        for (unsigned int i = 0; i < 1; i++)
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.001f, 0.001f, 0.0001f));
        model = glm::translate(model, glm::vec3(0.0f, -4.1f, -0.3f));
        model = glm::scale(model, glm::vec3(7.0f));
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metalTexture);
        // keep this texture bound for the next 3 objects
        model = glm::mat4(0.5f);
        lightingShader.setMat4("model", model);
        model = glm::mat4(0.5f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 2.0f));
        PenBody penBody;
        penBody.Draw();

        model = glm::mat4(0.5f);
        lightingShader.setMat4("model", model);
        model = glm::mat4(0.5f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 2.0f));
        PenClip penClip;
        penClip.Draw();

        model = glm::mat4(0.5f);
        model = glm::mat4(0.5f);
        lightingShader.setMat4("model", model);
        model = glm::mat4(0.5f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 2.0f));
        PenAccent sphereAccent;
        sphereAccent.Draw();


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ballpointTexture);
        model = glm::mat4(0.5f);
        lightingShader.setMat4("model", model);
        model = glm::mat4(0.5f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 2.0f));
        PenPoint penPoint;
        penPoint.Draw();

        glBindVertexArray(skyboxVAO);
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.2f));
        model = glm::scale(model, glm::vec3(7.0f));
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 6; i++)
        {
            if ((i == 2) || (i == 1))
            {
                sShader.use();
                sShader.setMat4("projection", projection);
                sShader.setMat4("view", view);
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.4f));
                sShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            if ((i == 3) || (i == 4) || (i == 5))
            {
                sShader.use();
                sShader.setMat4("projection", projection);
                sShader.setMat4("view", view);
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.4f));
                sShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            else
            {
                sShader.use();
                sShader.setMat4("projection", projection);
                sShader.setMat4("view", view);
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.4f));
                sShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    PenBody penbody;
    penbody.~PenBody();
    PenClip penClip;
    penClip.~PenClip();
    PenAccent penAccent;
    penAccent.~PenAccent();
    PenPoint penPoint;
    penPoint.~PenPoint();

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &bookVAO);
    glDeleteBuffers(1, &bookVBO);
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteVertexArrays(1, &blackboardVAO);
    glDeleteBuffers(1, &blackboardVBO);
    glDeleteVertexArrays(1, &compassVAO);
    glDeleteBuffers(1, &compassVBO);

    glDeleteTextures(1, &blueTexture);
    glDeleteTextures(1, &redTexture);
    glDeleteTextures(1, &lightgreyTexture);
    glDeleteTextures(1, &compassTexture);
    glDeleteTextures(1, &skyboxTexture);
    glDeleteTextures(1, &blackboardTexture);
    glDeleteTextures(1, &groundTexture);
    glDeleteTextures(1, &greyTexture);
    glDeleteTextures(1, &purpleTexture);
    glDeleteTextures(1, &metalTexture);
    glDeleteTextures(1, &deskTexture);
    glDeleteTextures(1, &ballpointTexture);


    glDeleteShader(lightingShader.ID);
    glDeleteShader(sShader.ID);



    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        redLight = true;
        blueLight = false;
        noLight = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        blueLight = true;
        redLight = false;
        noLight = false;
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        blueLight = false;
        redLight = false;
        noLight = true;
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
void GetDesktopResolution(float& horizontal, float& vertical)
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
}