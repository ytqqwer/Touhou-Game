#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ENEMYDATA_H
#define ENEMYDATA_H

#include <string>
#include <vector>
using namespace std;

struct EnemyData
{
    string tag;  //敌人标签
    string name; //敌人名字

    string face;

    /* 动画 */

    string defaultTexture;
    vector<string> standFrame;
    vector<string> runFrame;
    vector<string> preJumpFrame;
    vector<string> jumpFrame;
    vector<string> preFallFrame;
    vector<string> fallFrame;
    vector<string> dashFrame;
    vector<string> hitFrame;
    vector<string> downFrame;

    string standAnimationKey;
    string runAnimationKey;
    string preJumpAnimationKey;
    string jumpAnimationKey;
    string preFallAnimationKey;
    string fallAnimationKey;
    string dashAnimationKey;
    string hitAnimationKey;
    string downAnimationKey;

    float standFrameDelay;
    float runFrameDelay;
    float preJumpFrameDelay;
    float jumpFrameDelay;
    float preFallFrameDelay;
    float fallFrameDelay;
    float dashFrameDelay;
    float hitFrameDelay;
    float downFrameDelay;

    //属性
    int healthPoint;
};

#endif
