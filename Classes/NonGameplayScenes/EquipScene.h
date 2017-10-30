#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef EQUIP_SCENE_H
#define EQUIP_SCENE_H

#include "GameData/GameData.h"
#include "TouhouGame.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include <string>
#include <vector>

USING_NS_CC_EXT;
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

    class ItemMenu : public Layer, public TableViewDataSource, public TableViewDelegate
    {
    public:
        static ItemMenu* create(std::function<void(std::string)> func,
                                std::function<void()> callBack);
        virtual bool init();

        // 设置tableview的Cell大小
        virtual Size tableCellSizeForIndex(TableView* table, ssize_t idx);
        // 获取编号为idx的cell
        virtual TableViewCell* tableCellAtIndex(TableView* table, ssize_t idx);
        // 设置tableview中cell的个数
        virtual ssize_t numberOfCellsInTableView(TableView* table);
        // 当cell被点击时调用该方法 参数为当前的tableview对象与被点击的cell对象
        virtual void tableCellTouched(TableView* table, TableViewCell* cell);
        // 当滑动tableview时触发该方法 参数为当前的tableview对象
        virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {}
        // 当tableview被放大或缩小时触发该方法  参数为当前tableview对象
        virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {}

    private:
        ItemMenu(std::function<void(std::string)> func, std::function<void()> callBack);
        Size _visibleSize;

        vector<Item> items;
        std::function<void(std::string)> func;
        std::function<void()> callBack;
        TableView* itemTable;
    };

    class SpellCardMenu : public Layer, public TableViewDataSource, public TableViewDelegate
    {
    public:
        static SpellCardMenu* create(std::function<void(std::string)> func,
                                     std::function<void()> callBack);
        virtual bool init();

        virtual Size tableCellSizeForIndex(TableView* table, ssize_t idx);
        virtual TableViewCell* tableCellAtIndex(TableView* table, ssize_t idx);
        virtual ssize_t numberOfCellsInTableView(TableView* table);
        virtual void tableCellTouched(TableView* table, TableViewCell* cell);
        virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {}
        virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {}

    private:
        SpellCardMenu(std::function<void(std::string)> func, std::function<void()> callBack);
        Size _visibleSize;

        vector<SpellCard> spellCards;
        std::function<void(std::string)> func;
        std::function<void()> callBack;
        TableView* spellCardTable;
    };

private:
    EquipScene();

    string getNumber(int, int);
    Sprite* getRect(double, double, double, double);
    void loadCharacterProperty(const Character&);

private:
    static const std::string TAG;

    Size _visibleSize;
    GameData* gamedata;
    int tagSum;

    Vector<SelectCharacterButton*> SCButton;
};

#endif // EQUIP_SCENE_H
