#include <iostream>
#include <vector>
#include <set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "omp.h"

#include "bvh.h"
#include "mesh.h"
#include "camera.h"
#include "shader.h"
#include "fileio.h"
#include "obb.h"
#include "sobol.h"

#define WINDOW_WIDTH_INIT 1024
#define WINDOW_HEIGHT_INIT 1024
#define MOUSE_SENSITIVITY 0.005f
#define CAMERA_MOVE_SPEED 2.5f

Camera camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int frameCount = 1;

void MouseMoveCallback(GLFWwindow* window, double x, double y) {
    static bool firstIn = true;
    static float lastX = WINDOW_WIDTH_INIT / 2.0;
    static float lastY = WINDOW_HEIGHT_INIT / 2.0;
    float xPos = static_cast<float>(x);
    float yPos = static_cast<float>(y);
    if (firstIn) {
        lastX = xPos;
        lastY = yPos;
        firstIn = false;
    }
    float dx = xPos - lastX;
    float dy = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.RotateXY(MOUSE_SENSITIVITY * dx, MOUSE_SENSITIVITY * dy);

    frameCount = 1;
}

void KeyboardInputProcessing(GLFWwindow *glfwWindow) {
    if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(glfwWindow, true);
    }

    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
        frameCount = 1;
        camera.eyePosition += deltaTime * CAMERA_MOVE_SPEED * camera.GetFrontDirection();
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
        frameCount = 1;
        camera.eyePosition -= deltaTime * CAMERA_MOVE_SPEED * camera.GetFrontDirection();
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
        frameCount = 1;
        camera.eyePosition -= deltaTime * CAMERA_MOVE_SPEED * camera.GetRightDirection();
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
        frameCount = 1;
        camera.eyePosition += deltaTime * CAMERA_MOVE_SPEED * camera.GetRightDirection();
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
        frameCount = 1;
        camera.eyePosition += deltaTime * CAMERA_MOVE_SPEED * camera.GetUpDirection();
    }
    if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        frameCount = 1;
        camera.eyePosition -= deltaTime * CAMERA_MOVE_SPEED * camera.GetUpDirection();
    }
}

void MouseScrollCallback(GLFWwindow* window, double dx, double dy) {
    camera.fovy -= (float)(0.1 * dy);
    if (camera.fovy < glm::radians(15.0f)) {
        camera.fovy = glm::radians(15.0f);
    }
    if (camera.fovy > glm::radians(75.0f)) {
        camera.fovy = glm::radians(75.0f);
    }
    frameCount = 1;
}

void WindowResizeCallback(GLFWwindow *glfwWindow, int width, int height) {
    glViewport(0, 0, width, height);
    frameCount = 1;
}

int main() {
    //std::vector<unsigned int> v = generate_v32(6, R"(E:\CppCG\new-joe-kuo-6.21201)");
    std::vector<unsigned int> v = generate_v32(4, 1, std::vector<unsigned int>{1, 1, 3, 3});
    std::cout << v.size() << std::endl;
    for (unsigned int i = 0; i < 32; i++) {
        std::cout << v[i] << std::endl;
    }

    std::vector<unsigned int> sobol_v;
    for (unsigned int d = 1; d <= 8; d++) {
        std::vector<unsigned int> direction_vector = generate_v32(d, R"(E:\CppCG\new-joe-kuo-6.21201)");
        sobol_v.insert(sobol_v.end(), direction_vector.begin(), direction_vector.end());
    }


    camera.aspect = WINDOW_WIDTH_INIT / WINDOW_HEIGHT_INIT;

    Model model(R"(E:\obj\bunny.obj)");
    // add a plane
    float xMin = FLT_MAX;
    float xMax = -FLT_MAX;
    float yMin = FLT_MAX;
    float yMax = -FLT_MAX;
    float zMin = FLT_MAX;
    float zMax = -FLT_MAX;
    for (const auto vertex: model.meshes[0].vertices) {
        if (vertex.position.x < xMin) xMin = vertex.position.x;
        if (vertex.position.x > xMax) xMax = vertex.position.x;
        if (vertex.position.y < yMin) yMin = vertex.position.y;
        if (vertex.position.y > yMax) yMax = vertex.position.y;
        if (vertex.position.z < zMin) zMin = vertex.position.z;
        if (vertex.position.z > zMax) zMax = vertex.position.z;
    }
    unsigned int start_index = model.meshes[0].vertices.size();
    model.meshes[0].vertices.push_back(Vertex{{xMax, yMin, zMax}});
    model.meshes[0].vertices.push_back(Vertex{{xMin, yMin, zMax}});
    model.meshes[0].vertices.push_back(Vertex{{xMin, yMin, zMin}});
    model.meshes[0].vertices.push_back(Vertex{{xMax, yMin, zMin}});

    model.meshes[0].triangles.push_back(Triangle{{start_index+0, start_index+1, start_index+2}});
    model.meshes[0].triangles.push_back(Triangle{{start_index+0, start_index+2, start_index+3}});



    OrientedBoundingBox* obbs = new OrientedBoundingBox[model.meshes.size()];
    std::vector<BVHNode>* bvhs = new std::vector<BVHNode>[model.meshes.size()];

    std::cout << "---------------------------" << std::endl;
    std::cout << "triangles: " << model.meshes[0].triangles.size() << std::endl;

    omp_set_num_threads(128);
#pragma omp parallel for
    for (int i = 0; i < model.meshes.size(); i++) {
        obbs[i] = GenerateOBB(model.meshes[i].vertices, model.meshes[i].triangles);
        bvhs[i] = buildBVH(model.meshes[i].vertices, model.meshes[i].triangles, 8);
    }
    std::cout << "********************************" << std::endl;


    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow *glfwWindow = glfwCreateWindow(WINDOW_WIDTH_INIT, WINDOW_HEIGHT_INIT, "window", nullptr, nullptr);
    if (!glfwWindow) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(glfwWindow);

    glfwSetFramebufferSizeCallback(glfwWindow, WindowResizeCallback);
    glfwSetCursorPosCallback(glfwWindow, MouseMoveCallback);
    glfwSetScrollCallback(glfwWindow, MouseScrollCallback);
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }

    const GLubyte* version = glGetString(GL_VERSION);
    printf("OpenGL Version: %s\n", version);

    ShaderProgram shader(R"(E:\CppCG\shader.vert)", R"(E:\CppCG\shader.frag)");
    ShaderProgram update_program(R"(E:\CppCG\shader.vert)", R"(E:\CppCG\update.frag)");
    ShaderProgram display_program(R"(E:\CppCG\shader.vert)", R"(E:\CppCG\display.frag)");

    std::vector<VertexStd430> vertices_std430 = Vertices2VerticesStd430(model.meshes[0].vertices);
    GLuint ssbo_vertices;
    glGenBuffers(1, &ssbo_vertices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, vertices_std430.size() * sizeof(VertexStd430), vertices_std430.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_vertices);

    std::vector<TriangleStd430> triangles_std430 = Triangles2TrianglesStd430(model.meshes[0].triangles);
    GLuint ssbo_triangles;
    glGenBuffers(1, &ssbo_triangles);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_triangles);
    glBufferData(GL_SHADER_STORAGE_BUFFER, model.meshes[0].triangles.size() * sizeof(TriangleStd430), model.meshes[0].triangles.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_triangles);

    std::vector<BVHNodeStd430> bvh_nodes_std430 = BVHNodes2BVHNodesStd430(bvhs[0]);
    GLuint ssbo_bvh_nodes;
    glGenBuffers(1, &ssbo_bvh_nodes);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_bvh_nodes);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bvh_nodes_std430.size() * sizeof(BVHNodeStd430), bvh_nodes_std430.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_bvh_nodes);

    GLuint ssbo_sobol_v;
    glGenBuffers(1, &ssbo_sobol_v);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_sobol_v);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sobol_v.size() * sizeof(unsigned int), sobol_v.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo_sobol_v);


    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    std::vector<glm::vec3> square = {
        glm::vec3(-1, -1, 0),
        glm::vec3(1, -1, 0),
        glm::vec3(-1, 1, 0),
        glm::vec3(1, 1, 0),
        glm::vec3(-1, 1, 0),
        glm::vec3(1, -1, 0)
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * square.size(), &square[0], GL_STATIC_DRAW);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);



    GLuint fbo_previousFrame;
    glGenFramebuffers(1, &fbo_previousFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_previousFrame);

    // 创建纹理来存储颜色缓冲
    GLuint previousFrameTexture;
    glGenTextures(1, &previousFrameTexture);
    glBindTexture(GL_TEXTURE_2D, previousFrameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH_INIT, WINDOW_HEIGHT_INIT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previousFrameTexture, 0);

    GLuint fbo_mixtureFrame;
    glGenFramebuffers(1, &fbo_mixtureFrame);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_mixtureFrame);
    // 创建纹理来存储颜色缓冲
    GLuint mixtureFrameTexture;
    glGenTextures(1, &mixtureFrameTexture);
    glBindTexture(GL_TEXTURE_2D, mixtureFrameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH_INIT, WINDOW_HEIGHT_INIT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mixtureFrameTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    frameCount = 1;

    camera.eyePosition = glm::vec3(0, 0, 1);
    while(!glfwWindowShouldClose(glfwWindow)) {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        KeyboardInputProcessing(glfwWindow);


        int windowWidth, windowHeight;
        glfwGetFramebufferSize(glfwWindow, &windowWidth, &windowHeight);


        glBindFramebuffer(GL_FRAMEBUFFER, fbo_mixtureFrame);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader.glID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, previousFrameTexture);
        shader.SetVec3("eye_position", camera.eyePosition);
        shader.SetVec2("viewport_size", glm::vec2(windowWidth, windowHeight));
        shader.SetVec3("camera_x_direction", camera.GetRightDirection());
        shader.SetVec3("camera_y_direction", camera.GetUpDirection());
        shader.SetVec3("camera_z_direction", - camera.GetFrontDirection());
        shader.SetFloat("fovy", camera.fovy);
        shader.SetUint("frameCount", frameCount++);
        shader.SetInt("previousFrame", 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo_previousFrame);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(update_program.glID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mixtureFrameTexture);
        display_program.SetInt("mixtureFrame", 0);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(display_program.glID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, previousFrameTexture);
        display_program.SetInt("frame", 0);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);




        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();

    }

    glfwTerminate();

    delete[] obbs;
    delete[] bvhs;

    return 0;
}

