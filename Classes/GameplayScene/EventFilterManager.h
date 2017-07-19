#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef EVENT_FILTER_MANAGER_H
#define EVENT_FILTER_MANAGER_H

#include "cocos2d.h"
#include <functional>
#include <map>
#include <string>

USING_NS_CC;

class EventFilterManager : public Node
{
public:
    static EventFilterManager* create();

    // 添加一个 filter，可定时
    //  + 保证所有 add 的 filter 都能运作，不保证先加入的 filter 早于后加入的 filter 运作
    //  + 可在一个 filter 不过期之前指定同 tag 的 filter，新加入的 filter 会立即运作
    void addEventFilter(const std::function<void(EventCustom*)>& filter, float lastTime,
                        const std::string& tag);

    // 用于在一个 filter 定时结束之前就取消一个 filter。现 delay 参数未实现
    //  + 若给出的 filter tag  不存在或无效，则什么都不发生
    //  + 一次 removeEventFilter 调用只会删除一个 filter（删除哪个不一定）。
    //    如果外部添加了多个相同 tag 的 filter，需要外部自己多次调用以删除所有想同 tag 的 filter
    void removeEventFilter(const std::string& tag, float delay = 0);

    void removeAllEventFilters();

    // 停止现有所有 filter 定时器的计时
    void pause();

public:
    // 不供 GameplayScene 使用。用于给 EventDispatcher 回调
    void onRecvCustomEvents(EventCustom* event);

private:
    EventFilterManager() = default;

    bool registerAllCustomEvents();

private:
    // 使用 multimap 以允许外部添加多个相同 tag 的 filter
    std::multimap<std::string, std::function<void(EventCustom*)>> _filters;
};

#endif
