#ifndef APP_MACROS_H
#define APP_MACROS_H

#include "NonGameplayScenesCache.h"

#define APP_SCENE_CREATE_FUNC(__TYPE__, TAG)                                                       \
    static Scene* create()                                                                         \
    {                                                                                              \
        /*  1 if found cache */                                                                    \
                                                                                                   \
        auto cached = NonGameplayScenesCache::getInstance()->getScene(TAG);                        \
        if (cached)                                                                                \
            return cached;                                                                         \
                                                                                                   \
        /*  2 if not found cache */                                                                \
                                                                                                   \
        auto pRet = new (std::nothrow) __TYPE__();                                                 \
        if (pRet && pRet->init()) {                                                                \
            pRet->autorelease();                                                                   \
            NonGameplayScenesCache::getInstance()->addScene(TAG, pRet);                            \
            return pRet;                                                                           \
        } else {                                                                                   \
            delete pRet;                                                                           \
            pRet = nullptr;                                                                        \
            return nullptr;                                                                        \
        }                                                                                          \
    }

#endif
