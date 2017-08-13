#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_ENEMY_H
#define GAMEPLAY_ENEMY_H

#include "GameplayScene/Player/Player.h"

#include "GameData/EnemyData.h"
#include "GameData/GameData.h"
#include "GameplayScene/Enemy/Enemy.h"
#include "GameplayScene/common.h"

#include "cocos2d.h"

using namespace cocos2d;

typedef enum { Patrol = 1, Alert = 2 } EnemyState;

class Enemy : public Node
{
public:
    virtual bool init(std::string tag) = 0;
    static Enemy* create(std::string tag);

public:
    virtual void run() = 0;  //敌人移动
    virtual void jump() = 0; //敌人跳跃
    virtual void AI(float dt) = 0;
    virtual void decreaseHp(Node* node) = 0;

    void startSchedule(Player*& player);

public:
    std::string enemyTag;
    Player** curPlayer;

    int hp;
    bool _canJump = false;

    PhysicsBody* body;
    EnemyState curState = EnemyState::Patrol;

protected:
    Sprite* enemySprite; //敌人精灵

    std::string enemyTextureName;
    Animation* enemyAnimation;
};

#endif
