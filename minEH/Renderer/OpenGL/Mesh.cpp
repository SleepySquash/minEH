//
//  Mesh.cpp
//  minEH
//
//  Created by Никита Исаенко on 25.01.2021.
//

#include "Mesh.hpp"

#ifdef MINEH_OPENGL
#include "../../Engine/Collector/Shader.hpp"
#include "../../Engine/Collector/Buffer.hpp"
#include "../../Engine/Collector/Texture.hpp"
#include "../../Support/ResourcePath.hpp"
#include "Camera.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>

namespace mh
{
    namespace GL
    {
        GLMesh::GLMesh(GL::Context* context) : context(context) { }
        
        void GLMesh::create()
        {
            if (ShaderCollection::mesh) ++ShaderCollection::mesh->uses;
            else
            {
                ShaderCollection::mesh = new ShaderCollectorObject("Shaders/OpenGL/mesh.vert", "Shaders/OpenGL/mesh.frag");
                GLuint uniformBlockIndex = glGetUniformBlockIndex(ShaderCollection::mesh->shader.ID, "CBO");
                glUniformBlockBinding(ShaderCollection::mesh->shader.ID, uniformBlockIndex, 0);
            }
            ID = ShaderCollection::mesh->shader.ID;
            loc = glGetUniformLocation(ID, "model");
        }
        
        void GLMesh::destroy()
        {
            tc::erase(texture.second);
            bc::erase(modelPath);
            if (ShaderCollection::mesh)
            {
                if (ShaderCollection::mesh->uses != 0) // TODO: shader collector
                    --ShaderCollection::mesh->uses;
            }
        }
        
        void GLMesh::resize() { dirty(); }
        
        void GLMesh::draw()
        {
    #ifdef MINEH_DEBUG
            if (!texture.first) { std::cout << "Error :: Sprite :: Draw :: No texture is bound (nullptr) while attempting to draw.\n"; return; }
    #endif
            if (context->shaderID != ID) { glUseProgram(ID); glEnable(GL_DEPTH_TEST); context->shaderID = ID; }
            if (context->VAOID != VAO) { glBindVertexArray(VAO); context->VAOID = VAO; }
            if (context->VBOID != VBO) { glBindBuffer(GL_ARRAY_BUFFER, VBO); context->VBOID = VBO; }
            if (context->EBOID != EBO) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); context->EBOID = EBO; }
            if (mDirty)
            {
                model = glm::mat4(1.f);
                model = glm::translate(model, position);
                model = glm::rotate(model, rotation.x, glm::vec3(1.f, 0.f, 0.f));
                model = glm::rotate(model, rotation.y, glm::vec3(0.f, 1.f, 0.f));
                model = glm::rotate(model, rotation.z, glm::vec3(0.f, 0.f, 1.f));
                model = glm::scale(model, scale);
                mDirty = false;
            }
            glUniformMatrix4fv(loc, 1, false, glm::value_ptr(model));
            glActiveTexture(GL_TEXTURE0);
            if (context->textureID != texture.first->id) {
                glBindTexture(GL_TEXTURE_2D, texture.first->id); context->textureID = texture.first->id; }
            glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
        }
        
        void GLMesh::dirty() { mDirty = true; }
        
        void GLMesh::setContext(void* context) { this->context = (GL::Context*)context; }
        void GLMesh::setTexture(const std::string& path) { texture.first = (GL::Texture*)tc::get(path); texture.second = path; }
        void GLMesh::setModel(const std::string& path)
        {
            modelPath = path;
            if (bc::exists(path))
            {
                auto bco = bc::get(path);
                GL::Buffer* buffer = (GL::Buffer*)bco->buffer;
                VAO = buffer->VAO;
                VBO = buffer->VBO;
                EBO = buffer->EBO;
                indicesCount = bco->indices;
            }
            else
            {
                std::vector<Vertex<glm::vec3>> vertices;
                std::vector<uint32_t> indices;
                
                tinyobj::attrib_t attrib;
                std::vector<tinyobj::shape_t> shapes;
                std::vector<tinyobj::material_t> materials;
                std::string warn, err;
                
                if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (resourcePath() + path).c_str())) throw std::runtime_error("loadModel() failed: " + warn + err);
                
                std::unordered_map<Vertex<glm::vec3>, uint32_t> uniqueVertices = {};
                
                for (const auto& shape : shapes)
                {
                    for (const auto& index : shape.mesh.indices)
                    {
                        Vertex<glm::vec3> vertex;
                        
                        vertex.pos = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]
                        };

                        vertex.uv = {
                                  attrib.texcoords[2 * index.texcoord_index + 0],
                                  attrib.texcoords[2 * index.texcoord_index + 1]
                        };
                        
                        if (uniqueVertices.count(vertex) == 0) {
                            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                            vertices.push_back(vertex); }
                        indices.push_back(uniqueVertices[vertex]);
                    }
                }
                
                auto bco = bc::get(path);
                GL::Buffer* buffer = new GL::Buffer;
                bco->buffer = (void*)buffer;
                
                glGenVertexArrays(1, &buffer->VAO);
                glBindVertexArray(buffer->VAO);
                
                glGenBuffers(1, &buffer->VBO);
                glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
                
                glGenBuffers(1, &buffer->EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
                
                glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(0));                 glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
                
                VAO = buffer->VAO;
                VBO = buffer->VBO;
                EBO = buffer->EBO;
                indicesCount = bco->indices = static_cast<uint32_t>(indices.size());
            }
        }
    }
}
#endif
