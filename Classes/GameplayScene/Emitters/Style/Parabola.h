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
    void createBullet();
    void stopSchedule();
    void shootBullet(float dt);

private:
    Direction* direction; //玩家方向
    int counter;          //计数器
};

#endif // !PARABOLA_H
