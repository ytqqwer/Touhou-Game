#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "EventFilterManager.h"
#include "EventFilterManager.inc"
#include "common.h"
#include <algorithm>

using namespace std;

EventFilterManager*
EventFilterManager::create()
{
    auto mgr = new (nothrow) EventFilterManager;
    mgr->autorelease();

    return mgr;
}

EventFilterManager::EventFilterManager()
{
    _delegateNode = Node::create();
    _delegateNode->retain();
    _delegateNode->onEnter(); // Yes, we need this, or Node::_running will be false, all actions and
                              // schedules will have no effect
    _useFilters = true;
    registerAllCustomEvents();
}

EventFilterManager::~EventFilterManager()
{
    _delegateNode->release();
}

bool
EventFilterManager::registerAllCustomEvents()
{
    auto eventDispatcher = _delegateNode->getEventDispatcher();

    for (int i = 0; i < sizeof(eventFilterWorkingRange) / sizeof(char*); i++) {
        const char* type = eventFilterWorkingRange[i];
        auto listener = EventListenerCustom::create(
            type, bind(&EventFilterManager::onRecvCustomEvents, this, placeholders::_1));
        eventDispatcher->addEventListenerWithFixedPriority(listener, EVENT_FILTER_PRIORITY);
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

    if (lastTime != 0) {
        _delegateNode->scheduleOnce(
            bind(&EventFilterManager::removeEventFilter, this, tag, placeholders::_1), lastTime,
            tag);
    }
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
EventFilterManager::pauseAllFiltersTimer()
{
    _delegateNode->pause();
}
void
EventFilterManager::resumeAllFiltersTimer()
{
    _delegateNode->resume();
}

void
EventFilterManager::stopAllFilters()
{
    _useFilters = false;
}
void
EventFilterManager::resumeAllFilters()
{
    _useFilters = true;
}

void
EventFilterManager::onRecvCustomEvents(EventCustom* event)
{
#ifndef NDEBUG
    log("[EventFilterManager] Recv EventCustom: %s", event->getEventName().c_str());
#endif

    if (_useFilters) {
        for_each(_filters.begin(), _filters.end(),
                 [event](const pair<string, function<void(EventCustom*)>>& filter) -> void {
                     (filter.second)(event);
                 });
    }
}
