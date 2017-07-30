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

#define APP_CREATE_STYLE1(__TYPE__)                                                                \
    static __TYPE__* create(const StyleConfig& sc, Node* character)                                \
    {                                                                                              \
        __TYPE__* pRet;                                                                            \
        if (sc.defaultStyle == true) {                                                             \
            pRet = new __TYPE__(character);                                                        \
        } else {                                                                                   \
            pRet = new __TYPE__(sc, character);                                                    \
        }                                                                                          \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = NULL;                                                                           \
            return NULL;                                                                           \
        }                                                                                          \
    }

#define APP_CREATE_STYLE2(__TYPE__)                                                                \
    static __TYPE__* create(const StyleConfig& sc, Node* character, Node* target)                  \
    {                                                                                              \
        __TYPE__* pRet;                                                                            \
        if (sc.defaultStyle == true) {                                                             \
            pRet = new __TYPE__(character, target);                                                \
        } else {                                                                                   \
            pRet = new __TYPE__(sc, character, target);                                            \
        }                                                                                          \
                                                                                                   \
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
