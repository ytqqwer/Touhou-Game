#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenesCache.h"
#include <string>

using std::string;

NonGameplayScenesCache* NonGameplayScenesCache::_self;

NonGameplayScenesCache::NonGameplayScenesCache()
    : _cacheMap(20)
{
}

NonGameplayScenesCache*
NonGameplayScenesCache::getInstance()
{
    if (!_self) {
        // 这个对象并不大，而且会一直存在，不需要 delete 它
        _self = new NonGameplayScenesCache();
    }

    return _self;
}

Scene*
NonGameplayScenesCache::getScene(const string& sceneTag)
{
    return _cacheMap.at(sceneTag);
}

void
NonGameplayScenesCache::addScene(const string& sceneTag, Scene* s)
{
    _cacheMap.insert(sceneTag, s);
}

void
NonGameplayScenesCache::removeAllScenes()
{
    _cacheMap.clear();
}
