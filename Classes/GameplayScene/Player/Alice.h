#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef PLAYER_ALICE_H
#define PLAYER_ALICE_H

#include "GameplayScene/Player/Player.h"

class Alice : public Player
{
public:
    virtual bool init(const std::string& tag);

    static Alice* create(const std::string& tag)
    {
        Alice* pRet = new (std::nothrow) Alice();
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
    void horizontallyAccelerate(float dt);
    void jump();
    void dash();

    //更新角色状态
    void updatePlayerStatus();
    void regainDashCounts(float dt);

private:
    float regainDashTimeAccumulation = 0;
};

#endif
