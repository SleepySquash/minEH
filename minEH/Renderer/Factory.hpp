//
//  Factory.hpp
//  minEH
//
//  Created by Никита Исаенко on 19.05.2021.
//

#pragma once

#include "Context.hpp"

namespace mh
{
    namespace Renders
    {
        mh::Buffer* Buffer(Renderer* context, const BufferType& type = BufferType::Vertex, void* data = nullptr, const uint32_t& size = 0, const BufferUpdateType& updateType = BufferUpdateType::Static);
        mh::Texture* Texture(Renderer* context, const std::string& path = "");
        mh::Shader* Shader(Renderer* context, const ShaderStage& stage, const std::string& path, const std::string& subpath = std::string(), const std::string& main = "main");
        mh::Descriptor* Descriptor(Renderer* context);
        mh::Pipeline* Pipeline(Renderer* context);
    }
}
