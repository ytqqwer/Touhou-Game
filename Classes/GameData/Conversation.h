#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "cocos2d.h"
#include <string>
#include <vector>

USING_NS_CC;
using namespace std;

struct ConversationIndicator
{
    string conversationTag; //对话标签
    string icon;            //头像或图标
    string name;            //对话人姓名
};

#endif
