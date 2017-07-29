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

    virtual bool init() override;
    virtual void onEnterTransitionDidFinish() override;
    // virtual void update(float dt);

    void setPauseNode(Node* node);

private:
    ConversationLayer() = default; // for autorelease consideration, make ctor private

    void nextDialogue();
    void quitConversation();

    /* 人物与背景改变  */

    void changePic(Dialogue::PicturePosition pos, const string& pic);
    void displayWordArt(const string& wordArt, Dialogue::PicturePosition pos);
    void changeBgp(const string& bgp);
    void changeBgm(const string& bgm);

    /* 文字内容与特效 */

    void changeText(const string& speaker, const Color3B& speakerColor, const string& text);
    void playSoundEffect(const string& effect);
    void playScreenEffect(Dialogue::ScreenEffect effect);

private:
    // intorspection
    Size _visibleSize;

    Node* _pausedNode;
    string _conversationTag;
    vector<Dialogue> _dialogues;
    int _currIdx;

    Sprite* _bgp;
    Sprite* _lPic;
    Sprite* _mPic;
    Sprite* _rPic;

    Sprite* _textBlk;
    Label* _speaker;
    Label* _text;
};

#endif // CONVERSATION_LAYER_H
