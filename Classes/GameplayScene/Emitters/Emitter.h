#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef EMITTER_H
#define EMITTER_H

#include "Style/EmitterStyle.h"
#include "Style/Scatter.h"
#include "StyleConfig.h"
#include "cocos2d.h"

USING_NS_CC;

class Emitter : public Node
{

public:
    /* 无自机发射器 */
    static Emitter* create(Node* character);
    Emitter(Node* character);

    /* 自机发射器 */
    static Emitter* create(Node* character, Node* target);
    Emitter(Node* character, Node* target);

    /* 创建弹幕 */
    int playStyle(const StyleConfig& sc);

    /* 暂停弹幕 */
    void pauseStyle(int tag);
    void pauseAllStyle();

    /* 恢复弹幕 */
    void resumeStyle(int tag);
    void resumeAllStyle();

    /* 销毁弹幕 */
    void stopStyle(int tag);
    void stopAllStyle();

private:
    Node* character;        //绑定角色
    Node* target;           //自机目标
    Map<int, Node*> styles; //弹幕容器
    int tag;
};

#endif
