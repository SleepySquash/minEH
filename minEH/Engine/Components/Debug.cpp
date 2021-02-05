//
//  Debug.cpp
//  minEH
//
//  Created by Никита Исаенко on 05.02.2021.
//

#include "Debug.hpp"

#include "../../Graphics/Factory.hpp"

namespace mh
{
    namespace Components
    {
        Debug::Debug() { }
        void Debug::onInit()
        {
            text1 = Graphics::GetText(entity->composition->context);
            text2 = Graphics::GetText(entity->composition->context);
            text3 = Graphics::GetText(entity->composition->context);
            
            text1->setFont("Fonts/Arial.ttf");
            text1->halign = Align::Right;
            text1->valign = Align::Bottom;
            text1->setPosition({ 1.f, 1.f });
            text1->setScale(0.1f);
            text1->string = "Build 20200205.1";
            text1->create();
            
            text2->setFont("Fonts/Arial.ttf");
            text2->halign = Align::Right;
            text2->valign = Align::Bottom;
            text2->setScale(0.1f);
            text2->string = "minEH Application";
            text2->create();
            
            text3->setFont("Fonts/Arial.ttf");
            text3->halign = Align::Right;
            text3->valign = Align::Bottom;
            text3->setScale(0.1f);
            text3->string = "FPS: NaN";
            text3->create();
        }
        void Debug::onDestroy()
        {
            text1->destroy(); delete text1;
            text2->destroy(); delete text2;
            text3->destroy(); delete text3;
        }
        void Debug::onUpdate(const float& elapsed) { }
        void Debug::onEvent(Event& event)
        {
            switch (event.type)
            {
                default: break;
                case Event::Type::Resized:
                    text1->resize();
                    text2->setPosition({ 1.f, 1.f - text1->bounds.w - 0.01f }); text2->resize();
                    text3->setPosition({ 1.f, 1.f - text1->bounds.w - text2->bounds.w - 0.02f }); text3->resize();
                    break;
            }
        }
        void Debug::onRecord(const uint32_t& i) { text1->record(i); text2->record(i); text3->record(i); }
        void Debug::onDraw() { text1->draw(); text2->draw(); text3->draw(); }
    }
}
