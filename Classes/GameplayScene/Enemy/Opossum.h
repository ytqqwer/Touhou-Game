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

    static Opossum* create(std::string tag)
    {
        Opossum* pRet = new (std::nothrow) Opossum();
        if (pRet && pRet->init(tag)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

public:
    void run();  //敌人移动
    void jump(); //敌人跳跃
    void AI(float dt);
    void decreaseHp(Node* node);
    void startSchedule(Player*& player);
};

#endif
