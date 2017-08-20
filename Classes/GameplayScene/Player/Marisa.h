#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef PLAYER_MARISA_H
#define PLAYER_MARISA_H

#include "GameplayScene/Player/Player.h"

class Marisa : public Player
{
public:
    virtual bool init(std::string tag);

    static Marisa* create(std::string tag)
    {
        Marisa* pRet = new (std::nothrow) Marisa();
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
    void updatePlayerStatus(float dt);
};

#endif
