#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ODDEVEN_H
#define ODDEVEN_H

#include "EmitterStyle.h"

/* 奇偶数自机狙型弹幕
   特点：敌人，自机  */

class OddEven : public EmitterStyle
{
public:
    /* 默认弹幕 */
    APP_CREATE_STYLE2(OddEven);
    OddEven(Node** target);

    /* 定制弹幕 */
    APP_CREATE_STYLE3(OddEven);
    OddEven(const StyleConfig& sc, Node** target);

    /* 调度器 */
    void createBullet();
    void stopSchedule();
    void shootBullet(float dt);

private:
    Node** target;
    Vec2 targetPos; //角色位置
    float angle;    //夹角
    int counter;    //计数器
};

#endif // !ODDEVEN_H
