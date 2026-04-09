#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <windows.h>
#include <stack>
#include <queue>
#include <bits/stdc++.h> 

#define FRAMERATE 60
#define ANIMATIONSPEED 3
#define WIDTH 1000
#define HEIGHT 1000

class GameObject
{

};

class UpdatableObject : virtual public GameObject
{
public:
    virtual void update() = 0;
};

class DrawableObject : virtual public GameObject
{
public:
    DrawableObject() {}
    ~DrawableObject() {}

    virtual void draw(sf::RenderWindow* window) = 0;
};

class TransformableObject : virtual public GameObject
{
public:
    virtual void translate(int tx, int ty) = 0;
    virtual void rotate(int x0, int y0, int alfa) = 0;
    virtual void scale(int x0, int y0, float k) = 0;
};

class ShapeObject : public DrawableObject, public TransformableObject
{
    public:
    ShapeObject() {}
    ~ShapeObject() {}
};

class BitmapHandler
{
public:
    static sf::Sprite* createSprite() {
        return new sf::Sprite;
    }
    static void deleteSprite(sf::Sprite* sprite) {
        free(sprite);
    }

    static sf::Sprite* createSprite(std::string input) {
        sf::Texture* texture = new sf::Texture();
        texture->loadFromFile(input);
        return new sf::Sprite(*texture);
    }

    static void loadTexture(sf::Sprite sprite, std::string input) {
        sf::Texture* texture = new sf::Texture();
        texture->loadFromFile(input);
        sprite.setTexture(*texture);
    }

    static void saveSprite(sf::Sprite sprite, std::string filename) {
        sprite.getTexture()->copyToImage().saveToFile(filename);
    }

    static void swapSprites(sf::Sprite sprite1, sf::Sprite sprite2) {
        sf::Texture temp = *sprite1.getTexture();
        sprite1.setTexture(*sprite2.getTexture());
        sprite2.setTexture(temp);
    }

};  

class BitmapObject : public DrawableObject
{
protected:
    int xPos,yPos;
    sf::Sprite bitmap;
public:
    BitmapObject(int xPos, int yPos, sf::Sprite bitmap) {
        this->xPos = xPos;
        this->yPos = yPos;
        this->bitmap = bitmap;
    }
    ~BitmapObject() {}

    void draw(sf::RenderWindow* window) {
        bitmap.setPosition(xPos,yPos);
        window->draw(bitmap);
    }

    int getXPos() {
        return xPos;
    }
    int getYPos() {
        return yPos;
    }
    void setXPos(int xPos) {
        this->xPos = xPos;
    }
    void setYPos(int yPos) {
        this->yPos = yPos;
    }

    void move_right() {
        bitmap.setPosition(++xPos,yPos);
        //animate() => right movement animation
    }
    void move_left() {
        bitmap.setPosition(--xPos,yPos);
        //animate() => left movement animation
    }
    void move_up() {
        bitmap.setPosition(xPos,--yPos);
        //animate() => up movement animation
    }
    void move_down() {
        bitmap.setPosition(xPos,++yPos);
        //animate() => down movement animation
    }
};

class AnimatedObject
{
protected:
    std::vector<sf::Sprite> animationFrames;
    sf::Sprite defaultFrame;
    int timer = 0;
    int currentFrame = 0;
    int frameLimit;
public: 
    virtual void animate() {};
};

class SpriteObject : public BitmapObject, public AnimatedObject
{
public:
    SpriteObject(int xPos, int yPos, sf::Sprite bitmap) : BitmapObject(xPos, yPos, bitmap) {
        animationFrames.push_back(bitmap);
        bitmap.setPosition(xPos,yPos);
    }
    ~SpriteObject() {}

    void addAnimationFrame(sf::Sprite frame) {
        animationFrames.push_back(frame);
    }

    void setDefault(sf::Sprite frame) {
        defaultFrame = frame;
    }

    void setDefault(int frameIndex) {
        defaultFrame = animationFrames.at(frameIndex);
    }

    void resetAnimation() {
        timer = 0;
        currentFrame = 0;
    }

    void animate(int animationSpeed, bool useDefault, int defaultIndex) {
        if(animationSpeed) {
            frameLimit=FRAMERATE/animationSpeed;
            if(!timer) {
                bitmap = animationFrames.at(currentFrame);
                currentFrame++;
            }
            timer++;
            timer%=frameLimit;
            if(currentFrame>=animationFrames.size()) currentFrame = 0;
        }
        else {
            if(useDefault) {
                bitmap = defaultFrame;
            }
            else {
                if(defaultIndex<animationFrames.size() && defaultIndex>=0) bitmap = animationFrames.at(defaultIndex);
            }
            bitmap.setPosition(xPos,yPos);
            resetAnimation();
        }
    }
    
};

class Point
{
public:
    int x,y;
    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }
    ~Point() {}
};

class Background : public DrawableObject
{
    sf::Sprite* backgroundSprite;
public:
    Background(std::string backgroundFile) {
        backgroundSprite = BitmapHandler::createSprite(backgroundFile);
        backgroundSprite->setScale((float)WIDTH/backgroundSprite->getTexture()->getSize().x,(float)HEIGHT/backgroundSprite->getTexture()->getSize().y);
        backgroundSprite->setPosition(0,0);
    }

    void draw(sf::RenderWindow* window) {
        window->draw(*backgroundSprite);
    }

};

class Entity : public SpriteObject
{
    int xSize;
    int ySize;
    // sf::Image backgroundImage = BitmapHandler::createSprite("background.png")->getTexture()->copyToImage();
public:
    static std::vector<Point> aiPoints;
    Point destination = *new Point(xPos,yPos);
    int xDir, yDir;
    int entityType;
    int direction;
    HDC windowHandle;
    enum type {POINT, POWERUP, GHOST};
    enum Directions {RIGHT, LEFT, UP, DOWN};
    Entity(int xPos, int yPos, int entityType, HDC windowHandle) : SpriteObject(xPos, yPos, bitmap) {
        this->entityType = entityType;
        this->windowHandle = windowHandle;
        switch(entityType) {
            case POINT:
                xSize = ySize = 9;
                bitmap = *BitmapHandler::createSprite("point.png");
                break;
            case POWERUP:
                xSize = ySize = 35;
                bitmap = *BitmapHandler::createSprite("powerup.png");
                break;
            case GHOST:
                xSize = ySize = 58;
                bitmap = *BitmapHandler::createSprite("duszek.png");
                break;
            default:
                xSize = ySize = 0;
                break;
        }
    }
    

    sf::Color getPixelColor(int x, int y) {
        COLORREF temp = GetPixel(windowHandle,x,y);
        sf::Color color;
        color.r = GetRValue(temp);
        color.g = GetGValue(temp);
        color.b = GetBValue(temp);
        return color;
    }

    bool checkCollision(int direction) {
        switch(direction) {
            case UP:
                for(int i=bitmap.getPosition().x+1;i<=bitmap.getPosition().x+xSize-1;i+=7)
                    if(getPixelColor(i,bitmap.getPosition().y-1) == sf::Color(33,33,255))
                        return true;
                break;
            case DOWN:
                for(int i=bitmap.getPosition().x+1;i<=bitmap.getPosition().x+xSize-1;i+=7)
                    if(getPixelColor(i,bitmap.getPosition().y+ySize+1) == sf::Color(33,33,255))
                        return true;
                break;
            case LEFT:
                for(int i=bitmap.getPosition().y+1;i<=bitmap.getPosition().y+ySize-1;i+=7)
                    if(getPixelColor(bitmap.getPosition().x-1,i) == sf::Color(33,33,255))
                        return true;
                break;
            case RIGHT:
                for(int i=bitmap.getPosition().y+1;i<=bitmap.getPosition().y+ySize-1;i+=7)
                    if(getPixelColor(bitmap.getPosition().x+xSize+1,i) == sf::Color(33,33,255))
                        return true;
                break;
        }
        return false;
    }

    bool moveToPoint(int x, int y) {
        if(xPos > x)
            //if(!checkCollision(LEFT))
                move_left();
        if(xPos < x)
            //if(!checkCollision(RIGHT))
                move_right();
        if(yPos > y)
            //if(!checkCollision(UP))
                move_up(); 
        if(yPos < y)
            //if(!checkCollision(DOWN))
                move_down();
        if(xPos == x && yPos == y) return true;
        return false;
    }

    ~Entity() {}
    int getXSize() {
        return xSize;
    }
    int getYSize() {
        return ySize;
    }

    void draw(sf::RenderWindow* window) {
        bitmap.setPosition(xPos,yPos);
        bitmap.setScale((float)xSize/bitmap.getTexture()->getSize().x,(float)ySize/bitmap.getTexture()->getSize().y);
        window->draw(bitmap);
    }

    // AI

    double point2pointDist(Point A, Point B) {
        double temp = abs(A.x-B.x);
        double temp1 = abs(A.y-B.y);
        return sqrt(pow(temp,2)+pow(temp1,2));
    }

    Point searchForNearest(int xDir,int yDir,int x, int y) {
        Point dest(0,0);
        double destDist = 1000;
        double currentDist;
        //while(!dest.x) {
            for(std::vector<Point>::iterator it = Entity::aiPoints.begin(); it != Entity::aiPoints.end(); it++) {
                if(point2pointDist(Point(x,y),*it)>=destDist) continue;
                if((xDir == RIGHT && xPos<it->x && yPos == it->y) || (xDir == LEFT && xPos>it->x && yPos == it->y) || (yDir == UP && yPos>it->y && xPos == it->x) || (yDir == DOWN && yPos<it->y && xPos == it->x)) {
                    if(check2pointCollision(Point(xPos,yPos),*it,xDir,yDir)) continue;
                    currentDist = point2pointDist(Point(x,y),*it);
                    if(currentDist<destDist) {
                        destDist = currentDist;
                        dest = *it;
                    }
                }
            }
            if(!dest.x) {
                if(xDir == RIGHT) xDir = LEFT;
                else xDir = RIGHT;
                if(yDir == UP) yDir = DOWN;
                else yDir = UP;
                destDist = 1000;
                for(std::vector<Point>::iterator it = Entity::aiPoints.begin(); it != Entity::aiPoints.end(); it++) {
                    if(point2pointDist(Point(x,y),*it)>=destDist) continue;
                    if((xDir == RIGHT && xPos<it->x && yPos == it->y) || (xDir == LEFT && xPos>it->x && yPos == it->y) || (yDir == UP && yPos>it->y && xPos == it->x) || (yDir == DOWN && yPos<it->y && xPos == it->x)) {
                        if(check2pointCollision(Point(xPos,yPos),*it,xDir,yDir)) continue;
                        currentDist = point2pointDist(Point(x,y),*it);
                        if(currentDist<destDist) {
                            destDist = currentDist;
                            dest = *it;
                        }
                    }
                }
            }
        //}
        //if(point2pointDist(Point(x,y),Point(xPos,yPos))<point2pointDist(dest,Point(xPos,yPos))) return Point(x,y);
        return dest;
    }
    
    bool check2pointCollision(Point A, Point B, int xDir, int yDir) {
        int xRay;
        int yRay;
        if(A.y == B.y) {
            xRay = 4;
            if(A.x>B.x) std::swap(A,B);
            // modulo = B.x%A.x;
            // modulo %= xRay;
            // A.x += modulo;

            while(A.x < B.x) {
            if(getPixelColor(A.x,A.y) == sf::Color(33,33,255))
                return true;
            A.x+=xRay;
        }
        }
        if(A.x == B.x) {
            yRay = 4;
            if(A.y>B.y) std::swap(A,B);
            // modulo = B.y%A.y;
            // modulo %= yRay;
            // A.y += modulo;
            // std::cout << modulo << '\n';

            while(A.y < B.y) {
            if(getPixelColor(A.x,A.y) == sf::Color(33,33,255))
                return true;
            A.y+=yRay;
        }
        }
        
        
        return false;
    }

    void FollowPlayer(int x, int y) {
        
        std::cout << destination.x << ',' << destination.y << "---" << xPos << ',' << yPos << '\n';
        if(moveToPoint(destination.x,destination.y)) {
            if(x-xPos>0) xDir = RIGHT;
            if(x-xPos<0) xDir = LEFT;
            if(y-yPos>0) yDir = DOWN;
            if(y-yPos<0) yDir = UP;
            destination = searchForNearest(xDir,yDir,x,y);
            //moveToPoint(destination.x,destination.y);
        }
    }
};

class World
{
public:
    static std::vector<Entity> entities;
    static int pointsCounter;

    World() {}

    static void GenerateCollectibles(std::string referenceFile, HDC windowHandle) {
        sf::Image file;
        file.loadFromFile(referenceFile);
        
        for(int i=1;i<file.getSize().x;i++) {
            for(int j=1;j<file.getSize().y;j++) {
                if(file.getPixel(i,j) == sf::Color(255,183,174) && file.getPixel(i-1,j) == sf::Color(0,0,0) && file.getPixel(i,j-1) == sf::Color(0,0,0)) {
                    entities.push_back(*new Entity(i*((float)WIDTH/file.getSize().x),j*((float)HEIGHT/file.getSize().y),Entity::type::POINT,windowHandle));
                    pointsCounter++;
                }
                if(file.getPixel(i,j) == sf::Color::Red && file.getPixel(i-1,j) == sf::Color::Black && file.getPixel(i,j-1) == sf::Color::Black) {
                    entities.push_back(*new Entity(i*((float)WIDTH/file.getSize().x),j*((float)HEIGHT/file.getSize().y),Entity::type::POWERUP,windowHandle));
                    pointsCounter++;
                }
                if(file.getPixel(i,j) == sf::Color::Green && file.getPixel(i-1,j) == sf::Color(0,0,0) && file.getPixel(i,j-1) == sf::Color(0,0,0)) {
                    Entity::aiPoints.push_back(*new Point((i*((float)WIDTH/file.getSize().x))-23,(j*((float)HEIGHT/file.getSize().y))-23));
                    std::cout << i*((float)WIDTH/file.getSize().x)-23 << ',' << j*((float)HEIGHT/file.getSize().y)-23 << '\n';
                }
                if(file.getPixel(i,j) == sf::Color::Blue && file.getPixel(i-1,j) == sf::Color::Black && file.getPixel(i,j-1) == sf::Color::Black) {
                    entities.push_back(*new Entity(i*((float)WIDTH/file.getSize().x)-23,j*((float)HEIGHT/file.getSize().y)-23,Entity::type::GHOST,windowHandle));
                }
            }
        }
    }

    static void DrawEntities(sf::RenderWindow* window) {
        for(std::vector<Entity>::iterator it = entities.begin(); it != entities.end(); it++) {
            it->draw(window);
        }
    }
};
int World::pointsCounter = 0;
std::vector<Entity> World::entities;
std::vector<Point> Entity::aiPoints;

class Player : public SpriteObject
{
    // sf::Texture playerTexture;
    // sf::Sprite player;
    // int xPos = 300, yPos = 200;
    int xSize = 64;
    int ySize = 64;
    HDC windowHandle;
    int moveDirection = NONE;
    std::vector<sf::Sprite> rightMovement;
    std::vector<sf::Sprite> leftMovement;
    std::vector<sf::Sprite> upMovement;
    std::vector<sf::Sprite> downMovement;
    enum Animations {NONE, RIGHT, LEFT, UP, DOWN};
    int currentAnimation = NONE;
public:
    int score = 0;
    Player(int xPos, int yPos, HDC windowHandle) : SpriteObject(xPos, yPos, bitmap) {
        this->windowHandle = windowHandle;
        // sf::Texture playerTexture;
        // if(!playerTexture.loadFromFile("player.png")) { std::cout << "texture missing"; }
        // bitmap.setTexture(playerTexture);
        bitmap = *BitmapHandler::createSprite("player.png");
        animationFrames = upMovement;
        //bitmap.setScale(48/1536,64/2048);
        //bitmap.setScale(0.06,0.06);
    }

    void addRightMovementFrame(std::string input) {
        rightMovement.push_back(*BitmapHandler::createSprite(input));
    }
    void addLeftMovementFrame(std::string input) {
        leftMovement.push_back(*BitmapHandler::createSprite(input));
    }
    void addUpMovementFrame(std::string input) {
        upMovement.push_back(*BitmapHandler::createSprite(input));
    }
    void addDownMovementFrame(std::string input) {
        downMovement.push_back(*BitmapHandler::createSprite(input));
    }

    sf::Color getPixelColor(int x, int y) {
        COLORREF temp = GetPixel(windowHandle,x,y);
        sf::Color color;
        color.r = GetRValue(temp);
        color.g = GetGValue(temp);
        color.b = GetBValue(temp);
        return color;
    }

    bool checkCollision(int direction) {
        switch(direction) {
            case UP:
                for(int i=bitmap.getPosition().x+1;i<=bitmap.getPosition().x+xSize-1;i++)
                    if(getPixelColor(i,bitmap.getPosition().y-1) == sf::Color(33,33,255))
                        return true;
                break;
            case DOWN:
                for(int i=bitmap.getPosition().x+1;i<=bitmap.getPosition().x+xSize-1;i++)
                    if(getPixelColor(i,bitmap.getPosition().y+ySize+1) == sf::Color(33,33,255))
                        return true;
                break;
            case LEFT:
                for(int i=bitmap.getPosition().y+1;i<=bitmap.getPosition().y+ySize-1;i++)
                    if(getPixelColor(bitmap.getPosition().x-1,i) == sf::Color(33,33,255))
                        return true;
                break;
            case RIGHT:
                for(int i=bitmap.getPosition().y+1;i<=bitmap.getPosition().y+ySize-1;i++)
                    if(getPixelColor(bitmap.getPosition().x+xSize+1,i) == sf::Color(33,33,255))
                        return true;
                break;
        }
        return false;
    }

    void checkCollision(std::vector<Entity>::iterator entity, sf::RenderWindow* window) {
        if(entity->entityType == Entity::type::POINT) {
            if(xPos+(xSize*1/3) < entity->getXPos()+entity->getXSize() && xPos+xSize-(xSize*1/3) > entity->getXPos() && yPos+(ySize*1/3) < entity->getYPos()+entity->getYSize() && yPos+ySize-(ySize*1/3) > entity->getYPos()) {
                World::entities.erase(entity);
                World::pointsCounter--;
                score+=10;
            }
        }
        if(entity->entityType == Entity::type::POWERUP) {
            if(xPos+(xSize*1/3) < entity->getXPos()+entity->getXSize() && xPos+xSize-(xSize*1/3) > entity->getXPos() && yPos+(ySize*1/3) < entity->getYPos()+entity->getYSize() && yPos+ySize-(ySize*1/3) > entity->getYPos()) {
                World::entities.erase(entity);
                World::pointsCounter--;
                score+=50;
            }
        }
        if(entity->entityType == Entity::type::GHOST) {
            if(xPos < entity->getXPos()+entity->getXSize() && xPos+xSize > entity->getXPos() && yPos < entity->getYPos()+entity->getYSize() && yPos+ySize > entity->getYPos()) {
                window->close();
                MessageBoxA(NULL, NULL, "YOU LOST!!!!", MB_OK | MB_ICONWARNING);
            }
        }
    }

    // void move_right() {
    //     bitmap.setPosition(++++xPos,yPos);
    //     //animate() => right movement animation
    // }
    // void move_left() {
    //     bitmap.setPosition(----xPos,yPos);
    //     //animate() => left movement animation
    // }
    // void move_up() {
    //     bitmap.setPosition(xPos,----yPos);
    //     //animate() => up movement animation
    // }
    // void move_down() {
    //     bitmap.setPosition(xPos,++++yPos);
    //     //animate() => down movement animation
    // }
    void playerMovement() {
        if(moveDirection == UP && !checkCollision(UP)) {
                if(currentAnimation != UP) {
                    currentAnimation = UP;
                    animationFrames = upMovement;
                    resetAnimation();
                }
                animate(ANIMATIONSPEED,0,0);
                move_up();
            }
        else if(moveDirection == DOWN && !checkCollision(DOWN)) {
            if(currentAnimation != DOWN) {
                    currentAnimation = DOWN;
                    animationFrames = downMovement;
                    resetAnimation();
                }
                animate(ANIMATIONSPEED,0,0);
                move_down();
        }
        else if(moveDirection == LEFT && !checkCollision(LEFT)) {
            if(currentAnimation != LEFT) {
                    currentAnimation = LEFT;
                    animationFrames = leftMovement;
                    resetAnimation();
                }
                animate(ANIMATIONSPEED,0,0);
                move_left();
        }
        else if(moveDirection == RIGHT && !checkCollision(RIGHT)) {
            if(currentAnimation != RIGHT) {
                    currentAnimation = RIGHT;
                    animationFrames = rightMovement;
                    resetAnimation();
                }
                animate(ANIMATIONSPEED,0,0);
                move_right();
        }
        else {
            currentAnimation = NONE;
            animate(0,0,1);
        }
    }
    void inputs() {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveDirection = UP;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveDirection = DOWN;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveDirection = LEFT;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveDirection = RIGHT;
    }

    void draw(sf::RenderWindow* window) {
        inputs();
        playerMovement();
        for(std::vector<Entity>::iterator it = World::entities.begin(); it != World::entities.end(); it++) {
            checkCollision(it,window);
            if(it->entityType == Entity::type::GHOST) it->FollowPlayer(xPos,yPos);
            if(it == World::entities.end())
                break;
        }
        //bitmap.setScale(0.06,0.06);
        bitmap.setScale((float)xSize/bitmap.getTexture()->getSize().x,(float)ySize/bitmap.getTexture()->getSize().y);
        window->draw(bitmap);
        if(!World::pointsCounter) {
                std::string tempMessage = "Your final score:\n";
                tempMessage.append(std::to_string(score));
                window->close();
                MessageBoxA(NULL, tempMessage.c_str(), "YOU WON!!!!", MB_OK | MB_ICONINFORMATION);
        }
    }
};

class Point2D : public DrawableObject, public TransformableObject
{
    int xCoord;
    int yCoord;
    public:
    Point2D();
    Point2D(int x, int y);
    int getX();
    int getY();
    void setX(int);
    void setY(int);
    void draw(sf::RenderWindow* window);
    void translate(int tx, int ty);
    void rotate(int x0, int y0, int alfa);
    void scale(int x0, int y0, float k);
};

class PrimitiveRenderer
{
    std::vector<DrawableObject*> objects;
    sf::RenderWindow* window;
    public:
    PrimitiveRenderer(sf::RenderWindow* window) {
        this->window = window;
    }
    void drawRect(int posX, int posY, int sizeX, int sizeY) {
        sf::RectangleShape shape;
        shape.setPosition(sf::Vector2f(posX, posY));
        shape.setSize(sf::Vector2f(sizeX, sizeY));
        window->draw(shape);
    }

    void drawCircleManual(int x0, int y0, float radius) {
        sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(1,1));
        int x,y;
        float temp;
        for(int alfa=0;alfa<=90;alfa++) {
            temp = alfa*3.14159/180;
            x = radius*cosf(temp);
            y = radius*sinf(temp);
            drawRect(x0+x,y0-y,1,1);
            drawRect(x0+x,y0+y,1,1);
            drawRect(x0-x,y0-y,1,1);
            drawRect(x0-x,y0+y,1,1);
        }
    }

    void drawElipseManual(int x0, int y0, float radiusX, float radiusY) {
        sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(1,1));
        int x,y;
        float temp;
        for(int alfa=0;alfa<=90;alfa++) {
            temp = alfa*3.14159/180;
            x = radiusX*cosf(temp);
            y = radiusY*sinf(temp);
            drawRect(x0+x,y0-y,1,1);
            drawRect(x0+x,y0+y,1,1);
            drawRect(x0-x,y0-y,1,1);
            drawRect(x0-x,y0+y,1,1);
        }
    }

    void drawCircle(int posX, int posY, float radius) {
        sf::CircleShape shape;
        shape.setPosition(sf::Vector2f(posX, posY));
        shape.setRadius(radius);
        window->draw(shape);
    }

    void drawLine(int x0, int y0, int x1, int y1) {
        sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(1,1));
        float dx = x1-x0;
        float dy = y1-y0;
        float yi = y0;
        float xi = x0;
        float m = dy/dx;
        int temp;
        
        if((abs(m))>1) {
            if(y0 < y1) {
                temp = x0;
                x0 = x1;
                x1 = temp;
                temp = y0;
                y0 = y1;
                y1 = temp;
                yi = y0;
                xi = x0;
                dx = x1-x0;
                dy = y1-y0;
            }
            m = -dx/dy;
            for(int yit=y0;yit>=y1;yit--) {
                shape.setPosition(sf::Vector2f(round(xi),yit));
                xi+=m;
                window->draw(shape);
            }
        }
        else {
            if((x0 > x1)) {
                temp = x0;
                x0 = x1;
                x1 = temp;
                temp = y0;
                y0 = y1;
                y1 = temp;
                yi = y0;
                xi = x0;
                dx = x1-x0;
                dy = y1-y0;
                m = dy/dx;
            }
            for(int xit=x0;xit<=x1;xit++) {
                shape.setPosition(sf::Vector2f(xit,round(yi)));
                yi+=m;
                window->draw(shape);
            }
        }
    }

    void drawLine(Point2D *a, Point2D *b, Point2D *c) {
        drawLine(a->getX(),a->getY(),b->getX(),b->getY());
        drawLine(b->getX(),b->getY(),c->getX(),c->getY());
    }

    void drawPolygon(Point2D tab[], int n) {
        for(int i=0;i<n-1;i++) {
            drawLine(tab[i].getX(),tab[i].getY(),tab[i+1].getX(),tab[i+1].getY());
        }
        drawLine(tab[n-1].getX(),tab[n-1].getY(),tab[0].getX(),tab[0].getY());
    }

    void boundry_fill(HDC temp, Point2D P,sf::Color fill_color,sf::Color boundry_color) {
        
        // HDC temp = GetDC(window->getSystemHandle());
        
        // COLORREF fill_colorWin;
        // fill_colorWin = fill_colorWin | fill_color.r;
        // fill_colorWin<<8;
        // fill_colorWin = fill_colorWin | fill_color.g;
        // fill_colorWin<<8;
        // fill_colorWin = fill_colorWin | fill_color.b;
        // COLORREF boundry_colorWin;
        // boundry_colorWin = boundry_colorWin | boundry_color.r;
        // boundry_colorWin<<8;
        // boundry_colorWin = boundry_colorWin | boundry_color.g;
        // boundry_colorWin<<8;
        // boundry_colorWin = boundry_colorWin | boundry_color.b;

        // -------------------------Iteracja

        // COLORREF pixelWin;
        // sf::Color pixel;

        // std::stack<Point2D> stos;
        // stos.push(P);
        // while(!stos.empty()) {
        //     P = stos.top();
        //     stos.pop();
        //     pixelWin = GetPixel(temp,P.getX(),P.getY());
        //     pixel = sf::Color(GetRValue(pixelWin),GetGValue(pixelWin),GetBValue(pixelWin),255);
        //     if(pixel == fill_color) continue;
        //     if(pixel == boundry_color) continue;
            
        //     //shape.setPosition(sf::Vector2f(P.getX(),P.getY()));
        //     SetPixel(temp,P.getX(),P.getY(),0x000000FF);
        //     stos.push(Point2D(P.getX(),P.getY()-1));
        //     stos.push(Point2D(P.getX(),P.getY()+1));
        //     stos.push(Point2D(P.getX()-1,P.getY()));
        //     stos.push(Point2D(P.getX()+1,P.getY()));
        // }

        // ------------------------------

        //ReleaseDC(window->getSystemHandle(), temp);
        //std::cout << "done\n";

        //-------------------Rekurencja

        COLORREF pixelWin;
        sf::Color pixel;

        pixelWin = GetPixel(temp,P.getX(),P.getY());
        pixel = sf::Color(GetRValue(pixelWin),GetGValue(pixelWin),GetBValue(pixelWin),255);

        if(pixel != boundry_color && pixel != fill_color)
        {
            SetPixel(temp,P.getX(),P.getY(),0x000000FF);
            boundry_fill(temp,Point2D(P.getX() + 1, P.getY()), fill_color, boundry_color);
            boundry_fill(temp,Point2D(P.getX(), P.getY() + 1), fill_color, boundry_color);
            boundry_fill(temp,Point2D(P.getX() - 1, P.getY()), fill_color, boundry_color);
            boundry_fill(temp,Point2D(P.getX(), P.getY() - 1), fill_color, boundry_color);
        }

        //-------------------------------
    }

    void render() {
        if(!objects.empty()) {
            for(std::vector<DrawableObject*>::iterator it = objects.begin(); it != objects.end();it++) {
                (*it)->draw(window);
            }
        }
    }

    void addToRenderer(DrawableObject *newObject) {
        objects.push_back(newObject);
    }
};

class RectangleShape2D : public ShapeObject
{
    int posX;
    int posY;
    int sizeX;
    int sizeY;
    PrimitiveRenderer renderer = NULL;
public:
    RectangleShape2D() {}
    RectangleShape2D(PrimitiveRenderer renderer, int posX, int posY, int sizeX, int sizeY) {
        this->renderer=renderer;
        this->posX=posX;
        this->posY=posY;
        this->sizeX=sizeX;
        this->sizeY=sizeY;
    }
    ~RectangleShape2D() {}
    void draw(sf::RenderWindow* window) override {
        renderer.drawRect(posX, posY, sizeX, sizeY);
    }
    
    void translate(int tx, int ty) override {}
    void rotate(int x0, int y0, int alfa) override {}
    void scale(int x0, int y0, float k) override {}
};

class CircleShape2D : public ShapeObject
{
    int x0;
    int y0;
    float radius;
    PrimitiveRenderer renderer = NULL;
public:
    CircleShape2D() {}
    CircleShape2D(PrimitiveRenderer renderer, int x0, int y0, float radius) {
        this->renderer=renderer;
        this->x0=x0;
        this->y0=y0;
        this->radius=radius;
    }
    ~CircleShape2D() {}
    void draw(sf::RenderWindow* window) override {
        renderer.drawCircleManual(x0,y0,radius);
    }

    void translate(int tx, int ty) override {}
    void rotate(int x0, int y0, int alfa) override {}
    void scale(int x0, int y0, float k) override {}
};






    Point2D::Point2D() {

    }
    Point2D::Point2D(int x, int y) {
        xCoord = x;
        yCoord = y;
    }
    int Point2D::getX() {
        return xCoord;
    }
    int Point2D::getY() {
        return yCoord;
    }
    void Point2D::setX(int newX) {
        xCoord = newX;
    }
    void Point2D::setY(int newY) {
        yCoord = newY;
    }
    void Point2D::draw(sf::RenderWindow* window) {
        sf::RectangleShape shape;
        shape.setPosition(xCoord,yCoord);
        shape.setSize(sf::Vector2f(1,1));
        window->draw(shape);
    }
    void Point2D::translate(int tx, int ty) {
        xCoord += tx;
        yCoord -= ty;
    }
    void Point2D::rotate(int x0, int y0, int alfa) {
        float temp = alfa*3.14159f/180;
        float temp2;

        temp2 = x0+(xCoord-x0)*std::cos(temp)-(yCoord-y0)*std::sin(temp);
        yCoord = y0+(xCoord-x0)*std::sin(temp)+(yCoord-y0)*std::cos(temp);
        xCoord = temp2;
    }
    void Point2D::scale(int x0, int y0, float k) {
        xCoord = xCoord*k+(1-k)*x0;
        yCoord = yCoord*k+(1-k)*y0;
    }

class LineSegment : public DrawableObject, public TransformableObject
{
    Point2D* a;
    Point2D* b;
    public:
    LineSegment(int ax, int ay, int bx, int by) {
        a = new Point2D(ax, ay);
        b = new Point2D(bx, by);
    }
    ~LineSegment() {
        free(a);
        free(b);
    }
    
    Point2D getA() {
        return *a;
    }
    Point2D getB() {
        return *b;
    }
    void setA(Point2D newA) {
        a->setX(newA.getX());
        a->setY(newA.getY());
    }
    void setB(Point2D newB) {
        b->setX(newB.getX());
        b->setY(newB.getY());
    }
    void draw(sf::RenderWindow* window) {
        PrimitiveRenderer renderer(window);
        renderer.drawLine(a->getX(),a->getY(),b->getX(),b->getY());
    }

    void translate(int tx, int ty) {
        a->setX(a->getX()+tx);
        a->setY(a->getY()-ty);
        b->setX(b->getX()+tx);
        b->setY(b->getY()-ty);
    }
    void rotate(int x0, int y0, int alfa) {
        a->rotate(x0,y0,alfa);
        b->rotate(x0,y0,alfa);
    }
    void scale(int x0, int y0, float k) {
        a->scale(x0,y0,k);
        b->scale(x0,y0,k);
    }

};