#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ODDEVEN_H
#define ODDEVEN_H

#include "EmitterStyle.h"

/* 奇偶数自机狙型弹幕 */

class OddEven : public EmitterStyle
{
public:
    /* 默认弹幕 */
    APP_CREATE_STYLE1(OddEven);
    OddEven(Node* target);

    /* 定制弹幕 */
    APP_CREATE_STYLE3(OddEven);
    OddEven(const StyleConfig& sc, Node* target);

    /* 调度器 */
    void createBullet();
    void shootBullet(float dt);

private:
    Node* target;
    Vec2 targetPos;
    int counter;
};

#endif // !ODDEVEN_H
