#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef COMMON_H
#define COMMON_H

//游戏中用到的种别掩码
static const int groundCategory = 0x1 << 1;
static const int playerCategory = 0x1 << 2;
static const int bulletCategory = 0x1 << 3;
static const int enemyCategory = 0x1 << 4;
static const int lockCategory = 0x1 << 5;

static const int enemyCategoryTag = 103;
static const int bulletCategoryTag = 102;
static const int playerCategoryTag = 101;
static const int groundCategoryTag = 100;
static const int polylineCategoryTag = 99;
static const int polygonCategoryTag = 98;

//摄像机
static const int cameraTag = 50;

//游戏场景的重力
static const int gameGravity = 1000.0f;

// EventFilterManager 中的 filter 所工作的 priority
#define EVENT_FILTER_PRIORITY -1024

#endif
