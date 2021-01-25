//
//  CameraController.cpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#include "CameraController.hpp"

namespace mh
{
    FreeCamera::FreeCamera(Window* window, Camera* camera) : window(window), camera(camera) { }
    void FreeCamera::onDestroy() { if (camera) { camera->destroy(); delete camera; } }
    void FreeCamera::onUpdate(const float& elapsed)
    {
        if (!camera) return;
        if (moveUp || moveDown || moveS || moveD || moveW || moveA || rotA || rotD || rotW || rotS)
        {
            if (moveUp)        camera->position.y += cameraSpeed * elapsed;
            else if (moveDown) camera->position.y -= cameraSpeed * elapsed;
            
            // glm::normalize(glm::vec3( camera->direction.x, 0.f, camera->direction.z ))     <- on xz
            if (fps)
            {
                if (moveW)      camera->position -= glm::normalize(glm::vec3( camera->direction.x, 0.f, camera->direction.z )) * cameraSpeed * elapsed;
                else if (moveS) camera->position += glm::normalize(glm::vec3( camera->direction.x, 0.f, camera->direction.z )) * cameraSpeed * elapsed;
                if (moveA)      camera->position -= glm::normalize(glm::cross(camera->up, glm::normalize(glm::vec3( camera->direction.x, 0.f, camera->direction.z )))) * cameraSpeed * elapsed;
                else if (moveD) camera->position += glm::normalize(glm::cross(camera->up, glm::normalize(glm::vec3( camera->direction.x, 0.f, camera->direction.z )))) * cameraSpeed * elapsed;
            }
            else
            {
                if (moveW)      camera->position -= camera->direction * cameraSpeed * elapsed;
                else if (moveS) camera->position += camera->direction * cameraSpeed * elapsed;
                if (moveA)      camera->position -= glm::normalize(glm::cross(camera->up, camera->direction)) * cameraSpeed * elapsed;
                else if (moveD) camera->position += glm::normalize(glm::cross(camera->up, camera->direction)) * cameraSpeed * elapsed;
            }
            
            if (rotA || rotD || rotW || rotS)
            {
                if (rotA)      yaw -= cameraSensitivity * 500 * elapsed;
                else if (rotD) yaw += cameraSensitivity * 500 * elapsed;
                if (rotW)      pitch -= cameraSensitivity * 500 * elapsed;
                else if (rotS) pitch += cameraSensitivity * 500 * elapsed;
                
                camera->direction.x = cosf(yaw) * cosf(pitch);
                camera->direction.y = sinf(pitch);
                camera->direction.z = sinf(yaw) * cosf(pitch);
                camera->direction = glm::normalize(camera->direction);
            }
            
            camera->update();
        }
    }
    
    void FreeCamera::onEvent(Event& event)
    {
        if (!camera) return;
        switch (event.type)
        {
            case Event::Type::Resized: camera->resize(); break;
            case Event::Type::KeyPressed:
            case Event::Type::keyReleased:
                switch (event.data.key)
                {
                    case Keyboard::Key::A: moveA = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::D: moveD = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::W: moveW = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::S: moveS = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::Q: moveDown = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::E: moveUp = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::Up: rotW = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::Down: rotS = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::Right: rotD = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::Left: rotA = (event.type == Event::Type::KeyPressed); break;
                    case Keyboard::Key::LShift: cameraSpeed = (event.type == Event::Type::KeyPressed) ? cameraSpeedFast : cameraSpeedSlow; break;
                    default: break;
                }
                break;
            case Event::Type::MousePressed: if (!mouseLockEnabled) switchInputMode(); break;
            case Event::Type::MouseMove:
                if (!mouseLockEnabled) return;
                mouseMoveOffsetx = event.data.mouse.move.x;
                mouseMoveOffsety = event.data.mouse.move.y;
                mouseMoveOffsetx *= cameraSensitivity;
                mouseMoveOffsety *= cameraSensitivity;

                yaw   += mouseMoveOffsetx;
                pitch += mouseMoveOffsety;

                if (pitch > 1.57f) pitch = 1.57f;
                else if (pitch < -1.57f) pitch = -1.57f;

                camera->direction.x = cosf(yaw) * cosf(pitch);
                camera->direction.y = sinf(pitch);
                camera->direction.z = sinf(yaw) * cosf(pitch);
                camera->direction = glm::normalize(camera->direction);
                
                camera->update();
                break;
            default: break;
        }
        switch (event.type)
        {
            case Event::Type::KeyPressed:
                switch (event.data.key)
                {
                    case Keyboard::Key::Escape: switchInputMode(); break;
                    default: break;
                }
                break;
            default: break;
        }
    }
    void FreeCamera::onRecord(const uint32_t &i) { camera->draw(i); }
    
    void FreeCamera::switchInputMode()
    {
        if (!window) return;
        if (mouseLockEnabled)
        {
            mouseLockEnabled = false; firstMouse = true;
            window->setCursorMode(Mouse::CursorMode::Normal);
        }
        else
        {
            mouseLockEnabled = firstMouse = true;
            window->setCursorMode(Mouse::CursorMode::Disabled);
        }
    }
}
