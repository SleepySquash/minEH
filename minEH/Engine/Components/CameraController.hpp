//
//  CameraController.hpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#pragma once

#include "../Composition.hpp"
#include "../../Graphics/Camera.hpp"
#include "../../Window/Window.hpp"

namespace mh
{
    struct FreeCamera : Component
    {
        Camera* camera;
        Window* window;
        
        bool fps = false;
        bool moveA{ false }, moveD{ false }, moveW{ false }, moveS{ false }, moveUp{ false }, moveDown{ false };
        bool rotA{ false }, rotD{ false }, rotW{ false }, rotS{ false };

        bool mouseLockEnabled{ false }, firstMouse{ false };
        float lastxpos, lastypos, mouseMoveOffsetx, mouseMoveOffsety;

        float cameraSpeedSlow{ 0.8f }, cameraSpeedFast{ 4.f }, cameraSensitivity{ 0.002f };
        float cameraSpeed{ cameraSpeedSlow }, pitch{ 0 }, yaw{ glm::radians(90.f) };
        
        FreeCamera(Window* window = nullptr, Camera* camera = nullptr);
        void onDestroy() override;
        void onUpdate(const float& elapsed) override;
        void onEvent(Event& event) override;
        void onRecord(const uint32_t& i) override;
        
        void switchInputMode();
    };
}
