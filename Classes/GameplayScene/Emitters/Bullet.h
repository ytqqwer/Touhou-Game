#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef BULLET_H
#define BULLET_H

#include "cocos2d.h"
USING_NS_CC;

class Bullet : public Sprite
{
public:
private:
    int kind;  //种类
    int type;  //类型
    int life;  //生命
    int human; //伤害
};

#endif
