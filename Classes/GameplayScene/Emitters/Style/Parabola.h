#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef PARABOLA_H
#define PARABOLA_H

#include "EmitterStyle.h"

/* 抛物线型弹幕
   特点：玩家，无自机 */

class Parabola : public EmitterStyle
{
public:
    /* 角色默认弹幕 */
    APP_CREATE_STYLE4(Parabola);
    Parabola(Direction* direction);

    /* 角色定制弹幕 */
    APP_CREATE_STYLE5(Parabola);
    Parabola(const StyleConfig& sc, Direction* direction);

    /* 调度器 */
    void startShoot();
    void stopShoot();
    void shootBullet(float dt);
    void spawnBullet();

private:
    Direction* direction;
    unsigned int counter; //计数器

    float timeAccumulation;
    float elapsed;
    unsigned int spawnBulletCycleTimes; //发射函数循环次数
};

#endif // !PARABOLA_H
