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
    CREATE_FUNC(Emitter);
    Emitter();

    /* 自机发射器 */
    static Emitter* create(Node* target);
    Emitter(Node* target);

    /* 创建弹幕 */
    int playStyle(const StyleConfig& sc);
    int playStyle(StyleType st);

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
    Node* target;           //自机目标
    Map<int, Node*> styles; //弹幕容器
    int styleTag;
};

#endif
