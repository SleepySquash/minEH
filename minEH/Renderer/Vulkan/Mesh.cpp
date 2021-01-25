//
//  Mesh.cpp
//  minEH
//
//  Created by Никита Исаенко on 24.01.2021.
//

#include "Mesh.hpp"

#ifdef MINEH_VULKAN
#include "../../Engine/Collector/Buffer.hpp"
#include "../../Engine/Collector/Texture.hpp"
#include "../../Engine/Collector/Pipeline.hpp"
#include "../../Engine/Collector/Descriptor.hpp"
#include "../../Support/ResourcePath.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <tiny_obj_loader.h>

namespace mh
{
    namespace Vk
    {
        VkMesh::VkMesh(Vk::Context* context) : context(context) { }
        
        void VkMesh::setContext(void* context) { this->context = (Vk::Context*)context; }
        void VkMesh::destroy()
        {
            if (!context) return;
            bc::erase(modelPath + "-v");
            bc::erase(modelPath + "-i");
            pc::erase("sprite");
            dc::erase("texture " + texture.second);
            tc::erase(texture.second);
        }
        void VkMesh::create()
        {
            createDescriptors();
            createGraphicsPipeline();
        }
        void VkMesh::resize()
        {
            updateModel(true);
            createDescriptors();
            createGraphicsPipeline();
        }

        void VkMesh::createDescriptors()
        {
            DescriptorCollectorObject* dco = dc::get("mesh " + texture.second);
            if (!dco->loaded)
            {
                VkDescriptorSetLayoutBinding uboBinding;
                uboBinding.binding = 0;
                uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uboBinding.descriptorCount = 1;
                uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                uboBinding.pImmutableSamplers = nullptr;
                
                VkDescriptorSetLayoutBinding samplerBinding;
                samplerBinding.binding = 1;
                samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                samplerBinding.descriptorCount = 1;
                samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                samplerBinding.pImmutableSamplers = nullptr;

                dco->bindings.resize(2);
                dco->bindings[0] = uboBinding;
                dco->bindings[1] = samplerBinding;
                
                dco->types.resize(1);
                dco->types[0] = DescriptorType::Camera;

                dco->size.resize(2);
                dco->size[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                dco->size[0].descriptorCount = context->swapChainProps.images;
                dco->size[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                dco->size[1].descriptorCount = context->swapChainProps.images;
                
                VkDescriptorImageInfo imageInfo;
                imageInfo.sampler = texture.first->sampler;
                imageInfo.imageView = texture.first->image.view;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                dco->writeSets.resize(2);
                
                dco->writeSets[0].sType = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                dco->writeSets[0].dstBinding = 0;
                dco->writeSets[0].dstArrayElement = 0;
                dco->writeSets[0].descriptorCount = 1;
                dco->writeSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                dco->writeSets[0].pImageInfo = nullptr;
                dco->writeSets[0].pBufferInfo = nullptr;
                dco->writeSets[0].pTexelBufferView = nullptr;
                
                dco->writeSets[1].sType = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                dco->writeSets[1].dstBinding = 1;
                dco->writeSets[1].dstArrayElement = 0;
                dco->writeSets[1].descriptorCount = 1;
                dco->writeSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                dco->writeSets[1].pImageInfo = &imageInfo;
                dco->writeSets[1].pBufferInfo = nullptr;
                dco->writeSets[1].pTexelBufferView = nullptr;
                
                dco->createDescriptor(context);
            }
            descriptor = dco->descriptor;
            descriptorID = dco->id;
        }
        
        void VkMesh::createGraphicsPipeline()
        {
            PipelineCollectorObject* pco = pc::get("mesh");
            if (!pco->loaded)
            {
                pco->vertexShaderPath = resourcePath() + "/Shaders/spv/_mesh.vert.spv";
                pco->fragmentShaderPath = resourcePath() + "/Shaders/spv/_mesh.frag.spv";
                pco->depthEnabled = true;
                pco->cullMode = VK_CULL_MODE_FRONT_BIT;
                pco->descriptor = &descriptor;
                
                pco->vBindingDescription.binding = 0;
                pco->vBindingDescription.stride = sizeof(Vertex<glm::vec3>);
                pco->vBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                pco->vAttributeDescription.resize(2);
                
                pco->vAttributeDescription[0].binding = 0;
                pco->vAttributeDescription[0].location = 0;
                pco->vAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                pco->vAttributeDescription[0].offset = offsetof(Vertex<glm::vec3>, pos);

                pco->vAttributeDescription[1].binding = 0;
                pco->vAttributeDescription[1].location = 1;
                pco->vAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
                pco->vAttributeDescription[1].offset = offsetof(Vertex<glm::vec3>, uv);
                
                pco->pushConstantRanges.resize(1);
                pco->pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                pco->pushConstantRanges[0].offset = 0;
                pco->pushConstantRanges[0].size = sizeof(glm::mat4);
                
                pco->createPipeline(context);
            }
            graphicsPipeline = pco->pipeline;
            pipelineLayout = pco->layout;
            pipelineID = pco->id;
            return;
        }
        
        void VkMesh::record(const uint32_t& i)
        {
            VkCommandBuffer& commandBuffer = context->commandBuffers[i];
            if (context->pipelineID != pipelineID) {
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); context->pipelineID = pipelineID; }
            
            if (context->vertexID != vertexID) {
                VkDeviceSize offset = 0; vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, &offset); context->vertexID = vertexID; }
            if (context->indexID != indexID) { vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32); context->indexID = indexID; }
            
            if (context->descriptorID != descriptorID) {
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptor.sets[i], 0, nullptr);
                context->descriptorID = descriptorID; }
            
            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &model);
            vkCmdDrawIndexed(commandBuffer, indicesCount, 1, 0, 0, 0);
        }

        void VkMesh::updateModel(const bool& force)
        {
            if (mDirty || force)
            {
                model = glm::mat4(1.f);
                model = glm::translate(model, position);
                model = glm::rotate(model, rotation.x, glm::vec3(1.f, 0.f, 0.f));
                model = glm::rotate(model, rotation.y, glm::vec3(0.f, 1.f, 0.f));
                model = glm::rotate(model, rotation.z, glm::vec3(0.f, 0.f, 1.f));
                model = glm::scale(model, scale);
                mDirty = false;
            }
        }
        void VkMesh::dirty() { mDirty = true; updateModel(); }
        void VkMesh::setTexture(const std::string& path) { texture.first = (Vk::Texture*)tc::get(path); texture.second = path; }
        void VkMesh::setModel(const std::string& path)
        {
            modelPath = path;
            
            if (bc::exists(modelPath + "-v"))
            {
                auto vb = bc::get(modelPath + "-v");
                vertexBuffer = *(Vk::Buffer*)vb->buffer; vertexID = vb->id;
                auto ib = bc::get(modelPath + "-i");
                indexBuffer = *(Vk::Buffer*)ib->buffer; indexID = ib->id; indicesCount = ib->indices;
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
                            1.f - attrib.texcoords[2 * index.texcoord_index + 1]
                        };
                        
                        if (uniqueVertices.count(vertex) == 0) {
                            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                            vertices.push_back(vertex); }
                        indices.push_back(uniqueVertices[vertex]);
                    }
                }
                
                BufferCollectorObject* bcov = bc::get(modelPath + "-v");
                Vk::Buffer* buffer = new Vk::Buffer;
                bcov->buffer = (void*)buffer;
                context->generateSingleBuffer(sizeof(vertices[0]) * vertices.size(), *buffer, vertices.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
                vertexBuffer = *buffer;
                vertexID = bcov->id;
                
                BufferCollectorObject* bcoi = bc::get(modelPath + "-i");
                buffer = new Vk::Buffer;
                bcoi->buffer = (void*)buffer;
                context->generateSingleBuffer(sizeof(indices[0]) * indices.size(), *buffer, indices.data(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
                indicesCount = bcoi->indices = static_cast<uint32_t>(indices.size());
                indexBuffer = *buffer;
                indexID = bcoi->id;
            }
        }
    }
}

#endif
