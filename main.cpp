#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <set>

#include "fileio.h"
#include "camera.h"
#include "obb.h"

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

class VertexLess{
public:
    bool operator() (const Vertex& vertexLeft, const Vertex& vertexRight) const {
        if (vertexLeft.position.x != vertexRight.position.x) {
            return vertexLeft.position.x < vertexRight.position.x;
        }
        if (vertexLeft.position.y != vertexRight.position.y) {
            return vertexLeft.position.y < vertexRight.position.y;
        }
        return vertexLeft.position.z < vertexRight.position.z;
    }
};
int main() {

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

    Model model(R"(G:\LearnOpenGL-master\LearnOpenGL-master\resources\objects\backpack\backpack.obj)");
    std::cout << model.meshes[0].triangles.size() << std::endl;

    std::vector<OrientedBoundingBox> obbs;
    for (Mesh mesh: model.meshes) {
        obbs.push_back(GenerateOBB(mesh.vertices, mesh.triangles));
    }


    int index = 56;

    while(!glfwWindowShouldClose(glfwWindow)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        KeyboardInputProcessing(glfwWindow);

        if (glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS) {
            index ++;
            if (index >= model.meshes.size()) {
                index = (int)model.meshes.size() - 1;
            }
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
            index --;
            if (index < 0) {
                index = 0;
            }
        }
        


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the loaded model
        glm::mat4 matModel = glm::mat4(1.0f);
        matModel = glm::translate(matModel, glm::vec3(0.0f, 0.0f, 0.0f));
        matModel = glm::scale(matModel, glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 transMat = camera.GetPerspectiveMatrix() * camera.GetCameraMatrix() * matModel;

        Mesh mesh = model.meshes[index];
        OrientedBoundingBox obb = obbs[index];
        std::set<Vertex, VertexLess> vertices;
        //std::cout << index << std::endl;
        /*
        for (Triangle triangle: mesh.triangles) {
            for (Vertex vertex: triangle.vertices) {
                if (vertices.find(vertex) == vertices.end()) {
                    vertices.insert(vertex);
                }
            }
        }
        glBegin(GL_POINTS);
        for (Vertex vertex: vertices) {
            glm::vec4 point(transMat * glm::vec4(vertex.position, 1));
            glVertex2d(point[0]/point[3], point[1]/point[3]);
        }
        glEnd();
        */

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

                /*
                glBegin(GL_LINE_LOOP);

                if (point0[2] < 0 && c + point0[2] > 0) {
                    glVertex2d(point0[0]*c / -point0[2], point0[1]*c / -point0[2]);
                }
                if (point1[2] < 0 && c + point1[2] > 0) {
                    glVertex2d(point1[0]*c / -point1[2], point1[1]*c / -point1[2]);
                }
                if (point2[2] < 0 && c + point2[2] > 0) {
                    glVertex2d(point2[0]*c / -point2[2], point2[1]*c / -point2[2]);
                }
                glEnd();
                */

            }
        }


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


        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}
