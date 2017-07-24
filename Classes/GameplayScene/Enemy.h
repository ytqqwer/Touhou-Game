#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_ENEMY_H
#define GAMEPLAY_ENEMY_H

#include "GameplayScene/AnimateManager.h"
#include "GameplayScene/Player.h"
#include "cocos2d.h"

using namespace cocos2d;

class Enemy : public Node
{
public:
    virtual bool init(std::string tag);

    static Enemy* create(std::string tag)
    {
        Enemy* pRet = new (std::nothrow) Enemy();
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
    void run();  //敌人移动
    void jump(); //敌人跳跃

    void AI(float dt);
    void decreaseHp(Node* node);
    void startSchedule(Player*& player);

    Player** curPlayer;

    int hp;
    bool _canJump = false;
    // bool _isAir = true;
    // bool _isLand = false;

private:
    AnimateManager* animateManager;
    Sprite* enemySprite; //敌人精灵

    std::string enemyTextureName;
    Animation* enemyAnimation;
};

#endif
