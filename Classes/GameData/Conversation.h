#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "cocos2d.h"
#include <string>
#include <vector>

USING_NS_CC;
using namespace std;

struct ConversationIndicator
{
    string conversationTag; //对话标签
    string icon;            //头像或图标
    string name;            //对话人姓名
};

struct Dialogue
{
    enum class ScreenEffect
    {
        NONE,
        SHAKE, //屏幕晃动
    };

    enum class PicturePosition
    {
        NOT_GIVEN,
        LEFT,
        MID,
        RIGHT
    };

    /* 『人物与背景改变』与『文字内容与特效』不在一个 Dialogue 中同时出现。
     *
     *  - 『人物与背景改变』改变界面展示。ConversationLayer 播放完这一『改变』后，
     *     就自动切换到到下一 Dialogue
     *
     *  - 『文字与特效』播放文字。ConversationLayer 播放完这一『文字』后，
     *     需要由玩家手动点击才能到下一个 Dialogue
     */

    /* 人物与背景改变 */

    string characterName; // 人物名字（实际 ConversationLayer 中并不使用此字段）
    string characterNameWordArt; // 人名艺术字图片，当此字段不为空时，表示新人物出现
    string picture;              // 人物立绘或其他展示图片
    Dialogue::PicturePosition picPosition; // 人物立绘或其他展示图片的位置
    string bgp; // 对话时的背景图片，大多数时为空，出现变化时就更改背景图片
    string bgm; // 播放的背景音乐，大多数时为空，出现变化时就更改背景音乐

    /* 文字内容与特效 */

    string speakerName; //说话者姓名，可以为空
    Color3B speakerColor;
    string text;
    string soundEffect;                  //播放的音效
    Dialogue::ScreenEffect screenEffect; //对话时可能用到的特效
};

#endif
