//
//  Camera.hpp
//  minEH
//
//  Created by Никита Исаенко on 25.01.2021.
//

#pragma once

#include "../../Support/Config.hpp"

#ifdef MINEH_OPENGL
#include "../Context.hpp"
#include "../../Graphics/Camera.hpp"

namespace mh
{
    namespace GL
    {
        struct GLCamera : Camera
        {
            Context* context;
            CameraBufferObject cbo;
            GLuint CBO, cameraLoc;
            bool mDirty = false;
            
            GLCamera(Context* context = nullptr);
            
            void create() override;
            void destroy() override;
            void resize() override; // recreate perspective matrix and do dirty()
            void update() override; // recreate view matrix and do dirty()
            void draw(const uint32_t& i) override; // if dirty then update buffers
            
            void dirty() override;
            
            void setContext(void* context) override;
        };
    }
}

#endif
