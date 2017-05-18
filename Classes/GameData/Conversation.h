#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <string>
#include <vector>
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
        LEFT,
        MID,
        RIGHT
    };

    /* 对话的人 */

    string characterName;        //说话者姓名，可以为空
    string characterNameWordArt; //人物第一次出现时显示相应的艺术字图片
    string characterPicture;     //可以是人物立绘，也可以是其他图片，可以为空
    Dialogue::PicturePosition characterPicPosition; //图片显示的位置
    string content;

    /* 对话的背景 */

    string backgroundPicture; //对话时的背景图片，大多数时为空，出现变化时就更改背景图片
    string backgroundMusic; //播放的背景音乐，大多数时为空，出现变化时就更改背景音乐
    string soundEffect;                  //播放的音效
    Dialogue::ScreenEffect screenEffect; //对话时可能用到的特效
};

#endif
