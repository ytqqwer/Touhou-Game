#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef SCATTER_H
#define SCATTER_H

#include "EmitterStyle.h"

/* 相对自身固定：主要为全方位弹，以自身为中心向着四面八方放出弹幕
   特点：玩家，敌人，无自机 */

class Scatter : public EmitterStyle
{
public:
    /* 无自机默认弹幕 */
    CREATE_FUNC(Scatter);
    Scatter();

    /* 无自机定制弹幕 */
    APP_CREATE_STYLE1(Scatter);
    Scatter(const StyleConfig& sc);

    /* 角色默认弹幕 */
    APP_CREATE_STYLE4(Scatter);
    Scatter(Direction* direction);

    /* 角色定制弹幕 */
    APP_CREATE_STYLE5(Scatter);
    Scatter(const StyleConfig& sc, Direction* direction);

    /* 调度器 */
    void createBullet();
    void stopSchedule();
    void shootBullet(float dt);

private:
    bool isPlayer;
    Direction* direction; //玩家方向
    int counter;          //计数器
};

#endif
