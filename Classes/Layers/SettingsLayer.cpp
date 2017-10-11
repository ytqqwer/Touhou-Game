#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Layers/SettingsLayer.h"
#include "GameData/GameData.h"
#include "NonGameplayScenes/MainMenuScene.h"
#include "NonGameplayScenes/RoundSelectScene.h"
#include "NonGameplayScenes/SaveScene.h"

#include "resources.h.dir/settings_layer.h"

#include "AudioController.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

SettingsLayer::SettingsLayer(const string& TAG)
{
    _visibleSize = _director->getVisibleSize();
    currentScene = TAG;
}

Scene*
SettingsLayer::createDebugScene()
{
    auto s = Scene::create();
    auto l = SettingsLayer::create("HomeScene");
    if (s && l) {
        s->addChild(l);
        return s;
    } else {
        return nullptr;
    };
}

SettingsLayer*
SettingsLayer::create(const string& TAG)
{
    SettingsLayer* pRet = new (std::nothrow) SettingsLayer(TAG);
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
SettingsLayer::init()
{
    /*  1. super init first */

    if (!Layer::init()) {
        return false;
    }

    //触摸截断
    this->setLocalZOrder(8888);
    this->setTouchEnabled(true);
    EventDispatcher* eventDispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,
                                                                                          this);

    /*  2.创建背景 */
    auto background = Sprite::create(IMG_SETTING_BACKGROUND);
    background->setAnchorPoint(Vec2(0.5, 0.5));
    background->setScale(1.3);
    background->setPosition(_visibleSize.width / 2, _visibleSize.height / 2);
    addChild(background);

    /*  3.构建界面背景 */
    auto layout = Layout::create();
    if (currentScene == "MainMenuScene") {
        layout->setContentSize(Size(_visibleSize.width / 3.0, _visibleSize.height / 2.0));
    }
    if (currentScene == "HomeScene") {
        layout->setContentSize(Size(_visibleSize.width / 3.0, _visibleSize.height / 1.25));
    }
    if (currentScene == "GameplayScene") {
        layout->setContentSize(Size(_visibleSize.width / 3.0, _visibleSize.height / 1.5));
    }
    layout->setBackGroundColorType(LAYOUT_COLOR_SOLID);
    layout->setBackGroundColor(Color3B(255, 255, 255));
    layout->setBackGroundColorOpacity(90);
    layout->setAnchorPoint(Vec2(0.5, 0.5));
    layout->setPosition(Vec2(_visibleSize.width / 2.0, _visibleSize.height / 2.0));

    /*  4.音乐和音效*/

    string music = "音乐";
    auto musicVolume = Label::createWithTTF(music, "fonts/dengxian.ttf", 20);
    musicVolume->setTag(1);
    musicVolume->setAnchorPoint(Vec2(0.0, 0.5));

    auto _sliderMusicVolume = Slider::create();
    _sliderMusicVolume->setTag(2);
    _sliderMusicVolume->setTouchEnabled(true);
    _sliderMusicVolume->setPercent(SimpleAudioEngine::getInstance()->getBackgroundMusicVolume() *
                                   100);
    _sliderMusicVolume->loadBarTexture(IMG_SETTING_SLIDER_TRACK);
    _sliderMusicVolume->loadSlidBallTextures(IMG_SETTING_SLIDER_THUMB, IMG_SETTING_SLIDER_THUMB);
    _sliderMusicVolume->loadProgressBarTexture(IMG_SETTING_SLIDER_PROGRESS);
    _sliderMusicVolume->setAnchorPoint(Vec2(0.4, 0.5));
    _sliderMusicVolume->addEventListener(CC_CALLBACK_2(SettingsLayer::sliderEvent, this));

    string effect = "音效";
    auto effectVolume = Label::createWithTTF(effect, "fonts/dengxian.ttf", 20);
    effectVolume->setTag(3);
    effectVolume->setAnchorPoint(Vec2(0.0, 0.5));

    auto _sliderEffectVolume = Slider::create();
    _sliderEffectVolume->setTag(4);
    _sliderEffectVolume->setTouchEnabled(true);
    _sliderEffectVolume->setPercent(SimpleAudioEngine::getInstance()->getEffectsVolume() * 100);
    _sliderEffectVolume->loadBarTexture(IMG_SETTING_SLIDER_TRACK);
    _sliderEffectVolume->loadSlidBallTextures(IMG_SETTING_SLIDER_THUMB, IMG_SETTING_SLIDER_THUMB);
    _sliderEffectVolume->loadProgressBarTexture(IMG_SETTING_SLIDER_PROGRESS);
    _sliderEffectVolume->setAnchorPoint(Vec2(0.4, 0.5));
    _sliderEffectVolume->addEventListener(CC_CALLBACK_2(SettingsLayer::sliderEvent, this));

    if (currentScene == "MainMenuScene") {
        musicVolume->setPosition(
            Vec2(layout->getSize().width / 8.0, layout->getSize().height * 4.0 / 5.0));
        _sliderMusicVolume->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 4.0 / 5.0));
        effectVolume->setPosition(
            Vec2(layout->getSize().width / 8.0, layout->getSize().height * 3.0 / 5.0));
        _sliderEffectVolume->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 3.0 / 5.0));
    }
    if (currentScene == "HomeScene") {
        musicVolume->setPosition(
            Vec2(layout->getSize().width / 8.0, layout->getSize().height * 7.0 / 8.0));
        _sliderMusicVolume->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 7.0 / 8.0));
        effectVolume->setPosition(
            Vec2(layout->getSize().width / 8.0, layout->getSize().height * 6.0 / 8.0));
        _sliderEffectVolume->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 6.0 / 8.0));
    }
    if (currentScene == "GameplayScene") {
        musicVolume->setPosition(
            Vec2(layout->getSize().width / 8.0, layout->getSize().height * 5.0 / 6.0));
        _sliderMusicVolume->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 5.0 / 6.0));
        effectVolume->setPosition(
            Vec2(layout->getSize().width / 8.0, layout->getSize().height * 4.0 / 6.0));
        _sliderEffectVolume->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 4.0 / 6.0));
    }

    layout->addChild(musicVolume);
    layout->addChild(_sliderMusicVolume);
    layout->addChild(effectVolume);
    layout->addChild(_sliderEffectVolume);

    /*  5.文字速度*/

    if ((currentScene == "HomeScene") || (currentScene == "MainMenuScene")) {
        string texSpeed = "文字播放速度";
        auto texPlaySpeed = Label::createWithTTF(texSpeed, "fonts/dengxian.ttf", 20);
        texPlaySpeed->setTag(5);
        texPlaySpeed->setAnchorPoint(Vec2(0.0, 0.5));
        if (currentScene == "MainMenuScene") {
            texPlaySpeed->setPosition(
                Vec2(layout->getSize().width / 8.0, layout->getSize().height * 2.0 / 5.0));
        } else {
            texPlaySpeed->setPosition(
                Vec2(layout->getSize().width / 8.0, layout->getSize().height * 5.0 / 8.0));
        }
        layout->addChild(texPlaySpeed);
    }

    /*  6.按钮组*/

    if (currentScene == "HomeScene") {
        auto button1 = Button::create();
        button1->setTag(7);
        button1->setTouchEnabled(true);
        button1->setTitleText("保存");
        button1->setTitleFontSize(36);
        button1->loadTextures(IMG_SETTING_BUTTON_NORMAL, IMG_SETTING_BUTTON_PRESSED);
        button1->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 4.0 / 8.0));
        button1->setScale(0.7);
        button1->addTouchEventListener(this, (SEL_TouchEvent)(&SettingsLayer::touchEvent));
        layout->addChild(button1);
    }

    if ((currentScene == "HomeScene") || (currentScene == "GameplayScene")) {
        auto button2 = Button::create();
        button2->setTag(8);
        button2->setTouchEnabled(true);
        button2->setTitleText("返回开始界面");
        button2->setTitleFontSize(36);
        button2->loadTextures(IMG_SETTING_BUTTON_NORMAL, IMG_SETTING_BUTTON_PRESSED);
        if (currentScene == "HomeScene") {
            button2->setPosition(
                Vec2(layout->getSize().width / 2.0, layout->getSize().height * 3.0 / 8.0));
        } else {
            button2->setPosition(
                Vec2(layout->getSize().width / 2.0, layout->getSize().height * 3.0 / 6.0));
        }
        button2->setScale(0.7);
        button2->addTouchEventListener(this, toucheventselector(SettingsLayer::touchEvent));
        layout->addChild(button2);
    }

    if (currentScene == "HomeScene") {
        auto button3 = Button::create();
        button3->setTag(9);
        button3->setTouchEnabled(true);
        button3->setTitleText("退出游戏");
        button3->setTitleFontSize(36);
        button3->loadTextures(IMG_SETTING_BUTTON_NORMAL, IMG_SETTING_BUTTON_PRESSED);
        button3->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 2.0 / 8.0));
        button3->setScale(0.7);
        button3->addTouchEventListener(this, toucheventselector(SettingsLayer::touchEvent));
        layout->addChild(button3);
    }

    if (currentScene == "GameplayScene") {
        auto button4 = Button::create();
        button4->setTag(10);
        button4->setTouchEnabled(true);
        button4->setTitleText("放弃关卡");
        button4->setTitleFontSize(36);
        button4->loadTextures(IMG_SETTING_BUTTON_NORMAL, IMG_SETTING_BUTTON_PRESSED);
        button4->setPosition(
            Vec2(layout->getSize().width / 2.0, layout->getSize().height * 2.0 / 6.0));
        button4->setScale(0.7);
        button4->addTouchEventListener(this, toucheventselector(SettingsLayer::touchEvent));
        layout->addChild(button4);
    }

    auto button5 = Button::create();
    button5->setTag(11);
    button5->setTouchEnabled(true);
    button5->setTitleText("返回");
    button5->setTitleFontSize(36);
    button5->loadTextures(IMG_SETTING_BUTTON_NORMAL, IMG_SETTING_BUTTON_PRESSED);
    if (currentScene == "MainMenuScene") {
        button5->setPosition(Vec2(layout->getSize().width / 2.0, layout->getSize().height / 5.0));
    }
    if (currentScene == "HomeScene") {
        button5->setPosition(Vec2(layout->getSize().width / 2.0, layout->getSize().height / 8.0));
    }
    if (currentScene == "GameplayScene") {
        button5->setPosition(Vec2(layout->getSize().width / 2.0, layout->getSize().height / 6.0));
    }
    button5->setScale(0.7);
    button5->addTouchEventListener(this, toucheventselector(SettingsLayer::touchEvent));
    layout->addChild(button5);

    addChild(layout);
    return true;
}

void
SettingsLayer::sliderEvent(Ref* pSender, Slider::EventType type)
{
    if (type == Slider::EventType::ON_PERCENTAGE_CHANGED) {
        auto slider = dynamic_cast<Slider*>(pSender);
        int percent = slider->getPercent();
        if (slider->getTag() == 2) {
            SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(percent / 100.0);
            GameData::getInstance()->saveBgmVolume(percent / 100.0);
        } else if (slider->getTag() == 4) {
            SimpleAudioEngine::getInstance()->setEffectsVolume(percent / 100.0);
            GameData::getInstance()->saveEffectsVolume(percent / 100.0);
        }
    }
}

void
SettingsLayer::touchEvent(Ref* pSender, TouchEventType type)
{
    Button* button = (Button*)pSender;
    int tag = button->getTag();
    if (type == TOUCH_EVENT_ENDED) {
        AudioController::getInstance()->playEffect("button_click.wav");
        switch (tag) {
            case 7: {
                this->removeFromParent();
                Director::getInstance()->pushScene(SaveScene::create(true));
                break;
            }
            case 8: {
                this->removeFromParent();
                AudioController::getInstance()->stopMusic();
                Director::getInstance()->popToRootScene();
                Director::getInstance()->replaceScene(MainMenuScene::create());
                break;
            }
            case 9: {
                Director::getInstance()->end();
                break;
            }
            case 10: {
                this->removeFromParent();
                Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
                Director::getInstance()->popToRootScene();
                Director::getInstance()->replaceScene(RoundSelectScene::create());
                break;
            }
            case 11: {
                if (currentScene == "GameplayScene") {
                    this->pauseNode->onEnter();
                }
                this->removeFromParent();
                break;
            }
            default:
                break;
        }
    }
}

void
SettingsLayer::setPauseNode(Node* node)
{
    this->pauseNode = node;
}
