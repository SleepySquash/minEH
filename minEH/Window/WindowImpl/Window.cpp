//
//  Window.cpp
//  minEH
//
//  Created by Никита Исаенко on 23.01.2021.
//

#include "../Window.hpp"

namespace mh
{
    void Window::resize(Event &event)
    {
        mResized = shouldRender = true;
        aspect = (float)frame.width/frame.height;
        event.data.size = frame;
    }
}
