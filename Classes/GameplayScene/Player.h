#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_PLAYER_H
#define GAMEPLAY_PLAYER_H

#include "GameplayScene/AnimateManager.h"
#include "cocos2d.h"

#define MAX_SPEED 400.0
#define ACCELERATE_TIME 1.0

using namespace cocos2d;

class Player : public Node
{
public:
    virtual bool init(std::string tag);

    static Player* create(std::string tag)
    {
        Player* pRet = new (std::nothrow) Player();
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

    void resetAction(Node* node);
    void regainDashCounts(float dt);

public:
    String playerDirection = "right";
    int jumpCounts = 2;
    int dashCounts = 2;

private:
    AnimateManager* animateManager;
    Sprite* playerSprite;

    std::string playerTextureName;
    Animation* runAnimation;
    Animation* jumpAnimation;
    Animation* dashAnimation;
};

#endif
