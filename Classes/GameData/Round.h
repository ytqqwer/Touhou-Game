#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ROUND_H
#define ROUND_H

#include <string>
using namespace std;

struct Round
{
    enum class Status
    {
        COMPLETE, //已通过
        LOCKED,   //锁定
        UNLOCK    //解锁
    };

    enum class Difficulty
    {
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE
    };

    string tag;                   //关卡标签
    string name;                  //关卡名
    string previewPicture;        //预览图片
    string description;           //关卡描述
    Round::Difficulty difficulty; //关卡难度评级

    string TMXMap; //该关卡对应的TMX地图
};

#endif
