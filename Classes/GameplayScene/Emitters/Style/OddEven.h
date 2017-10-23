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
    /* 自机默认弹幕 */
    APP_CREATE_STYLE2(OddEven);
    OddEven(Node** target);

    /* 自机定制弹幕 */
    APP_CREATE_STYLE3(OddEven);
    OddEven(const StyleConfig& sc, Node** target);

    /* 调度器 */
    void startShoot();
    void stopShoot();
    void shootBullet(float dt);
    void spawnBullet();

private:
    Node** target;
    Vec2 targetPos; //角色位置
    float angle;    //夹角
    int counter;    //计数器

    float timeAccumulation;
    float elapsed;
    unsigned int spawnBulletCycleTimes; //发射函数循环次数
};

#endif // !ODDEVEN_H
