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

    ~Enemy()
    {
        delete modeStateMachine;
        delete animateStateMachine;
    }

public:
    virtual void decreaseHp(int damage) = 0;
    virtual void autoChangeDirection(float dt) = 0;
    virtual void setTarget(Player*& player);
    virtual void resetJump();

public:
    std::string enemyTag;
    Node** curTarget;

    int hp;
    bool _canJump = false;

    Direction enemyDirection = Direction::LEFT;

    StateMachine<Enemy>* modeStateMachine;
    StateMachine<Enemy>* animateStateMachine;

protected:
    PhysicsBody* body;
    Sprite* enemySprite;

    Action* currentAnimateAction;
    Animation* standAnimation;
    Animation* runAnimation;
    Animation* jumpAnimation;
    Animation* preJumpAnimation;
    Animation* fallAnimation;
    Animation* preFallAnimation;
};

#endif
