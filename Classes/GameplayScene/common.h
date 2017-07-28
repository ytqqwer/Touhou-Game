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

static const int enemyTag = 103;
static const int bulletTag = 102;
static const int playerTag = 101;
static const int groundTag = 100;
static const int polylineTag = 99;
static const int polygonTag = 98;

//游戏场景的重力
static const int gameGravity = 1000.0f;

// EventFilterManager 中的 filter 所工作的 priority
#define EVENT_FILTER_PRIORITY -1024

#endif
