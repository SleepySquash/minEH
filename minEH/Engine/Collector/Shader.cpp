//
//  Shader.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 17.01.2021.
//

#include "Shader.hpp"
#ifdef MINEH_OPENGL

namespace mh
{
    ShaderCollectorObject::ShaderCollectorObject(const char* vs, const char* fs, const char* gs) : shader(vs, fs, gs) { }
    namespace ShaderCollection
    {
        ShaderCollectorObject* sprite = nullptr;
        ShaderCollectorObject* text = nullptr;
        ShaderCollectorObject* mesh = nullptr;
    }
}

#endif
