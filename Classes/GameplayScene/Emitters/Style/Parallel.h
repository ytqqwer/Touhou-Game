#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef PARALLEL_H
#define PARALLEL_H

#include "EmitterStyle.h"

/* 相对自身固定：指定方向平行弹幕
特点：玩家无自机，敌人自机 */

class Parallel : public EmitterStyle
{
public:
    /* 自机默认弹幕 */
    APP_CREATE_STYLE2(Parallel);
    Parallel(Node** target);

    /* 自机定制弹幕 */
    APP_CREATE_STYLE3(Parallel);
    Parallel(const StyleConfig& sc, Node** target);

    /* 角色默认弹幕 */
    APP_CREATE_STYLE4(Parallel);
    Parallel(Direction* direction);

    /* 角色定制弹幕 */
    APP_CREATE_STYLE5(Parallel);
    Parallel(const StyleConfig& sc, Direction* direction);

    /* 调度器 */
    void createBullet();
    void stopShoot();
    void shootBullet(float dt);

private:
    bool isPlayer;
    Direction* direction; //玩家方向
    Node** target;
    Vec2 targetPos; //角色位置
    int counter;    //计数器
};

#endif
