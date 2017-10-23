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

class Enemy : public Node
{
public:
    virtual bool init(std::string tag) = 0;

    static Enemy* create(std::string tag);

    ~Enemy() { delete stateMachine; }

public:
    virtual void decreaseHp(int damage) = 0;
    virtual void autoChangeDirection(float dt);
    virtual void setTarget(Player*& player);
    virtual void setEmitter();
    virtual void resetJump();

public:
    std::string enemyTag;
    Node** curTarget;

    int hp;
    bool _canJump = false;

    Sprite* enemySprite;

    Emitter* emitter;

    Direction enemyDirection = Direction::LEFT;

    StateMachine<Enemy>* stateMachine;

protected:
    PhysicsBody* body;

    Action* currentAnimateAction;
    Animation* standAnimation;
    Animation* runAnimation;
    Animation* jumpAnimation;
    Animation* preJumpAnimation;
    Animation* fallAnimation;
    Animation* preFallAnimation;
};

#endif
