//
//  RawSprite.cpp
//  minEH
//
//  Created by Никита Исаенко on 08.07.2021.
//

#include "RawSprite.hpp"
#include "../../../minEH/Renderer/Factory.hpp"
#include "../../../minEH/Support/ResourcePath.hpp"

namespace mh
{
    RawSprite::RawSprite(Renderer* context) : context(context), texture(nullptr) { create(); }
    RawSprite::RawSprite(Renderer* context, Texture* texture) : context(context), texture(texture) { create(); setTexture(texture); }
    
    void RawSprite::create()
    {
        // То, что будет лежать в буферах
        float vertices[] = { -1.f, -1.f,   1.f, -1.f,   -1.f, 1.f,   1.f, 1.f };
        float uvs[] = { 0.0f, (float)context->UV_0,  1.0f, (float)context->UV_0,  0.0f, (float)context->UV_1,  1.0f, (float)context->UV_1 };
        uint32_t indices[] = { 0, 1, 2,   1, 3, 2 };
        
        // Сами буферы
        vertexBuffer = Renders::Buffer(context, BufferType::Vertex, vertices, sizeof(vertices));
        uvBuffer = Renders::Buffer(context, BufferType::Vertex, uvs, sizeof(uvs), type);
        indexBuffer = Renders::Buffer(context, BufferType::Index, indices, sizeof(indices));
        
        // Дескриптор, т.е. ссылка на изображение (CombinedImageSampler = текстура)
        descriptor = Renders::Descriptor(context);
        descriptor->layouts = { { DescriptorType::CombinedImageSampler, 0, ShaderStage::Fragment } };
        descriptor->allocate();
        
        // Шейдеры и пайплайн с используемыми шейдерами атрибутами
        vertexShader = Renders::Shader(context, ShaderStage::Vertex, resourcePath() + "Shaders", "sprite.vert");
        fragmentShader = Renders::Shader(context, ShaderStage::Fragment, resourcePath() + "Shaders", "sprite.frag");
        pipeline = Renders::Pipeline(context);
        pipeline->shaders = { vertexShader, fragmentShader };
        pipeline->bindings = {
            { 0, sizeof(float) * 2 },
            { 1, sizeof(float) * 2 }
        };
        pipeline->attributes = {
            { 0, 0, 0, VertexFormat::R32G32_SFLOAT },
            { 1, 1, 0, VertexFormat::R32G32_SFLOAT },
        };
        pipeline->pushConstantRanges = {
            {   ShaderStage::Vertex, "model",                 0, sizeof(glm::mat4) },
            { ShaderStage::Fragment, "color", sizeof(glm::mat4), sizeof(glm::vec4) },
        };
        pipeline->descriptor = descriptor;
        pipeline->allocate();
    }
    void RawSprite::resize() { dirty(); pipeline->recreate(); }
    void RawSprite::destroy() {
        vertexBuffer->free();
        uvBuffer->free();
        indexBuffer->free();
        descriptor->free();
        pipeline->free();
        vertexShader->free();
        fragmentShader->free();
    }
    
    void RawSprite::setTexture(Texture* texture) {
        this->texture = texture; dirty();
        uvWH = { texture->width, texture->height };
        descriptor->update({ texture });
    }
    void RawSprite::setUV(const glm::vec4& uv) {
        glm::vec4 nuv = { uv.x / texture->width, uv.y / texture->height,
                          uv.z / texture->width, uv.w / texture->height };
        float uvs[] = {
            nuv.x        , nuv.y + context->UV_0 * nuv.w,
            nuv.x + nuv.z, nuv.y + context->UV_0 * nuv.w,
            nuv.x        , nuv.y + context->UV_1 * nuv.w,
            nuv.x + nuv.z, nuv.y + context->UV_1 * nuv.w };
        uvBuffer->update(uvs, sizeof(uvs));
        if (uvWH.x != uv.z || uvWH.y != uv.w) { uvWH = { uv.z, uv.w }; dirty(); }
    }
    
    void RawSprite::record(const uint32_t& i) {
        pipeline->onRecord(i);
        pipeline->vertex({ vertexBuffer, uvBuffer });
        pipeline->index(indexBuffer);
        descriptor->onRecord(pipeline);
        pipeline->push(0, &model);
        pipeline->push(1, &color);
        pipeline->drawIndexed(6, 6);
    }
    void RawSprite::dirty() {
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));
        model = glm::scale(model, glm::vec3(uvWH.x, uvWH.y, 1.f));
        model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.f));
        model = context->ortho * model;
    }
}
