#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ENEMY_FROG_H
#define ENEMY_FROG_H

#include "GameplayScene/Enemy/Enemy.h"

class Frog : public Enemy
{
public:
    virtual bool init(std::string tag);

    static Frog* create(std::string tag)
    {
        Frog* pRet = new (std::nothrow) Frog();
        if (pRet && pRet->init(tag)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

public:
    void run();
    void jump();
    void AI(float dt);
    void decreaseHp(Node* node);
    void startSchedule(Player*& player);
};

#endif
