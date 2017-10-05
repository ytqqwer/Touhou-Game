#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ENEMY_FROG_H
#define ENEMY_FROG_H

#include "GameplayScene/Enemy/Enemy.h"

class Frog : public Enemy
{
public:
    virtual bool init(std::string tag);

public:
    void jump();
    void decreaseHp(int damage);

    void alertMode(float dt);
    void patrolMode(float dt);

    void autoChangeDirection(float dt);
    void autoSwitchAnimation(float dt);

protected:
    Animation* idleAnimation;
    Animation* jumpAnimation;
    Animation* fallAnimation;
};

class FrogAlertState : public State<Enemy>
{
public:
    static FrogAlertState* getInstance();
    void Enter(Enemy*);
    void Exit(Enemy*);
    void changeToState(Enemy*);
    FrogAlertState() {}
    ~FrogAlertState() {}
};

class FrogPatrolState : public State<Enemy>
{
public:
    static FrogPatrolState* getInstance();
    void Enter(Enemy*);
    void Exit(Enemy*);
    void changeToState(Enemy*);
    FrogPatrolState() {}
    ~FrogPatrolState() {}
};

#endif
