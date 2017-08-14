#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef EVENTDATA_H
#define EVENTDATA_H

#include "cocos2d.h"
#include <string>
#include <vector>

using namespace std;

struct EventData
{
    string eventType;

    string conversationTag;

    bool jump = false;

    float delay;
    float duration;
};

#endif
