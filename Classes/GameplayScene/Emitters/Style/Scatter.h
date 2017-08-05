#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef SCATTER_H
#define SCATTER_H

#include "EmitterStyle.h"

/* 相对敌机固定：主要为全方位弹，以敌机为中心向着四面八方放出弹幕 */

class Scatter : public EmitterStyle
{
public:
    /* 默认弹幕 */
    CREATE_FUNC(Scatter);
    Scatter();

    /* 定制弹幕 */
    APP_CREATE_STYLE2(Scatter);
    Scatter(const StyleConfig& sc);

    /* 调度器 */
    void createBullet();
    void shootBullet(float dt);

private:
    int counter;
};

#endif
