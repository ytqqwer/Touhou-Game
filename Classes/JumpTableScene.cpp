#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "JumpTableScene.h"
#include "Layers/ConversationLayer.h"
#include "Layers/SettingsLayer.h"
#include "NonGameplayScenes/ArmsStoreScene.h"
#include "NonGameplayScenes/BackgroundIntroScene.h"
#include "NonGameplayScenes/EquipScene.h"
#include "NonGameplayScenes/HomeScene.h"
#include "NonGameplayScenes/InventoryScene.h"
#include "NonGameplayScenes/KnowledgeBaseScene.h"
#include "NonGameplayScenes/KoumakanLibraryScene.h"
#include "NonGameplayScenes/KourindouScene.h"
#include "NonGameplayScenes/LocationSelectScene.h"
#include "NonGameplayScenes/MainMenuScene.h"
#include "NonGameplayScenes/RoundSelectScene.h"
#include "NonGameplayScenes/SaveScene.h"
#include "NonGameplayScenes/StaffScene.h"
#include "GameplayScene/Emitters/test/EmitterTestScene.h"
#include "PlaceHolder.h"

#ifdef ADD_MENU_ITEM_SCENE
#undef ADD_MENU_ITEM_SCENE
#endif
#define ADD_MENU_ITEM_SCENE(__TYPE__)                                                              \
    _jumpTable->addChild(MenuItemSprite::create(                                                   \
        PlaceHolder::createRect(Size(200, 100), #__TYPE__, 16),                                    \
        PlaceHolder::createRect(Size(200, 100), #__TYPE__, 16, Color3B(100, 100, 100)),            \
        [this](Ref*) {                                                                             \
            auto s = __TYPE__::create();                                                           \
            _director->pushScene(s);                                                               \
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
            _director->pushScene(s);                                                               \
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
    ADD_MENU_ITEM_SCENE(LocationSelectScene);
    ADD_MENU_ITEM_SCENE(RoundSelectScene);

    // SaveScene 有两种创建参数，不能直接使用 ADD_MENU_ITEM_SCENE
    _jumpTable->addChild(MenuItemSprite::create(
        PlaceHolder::createRect(Size(200, 100), "SaveScene\n(LoadAction)", 16),
        PlaceHolder::createRect(Size(200, 100), "SaveScene\n(LoadAction)", 16,
                                Color3B(100, 100, 100)),
        [this](Ref*) {
            auto s = SaveScene::create(false);
            _director->pushScene(s);
        }));
    _jumpTable->addChild(MenuItemSprite::create(
        PlaceHolder::createRect(Size(200, 100), "SaveScene\n(SaveAction)", 16),
        PlaceHolder::createRect(Size(200, 100), "SaveScene\n(SaveAction)", 16,
                                Color3B(100, 100, 100)),
        [this](Ref*) {
            auto s = SaveScene::create(true);
            _director->pushScene(s);
        }));

    ADD_MENU_ITEM_SCENE(StaffScene);

    // SettingsLayer / ConversationLayer 是层，不能直接使用 ADD_MENU_ITEM_SCENE
    ADD_MENU_ITEM_LAYER(SettingsLayer);
    ADD_MENU_ITEM_LAYER(ConversationLayer);

    // Emitters::GameplayScene
    ADD_MENU_ITEM_SCENE(EmitterTest::GameplayScene);

    /*  4 排列这些 MenuItem */

    _jumpTable->alignItemsInColumns(4, 4, 4, 4, 1, NULL);

    return true;
}
