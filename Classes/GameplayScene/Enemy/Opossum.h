#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ENEMY_OPOSSUM_H
#define ENEMY_OPOSSUM_H

#include "GameplayScene/Enemy/Enemy.h"

//负鼠，是负鼠目、负鼠科动物的通称。是一种比较原始的有袋类动物，主要产自拉丁美洲。负鼠是一种原始、低等的哺乳动物。
class Opossum : public Enemy
{
public:
    virtual bool init(std::string tag);

public:
    void run(float dt);
    void decreaseHp(int damage);

protected:
    class RunAnimation : public State<Enemy>
    {
    public:
        static RunAnimation* getInstance();
        void Enter(Enemy*);
        void Exit(Enemy*);
        void changeToState(Enemy*);
    };

private:
    float timeAccumulation = 0;
};

class OpossumAlertState : public State<Enemy>
{
public:
    static OpossumAlertState* getInstance();
    void Enter(Enemy*);
    void Exit(Enemy*);
    void changeToState(Enemy*);
};

class OpossumPatrolState : public State<Enemy>
{
public:
    static OpossumPatrolState* getInstance();
    void Enter(Enemy*);
    void Exit(Enemy*);
    void changeToState(Enemy*);
};

#endif
