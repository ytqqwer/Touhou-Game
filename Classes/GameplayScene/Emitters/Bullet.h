#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef BULLET_H
#define BULLET_H

#include "StyleConfig.h"
#include "cocos2d.h"
USING_NS_CC;

class Bullet : public Sprite
{
public:
    static Bullet* create(const BulletConfig& bc);
    Bullet(const BulletConfig& bc);
    bool init();

private:
    BulletConfig bc;
};

#endif
