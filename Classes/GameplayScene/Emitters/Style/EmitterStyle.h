#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef EMITTER_STYLE_H
#define EMITTER_STYLE_H

#include "GameplayScene/Emitters/Bullet.h"
#include "GameplayScene/Emitters/StyleConfig.h"
#include "GameplayScene/common.h"
#include "cocos2d.h"

USING_NS_CC;

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

    Vector<Node*>& getBullets() { return bullets; }

    virtual void createBullet() = 0;
    virtual void stopShoot() = 0;
    virtual void shootBullet(float dt) = 0;

protected:
    StyleConfig sc;        // Style参数
    Vector<Node*> bullets; //子弹容器
};

/* 无自机默认弹幕 */

/* 无自机定制弹幕 */
#define APP_CREATE_STYLE1(__TYPE__)                                                                \
    static __TYPE__* create(const StyleConfig& sc)                                                 \
    {                                                                                              \
        __TYPE__* pRet = new (std::nothrow) __TYPE__(sc);                                          \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = NULL;                                                                           \
            return NULL;                                                                           \
        }                                                                                          \
    }

/* 自机默认弹幕 */
#define APP_CREATE_STYLE2(__TYPE__)                                                                \
    static __TYPE__* create(Node** target)                                                         \
    {                                                                                              \
        __TYPE__* pRet = new (std::nothrow) __TYPE__(target);                                      \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = NULL;                                                                           \
            return NULL;                                                                           \
        }                                                                                          \
    }

/* 自机定制弹幕 */
#define APP_CREATE_STYLE3(__TYPE__)                                                                \
    static __TYPE__* create(const StyleConfig& sc, Node** target)                                  \
    {                                                                                              \
        __TYPE__* pRet = new (std::nothrow) __TYPE__(sc, target);                                  \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = NULL;                                                                           \
            return NULL;                                                                           \
        }                                                                                          \
    }

/* 角色默认弹幕 */
#define APP_CREATE_STYLE4(__TYPE__)                                                                \
    static __TYPE__* create(Direction* direction)                                                  \
    {                                                                                              \
        __TYPE__* pRet = new (std::nothrow) __TYPE__(direction);                                   \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = NULL;                                                                           \
            return NULL;                                                                           \
        }                                                                                          \
    }

/* 角色定制弹幕 */
#define APP_CREATE_STYLE5(__TYPE__)                                                                \
    static __TYPE__* create(const StyleConfig& sc, Direction* direction)                           \
    {                                                                                              \
        __TYPE__* pRet = new (std::nothrow) __TYPE__(sc, direction);                               \
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