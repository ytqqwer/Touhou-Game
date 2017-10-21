#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "ConversationLayer.h"
#include "GameData/Conversation.h"
#include "GameData/GameData.h"
#include "PlaceHolder.h"

#include "AudioController.h"
#include "SimpleAudioEngine.h"

#define CHANGE_TIME 1.0

ConversationLayer*
ConversationLayer::create(const string& cTag)
{
    ConversationLayer* cLayer = new (std::nothrow) ConversationLayer();
    cLayer->_pausedNode = nullptr;
    cLayer->_conversationTag = cTag;

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

    /*  3. 从 GameData 那里拿到对话数据 */

    _dialogues = GameData::getInstance()->getConversation(_conversationTag);
    _currIdx = 0;

    /*  4. 初始化界面 */

    _bgp = Sprite::create("place_holder.png");
    _bgp->setPosition(_visibleSize / 2);
    _bgp->setContentSize(_visibleSize);
    addChild(_bgp);

    _lPic = Sprite::create();
    _lPic->setVisible(false);
    _lPic->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height / 2));
    _lPic->setContentSize(Size(0.35 * _visibleSize.width, _visibleSize.height));
    addChild(_lPic);

    _mPic = Sprite::create();
    _mPic->setVisible(false);
    _mPic->setPosition(_visibleSize * 0.5);
    _mPic->setContentSize(Size(0.3 * _visibleSize.width, _visibleSize.height));
    addChild(_mPic);

    _rPic = Sprite::create();
    _rPic->setVisible(false);
    _rPic->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height / 2));
    _rPic->setContentSize(Size(0.35 * _visibleSize.width, _visibleSize.height));
    _rPic->setFlippedX(true);
    addChild(_rPic);

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

    AudioController::getInstance()->recordFormerMusic();

    return true;
}

void
ConversationLayer::onEnterTransitionDidFinish()
{
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
    unscheduleAllCallbacks();
    changeText("", Color3B::BLACK, "");

    /*  1. 是『人物与背景改变』还是『文字内容与特效』？ */

    if (_currIdx == _dialogues.size() - 1) {
        quitConversation();
        return;
    }

    auto& d = _dialogues[_currIdx++];

    if (d.speakerName.length() == 0) {
#ifndef NDEBUG
        log("[Conversationlayer] >> current dialogue is change");
#endif
        // 『人物与背景改变』
        if (d.characterNameWordArt.length() != 0)
            displayWordArt(d.characterNameWordArt, d.picPosition);

        if (d.picPosition != Dialogue::PicturePosition::NOT_GIVEN)
            changePic(d.picPosition, d.picture);

        if (d.bgp.length() != 0)
            changeBgp(d.bgp);

        if (d.bgm.length() != 0)
            changeBgm(d.bgm);

        scheduleOnce([this](float unused) -> void { this->nextDialogue(); }, CHANGE_TIME,
                     "schedule_next_dialogue");
    } else {
#ifndef NDEBUG
        log("[Conversationlayer] >> current dialogue is text");
#endif
        // 『文字内容与特效』
        changeText(d.speakerName, d.speakerColor, d.text);

        if (d.soundEffect.length() != 0)
            playSoundEffect(d.soundEffect);

        playScreenEffect(d.screenEffect);
    }
}

void
ConversationLayer::quitConversation()
{
    if (_pausedNode != nullptr) {
        _pausedNode->onEnter();
    }

    _eventDispatcher->dispatchCustomEvent("conversation_end");

    this->removeFromParent();

    AudioController::getInstance()->resumeFormerMusic();
}

void
ConversationLayer::changePic(Dialogue::PicturePosition pos, const string& pic)
{
#ifndef NDEBUG
    log("[ConversationLayer] change/remove pic");
#endif

    Sprite* picToChange = nullptr;
    if (pos == Dialogue::PicturePosition::LEFT) {
        picToChange = _lPic;
    } else if (pos == Dialogue::PicturePosition::MID) {
        picToChange = _mPic;
    } else if (pos == Dialogue::PicturePosition::RIGHT) {
        picToChange = _rPic;
    }

    if (pic.length() == 0) {
        picToChange->setVisible(false);
    } else {
        picToChange->setTexture(pic);
        picToChange->setVisible(true);
    }
}

void
ConversationLayer::displayWordArt(const string& wordArt, Dialogue::PicturePosition pos)
{
}

void
ConversationLayer::changeBgp(const string& bgp)
{
#ifndef NDEBUG
    log("[ConversationLayer] change bgp");
#endif

    _bgp->setTexture(bgp.c_str());
    _bgp->setContentSize(_visibleSize);
}

void
ConversationLayer::changeBgm(const string& bgm)
{
    AudioController::getInstance()->playMusic(bgm, true);
}

void
ConversationLayer::changeText(const string& speaker, const Color3B& color, const string& text)
{
#ifndef NDEBUG
    log("[ConversationLayer] text: %s", text.c_str());
#endif

    _speaker->setString(speaker);
    _speaker->setColor(color);
    if (text.length() != 0)
        _text->setString("『" + text + "』");
}

void
ConversationLayer::playSoundEffect(const string& effect)
{
    static unsigned int lastEffect;

    if (lastEffect != 0) {
        AudioController::getInstance()->stopEffect(lastEffect);
    }
    lastEffect = AudioController::getInstance()->playEffect(effect);
}

void
ConversationLayer::playScreenEffect(Dialogue::ScreenEffect effect)
{
    if (effect == Dialogue::ScreenEffect::SHAKE) {
        log("[ConversationLayer] play screenEffect 'SHAKE'");
    } else if (effect == Dialogue::ScreenEffect::NONE) {
        log("[ConversationLayer] play screenEffect 'NONE'");
    }
}
