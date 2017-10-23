#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef LASER_H
#define LASER_H

#include "EmitterStyle.h"

/*  激光型弹幕：预警后射出攻击光线
    特点：玩家，敌人，自机 */

class Laser : public EmitterStyle
{
public:
    /* 自机默认弹幕 */
    APP_CREATE_STYLE2(Laser);
    Laser(Node** target);

    /* 自机定制弹幕 */
    APP_CREATE_STYLE3(Laser);
    Laser(const StyleConfig& sc, Node** target);

    /* 角色默认弹幕 */
    APP_CREATE_STYLE4(Laser);
    Laser(Direction* direction);

    /* 角色定制弹幕 */
    APP_CREATE_STYLE5(Laser);
    Laser(const StyleConfig& sc, Direction* direction);

    /* 调度器 */
    void startShoot();
    void stopShoot();
    void shootBullet(float dt);
    void spawnBullet();

private:
    bool isPlayer;
    Node** target;
    Direction* direction;
    int counter;

    float timeAccumulation;
    float elapsed;
    unsigned int spawnBulletCycleTimes; //发射函数循环次数
};
#endif // !LASER_H
