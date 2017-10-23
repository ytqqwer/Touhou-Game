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

private:
    class Patrol : public State<Enemy>
    {
    public:
        static Patrol* getInstance();
        void Enter(Enemy*);
        void Exit(Enemy*);
        void defaultChangeState(Enemy*);
    };

    class Alert : public State<Enemy>
    {
    public:
        static Alert* getInstance();
        void Enter(Enemy*);
        void Exit(Enemy*);
        void defaultChangeState(Enemy*);
    };

    class Jump : public State<Enemy>
    {
    public:
        static Jump* getInstance();
        void Enter(Enemy*);
        void Exit(Enemy*);
        void defaultChangeState(Enemy*);
    };

    class Fall : public State<Enemy>
    {
    public:
        static Fall* getInstance();
        void Enter(Enemy*);
        void Exit(Enemy*);
        void defaultChangeState(Enemy*);
    };
};

#endif
