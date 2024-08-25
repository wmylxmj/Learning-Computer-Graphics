//
// Created by wmy on 2023/12/16.
//

#include "camera.h"

void Camera::RotateXY(float dThetaX, float dThetaY) {
    _gazeAngle.x += dThetaX;
    _gazeAngle.y += dThetaY;
    UpdateCoordinateSystem();
}

void Camera::UpdateCoordinateSystem() {
    glm::vec3 gazeDirection;
    gazeDirection.x = glm::sin(_gazeAngle.x) * glm::cos(_gazeAngle.y);
    gazeDirection.z = - glm::cos(_gazeAngle.x) * glm::cos(_gazeAngle.y);
    gazeDirection.y = glm::sin(_gazeAngle.y);
    _w = - gazeDirection;
    _v.x = glm::sin(_gazeAngle.x) * glm::cos(_gazeAngle.y + glm::radians(90.0f));
    _v.z = - glm::cos(_gazeAngle.x) * glm::cos(_gazeAngle.y + glm::radians(90.0f));
    _v.y = glm::sin(_gazeAngle.y + glm::radians(90.0f));
    _u = glm::cross(_v, _w);
}

glm::mat4 Camera::GetCameraMatrix() {
    return glm::lookAt(eyePosition, eyePosition + GetFrontDirection(), GetUpDirection());
}

glm::mat4 Camera::GetPerspectiveMatrix() {
    return glm::perspective(fovy, aspect, zNear, zFar);
}

glm::vec3 Camera::GetFrontDirection() {
    return - _w;
}

glm::vec3 Camera::GetUpDirection() {
    return _v;
}

glm::vec3 Camera::GetRightDirection() {
    return _u;
}