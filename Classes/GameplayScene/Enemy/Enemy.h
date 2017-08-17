#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_ENEMY_H
#define GAMEPLAY_ENEMY_H

#include "GameData/EnemyData.h"
#include "GameData/GameData.h"
#include "GameplayScene/Enemy/Enemy.h"
#include "GameplayScene/Player/Player.h"
#include "GameplayScene/common.h"

#include "cocos2d.h"
using namespace cocos2d;

typedef enum { Patrol = 1, Alert = 2 } EnemyActionMode;

class Enemy : public Node
{
public:
    virtual bool init(std::string tag) = 0;
    static Enemy* create(std::string tag);

public:
    virtual void run(float dt) = 0; //敌人移动
    virtual void jump() = 0;        //敌人跳跃

    virtual void decreaseHp(Node* node) = 0;

    virtual void switchMode() = 0;
    virtual void patrolMode(float dt) = 0;
    virtual void alertMode(float dt) = 0;

    //动作切换
    virtual void resetAction(Node* node);
    virtual void autoSwitchAnimation(float dt) = 0;
    virtual void autoChangeDirection(float dt) = 0;

    virtual void setTarget(Player*& player);

public:
    std::string enemyTag;
    Player** curPlayer;

    int hp;
    bool _canJump = false;

    PhysicsBody* body;
    EnemyActionMode curState = EnemyActionMode::Patrol;

    Direction enemyDirection = Direction::LEFT;

    //当前动作状态
    ActionState curAction = ActionState::Default;

protected:
    Sprite* enemySprite; //敌人精灵
    std::string enemyTextureName;
};

#endif
