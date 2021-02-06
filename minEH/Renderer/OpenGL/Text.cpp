//
//  Text.cpp
//  minEH
//
//  Created by Никита Исаенко on 02.02.2021.
//

#include "Text.hpp"
#ifdef MINEH_OPENGL

#include <iostream>
using std::cout;
using std::endl;

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Context.hpp"
#include "../../Support/ResourcePath.hpp"
#include "../../Engine/Collector/Shader.hpp"
#include "../../Engine/Collector/Buffer.hpp"
#include "../../Engine/Collector/Font.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mh
{
    namespace GL
    {
#pragma mark -
#pragma mark Font
        
        GLFont::GLFont() { }
        GLFont::GLFont(Context* context) : context(context) { }
        
        bool GLFont::open(const std::string& path)
        {
            close();
            
            FT_Library ft;
            if (FT_Init_FreeType(&ft)) { cout << "Error :: FreeType :: Could not init FreeType Library.\n"; return false; }
            
            FT_Face face;
            if (FT_New_Face(ft, (resourcePath() + path).c_str(), 0, &face)) {
                cout << "Error :: FreeType :: Failed to load font \"" << path << "\".\n"; return false; }
            FT_Set_Pixel_Sizes(face, 0, 48);
            
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
              
            for (unsigned char c = 0; c < 128; ++c)
            {
                // load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                    std::cout << "Error :: FreeType :: Failed to load Glyph '" << c << "' (" << (int)c << ").\n"; continue; }
                // generate texture
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                GLCharacter character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    face->glyph->advance.x
                };
                characters.insert(std::pair<char, GLCharacter>(c, character));
            }
            
            FT_Done_Face(face);
            FT_Done_FreeType(ft);
            return true;
        }
        
        void GLFont::close()
        {
            for (auto& c : characters)
                glDeleteTextures(1, &c.second.textureID);
            characters.clear();
        }
        
        void GLFont::setContext(void* context) { this->context = (Context*)context; }
        
#pragma mark -
#pragma mark Text
        
        GLText::GLText() { }
        GLText::GLText(Context* context) : context(context) { }
        
        void GLText::create()
        {
            GL::Buffer* buffer = (GL::Buffer*)bc::get("text")->buffer;
            VAO = buffer->VAO; VBO = buffer->VBO;
            
            if (ShaderCollection::text) ++ShaderCollection::text->uses;
            else ShaderCollection::text = new ShaderCollectorObject("Shaders/OpenGL/text.vert", "Shaders/OpenGL/text.frag");
            ID = ShaderCollection::text->shader.ID;
            mloc = glGetUniformLocation(ID, "model");
            cloc = glGetUniformLocation(ID, "color");
        }
        void GLText::destroy()
        {
            if (font.first) fc::erase(font.second);
            bc::erase("text");
            if (ShaderCollection::text)
            {
                if (ShaderCollection::text->uses != 0) // TODO: shader collector
                    --ShaderCollection::text->uses;
            }
        }
        void GLText::resize()
        {
            float width = 0, height = 0;
            for (auto c = string.begin(); c != string.end(); ++c)
            {
                GLCharacter ch = font.first->characters[*c];
                width += (ch.advance >> 6);
                if (ch.size.y > height) height = ch.size.y;
            }
            
            xscale = 2*scale.x/width;
            yscale = 2*scale.y/width * context->window->aspect;
            
            bounds.x = position.x;
            bounds.y = position.y;
            bounds.z = 2*scale.x;
            bounds.w = height * yscale;
            if (bounds.w > scale.y)
            {
                yscale =  scale.y/height;
                xscale = (scale.x/height) / context->window->aspect;
                
                bounds.w = scale.x;
                bounds.z = width * xscale;
            }
            
            switch (halign)
            {
                default: xbegin = bounds.x - bounds.z/2; break;
                case Align::Left: xbegin = bounds.x; break;
                case Align::Right: xbegin = bounds.x - bounds.z; break;
            }
            switch (valign)
            {
                default: ybegin = bounds.y - bounds.w/2; break;
                case Align::Top: ybegin = bounds.y - bounds.w; break;
                case Align::Bottom: ybegin = bounds.y; break;
            }
        }
        
        void GLText::draw()
        {
            if (context->shaderID != ID) { glUseProgram(ID); glDisable(GL_DEPTH_TEST); context->shaderID = ID; }
            if (context->VAOID != VAO) { glBindVertexArray(VAO); context->VAOID = VAO; }
            if (context->VBOID != VBO) { glBindBuffer(GL_ARRAY_BUFFER, VBO); context->VBOID = VBO; }
            context->textureID = 0;
            
            glUniform4f(cloc, color.r, color.g, color.b, color.a);
            glActiveTexture(GL_TEXTURE0);
            
            float x = xbegin, y = ybegin;
            for (auto c = string.begin(); c != string.end(); ++c)
            {
                GLCharacter ch = font.first->characters[*c];

                float xpos = x + ch.bearing.x * xscale;
                float ypos = y - (ch.size.y - ch.bearing.y) * yscale;

                float w = ch.size.x * xscale;
                float h = ch.size.y * yscale;
                
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(xpos, ypos, 0.f));
                model = glm::scale(model, glm::vec3(w, h, 0.f));
                glUniformMatrix4fv(mloc, 1, false, glm::value_ptr(model));
                
                glBindTexture(GL_TEXTURE_2D, ch.textureID);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                x += (ch.advance >> 6) * xscale;
            }
        }
        void GLText::dirty() { mDirty = true; }
        
        void GLText::setContext(void* context) { this->context = (GL::Context*)context; }
        void GLText::setFont(const std::string& path) { font.first = (GL::GLFont*)fc::get(path)->font; font.second = path; }
        void GLText::setPosition(const glm::vec2& pos) { position = glm::vec2(pos.x, -pos.y); dirty(); }
    }
}

#endif
