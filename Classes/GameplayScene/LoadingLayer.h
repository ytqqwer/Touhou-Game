#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef LOADING_LAYER_H
#define LOADING_LAYER_H

#include "cocos2d.h"

USING_NS_CC;

struct LoadingInfo
{
    float progress;
    std::string information;
};

class LoadingLayer : public Layer
{
public:
    bool init();

    CREATE_FUNC(LoadingLayer);

    ProgressTimer* loadingProgress;
    int progress = 0;
    EventListenerCustom* ELC;
    Label* text;
};
#endif
