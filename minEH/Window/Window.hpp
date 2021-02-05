//
//  Window.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 14.01.2021.
//

#ifndef Window_hpp
#define Window_hpp

#include <string>
#include <queue>

#include "Event.hpp"

namespace mh { struct Window; }
struct DisplayData
{
    mh::Window* window;
    void* data = nullptr;
    void (*callback)(void*) = nullptr;
};

namespace mh
{
    struct Window
    {
        unsigned int x = 0, y = 0, width = 800, height = 600;
        std::string title = "minEH Application";
        bool fullscreen = false;
        
        WindowSize frame;
        float aspect;
        
        bool isOpen = false;
        bool mayRender = true, shouldRender = true;
        bool mResized = true;
        std::queue<Event> events;
        
        DisplayData displayData;
        
        void *window, *view, *delegate;
        void* caMetalLayer;
        void* mTitle;
        
    public:
        Window();
        ~Window();
        
        void open();
        void close();
        
        void focus();
        void resize(Event& event);
        void setCallback(void (*fun)(void*), void* data);
        void setTitle(const std::string& title);
        void setCursorMode(const Mouse::CursorMode& mode);
        
        bool pollEvent(Event& event);
        void display();
    };
}
    
#endif /* Window_hpp */
