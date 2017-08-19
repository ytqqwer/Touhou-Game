#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef EMITTER_H
#define EMITTER_H

#include "GameplayScene/common.h"
#include "StyleConfig.h"
#include "cocos2d.h"

USING_NS_CC;

class Emitter : public Node
{

public:
    /* 角色发射器 */
    static Emitter* create(Direction* direction);
    Emitter(Direction* direction);

    /* 敌人发射器 */
    static Emitter* create(Node** target);
    Emitter(Node** target);

    /* 创建弹幕 */
    int playStyle(const StyleConfig& sc);
    int playStyle(StyleType st);

    /* 暂停弹幕 */
    void pauseStyle(int styleTag);
    void pauseAllStyle();

    /* 恢复弹幕 */
    void resumeStyle(int styleTag);
    void resumeAllStyle();

    /* 销毁弹幕 */
    void stopStyle(int styleTag);
    void stopAllStyle();

private:
    bool isPlayer;
    Direction* direction;   //玩家方向
    Node** target;          //自机目标
    Map<int, Node*> styles; //弹幕容器
    int styleTag;           //弹幕计数
};

#endif
