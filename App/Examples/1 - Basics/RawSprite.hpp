//
//  RawSprite.hpp
//  minEH
//
//  Created by Никита Исаенко on 08.07.2021.
//

#pragma once
#include "Drawable.hpp"
#include "../../../minEH/Renderer/Renderer.hpp"

namespace mh
{
    struct RawSprite : Drawable, Transform2D
    {
        Renderer* context;
        Texture* texture;
        
        Buffer* vertexBuffer, *uvBuffer, *indexBuffer;
        Descriptor* descriptor;
        Shader* vertexShader, *fragmentShader;
        Pipeline* pipeline;
        
        BufferUpdateType type{ BufferUpdateType::Static };
        glm::mat4 model{ 1.f };
        glm::vec4 color{ 1.f, 1.f, 1.f, 1.f };
        glm::vec2 uvWH;
        
        RawSprite(Renderer* context = nullptr);
        RawSprite(Renderer* context, Texture* texture);
        
        void create();
        void resize();
        void destroy();
        void setTexture(Texture* texture);
        void setUV(const glm::vec4& uv);
        
        void record(const uint32_t& i) override;
        void dirty() override;
    };
}
