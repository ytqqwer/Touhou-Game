#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAMESCENE_EVENT_SCRIPT_HANDING_H
#define GAMESCENE_EVENT_SCRIPT_HANDING_H

#include "cocos2d.h"
USING_NS_CC;
#include "GameData/GameData.h"

class EventScriptHanding
{
public:
    EventScriptHanding(Node*);

    void eventHandling(EventCustom*);
    void eventActionHandling(float delay, float duration);
    void nextEvent();

private:
    vector<EventData> eventList;
    int _curEventIndex;

    Node* owner;
};

#endif
