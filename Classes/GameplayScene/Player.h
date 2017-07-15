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
    void playerRunLeft(float dt);
    void playerRunRight(float dt);
    void playerJump();

    void resetAction(Node* pNode);

    String attackDirection = "right";

private:
    Sprite* playerSprite; //角色精灵
    Animate* playerAnim;
    Texture2D* jumpTexture;

    Animation* p1Animation;
    Animation* jumpAnimation;

    // int hp = 100;
};

#endif
