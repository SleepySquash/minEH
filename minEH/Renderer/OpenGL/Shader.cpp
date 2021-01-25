//
//  Shader.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 15.01.2021.
//

#include "Shader.hpp"
#ifdef MINEH_OPENGL

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <glm/gtc/type_ptr.hpp>

#include "../../Support/ResourcePath.hpp"

namespace mh
{
    ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
    {
        std::string vCode, fCode, gCode;
        std::ifstream vFile(resourcePath() + vertexPath),
                      fFile(resourcePath() + fragmentPath);
        
        if (vFile.is_open() && fFile.is_open())
        {
            std::stringstream vStream, fStream, gStream;
            vStream << vFile.rdbuf();
            fStream << fFile.rdbuf();
            vCode = vStream.str();
            fCode = fStream.str();
            
            const char* _vCode = vCode.c_str();
            const char* _fCode = fCode.c_str();
            const char* _gCode;

            GLuint vertex, fragment, geometry = 0;
            int ec; char ecode[512];
            
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &_vCode, NULL);
            glCompileShader(vertex);
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &ec);
            if (!ec)
            {
                glGetShaderInfoLog(vertex, 512, NULL, ecode);
                std::cout << "glCompileShader(vertex) Error: " << ecode << "\n";
            }
            
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &_fCode, NULL);
            glCompileShader(fragment);
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &ec);
            if (!ec)
            {
                glGetShaderInfoLog(fragment, 512, NULL, ecode);
                std::cout << "glCompileShader(fragment) Error: " << ecode << "\n";
            }
            
            if (geometryPath)
            {
                std::ifstream gFile(resourcePath() + geometryPath);
                if (gFile.is_open())
                {
                    gStream << gFile.rdbuf();
                    gCode = gStream.str();
                    _gCode = gCode.c_str();
                    
                    geometry = glCreateShader(GL_GEOMETRY_SHADER);
                    glShaderSource(geometry, 1, &_gCode, NULL);
                    glCompileShader(geometry);
                    glGetShaderiv(geometry, GL_COMPILE_STATUS, &ec);
                    if (!ec)
                    {
                        glGetShaderInfoLog(geometry, 512, NULL, ecode);
                        std::cout << "glCompileShader(geometry) Error: " << ecode << "\n";
                    }
                }
                else std::cout << "ShaderProgram::CTOR: file does not exists\n";
            }
            
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            if (geometryPath) glAttachShader(ID, geometry);
            glLinkProgram(ID);
            glGetProgramiv(ID, GL_LINK_STATUS, &ec);
            if (!ec)
            {
                glGetProgramInfoLog(ID, 512, NULL, ecode);
                std::cout << "glLinkProgram() Error(s):\n" << ecode << "\n";
            }
            
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            if (geometryPath) glDeleteShader(geometry);
        }
        else std::cout << "ShaderProgram::CTOR: file does not exists\n";
    }
    ShaderProgram::~ShaderProgram() { glDeleteProgram(ID); }

    void ShaderProgram::use() { glUseProgram(ID); }

    void ShaderProgram::setBool(const std::string &name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
    void ShaderProgram::setInt(const std::string &name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
    void ShaderProgram::setFloat(const std::string &name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), (float)value); }
    void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z); }
    void ShaderProgram::setVec3(const std::string& name, const float& x, const float& y, const float& z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); }
    void ShaderProgram::setMat4(const std::string& name, const glm::mat4& value) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, false, glm::value_ptr(value)); }
}

#endif
