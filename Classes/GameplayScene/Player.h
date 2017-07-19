#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_PLAYER_H
#define GAMEPLAY_PLAYER_H

#include "cocos2d.h"
#include "common.h"

#define MAX_SPEED 400.0
#define ACCELERATE_TIME 1.0

using namespace cocos2d;

class Player : public Node
{
public:
    virtual bool init() override;
    CREATE_FUNC(Player);

public:
    void playerRun(float dt);
    void playerJump();
    void playerDash();

    void resetAction(Node* node);

    void regainDashCounts(float dt);

    String playerDirection = "right";
    int jumpCounts = 2;
    int dashCounts = 2;

private:
    Sprite* playerSprite; //角色精灵
    Animate* playerAnim;
    Texture2D* jumpTexture;

    Animation* p1Animation;
    Animation* jumpAnimation;
    Animation* dashAnimation;
};

#endif
