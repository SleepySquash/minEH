//
//  Keys.hpp
//  minEH
//
//  Created by Никита Исаенко on 31.01.2021.
//

#pragma once

#include "../../minEH/minEH.hpp"
using namespace mh;

#include <fstream>
#include <unordered_map>
#include <glm/glm.hpp>

namespace keys
{
    struct Game : Component
    {
        Entity system;
        Audio song;
        Sprite* background;
        
        std::unordered_map<char, glm::vec2> positions;
        float approach{ 1.7f }, appear{ 0.1f }, disappear{ 0.2f }, accuracy{ 0.2f };
        float scale{ 1.f };
        
        nss::Parser parser;
        float position{ 0.f };
        std::string folder, onhold;
        unsigned int time;
        
        std::ifstream file;
        bool eof{ false };
        
        Game(const std::string& filename);
        ~Game();
        void onInit() override;
        void onDestroy() override;
        void onUpdate(const float& elapsed) override;
        void onEvent(Event& event) override;
        void onRecord(const uint32_t& i) override;
        void onDraw() override;
        
        void generatePositions();
        char getKeyByIndex(const unsigned int& x, const unsigned int& y);
    };
    
    struct Key : Component
    {
        Sprite* key;
        Sprite* overlay;
        // Text* text;
        
        Game* game; char sym;
        Keyboard::Key keycode;
        float gap{ 0.f }, temp{ 0.f };
        bool pressed{ false };
        
        Mode mode{ Mode::Appear };
        
        Key(Game* game, const std::string& sym);
        void onInit() override;
        void onDestroy() override;
        void onUpdate(const float& elapsed) override;
        void onEvent(Event& event) override;
        void onRecord(const uint32_t& i) override;
        void onDraw() override;
        
        static Keyboard::Key symToKeyCode(const char&);
    };
}
