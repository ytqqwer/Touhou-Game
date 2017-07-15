#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "cocos2d.h"

using namespace cocos2d;

class Enemy : public Node
{
public:
    CREATE_FUNC(Enemy);

    virtual bool init() override;

public:
    void run();  //敌人移动
    void jump(); //敌人跳跃

    void AI(float dt);

    void decreaseHp(Node* node);

    bool _canJump = false;

private:
    Sprite* enemySprite; //敌人精灵

    Animate* enemyAnim;
    Texture2D* jumpTexture;

    int hp = 200;
    // bool _isAir = true;
    // bool _isLand = false;
};

#endif