#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef LOADING_LAYER_H
#define LOADING_LAYER_H

#include "cocos2d.h"
USING_NS_CC;

#include "GameplayScene/GameplayScene.h"

class LoadingLayer : public Layer
{
public:
    bool init();
    void onEnter() override;

    static LoadingLayer* create(std::string);

private:
    LoadingLayer(std::string);

    void initBackGround();
    void initMap();
    void initAnimationCache();
    void initCharacter();
    void initCtrlPanel();
    void initArea();
    void initCamera();
    void initPhysicsContactListener();
    void initCustomEventListener();

private:
    ProgressTimer* loadingProgress;
    int progress;
    Label* text;

    GameplayScene* gameplayScene;
    std::string _map;
};
#endif
