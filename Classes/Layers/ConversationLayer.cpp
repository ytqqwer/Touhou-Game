#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include <cstring>
#include <string>

#include "ConversationLayer.h"
#include "GameData/Conversation.h"
#include "GameData/GameData.h"
#include "PlaceHolder.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"

#include "AudioController.h"
#include "SimpleAudioEngine.h"

ConversationLayer* ConversationLayer::_self = nullptr;

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

    loadLuaFile();

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
        this->unscheduleAllCallbacks();
        this->nextDialogue();
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void
ConversationLayer::onEnterTransitionDidFinish()
{
    AudioController::getInstance()->recordFormerMusic();
    nextDialogue();
}

void
ConversationLayer::setPauseNode(cocos2d::Node* node)
{
    _pausedNode = node;
}

void
ConversationLayer::nextDialogue()
{
    static int n = nDialogues(); // 总对话数量
    static int i = 0;            // 已经播放过的对话数量

    if (i < n) {
        ++i;
        // 若使用 LuaStack 来调用 Lua 函数，我们将不得不引入 LuaCallFunc、ScriptHandler、
        //   ScriptHandlerMgr、toluafix_xxx 等各种 cocos2dx **做给它自己用** 的设施；
        //   这样做只会徒增复杂度。
        //   此处，我们直接获取 lua_State 进行操作。
        auto L = LuaEngine::getInstance()->getLuaStack()->getLuaState();
        lua_settop(L, 0);
        lua_getglobal(L, "playDialogue");
        lua_getglobal(L, "dialogues");
        lua_pushinteger(L, i);
        lua_call(L, 2, 0);

        scheduleOnce([this](float unused) -> void { this->nextDialogue(); }, _dialogueInterval,
                     "schedule_next_dialogue" + to_string(i));

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
    Layer::onExit();

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
    log("[ConversationLayer] change %s character to %s", pos.c_str(), name.c_str());
#endif

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

    if (!name.empty()) {
        _speakerColor[name] = color;
        showWordArt(pos, wordArt);
    }

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

void
ConversationLayer::loadLuaFile()
{
    auto reportLuaErr = [](int errcode, int srcLine) {
        switch (errcode) {
            case LUA_ERRSYNTAX:
                log("[ConversationLayer] FATEL: %s:%d LUA_ERRSYNTAX", __FILE__, srcLine);
                break;
            case LUA_ERRFILE:
                log("[ConversationLayer] FATEL: %s:%d LUA_ERRFILE", __FILE__, srcLine);
                break;
            case LUA_ERRMEM:
                log("[ConversationLayer] FATEL: %s:%d LUA_ERRMEM", __FILE__, srcLine);
                break;
        }

    };

    // LuaEngine 和 LuaStack 默认忽略错误，不利于 Lua 代码的调试。
    //   此处我们直接使用 lua_XXX 和 luaL_XXX API

    lua_State* L = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    int ret;

// 在启动 cocos2d-x 引擎前，Java Activity 已经将 assets/gamedata 拷贝至
// writablePath/gamedata 了

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    string prefix = FileUtils::getInstance()->getWritablePath() + "LuaScripts/";
#endif

    /*  1. init.lua */

    string init_lua;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    init_lua = prefix + "conversations/init.lua";
#else
    init_lua = FileUtils::getInstance()->fullPathForFilename("conversations/init.lua");
#endif

    ret = luaL_loadfile(L, init_lua.c_str());
    if (ret) {
        reportLuaErr(ret, __LINE__);
        Director::getInstance()->end();
    }
    lua_call(L, 0, 0);

    /*  2. conversations/<conversationTag>.lua */

    for_each(_conversationTag.begin(), _conversationTag.end(), [](char& c) { c = tolower(c); });
    const string& luaFile = string("conversations/") + _conversationTag + ".lua";

    string luaFileFullPath;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    luaFileFullPath = prefix + luaFile;
#else
    luaFileFullPath = FileUtils::getInstance()->fullPathForFilename(luaFile);
#endif

    ret = luaL_loadfile(L, luaFileFullPath.c_str());
    if (ret) {
        reportLuaErr(ret, __LINE__);
        Director::getInstance()->end();
    }
    lua_call(L, 0, 0);
}

int
ConversationLayer::nDialogues()
{
    int n;
    lua_State* L;

    L = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    lua_settop(L, 0);
    lua_getglobal(L, "nDialogues");
    lua_getglobal(L, "dialogues");
    lua_call(L, 1, 1);
    n = lua_tointeger(L, 1);
    lua_settop(L, 0);
#ifndef NDEBUG
    log("[ConversationLayer] nDialogues : %d", n);
#endif

    return n;
}

void
ConversationLayer::setDialogueInterval(float v)
{
    _dialogueInterval = v;
}

void
ConversationLayer::showWordArt(const string& pos, const string& wordArt)
{
    // TODO
}
