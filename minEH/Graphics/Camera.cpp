//
//  Camera.cpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#include "Camera.hpp"

namespace mh
{
    Camera::~Camera() { }
    void Camera::setPosition(const glm::vec3& pos) { position = pos; update(); }
    void Camera::setFOV(const float& fov) { this->fov = fov; resize(); }
}
