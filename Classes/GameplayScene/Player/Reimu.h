#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef PLAYER_REIMU_H
#define PLAYER_REIMU_H

#include "GameplayScene/Player/Player.h"

class Reimu : public Player
{
public:
    virtual bool init(const std::string& tag);

    static Reimu* create(const std::string& tag)
    {
        Reimu* pRet = new (std::nothrow) Reimu();
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
