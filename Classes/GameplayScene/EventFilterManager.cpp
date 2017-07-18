#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "EventFilterManager.h"
#include "EventFilterManager.inc"
#include "common.h"
#include <algorithm>

using namespace std;

EventFilterManager*
EventFilterManager::create(Node* target)
{
    auto mgr = new (nothrow) EventFilterManager;

    mgr->_delegateNode = Node::create();
    target->addChild(mgr->_delegateNode);

    mgr->registerAllCustomEvents();

    return mgr;
}

bool
EventFilterManager::registerAllCustomEvents()
{
    auto eventDispather = _delegateNode->getEventDispatcher();

    for (int i = 0; i < sizeof(eventFilterWorkingRange) / sizeof(char*); i++) {
        const char* type = eventFilterWorkingRange[i];
        auto listener = EventListenerCustom::create(
            type, bind(&EventFilterManager::onRecvCustomEvents, this, placeholders::_1));
        eventDispather->addEventListenerWithFixedPriority(listener, EVENT_FILTER_PRIORITY);
    }

    return true;
}

void
EventFilterManager::addEventFilter(const function<void(EventCustom*)>& filter, float lastTime,
                                   const string& tag)
{
    /*  1. 添加 */

    _filters.insert(make_pair(tag, filter));

    /*  2. 定时 */

    _delegateNode->scheduleOnce(
        bind(&EventFilterManager::removeEventFilter, this, tag, placeholders::_1), lastTime, tag);
}

void
EventFilterManager::removeEventFilter(const std::string& tag, float unused)
{
#ifndef NDEBUG
    log("[EventFilterManager] Remove filter %s", tag.c_str());
#endif
    auto it = find_if(_filters.begin(), _filters.end(),
                      [&tag](const pair<string, function<void(EventCustom*)>>& filter) -> bool {
                          return (filter.first == tag);
                      });

    if (it != _filters.end()) {
        _filters.erase(it);
    }
}

void
EventFilterManager::removeAllEventFilters()
{
    _filters.clear();
}

void
EventFilterManager::pause()
{
    _delegateNode->pause();
}

void
EventFilterManager::onRecvCustomEvents(EventCustom* event)
{
#ifndef NDEBUG
    log("[EventFilterManager] Recv EventCustom: %s", event->getEventName().c_str());
#endif

    for_each(_filters.begin(), _filters.end(),
             [event](const pair<string, function<void(EventCustom*)>>& filter) -> void {
                 (filter.second)(event);
             });
}
