#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include <cstring>
#include <string>

#include "ConversationLayerWithLua.h"
#include "GameData/Conversation.h"
#include "GameData/GameData.h"
#include "PlaceHolder.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"

#include "AudioController.h"
#include "SimpleAudioEngine.h"

#define CHANGE_TIME 1.0

ConversationLayer* ConversationLayer::_self;

ConversationLayer*
ConversationLayer::create(const string& cTag)
{
    ConversationLayer* cLayer = new (std::nothrow) ConversationLayer();
    cLayer->_pausedNode = nullptr;
    cLayer->_conversationTag = cTag;
    _self = cLayer;

    if (cLayer && cLayer->init()) {
        cLayer->autorelease();
        return cLayer;
    } else {
        delete cLayer;
        cLayer = nullptr;
        return nullptr;
    }
}

Scene*
ConversationLayer::createDebugScene()
{
    auto s = Scene::create();
    auto l = ConversationLayer::create("C1");

    if (s && l) {
        s->addChild(l);
        return s;
    } else {
        return nullptr;
    };
}

bool
ConversationLayer::init()
{
    /*  1. super init first */

    if (!Layer::init()) {
        return false;
    }

    /*  2. 拿到运行时系统数据(introspection) */

    _visibleSize = _director->getVisibleSize();

    /*  3. 加载 Lua 数据 */

    auto engine = LuaEngine::getInstance();
    engine->executeScriptFile("conversations/init.lua");
    for_each(_conversationTag.begin(), _conversationTag.end(), [](char& c) { c = tolower(c); });
    engine->executeScriptFile(("conversations/" + _conversationTag).c_str());

    /*  4. 初始化界面 */

    _bgp = Sprite::create("place_holder.png");
    _bgp->setPosition(_visibleSize / 2);
    _bgp->setContentSize(_visibleSize);
    addChild(_bgp);

    _lCharacter = Sprite::create();
    _lCharacter->setVisible(false);
    _lCharacter->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height / 2));
    _lCharacter->setContentSize(Size(0.35 * _visibleSize.width, _visibleSize.height));
    addChild(_lCharacter);

    _mCharacter = Sprite::create();
    _mCharacter->setVisible(false);
    _mCharacter->setPosition(_visibleSize * 0.5);
    _mCharacter->setContentSize(Size(0.3 * _visibleSize.width, _visibleSize.height));
    addChild(_mCharacter);

    _rCharacter = Sprite::create();
    _rCharacter->setVisible(false);
    _rCharacter->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height / 2));
    _rCharacter->setContentSize(Size(0.35 * _visibleSize.width, _visibleSize.height));
    _rCharacter->setFlippedX(true);
    addChild(_rCharacter);

    _textBlk = Sprite::create("place_holder.png");
    _textBlk->setColor(Color3B(69, 69, 69));
    // _textBlk->setOpacityModifyRGB(true);
    // _textBlk->setOpacity(64);
    _textBlk->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height * 0.1));
    _textBlk->setContentSize(Size(_visibleSize.width, _visibleSize.height * 0.2));
    addChild(_textBlk);

    _speaker = Label::createWithTTF("", "fonts/dengxian.ttf", 24);
    _speaker->enableBold();
    // 相对于 _textBlk 布局
    _speaker->setAnchorPoint(Vec2(0, 0));
    _speaker->setPosition(
        Vec2(_textBlk->getContentSize().width * 0.18, _textBlk->getContentSize().height * 0.7));
    _textBlk->addChild(_speaker);

    _text = Label::createWithTTF("", "fonts/dengxian.ttf", 18);
    _text->setColor(Color3B::WHITE);
    // 相对于 _textBlk 布局
    _text->setAnchorPoint(Vec2(0, 1));
    _text->setPosition(
        Vec2(_textBlk->getContentSize().width * 0.2, _textBlk->getContentSize().height * 0.55));
    _text->setMaxLineWidth(_textBlk->getContentSize().width * 0.6);
    _text->setLineSpacing(8);
    _textBlk->addChild(_text);

    /*  5. 设置触摸监听器，点击屏幕任意位置继续对话（同时截断触摸事件，不再向下传递） */

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* t, Event* e) -> bool {
        this->nextDialogue();
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void
ConversationLayer::setPauseNode(cocos2d::Node* node)
{
    _pausedNode = node;
}

void
ConversationLayer::onEnterTransitionDidFinish()
{
    AudioController::getInstance()->recordFormerMusic();
    nextDialogue();
}

void
ConversationLayer::nextDialogue()
{
    static int n = nDialogues(); // 总对话数量
    static int i = 0;            // 已经播放过的对话数量

    if (i < n) {
        // FIXME: call lua function

        scheduleOnce([this](float unused) -> void { this->nextDialogue(); }, CHANGE_TIME,
                     "schedule_next_dialogue");
        ++i;
    } else {
        quitConversation();
    }
}

void
ConversationLayer::quitConversation()
{
    _eventDispatcher->dispatchCustomEvent("conversation_end");

    this->removeFromParent();
}

void
ConversationLayer::onExit()
{
    if (_pausedNode != nullptr) {
        _pausedNode->onEnter();
    }

    AudioController::getInstance()->resumeFormerMusic();
}

ConversationLayer*
ConversationLayer::currInstance()
{
    return _self;
}

ConversationLayer*
ConversationLayer::changeBgp(const string& bgp)
{
#ifndef NDEBUG
    log("[ConversationLayer] change bgp %s", bgp.c_str());
#endif

    _bgp->setTexture(bgp.c_str());
    _bgp->setContentSize(_visibleSize);

    return this;
}

ConversationLayer*
ConversationLayer::changeBgm(const string& bgm)
{
    AudioController::getInstance()->playMusic(bgm, true);
    return this;
}

ConversationLayer*
ConversationLayer::changeCharacter(const string& pos, const string& name, const string& pic,
                                   const Color3B& color, const string& wordArt)
{
#ifndef NDEBUG
    log("[ConversationLayer] change %s character %s", pos.c_str(), name.c_str());
#endif

    if (name.empty()) {
        return this;
    }

    Sprite* cToChange = nullptr;
    if (pos == "LEFT") {
        cToChange = _lCharacter;
    } else if (pos == "MIDDLE") {
        cToChange = _mCharacter;
    } else if (pos == "RIGHT") {
        cToChange = _rCharacter;
    }

    if (pic.length() == 0) {
        cToChange->setVisible(false);
    } else {
        cToChange->setTexture(pic);
        cToChange->setVisible(true);
    }

    _speakerColor[name] = color;
    displayWordArt(pos, wordArt);

    return this;
}

ConversationLayer*
ConversationLayer::changeSpeakerColor(const string& speaker, const cocos2d::Color3B& color)
{
    if (!speaker.empty()) {
        _speakerColor[speaker] = color;
    }

    return this;
}

ConversationLayer*
ConversationLayer::changeText(const string& speaker, const string& text)
{
#ifndef NDEBUG
    log("[ConversationLayer] %s: %s", speaker.c_str(), text.c_str());
#endif

    _speaker->setString(speaker);
    _speaker->setColor(_speakerColor[speaker]);
    if (text.length() != 0)
        _text->setString("『" + text + "』");

    return this;
}

ConversationLayer*
ConversationLayer::playSoundEffect(const string& effect)
{
    static unsigned int lastEffect;

    if (lastEffect != 0) {
        AudioController::getInstance()->stopEffect(lastEffect);
    }
    lastEffect = AudioController::getInstance()->playEffect(effect);

    return this;
}

ConversationLayer*
ConversationLayer::playScreenEffect(const string& effect)
{
    // TODO
    return this;
}

int
ConversationLayer::nDialogues()
{
    // FIXME: call lua function
    auto engine = LuaEngine::getInstance();
    auto stack = engine->getLuaStack();

    return 0;
}

void
ConversationLayer::displayWordArt(const string& pos, const string& wordArt)
{
    // TODO
}
