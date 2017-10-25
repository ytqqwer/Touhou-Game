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
    virtual bool init(const std::string& tag);

public:
    void horizontallyAccelerate(float dt);
    void decreaseHp(int damage);

private:
    float timeAccumulation = 0;

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
};
#endif
