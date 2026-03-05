// temporary header class for player
#pragma once
#include <SFML/System.hpp>

class Player
{
private:

    sf::Vector2f position;
    int health;
    int hunger;

public:

    Player()
    {
        position = { 0,0 };
        health = 100;
        hunger = 100;
    }

    void setPosition(float x, float y)
    {
        position = { x,y };
    }

    float getPositionX()
    {
        return position.x;
    }

    float getPositionY()
    {
        return position.y;
    }

    int getHealth()
    {
        return health;
    }

    int getHunger()
    {
        return hunger;
    }

    void setHealth(int h)
    {
        health = h;
    }

    void setHunger(int h)
    {
        hunger = h;
    }
};