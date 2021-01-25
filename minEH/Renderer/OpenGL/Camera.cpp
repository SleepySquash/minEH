//
//  Camera.cpp
//  minEH
//
//  Created by Никита Исаенко on 25.01.2021.
//

#include "Camera.hpp"

#ifdef MINEH_OPENGL

#include <glm/gtc/type_ptr.hpp>

namespace mh
{
    namespace GL
    {
        GLCamera::GLCamera(GL::Context* context) : context(context) { }
        void GLCamera::create()
        {
            cbo.view = glm::lookAt(position, position - direction, up);
            
            glGenBuffers(1, &CBO);
            glBindBuffer(GL_UNIFORM_BUFFER, CBO);
            glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferRange(GL_UNIFORM_BUFFER, 0, CBO, 0, 2 * sizeof(glm::mat4));
            
            dirty();
        }
        
        void GLCamera::destroy() { glDeleteBuffers(1, &CBO); }
        void GLCamera::resize()
        {
            cbo.proj = glm::perspective(fov, context->window->frame.width / (float)context->window->frame.height, near, far);
            glBindBuffer(GL_UNIFORM_BUFFER, CBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cbo.proj));
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        
        void GLCamera::update() { cbo.view = glm::lookAt(position, position - direction, up); dirty(); }
        void GLCamera::draw(const uint32_t& i) { }
        
        void GLCamera::dirty()
        {
            glBindBuffer(GL_UNIFORM_BUFFER, CBO);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cbo.view));
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        void GLCamera::setContext(void* context) { this->context = (GL::Context*)context; }
    }
}

#endif
