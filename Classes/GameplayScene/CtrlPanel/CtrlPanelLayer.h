﻿#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef CTRL_PANEL_LAYER_H
#define CTRL_PANEL_LAYER_H

#include "cocos2d.h"

USING_NS_CC;

class GameData;

class CtrlPanelLayer : public Layer
{
public:
    CREATE_FUNC(CtrlPanelLayer)

    bool init() override;

    void createBossHpBar(const Node* target, const int maxHpValue, const std::string& face);
    void removeBossHpBar();

private:
    CtrlPanelLayer() = default;

    void initCharacterPanelUIAndListener();
    void initTouchListener();
    void initKeyboardListener();

private:
    GameData* _gamedata;
    Size _visibleSize;

    Layer* _characterCtrlPanel[2];
    unsigned int _currCharacterIdx;

    Vector<Node*> _hpBars;
};

#endif // CTRL_PANEL_LAYER_H
