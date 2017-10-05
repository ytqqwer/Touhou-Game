#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef PLAYER_REIMU_H
#define PLAYER_REIMU_H

#include "GameplayScene/Player/Player.h"

class Reimu : public Player
{
public:
    virtual bool init(std::string tag);

    static Reimu* create(std::string tag)
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
    void playerRun(float dt);
    void playerJump();
    void playerDash();

    //更新角色状态
    void updatePlayerStatus();
    void regainDashCounts(float dt);

private:
    float regainDashTimeAccumulation = 0;
};

#endif
