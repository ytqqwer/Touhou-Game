#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ENEMY_SAKUYA_H
#define ENEMY_SAKUYA_H

#include "GameplayScene/Enemy/Enemy.h"

class Sakuya : public Enemy
{
public:
    virtual bool init(std::string tag);

public:
    void run(float dt);
    void decreaseHp(int damage);

private:
    float timeAccumulation = 0;
    float damageAccumulation = 0;
};

#endif
