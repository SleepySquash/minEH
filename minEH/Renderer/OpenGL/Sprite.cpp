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
            GL::Buffer* buffer = (GL::Buffer*)bc::get("quad")->buffer;
            VAO = buffer->VAO;
            VBO = buffer->VBO;
            
            if (ShaderCollection::sprite) ++ShaderCollection::sprite->uses;
            else ShaderCollection::sprite = new ShaderCollectorObject("Shaders/sprite.vert", "Shaders/sprite.frag");
            ID = ShaderCollection::sprite->shader.ID;
            loc = glGetUniformLocation(ID, "model");
        }
        
        void GLSprite::destroy()
        {
            tc::erase(texture.second);
            bc::erase("quad");
            if (ShaderCollection::sprite)
            {
                if (ShaderCollection::sprite->uses != 0) // TODO: shader collector
                    --ShaderCollection::sprite->uses;
            }
        }
        
        void GLSprite::resize()
        {
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
            if (context->shaderID != ID) { glUseProgram(ID); context->shaderID = ID; }
            if (context->VAOID != VAO) { glBindVertexArray(VAO); context->VAOID = VAO; }
            if (context->VBOID != VBO) { glBindBuffer(GL_ARRAY_BUFFER, VBO); context->VBOID = VBO; }
            if (mDirty)
            {
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));
                model = glm::scale(model, glm::vec3(scale.x * xcoef, scale.y * ycoef, 0.f));
                mDirty = false;
            }
            glUniformMatrix4fv(loc, 1, false, glm::value_ptr(model));
            glActiveTexture(GL_TEXTURE0);
            if (context->textureID != texture.first->id) {
                glBindTexture(GL_TEXTURE_2D, texture.first->id); context->textureID = texture.first->id; }
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        void GLSprite::dirty() { mDirty = true; }
        
        void GLSprite::setContext(void* context) { this->context = (GL::Context*)context; }
        void GLSprite::setTexture(const std::string& path) { texture.first = (GL::Texture*)tc::get(path); }
    }
}
#endif
