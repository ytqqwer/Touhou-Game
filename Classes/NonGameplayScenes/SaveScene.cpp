﻿#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/SaveScene.h"
#include "NonGameplayScenes/MainMenuScene.h"
#include "PlaceHolder.h"
// #include "resources.h.dir/save.h"

#include "AudioController.h"
#include "GameData/GameData.h"
#include "NonGameplayScenes/HomeScene.h"

#include "ui/CocosGUI.h"
using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string SaveScene::TAG{ "SaveScene" };

SaveScene::SaveScene(bool isSaveAction)
{
    _isSaveAction = isSaveAction;
    _visibleSize = _director->getVisibleSize();
}

Scene*
SaveScene::create(bool isSaveAction)
{
    auto pRet = new (std::nothrow) SaveScene(isSaveAction);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool
SaveScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("SaveScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    auto BackButton = Button::create();
    BackButton->setTitleText("返回");
    BackButton->setTitleFontName("fonts/dengxian.ttf");
    BackButton->setTitleColor(Color3B(194, 134, 11));
    BackButton->setTitleFontSize(50);
    BackButton->setAnchorPoint(Vec2(0, 0));
    BackButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.43));
    BackButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            Director::getInstance()->popScene();
        }
    });
    addChild(BackButton);

    return true;
}

void
SaveScene::onEnter()
{
    Scene::onEnter();

    if (_isSaveAction) {
        auto saveList = GameData::getInstance()->getSaveList();
        for (int index = 1; index <= 4; index++) {
            auto button = Button::create();
            button->setTitleFontName("fonts/dengxian.ttf");
            button->setTitleColor(Color3B(194, 134, 11));
            button->setTitleFontSize(40);
            button->setAnchorPoint(Vec2(0, 0));
            button->setPosition(
                Vec2(_visibleSize.width * 0.2, _visibleSize.height * (0.99 - index * 0.2)));
            button->setTag(998);
            this->addChild(button);

            button->setTitleText("None");
            for (auto& v : saveList) {
                if (v.tag == index) {
                    button->setTitleText(v.name + "   " + v.locationTag + "   " + v.time);
                }
            }
            button->addTouchEventListener(
                [index, button](Ref* pSender, Widget::TouchEventType type) {
                    if (type == Widget::TouchEventType::ENDED) {
                        AudioController::getInstance()->playClickButtonEffect();

                        GameData::getInstance()->saveSave(index);

                        button->setTitleText("");
                    }
                });
        }

    } else {
        auto saveList = GameData::getInstance()->getSaveList();
        for (int index = 1; index <= 4; index++) {
            auto button = Button::create();
            button->setTitleFontName("fonts/dengxian.ttf");
            button->setTitleColor(Color3B(194, 134, 11));
            button->setTitleFontSize(40);
            button->setAnchorPoint(Vec2(0, 0));
            button->setPosition(
                Vec2(_visibleSize.width * 0.2, _visibleSize.height * (0.99 - index * 0.2)));
            button->setTag(998);
            this->addChild(button);

            button->setTitleText("None");
            for (auto& v : saveList) {
                if (v.tag == index) {
                    button->setTitleText(v.name + "   " + v.locationTag + "   " + v.time);
                    button->addTouchEventListener(
                        [index](Ref* pSender, Widget::TouchEventType type) {
                            if (type == Widget::TouchEventType::ENDED) {
                                AudioController::getInstance()->playClickButtonEffect();
                                GameData::getInstance()->switchSave(index);
                                AudioController::getInstance()->stopMusic();
                                TransitionScene* transition =
                                    TransitionFade::create(1.0f, HomeScene::create());
                                Director::getInstance()->replaceScene(transition);
                            }
                        });
                    break;
                }
            }
        }
    }
}

void
SaveScene::onExit()
{
    Scene::onExit();

    this->removeChildByTag(998);
}
