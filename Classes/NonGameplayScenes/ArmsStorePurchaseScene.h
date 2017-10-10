#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ARMS_STORE_PURCHASE_SCENE_H
#define ARMS_STORE_PURCHASE_SCENE_H

#include "GameData/GameData.h"
#include "TouhouGame.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC_EXT;
USING_NS_CC;

class ArmsStorePurchaseScene : public Scene, public TableViewDataSource, public TableViewDelegate
{
public:
    APP_SCENE_CREATE_FUNC(ArmsStorePurchaseScene, TAG);

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

    // 设置tableview的Cell大小
    virtual Size tableCellSizeForIndex(TableView* table, ssize_t idx);
    //设置每项cell的尺寸大小
    // virtual Size cellSizeForTable(TableView *table);
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
    ArmsStorePurchaseScene();

private:
    static const std::string TAG;

    GameData* gamedata;
    Size _visibleSize;

    Item::Type currentType;
    vector<SpellCard> spellCards;
    vector<Item> normalItems;

    TableView* itemTable;
};

#endif
