#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef COMMON_H
#define COMMON_H

#include "cocos2d.h"
using namespace cocos2d;

enum class Direction
{
    RIGHT = 1,
    LEFT = 2
};

//游戏中用到的种别掩码
static const int groundCategory = 0x1 << 1;
static const int playerCategory = 0x1 << 2;
static const int bulletCategory = 0x1 << 3;
static const int enemyCategory = 0x1 << 4;
static const int eventCategory = 0x1 << 5;
static const int lockCategory = 0x1 << 6;
static const int elevatorCategory = 0x1 << 7;

static const int elevatorCategoryTag = 1005;
static const int eventCategoryTag = 1004;
static const int enemyCategoryTag = 1003;
static const int bulletCategoryTag = 1002;
static const int playerCategoryTag = 1001;
static const int groundCategoryTag = 1000;
static const int polylineCategoryTag = 999;
static const int polygonCategoryTag = 998;
static const int lockCategoryTag = 997;

//摄像机
static const int cameraTag = 50;

//游戏场景的重力
static const int gameGravity = 1000.0f;

// EventFilterManager 中的 filter 所工作的 priority
#define EVENT_FILTER_PRIORITY -1024

struct DamageInfo
{
    Node* target;
    unsigned int damage;
};

struct Hp_Mp_Change
{
    std::string tag;
    Node* target;
    int value;
};

#endif
