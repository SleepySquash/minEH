//
//  Drawable.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 21.01.2021.
//

#include "Drawable.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace mh
{
    void Drawable::record(const uint32_t& i) { }
    void Drawable::draw()   { }
    
    
    void Transform2D::dirty() { }
    
    void Transform2D::setPosition(const glm::vec2& pos) { position = pos; dirty(); }
    void Transform2D::setRotation(const float&     rot) { rotation = rot; dirty(); }
    void Transform2D::setScale   (const glm::vec2& scl) { scale    = scl; dirty(); }
    void Transform2D::setScale   (const float&       s) { scale    = glm::vec2(s, s); dirty(); }
    
    const glm::vec2& Transform2D::getPosition() { return position; }
    const float&     Transform2D::getRotation() { return rotation; }
    const glm::vec2& Transform2D::getScale()    { return scale;    }
    
    
    
    bool Font::open(const std::string& path) { return false; }
    void Font::close() { }
    void Font::setContext(void *context) { }
    
    
    void Text::create() { }
    void Text::resize() { }
    void Text::destroy() { }
    
    void Text::setContext(void* context) { }
    void Text::setFont(const std::string& path) { }
    
    
    void Transform3D::dirty() { }
    
    void Transform3D::setPosition(const glm::vec3& pos) { position = pos; dirty(); }
    void Transform3D::setRotation(const glm::vec3& rot) { rotation = rot; dirty(); }
    void Transform3D::setScale   (const glm::vec3& scl) { scale    = scl; dirty(); }
    
    const glm::vec3& Transform3D::getPosition() { return position; }
    const glm::vec3& Transform3D::getRotation() { return rotation; }
    const glm::vec3& Transform3D::getScale()    { return scale;    }
    
    
    void Mesh::create() { }
    void Mesh::resize() { }
    void Mesh::destroy() { }
    
    void Mesh::setContext(void* context) { }
    void Mesh::setTexture(const std::string& path) { }
    void Mesh::setModel(const std::string& path) { }
}
