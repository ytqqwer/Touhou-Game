#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef EMITTER_STYLE_H
#define EMITTER_STYLE_H

#include "../Bullet.h"
#include "../StyleConfig.h"
#include "cocos2d.h"
USING_NS_CC;

#define Pi 3.14

class EmitterStyle : public Node
{

public:
    void removeBullet(Node* pNode)
    {
        if (NULL == pNode) {
            return;
        }
        pNode->removeFromParent();
        bullets.eraseObject(pNode);
    }

protected:
    StyleConfig sc;
    Vector<Node*> bullets;
};

/* 无自机默认发射器 */

/* 自机默认发射器 */
#define APP_CREATE_STYLE1(__TYPE__)                                                                \
    static __TYPE__* create(Node* target)                                                          \
    {                                                                                              \
        __TYPE__* pRet = new __TYPE__(target);                                                     \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = NULL;                                                                           \
            return NULL;                                                                           \
        }                                                                                          \
    }

/* 无自机发射器 */
#define APP_CREATE_STYLE2(__TYPE__)                                                                \
    static __TYPE__* create(const StyleConfig& sc)                                                 \
    {                                                                                              \
        __TYPE__* pRet = new __TYPE__(sc);                                                         \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = NULL;                                                                           \
            return NULL;                                                                           \
        }                                                                                          \
    }

/* 自机发射器 */
#define APP_CREATE_STYLE3(__TYPE__)                                                                \
    static __TYPE__* create(const StyleConfig& sc, Node* target)                                   \
    {                                                                                              \
        __TYPE__* pRet = new __TYPE__(sc, target);                                                 \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = NULL;                                                                           \
            return NULL;                                                                           \
        }                                                                                          \
    }

#endif // EMITTER_STYLE_H
