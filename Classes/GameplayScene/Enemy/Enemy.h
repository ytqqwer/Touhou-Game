#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMEPLAY_ENEMY_H
#define GAMEPLAY_ENEMY_H

#include "GameplayScene/Player/Player.h"
#include "GameplayScene/State.h"
#include "GameplayScene/common.h"

#include "cocos2d.h"
using namespace cocos2d;

typedef enum { Patrol = 1, Alert = 2 } EnemyActionMode;

class Enemy : public Node
{
public:
    virtual bool init(std::string tag) = 0;

    static Enemy* create(std::string tag);

    ~Enemy() { delete stateMachine; }

public:
    virtual void decreaseHp(int damage) = 0;

    //动作切换
    virtual void resetAction(Node* node);
    virtual void autoSwitchAnimation(float dt) = 0;
    virtual void autoChangeDirection(float dt) = 0;

    virtual void setTarget(Player*& player);

    virtual void resetJump();

public:
    std::string enemyTag;
    Node** curTarget;

    int hp;
    bool _canJump = false;

    PhysicsBody* body;

    StateMachine<Enemy>* stateMachine;

    Direction enemyDirection = Direction::LEFT;

    //当前动作状态
    ActionState curActionState = ActionState::Default;

protected:
    Sprite* enemySprite; //敌人精灵
};

#endif
