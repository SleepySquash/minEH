//
//  Test.cpp
//  OpenGL App
//
//  Created by Никита Исаенко on 18.01.2021.
//

#include "Test.hpp"

namespace mh
{
    namespace test
    {
        NPC::NPC(const std::string& path) : path(path) { }
        
        void NPC::onInit() { sprite = (Sprite*)Graphics::GetSprite(entity->composition->context); sprite->setTexture(path); sprite->create(); }
        void NPC::onDestroy() { sprite->destroy(); delete sprite; }
        
        void NPC::onUpdate(const float &elapsed)
        {
            if (randomMovement)
            {
                if (onPoint)
                {
                    if (elapsedStanding > 0.f) elapsedStanding -= elapsed;
                    else
                    {
                        movementPoint = { (rand() % 1000)/500.f - 1.f, (rand() % 1000)/500.f - 1.f };
                        onPoint = false; elapsedStanding = (rand() % 1000) / 1000.f;
                    }
                }
                else
                {
                    if (sprite->position.x > movementPoint.x - 0.05f && sprite->position.x < movementPoint.x + 0.05f && sprite->position.y > movementPoint.y - 0.05f && sprite->position.y < movementPoint.y + 0.05f) { onPoint = true; moveA = moveD = moveW = moveS = false; }
                    else
                    {
                        if (sprite->position.x > movementPoint.x + 0.05f) { moveA = true; moveD = false; }
                        else if (sprite->position.x < movementPoint.x - 0.05f) { moveA = false; moveD = true; }
                        else { moveA = moveD = false; }
                        if (sprite->position.y > movementPoint.y + 0.05f) { moveS = false; moveW = true; }
                        else if (sprite->position.y < movementPoint.y - 0.05f) { moveS = true; moveW = false; }
                        else { moveW = moveS = false; }
                    }
                }
            }
            
            // TODO: vector movement (move in the direction of vector, which is changing)
            if (moveA || moveD || moveW || moveS)
            {
                if (moveA) sprite->position.x -= speed * elapsed;
                if (moveD) sprite->position.x += speed * elapsed;
                if (moveW) sprite->position.y -= speed * elapsed;
                if (moveS) sprite->position.y += speed * elapsed;
                sprite->dirty();
            }
        }
        
        void NPC::onEvent(Event& event)
        {
            switch (event.type)
            {
                default: break;
                case Event::Type::Resized: sprite->resize(); break;
                case Event::Type::KeyPressed: if (randomMovement) break;
                    switch (event.data.key)
                    {
                        default: break;
                        case Keyboard::Key::W: moveW = true; break;
                        case Keyboard::Key::S: moveS = true; break;
                        case Keyboard::Key::A: moveA = true; break;
                        case Keyboard::Key::D: moveD = true; break;
                    }
                    break;
                case Event::Type::keyReleased: if (randomMovement) break;
                    switch (event.data.key)
                    {
                        default: break;
                        case Keyboard::Key::W: moveW = false; break;
                        case Keyboard::Key::S: moveS = false; break;
                        case Keyboard::Key::A: moveA = false; break;
                        case Keyboard::Key::D: moveD = false; break;
                    }
                    break;
            }
        }
        
        void NPC::onRecord(const uint32_t& i) { sprite->record(i); }
        void NPC::onDraw() { sprite->draw(); }
        
        
        
        
        MeshTest::MeshTest(const std::string& model, const std::string& texture): model(model), texture(texture) { }
        void MeshTest::onInit() { mesh = (Mesh*)Graphics::GetMesh(entity->composition->context); mesh->setTexture(texture); mesh->setModel(model); mesh->create(); }
        void MeshTest::onDestroy() { mesh->destroy(); delete mesh; }
        void MeshTest::onEvent(Event& event)
        {
            switch (event.type)
            {
                default: break;
                case Event::Type::Resized: mesh->resize(); break;
            }
        }
        void MeshTest::onRecord(const uint32_t& i) { mesh->record(i); }
        void MeshTest::onDraw() { mesh->draw(); }
    }
}
