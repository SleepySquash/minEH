//
//  Keys.cpp
//  minEH
//
//  Created by Никита Исаенко on 31.01.2021.
//

#include "Keys.hpp"

#include <iostream>
using std::cout;
using std::endl;

namespace keys
{
#pragma mark -
#pragma mark Game Component
    
    Game::Game(const std::string& filename)
    {
        file.open(filename);
        if (!file.is_open()) { cout << "Error :: " << __FUNCTION__ << " :: " << "Can't open file: \"" << filename << "\"\n"; this->die(); return; }
        folder = util::getFolderPath(util::getPathWOResourcePath(filename));
    }
    Game::~Game() { }
    
    void Game::onInit()
    {
        std::string line, music, back;
        bool done = false;
        while (!done)
        {
            std::getline(file, line); parser.put(line);
            if (parser.starts("{")) done = true;
            else
            {
                if (parser.starts("background ")) back = parser.string();
                else if (parser.starts("music ")) music = parser.string();
            }
        }
        
        if (music.length())
        {
            song.open(resourcePath() + folder + music);
            song.play();
        }
        
        background = Graphics::GetSprite(entity->composition->context);
        if (back.length()) { background->setTexture(folder + back); background->create(); }
        
        tc::get("Images/key_base.png");
        tc::get("Images/key_overlay.png");
        fc::get("Fonts/Arial.ttf");
        
        generatePositions();
        system.composition = entity->composition;
    }
    
    void Game::onDestroy()
    {
        song.close();
        system.destroy();
        
        tc::erase("Images/key_base.png");
        tc::erase("Images/key_overlay.png");
        fc::erase("Fonts/Arial.ttf");
    }
    
    void Game::onUpdate(const float& elapsed)
    {
        if (!song.isPlaying() || (eof && system.components.empty())) { this->die(); return; }
        position = song.getPosition();
        
        while (onhold.empty() && !eof)
        {
            std::getline(file, onhold); parser.put(onhold);
            if (parser.useful())
            {
                if (parser.starts("}", false)) { onhold.clear(); eof = true; return; }
                time = fatoi(parser.word().c_str());
                if (position >= time/1000.f - approach)
                {
                    std::string sym = parser.word();
                    auto k = system.addComponent<Key>(this, sym); k->doResize();
                    if (position > time/1000.f - approach)
                        k->onUpdate(position - (time/1000.f - approach));
                    onhold.clear();
                }
            }
            eof = file.eof();
        }
        
        if (!onhold.empty())
        {
            if (position >= time/1000.f - approach)
            {
                std::string sym = parser.word();
                system.addComponent<Key>(this, sym)->doResize();
                onhold.clear();
            }
        }
        system.onUpdate(elapsed);
    }
    
    void Game::onEvent(Event& event)
    {
        switch (event.type)
        {
            case Event::Type::Resized:
                background->resize();
                if (background->xcoef == 1.f)
                    background->setScale({1/background->ycoef, 1/background->ycoef});
                else background->setScale({1/background->xcoef, 1/background->xcoef});
                scale = (1/10.f);
                break;
            default: break;
        }
        system.onEvent(event);
    }
    
    void Game::onRecord(const uint32_t& i)
    {
        background->record(i);
        system.onRecord(i);
    }
    
    void Game::onDraw()
    {
        background->draw();
        system.onDraw();
    }
    
    
#pragma mark -
#pragma mark Key Component
    
    Key::Key(Game* game, const std::string& sym) : game(game), sym(sym[0]) { keycode = symToKeyCode(this->sym); }
    void Key::onInit()
    {
        key = Graphics::GetSprite(entity->composition->context);
        key->setTexture("Images/key_base.png"); key->create(); key->color.a = 0.f;
        key->setPosition(game->positions[sym]);
        
        overlay = Graphics::GetSprite(entity->composition->context);
        overlay->setTexture("Images/key_overlay.png"); overlay->create();
        overlay->color = { 0.f, 1.f, 0.f, 0.f }; overlay->setScale({0.f, 0.f});
        overlay->setPosition(game->positions[sym]);
        
        text = Graphics::GetText(entity->composition->context);
        text->setFont("Fonts/Arial.ttf"); text->create();
        text->setPosition(game->positions[sym]);
        text->setScale(0.1f); text->string = sym;
    }
    void Key::onDestroy() { key->destroy(); overlay->destroy(); text->destroy(); delete key; delete overlay; delete text; }
    void Key::onUpdate(const float& elapsed)
    {
        switch (mode)
        {
            default: break;
            case Mode::Appear:
                temp += elapsed;
                if (temp >= game->appear)
                {
                    key->color.a = overlay->color.a = text->color.a = 1.f;
                    mode = Mode::Exist; temp = 0.f;
                }
                else
                {
                    float alpha = temp/game->appear;
                    key->color.a = overlay->color.a = text->color.a = alpha;
                }
            break;
            case Mode::Disappear:
                temp += elapsed;
                if (temp >= game->disappear)
                {
                    key->color.a = overlay->color.a = text->color.a = 0.f;
                    mode = Mode::Deprecate; this->die();
                }
                else
                {
                    float alpha = temp/game->disappear;
                    key->color.a = overlay->color.a = text->color.a = 1.f - alpha;
                }
            break;
        }
        
        if (pressed) return;
        if (gap >= game->approach)
        {
            if (gap >= game->approach + game->accuracy)
            {
                mode = Mode::Disappear;
                overlay->color.r = 1.f; overlay->color.g = 0.f;
                pressed = true;
            }
            else gap += elapsed;
        }
        else
        {
            overlay->setScale(game->scale * gap / game->approach);
            
            gap += elapsed;
            if (gap >= game->approach)
                overlay->setScale(game->scale);
        }
    }
    void Key::onEvent(Event& event)
    {
        switch (event.type)
        {
            default: break;
            case Event::Type::Resized: key->resize(); overlay->resize(); text->resize();
                key->setScale(game->scale); overlay->setScale(game->scale); text->setScale(game->scale);
                break;
            case Event::Type::KeyPressed:
                if (!pressed && event.data.key == keycode)
                {
                    pressed = true;
                    mode = Mode::Disappear;
                    cout << "pressed\n";
                }
                break;
        }
    }
    void Key::onRecord(const uint32_t& i) { overlay->record(i); key->record(i); text->record(i); }
    void Key::onDraw() { overlay->draw(); key->draw(); text->draw(); }
    
    
#pragma mark -
#pragma mark Utility
    
    void Game::generatePositions()
    {
        float x, y = 0, xstep = 1.f/12, ystep = 1.f/7;
        unsigned int layout = 0;
        
        y = 2.5f * ystep;
        for (unsigned int i = 0; i < 3; ++i)
        {
            x = xstep + xstep * layout/2;
            for (unsigned int j = 0; j < 10 - layout; ++j)
            {
                positions[getKeyByIndex(i, j)] = glm::vec2(x * 2 - 1, y * 2 - 1);
                x += xstep;
            }
            if (i == 1) layout = 3; else layout = 1;
            y += ystep;
        }
    }
    
    char Game::getKeyByIndex(const unsigned int &x, const unsigned int &y)
    {
        switch (x)
        {
            case 0:
                switch (y)
                {
                    default: return 0; break;
                    case 0: return 'q'; break;
                    case 1: return 'w'; break;
                    case 2: return 'e'; break;
                    case 3: return 'r'; break;
                    case 4: return 't'; break;
                    case 5: return 'y'; break;
                    case 6: return 'u'; break;
                    case 7: return 'i'; break;
                    case 8: return 'o'; break;
                    case 9: return 'p'; break;
                }
                break;
            case 1:
                switch (y)
                {
                    default: return 0; break;
                    case 0: return 'a'; break;
                    case 1: return 's'; break;
                    case 2: return 'd'; break;
                    case 3: return 'f'; break;
                    case 4: return 'g'; break;
                    case 5: return 'h'; break;
                    case 6: return 'j'; break;
                    case 7: return 'k'; break;
                    case 8: return 'l'; break;
                }
                break;
            case 2:
                switch (y)
                {
                    default: return 0; break;
                    case 0: return 'z'; break;
                    case 1: return 'x'; break;
                    case 2: return 'c'; break;
                    case 3: return 'v'; break;
                    case 4: return 'b'; break;
                    case 5: return 'n'; break;
                    case 6: return 'm'; break;
                }
                break;
            default: return 0; break;
        }
    }
    
    Keyboard::Key Key::symToKeyCode(const char& sym)
    {
        switch (sym)
        {
            default: return Keyboard::Key::Unknown; break;
            case 'Q': case 'q': return Keyboard::Key::Q; break;
            case 'W': case 'w': return Keyboard::Key::W; break;
            case 'E': case 'e': return Keyboard::Key::E; break;
            case 'R': case 'r': return Keyboard::Key::R; break;
            case 'T': case 't': return Keyboard::Key::T; break;
            case 'Y': case 'y': return Keyboard::Key::Y; break;
            case 'U': case 'u': return Keyboard::Key::U; break;
            case 'I': case 'i': return Keyboard::Key::I; break;
            case 'O': case 'o': return Keyboard::Key::O; break;
            case 'P': case 'p': return Keyboard::Key::P; break;
            case 'A': case 'a': return Keyboard::Key::A; break;
            case 'S': case 's': return Keyboard::Key::S; break;
            case 'D': case 'd': return Keyboard::Key::D; break;
            case 'F': case 'f': return Keyboard::Key::F; break;
            case 'G': case 'g': return Keyboard::Key::G; break;
            case 'H': case 'h': return Keyboard::Key::H; break;
            case 'J': case 'j': return Keyboard::Key::J; break;
            case 'K': case 'k': return Keyboard::Key::K; break;
            case 'L': case 'l': return Keyboard::Key::L; break;
            case 'Z': case 'z': return Keyboard::Key::Z; break;
            case 'X': case 'x': return Keyboard::Key::X; break;
            case 'C': case 'c': return Keyboard::Key::C; break;
            case 'V': case 'v': return Keyboard::Key::V; break;
            case 'B': case 'b': return Keyboard::Key::B; break;
            case 'N': case 'n': return Keyboard::Key::N; break;
            case 'M': case 'm': return Keyboard::Key::M; break;
            case '_':           return Keyboard::Key::Space; break;
        }
    }
}
