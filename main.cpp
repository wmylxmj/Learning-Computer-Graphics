#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <set>
#include "omp.h"

#include "fileio.h"
#include "camera.h"
#include "obb.h"
#include "bvh.h"

#define WINDOW_WIDTH_INIT 1920
#define WINDOW_HEIGHT_INIT 1080
#define MOUSE_SENSITIVITY 0.001f
#define CAMERA_MOVE_SPEED 1.5f

Camera camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
}

void KeyboardInputProcessing(GLFWwindow *glfwWindow) {
    if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(glfwWindow, true);
    }

    if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
        camera.eyePosition += deltaTime * CAMERA_MOVE_SPEED * camera.GetFrontDirection();
    if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
        camera.eyePosition -= deltaTime * CAMERA_MOVE_SPEED * camera.GetFrontDirection();
    if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
        camera.eyePosition -= deltaTime * CAMERA_MOVE_SPEED * camera.GetRightDirection();
    if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
        camera.eyePosition += deltaTime * CAMERA_MOVE_SPEED * camera.GetRightDirection();
    if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.eyePosition += deltaTime * CAMERA_MOVE_SPEED * camera.GetUpDirection();
    if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.eyePosition -= deltaTime * CAMERA_MOVE_SPEED * camera.GetUpDirection();
}

void MouseScrollCallback(GLFWwindow* window, double dx, double dy) {
    camera.fovy -= (float)(0.1 * dy);
    if (camera.fovy < glm::radians(15.0f)) {
        camera.fovy = glm::radians(15.0f);
    }
    if (camera.fovy > glm::radians(75.0f)) {
        camera.fovy = glm::radians(75.0f);
    }
}

void WindowResizeCallback(GLFWwindow *glfwWindow, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    Model model(R"(G:\LearnOpenGL-master\LearnOpenGL-master\resources\objects\backpack\backpack.obj)");
    OrientedBoundingBox* obbs = new OrientedBoundingBox[model.meshes.size()];
    std::vector<BVHNode>* bvhs = new std::vector<BVHNode>[model.meshes.size()];

    std::cout << sizeof(std::vector<BVHNode>) << std::endl;
    std::cout << "---------------------------" << std::endl;
    //std::vector<OrientedBoundingBox> obbs();
    //std::vector<std::vector<BVHNode>> bvhs;
    omp_set_num_threads(128);
#pragma omp parallel for
    for (int i = 0; i < model.meshes.size(); i++) {
        std::cout << i << std::endl;
        obbs[i] = GenerateOBB(model.meshes[i].vertices, model.meshes[i].triangles);
        bvhs[i] = buildBVH(model.meshes[i].vertices, model.meshes[i].triangles, 2);
    }
    std::cout << "12345678" << std::endl;
    /*
    Mesh mesh = model.meshes[56];
    std::vector<BVHNode> nodes = buildBVH(model.meshes[56].vertices, model.meshes[56].triangles);
    std::cout << nodes.size() << std::endl;
    */

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

    //ShaderProgram shader(R"(C:\Users\user\CLionProjects\CppCG\shader.vert)", R"(C:\Users\user\CLionProjects\CppCG\shader.frag)");






    int index_ud = 0;
    int index_lr = 0;

    while(!glfwWindowShouldClose(glfwWindow)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        KeyboardInputProcessing(glfwWindow);

        if (glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS) {
            index_lr = 0;
            index_ud ++;
            if (index_ud >= model.meshes.size()) {
                index_ud = (int)model.meshes.size() - 1;
            }
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
            index_lr = 0;
            index_ud --;
            if (index_ud < 0) {
                index_ud = 0;
            }
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            index_lr ++;
            if (index_lr >= bvhs[index_ud].size()) {
                index_lr = (int)bvhs[index_ud].size() - 1;
            }
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
            index_lr --;
            if (index_lr < 0) {
                index_lr = 0;
            }
        }



        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the loaded model
        glm::mat4 matModel = glm::mat4(1.0f);
        matModel = glm::translate(matModel, glm::vec3(0.0f, 0.0f, -5.0f));
        matModel = glm::scale(matModel, glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 transMat = camera.GetPerspectiveMatrix() * camera.GetCameraMatrix() * matModel;


        glUseProgram(0);
        glColor4ub(255, 255, 255, 255);
        Mesh mesh = model.meshes[index_ud];
        //for (Mesh mesh: model.meshes)
        {
            for (Triangle triangle: mesh.triangles) {
                glm::vec4 point0(transMat * glm::vec4(mesh.vertices[triangle.indices[0]].position, 1));
                glm::vec4 point1(transMat * glm::vec4(mesh.vertices[triangle.indices[1]].position, 1));
                glm::vec4 point2(transMat * glm::vec4(mesh.vertices[triangle.indices[2]].position, 1));


                glBegin(GL_LINE_LOOP);

                if (point0[3] > 0) {
                    glVertex2d(point0[0] / point0[3], point0[1] / point0[3]);
                }
                if (point1[3] > 0) {
                    glVertex2d(point1[0] / point1[3], point1[1] / point1[3]);
                }
                if (point2[3] > 0) {
                    glVertex2d(point2[0] / point2[3], point2[1] / point2[3]);
                }
                glEnd();



            }
        }
        glColor4ub(255, 255, 0, 255);
        //for (std::vector<BVHNode> nodes: bvhs)
        {
            //std::vector<BVHNode> nodes = bvhs[index];
            //for (BVHNode node: bvhs[index_ud])
            {
                //OrientedBoundingBox obb = node.obb;
                OrientedBoundingBox obb = bvhs[index_ud][index_lr].obb;



                //glUseProgram(shader.glID);
                glm::vec4 p[8];
                //glm::mat3 basis(obb.basis[0], obb.basis[1], obb.basis[2]);

                p[0] = transMat * glm::vec4(obb.center + obb.basis * glm::vec3(-0.5*obb.size.x, -0.5*obb.size.y, -0.5*obb.size.z), 1);
                p[1] = transMat * glm::vec4(obb.center + obb.basis * glm::vec3(+0.5*obb.size.x, -0.5*obb.size.y, -0.5*obb.size.z), 1);
                p[2] = transMat * glm::vec4(obb.center + obb.basis * glm::vec3(+0.5*obb.size.x, +0.5*obb.size.y, -0.5*obb.size.z), 1);
                p[3] = transMat * glm::vec4(obb.center + obb.basis * glm::vec3(-0.5*obb.size.x, +0.5*obb.size.y, -0.5*obb.size.z), 1);

                p[4] = transMat * glm::vec4(obb.center + obb.basis * glm::vec3(-0.5*obb.size.x, -0.5*obb.size.y, +0.5*obb.size.z), 1);
                p[5] = transMat * glm::vec4(obb.center + obb.basis * glm::vec3(+0.5*obb.size.x, -0.5*obb.size.y, +0.5*obb.size.z), 1);
                p[6] = transMat * glm::vec4(obb.center + obb.basis * glm::vec3(+0.5*obb.size.x, +0.5*obb.size.y, +0.5*obb.size.z), 1);
                p[7] = transMat * glm::vec4(obb.center + obb.basis * glm::vec3(-0.5*obb.size.x, +0.5*obb.size.y, +0.5*obb.size.z), 1);

                glBegin(GL_LINE_LOOP);
                for (int i = 0; i < 4; i++) {
                    glVertex2d(p[i][0]/p[i][3], p[i][1]/p[i][3]);
                }
                glEnd();
                glBegin(GL_LINE_LOOP);
                for (int i = 4; i < 8; i++) {
                    glVertex2d(p[i][0]/p[i][3], p[i][1]/p[i][3]);
                }
                glEnd();
                for (int i = 0; i < 4; i++) {
                    glBegin(GL_LINE_LOOP);
                    glVertex2d(p[i][0]/p[i][3], p[i][1]/p[i][3]);
                    glVertex2d(p[i+4][0]/p[i+4][3], p[i+4][1]/p[i+4][3]);
                    glEnd();
                }
            }
        }





        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}
