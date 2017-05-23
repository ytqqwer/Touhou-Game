#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef NONGAMEPLAYSCENESCACHE_H
#define NONGAMEPLAYSCENESCACHE_H

#include "cocos2d.h"

USING_NS_CC;

class NonGameplayScenesCache
{
public:
    static NonGameplayScenesCache* getInstance();

    // 如果场景不在缓存中，返回 nullptr
    Scene* getScene(const std::string& sceneTag);

    // 如果场景已经在缓存中，什么都不做
    void addScene(const std::string& sceneTag, Scene* s);

    // 在进入 Gameplay 场景时使用，用于删除所有缓存场景
    void removeAllScenes();

private:
    NonGameplayScenesCache();

private:
    static NonGameplayScenesCache* _self;
    Map<std::string, Scene*> _cacheMap;
};

#endif
