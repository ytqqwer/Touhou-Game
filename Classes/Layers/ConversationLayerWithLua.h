#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef CONVERSATION_LAYER_H
#define CONVERSATION_LAYER_H

#include "GameData/Conversation.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class ConversationLayer : public Layer
{
public:
    static ConversationLayer* create(const string& conversationTag);
    static Scene* createDebugScene();

    // for GameplayScene use
    void setPauseNode(Node* node);

    virtual bool init() override;
    virtual void onEnterTransitionDidFinish() override;
    void nextDialogue();
    void quitConversation();
    virtual void onExit() override;

    // For Lua use, 若改变此处的接口定义，记得重新生成 lua 绑定
    //   - play/displayXXX 表示这个 XXX 属性/效果 只存在于当前 dialogue，
    //      在当前 dialogue 结束后属性/效果 会自动消失
    //   - changeXXX 表示这个 XXX 属性/效果 会一直存在于后续 dialogue 中，直至显式改变
    ConversationLayer* currInstance(); // Lua 端需要一个获取到当前 ConversationLayer 的接口
    ConversationLayer* changeBgp(const string& bgp);
    ConversationLayer* changeBgm(const string& bgm);
    ConversationLayer* changeCharacter(const string& pos, const string& name,
                                       const string& pic = "none",
                                       const Color3B& color = Color3B::WHITE,
                                       const string& wordArt = "none");
    ConversationLayer* changeSpeakerColor(const string& speaker, const Color3B& color);
    ConversationLayer* changeText(const string& speaker, const string& text);
    ConversationLayer* playSoundEffect(const string& effect);
    ConversationLayer* playScreenEffect(const string& effect);

private:
    ConversationLayer() = default; // for autorelease consideration, make ctor private

    void loadLuaScript(const string& script);
    int nDialogues();
    void displayWordArt(const string& pos, const string& wordArt);

private:
    static ConversationLayer* _self; // for getInstace() use

    // intorspection
    Size _visibleSize;

    Node* _pausedNode;
    string _conversationTag;

    Sprite* _bgp;
    Sprite* _lCharacter;
    Sprite* _mCharacter;
    Sprite* _rCharacter;

    Sprite* _textBlk;
    Label* _speaker;
    Label* _text;

    std::map<string, Color3B> _speakerColor;
};

#endif // CONVERSATION_LAYER_H
