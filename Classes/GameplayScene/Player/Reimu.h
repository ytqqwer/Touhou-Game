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

    //切换攻击方式
    void changeAttackType(const std::string& startType);
    void stopAttackType(const std::string& stopType);

    //更新角色状态
    void updateStatus(float dt);
};

#endif
