#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>
using namespace std;

struct Character
{
    struct Attack
    {
        enum class Type
        {
            FOCUS,
            SPLIT
        };

        string tag;
        string description;
        Attack::Type type;
    };

    /* 人物展示 */

    string tag;        //角色标签
    string name;       //角色名
    string rectAvatar; //人物头像或图标地址
    string circularAvatar;
    string portrait;        //人物立绘，就一张
    vector<string> preview; //人物的动作预览图，在代码中组合成动态图片，
                            //用在 EquipScene 和 RoundSelectScene

    /* 人物动画 */

    string defaultTexture;
    vector<string> standFrame;
    vector<string> runFrame;
    vector<string> preJumpFrame;
    vector<string> jumpFrame;
    vector<string> preFallFrame;
    vector<string> fallFrame;
    vector<string> dashFrame;

	string standAnimationKey;
	string runAnimationKey;
	string preJumpAnimationKey;
	string jumpAnimationKey;
	string preFallAnimationKey;
	string fallAnimationKey;
	string dashAnimationKey;

    float standFrameDelay;
    float runFrameDelay;
    float preJumpFrameDelay;
    float jumpFrameDelay;
    float preFallFrameDelay;
    float fallFrameDelay;
    float dashFrameDelay;

    /* 人物各项属性 */

    int itemSlotNum;      //道具栏数量
    int spellCardSlotNum; //符卡栏数量

    int healthPointBase; //基础属性
    int healthPointInc;  //增量属性，可为负

    int manaBase;
    int manaInc;

    int walkSpeedBase;
    int walkMaxSpeed;
    float walkAccelerationTimeBase;
    float walkAccelerationBase;

    float dashAccelerationBase;

    int walkSpeedInc;
    float walkAccelerationInc;
    float dashAccelerationInc;
};

#endif
