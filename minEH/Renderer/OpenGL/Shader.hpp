//
//  Shader.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 15.01.2021.
//

#pragma once

#include "../../Support/Config.hpp"
#ifdef MINEH_OPENGL

#include <iostream>
#include <sstream>
#include <fstream>

#include <glm/glm.hpp>

namespace mh
{
    struct ShaderProgram
    {
        unsigned int ID;
        
        ShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
        ~ShaderProgram();
        
        void use();
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setVec3(const std::string& name, const float& x, const float& y, const float& z) const;
        void setMat4(const std::string& name, const glm::mat4& value) const;
    };
}

#endif
