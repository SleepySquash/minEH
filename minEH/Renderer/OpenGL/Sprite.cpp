//
//  Sprite.cpp
//  minEH
//
//  Created by Никита Исаенко on 23.01.2021.
//

#include "Sprite.hpp"

#ifdef MINEH_OPENGL
#include "../../Engine/Collector/Shader.hpp"
#include "../../Engine/Collector/Buffer.hpp"
#include "../../Engine/Collector/Texture.hpp"
#include "../../Support/ResourcePath.hpp"
#include "../Factory.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mh
{
    namespace GL
    {
        GLSprite::GLSprite() { }
        GLSprite::GLSprite(GL::Context* context) : context(context) { }
        
        void GLSprite::create()
        {
            VAO = (GL::Buffer*)Renders::Buffer(context); VAO->type = BufferType::Array; VAO->allocate();
            UVO = (GL::Buffer*)Renders::Buffer(context); UVO->type = BufferType::UV; UVO->allocate();
            VBO = (GL::Buffer*)bc::get("quad-v")->buffer;
            EBO = (GL::Buffer*)bc::get("quad-i")->buffer;
            
            glBindVertexArray(VAO->id);
            glBindBuffer(GL_ARRAY_BUFFER, VBO->id);
            glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)(0));
            setUV(glm::vec4(0, 0, 1, 1));
            glBindBuffer(GL_ARRAY_BUFFER, UVO->id);
            glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)(0));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO->id);
            glBindVertexArray(0);
            
            if (ShaderCollection::sprite) ++ShaderCollection::sprite->uses;
            else ShaderCollection::sprite = new ShaderCollectorObject("Shaders/OpenGL/sprite.vert", "Shaders/OpenGL/sprite.frag");
            ID = ShaderCollection::sprite->shader.ID;
            mloc = glGetUniformLocation(ID, "model");
            cloc = glGetUniformLocation(ID, "color");
        }
        
        void GLSprite::destroy()
        {
            if (texture.first) tc::erase(texture.second);
            bc::erase("sprite");
            if (ShaderCollection::sprite)
            {
                if (ShaderCollection::sprite->uses != 0) // TODO: shader collector
                    --ShaderCollection::sprite->uses;
            }
        }
        
        void GLSprite::resize()
        {
#ifdef MINEH_DEBUG
            if (!texture.first) return;
#endif
            float aspect = (float)context->window->frame.width/context->window->frame.height;
            float rwidth = texture.first->image.width, rheight = texture.first->image.height;
            if (aspect > 1.f) rwidth /= aspect; else rheight *= aspect;
            if (rwidth > rheight) { ycoef = ((float)rheight/rwidth); xcoef = 1.f; }
            else if (rwidth < rheight) { xcoef = ((float)rwidth/rheight); ycoef = 1.f; }
            dirty();
        }
        
        void GLSprite::draw()
        {
    #ifdef MINEH_DEBUG
            if (!texture.first) { std::cout << "Error :: Sprite :: Draw :: No texture is bound (nullptr) while attempting to draw.\n"; return; }
    #endif
            if (context->shaderID != ID) { glUseProgram(ID); glDisable(GL_DEPTH_TEST); context->shaderID = ID; }
            if (context->VAOID != VAO->id) { glBindVertexArray(VAO->id); context->VAOID = VAO->id; }
            if (mDirty)
            {
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));
                model = glm::scale(model, glm::vec3(scale.x * xcoef, scale.y * ycoef, 0.f));
                mDirty = false;
            }
            glUniformMatrix4fv(mloc, 1, false, glm::value_ptr(model));
            glUniform4f(cloc, color.r, color.g, color.b, color.a);
            glActiveTexture(GL_TEXTURE0);
            if (context->textureID != texture.first->id) {
                glBindTexture(GL_TEXTURE_2D, texture.first->id); context->textureID = texture.first->id; }
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        
        void GLSprite::dirty() { mDirty = true; }
        
        void GLSprite::setContext(void* context) { this->context = (GL::Context*)context; }
        void GLSprite::setTexture(const std::string& path) { texture.first = (GL::Texture*)tc::get(path)->texture; texture.second = path; }
        void GLSprite::setPosition(const glm::vec2& pos) { position = glm::vec2(pos.x, -pos.y); dirty(); }
        void GLSprite::setUV(const glm::vec4 &uv) {
            glBindBuffer(GL_ARRAY_BUFFER, UVO->id);
            float uvs[] = { 0, 0,  0, 1,  1, 0,  1, 1 };
            glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_VERTEX_SHADER_BIT);
        }
    }
}
#endif
