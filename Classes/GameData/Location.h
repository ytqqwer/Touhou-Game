#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef LOCATION_H
#define LOCATION_H

#include "Conversation.h"
#include "Round.h"

struct Location
{
    string tag;               //地点标签
    string name;              //地点名称
    string previewPicture;    //选取地点时的预览图片
    string backgroundPicture; //背景图片
    string backgroundMusic;   //背景音乐
    int passedRound;          //当前地点下，已经通过的关卡数
    int totalRound;           //当前地点下的关卡总数
};

#endif
