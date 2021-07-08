//
//  Test.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 18.01.2021.
//

#ifndef Test_hpp
#define Test_hpp

#include "../../minEH/minEH.hpp"

namespace mh
{
    namespace test
    {
        struct NPC : Component
        {
            const std::string& path;
            // Sprite* sprite;
            
            bool moveA{ false }, moveD{ false }, moveW{ false }, moveS{ false };
            float speed{ 0.4f };
            
            bool randomMovement{ true }, onPoint{ true };
            float elapsedStanding = 0.4f;
            glm::vec2 movementPoint;
            
            NPC(const std::string& path = "Images/npc.png");
            void onInit() override;
            void onDestroy() override;
            void onUpdate(const float& elapsed) override;
            void onEvent(Event& event) override;
            void onRecord(const uint32_t& i) override;
            void onDraw() override;
        };
        
        struct MeshTest : Component
        {
            const std::string& model, texture;
            Mesh* mesh;
            
            MeshTest(const std::string& model = "Models/gothloli.obj", const std::string& texture = "Models/gothloli_d.jpg");
            void onInit() override;
            void onDestroy() override;
            void onEvent(Event& event) override;
            void onRecord(const uint32_t& i) override;
            void onDraw() override;
        };
    }
}

#endif /* Test_hpp */
