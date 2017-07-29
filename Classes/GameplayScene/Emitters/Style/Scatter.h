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
    APP_CREATE_STYLE1(Scatter)

    /* 定制弹幕 */
    Scatter(const StyleConfig& sc, Node* character);

    /* 默认弹幕 */
    Scatter(Node* character);

    /* 调度器 */
    void createBullet();
    void shootBullet(float dt);

private:
    Node* character;
    int count;
};

#endif
