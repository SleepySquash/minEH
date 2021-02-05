//
//  Debug.hpp
//  minEH
//
//  Created by Никита Исаенко on 05.02.2021.
//

#pragma once

#include "../Composition.hpp"
#include "../../Graphics/Drawable.hpp"

namespace mh
{
    namespace Components
    {
        struct Debug : Component
        {
            Text *text1, *text2, *text3;
            
            Debug();
            void onInit() override;
            void onDestroy() override;
            void onUpdate(const float& elapsed) override;
            void onEvent(Event& event) override;
            void onRecord(const uint32_t& i) override;
            void onDraw() override;
        };
    }
}
