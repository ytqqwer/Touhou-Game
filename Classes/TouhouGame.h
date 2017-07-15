#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef TOUHOUGAME_H
#define TOUHOUGAME_H

#include "NonGameplayScenesCache.h"

#define APP_SCENE_CREATE_FUNC(__TYPE__, __TAG__)                                                   \
    static Scene* create()                                                                         \
    {                                                                                              \
        /*  1 if found cache */                                                                    \
                                                                                                   \
        auto cached = NonGameplayScenesCache::getInstance()->getScene(__TAG__);                    \
        if (cached)                                                                                \
            return cached;                                                                         \
                                                                                                   \
        /*  2 if not found cache */                                                                \
                                                                                                   \
        auto pRet = new (std::nothrow) __TYPE__();                                                 \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            NonGameplayScenesCache::getInstance()->addScene(__TAG__, pRet);                        \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = nullptr;                                                                        \
            return nullptr;                                                                        \
        }                                                                                          \
    }

#endif
