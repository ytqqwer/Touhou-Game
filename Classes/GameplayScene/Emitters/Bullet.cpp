#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Bullet.h"

Bullet*
Bullet::create(const BulletConfig& bc)
{
    Bullet* pRet = new Bullet(bc);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

Bullet::Bullet(const BulletConfig& bc)
{
    this->bc = bc;
}

bool
Bullet::init()
{
    //绑定刚体，设置掩码值
    return true;
}