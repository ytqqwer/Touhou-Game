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
    void run(float dt);
    void jump();
    void decreaseHp(Node* node);

    void switchMode();
    void alertMode(float dt);
    void patrolMode(float dt);

    void autoChangeDirection(float dt);
    void autoSwitchAnimation(float dt);

protected:
    Animation* idleAnimation;
    Animation* jumpAnimation;
    Animation* fallAnimation;
};

#endif
