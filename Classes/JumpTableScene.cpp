#include "JumpTableScene.h"
#include "ArmsStoreScene.h"
#include "BackgroundIntroScene.h"
#include "ConversationLayer.h"
#include "EquipScene.h"
#include "HomeScene.h"
#include "InventoryScene.h"
#include "KnowledgeBaseScene.h"
#include "KoumakanLibraryScene.h"
#include "KourindouScene.h"
#include "MainMenuScene.h"
#include "MapSelectScene.h"
#include "PlaceHolder.h"
#include "RoundSelectScene.h"
#include "SaveScene.h"
#include "SettingsLayer.h"
#include "StaffScene.h"

#ifdef ADD_MENU_ITEM_SCENE
#undef ADD_MENU_ITEM_SCENE
#endif
#define ADD_MENU_ITEM_SCENE(__TYPE__)                                                              \
    _jumpTable->addChild(MenuItemSprite::create(                                                   \
        PlaceHolder::createRect(Size(200, 100), #__TYPE__, 16),                                    \
        PlaceHolder::createRect(Size(200, 100), #__TYPE__, 16, Color3B(100, 100, 100)),            \
        [this](Ref*) {                                                                             \
            auto s = __TYPE__::create();                                                           \
            _director->replaceScene(s);                                                            \
        }));

#ifdef ADD_MENU_ITEM_LAYER
#undef ADD_MENU_ITEM_LAYER
#endif
#define ADD_MENU_ITEM_LAYER(__TYPE__)                                                              \
    _jumpTable->addChild(MenuItemSprite::create(                                                   \
        PlaceHolder::createRect(Size(200, 100), #__TYPE__, 16),                                    \
        PlaceHolder::createRect(Size(200, 100), #__TYPE__, 16, Color3B(100, 100, 100)),            \
        [this](Ref*) {                                                                             \
            auto s = __TYPE__::createDebugScene();                                                 \
            _director->replaceScene(s);                                                            \
        }));

const std::string JumpTableScene::TAG{ "JumpTableScene" };

JumpTableScene::JumpTableScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
JumpTableScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

    /*  2. Menu */

    _jumpTable = Menu::create();
    this->addChild(_jumpTable);

    /*  3. MenuItem and Callback */

    ADD_MENU_ITEM_SCENE(ArmsStoreScene)
    ADD_MENU_ITEM_SCENE(BackgroundIntroScene);
    ADD_MENU_ITEM_SCENE(EquipScene);
    ADD_MENU_ITEM_SCENE(HomeScene);
    ADD_MENU_ITEM_SCENE(InventoryScene);
    ADD_MENU_ITEM_SCENE(KnowledgeBaseScene);
    ADD_MENU_ITEM_SCENE(KoumakanLibraryScene);
    ADD_MENU_ITEM_SCENE(KourindouScene);
    ADD_MENU_ITEM_SCENE(MainMenuScene);
    ADD_MENU_ITEM_SCENE(MapSelectScene);
    ADD_MENU_ITEM_SCENE(RoundSelectScene);

    // SaveScene 有两种创建参数，不能直接使用 ADD_MENU_ITEM_SCENE
    _jumpTable->addChild(MenuItemSprite::create(
        PlaceHolder::createRect(Size(200, 100), "SaveScene\n(LoadAction)", 16),
        PlaceHolder::createRect(Size(200, 100), "SaveScene\n(LoadAction)", 16,
                                Color3B(100, 100, 100)),
        [this](Ref*) {
            auto s = SaveScene::create(false);
            _director->replaceScene(s);
        }));
    _jumpTable->addChild(MenuItemSprite::create(
        PlaceHolder::createRect(Size(200, 100), "SaveScene\n(SaveAction)", 16),
        PlaceHolder::createRect(Size(200, 100), "SaveScene\n(SaveAction)", 16,
                                Color3B(100, 100, 100)),
        [this](Ref*) {
            auto s = SaveScene::create(true);
            _director->replaceScene(s);
        }));

    ADD_MENU_ITEM_SCENE(StaffScene);

    // SettingsLayer / ConversationLayer 是层，不能直接使用 ADD_MENU_ITEM_SCENE
    ADD_MENU_ITEM_LAYER(SettingsLayer);
    ADD_MENU_ITEM_LAYER(ConversationLayer);

    /*  4 排列这些 MenuItem */

    _jumpTable->alignItemsInColumns(4, 4, 4, 4, NULL);

    return true;
}
