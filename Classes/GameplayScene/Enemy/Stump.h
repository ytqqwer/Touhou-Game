#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ENEMY_STUMP_H
#define ENEMY_STUMP_H

#include "GameplayScene/Enemy/Enemy.h"

class Stump : public Enemy
{
public:
    virtual bool init(const std::string& tag);

public:
    void horizontallyAccelerate(float dt);
    void decreaseHp(int damage);

private:
    float timeAccumulation = 0;

    class Stand : public State<Enemy>
    {
    public:
        static Stand* getInstance();
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

    class Hit : public State<Enemy>
    {
    public:
        static Hit* getInstance();
        void Enter(Enemy*);
        void Exit(Enemy*);
        void defaultChangeState(Enemy*);
    };

    class Down : public State<Enemy>
    {
    public:
        static Down* getInstance();
        void Enter(Enemy*);
        void Exit(Enemy*);
        void defaultChangeState(Enemy*);
    };
};
#endif
