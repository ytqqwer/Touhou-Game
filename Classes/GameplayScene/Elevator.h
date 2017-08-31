#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "cocos2d.h"

#include "GameplayScene/Enemy/Enemy.h"
#include "GameplayScene/Player/Player.h"

USING_NS_CC;

class Elevator : public Node
{
public:
    bool init();

    static Elevator* create()
    {
        Elevator* pRet;
        pRet = new (std::nothrow) Elevator();
        if (pRet && pRet->init()) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    Vec2 prePosition;
    void moveTogether(float dt);

    vector<Node*> passengers;
    void addPassenger(Player* target);
    void addPassenger(Enemy* target);
    void removePassenger(Player* target);
    void removePassenger(Enemy* target);
};

#endif
