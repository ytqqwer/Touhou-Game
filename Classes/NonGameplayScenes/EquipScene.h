#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef EQUIP_SCENE_H
#define EQUIP_SCENE_H

#include "GameData/GameData.h"
#include "TouhouGame.h"
#include "cocos2d.h"
#include <string>
#include <vector>

USING_NS_CC;
#include "ui/CocosGUI.h"
using namespace ui;

class EquipScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(EquipScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

private:
    class SelectCharacterButton : public Button
    {
    public:
        static SelectCharacterButton* create(const Character&, const Node* parent);
        SelectCharacterButton(const Character&, const Node* parent);
        virtual bool init();

    private:
        Character character;
        const Node* parent;
    };

    void loadCharacterProperty(const Character&);

private:
    EquipScene();

    string getNumber(int, int);
    Sprite* getRect(double, double, double, double);

private:
    static const std::string TAG;

    Size _visibleSize;
    GameData* gamedata;
    int tagSum;

    Vector<SelectCharacterButton*> SCButton;
};

#endif // EQUIP_SCENE_H
