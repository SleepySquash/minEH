//
//  Shader.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#pragma once

#include "../../Support/Config.hpp"

#ifdef MINEH_OPENGL
#include "../../Renderer/OpenGL/Shader.hpp"

namespace mh
{
    struct ShaderCollectorObject
    {
        unsigned int uses = 0;
        ShaderProgram shader;
        
        ShaderCollectorObject(const char* vs, const char* fs, const char* gs = nullptr);
    };
    
    namespace ShaderCollection
    {
        extern ShaderCollectorObject* sprite;
        extern ShaderCollectorObject* mesh;
    }
    struct ShaderCollector
    {
        
    };
}

#endif
