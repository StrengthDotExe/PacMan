#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <windows.h>
#include <stack>
#include <queue>
#include <filesystem>

#include "renderer.cpp"

#define SCORE "SCORE:\n"

class Engine {
    public:
    sf::RenderWindow window;
    bool fullscreen = false;
    // int chosenTool = 0;
    sf::Text scoreText;
    
    
    Engine() {
        
        window.create(sf::VideoMode(WIDTH+(WIDTH/4),HEIGHT),"PACMAN");
        window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width/2)-(WIDTH/2)-125,0));
        // PrimitiveRenderer renderer(&window);

        HDC temp = GetDC(window.getSystemHandle());
        
        window.setFramerateLimit(FRAMERATE);

        sf::Font font;
        if (!font.loadFromFile("LEMONMILK-Bold.otf")) std::cout << "loading font failed.";
        scoreText.setFont(font);
        scoreText.setString("SCORE:\n");
        scoreText.setPosition(sf::Vector2f(1050,50));
        scoreText.setCharacterSize(40);
        
        Background background("background.png");

        sf::Texture playerTexture;
        playerTexture.loadFromFile("pacman.png");
        sf::Sprite playerSprite(playerTexture);
        Player player(400,532,temp);
        player.setDefault(playerSprite);
        Entity entity(400,339,Entity::type::GHOST,temp);
        // Entity entity1(500,339,Entity::type::GHOST,temp);

        std::string path = "./animations/rightMovement";
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            // std::cout << entry.path().filename() << std::endl;
            player.addRightMovementFrame(entry.path().generic_string());
        }
        path = "./animations/leftMovement";
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            player.addLeftMovementFrame(entry.path().generic_string());
        }
        path = "./animations/upMovement";
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            player.addUpMovementFrame(entry.path().generic_string());
        }
        path = "./animations/downMovement";
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            player.addDownMovementFrame(entry.path().generic_string());
        }

        // player.addUpMovementFrame("pacman.png");
        // player.addDownMovementFrame("pacman.png");
        // player.addRightMovementFrame("pacman.png");
        // player.addRightMovementFrame("pacman2.png");
        // player.addRightMovementFrame("pacman.png");
        // player.addRightMovementFrame("pacman123.png");
        // player.addLeftMovementFrame("pacman.png");
        

        std::string scoreString;
        sf::SoundBuffer buffer;
        buffer.loadFromFile("dzwiek.wav");
        sf::Sound sound;
        sound.setBuffer(buffer);
        sound.setVolume(100);
        World::GenerateCollectibles("entitySpots.png",temp);

        sf::Vector2i startPos;
            sf::Vector2i endPos;
            bool mouseClick = false;

        while(window.isOpen())
        {

            window.clear();
            sf::Event event;
            background.draw(&window);
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    ReleaseDC(window.getSystemHandle(), temp);
                }
                
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
                ReleaseDC(window.getSystemHandle(), temp);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::T) && sound.getStatus() != sf::SoundSource::Status::Playing) {
                sound.play();
            }
            
            scoreString = SCORE+std::to_string(player.score);
            scoreText.setString(scoreString);
            window.draw(scoreText);
            
            // renderer.render();
            // renderer.drawCircleManual(100,100,50);
            
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        if(!mouseClick) {
                            startPos = sf::Mouse::getPosition(window);
                        }
                        endPos = sf::Mouse::getPosition(window);
                        mouseClick=1;
                    }
                    if(!sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouseClick) {
                        mouseClick=0;
                        std::cout << startPos.x << ',' << startPos.y << ',' << endPos.x << ',' << endPos.y << '\n';
                        std::cout << entity.check2pointCollision(Point(startPos.x,startPos.y),Point(endPos.x,startPos.y),0,3);
                }


            // line.draw(&window);
            // point.draw(&window);
            World::DrawEntities(&window);
            player.draw(&window);
            // entity.draw(&window);
            // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
            //         entity.FollowPlayer(player.getXPos(),player.getYPos());
            //         entity1.FollowPlayer(player.getXPos(),player.getYPos());
            // }
            // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
            //         std::cout << player.getXPos() << ',' << player.getYPos() << '\n';
            // }
            

            window.display();
        }
    }
};

int main()
{
    Engine engine1;
    return 0;
}