#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef PLACE_HOLDER_H
#define PLACE_HOLDER_H

#include "cocos2d.h"
#include <string>
USING_NS_CC;

class PlaceHolder
{
public:
    static Sprite* createRect(const Size& size, const std::string& text, int fontSize = 16,
                              Color3B bgColor = Color3B(128, 128, 128),
                              Color3B textColor = Color3B::WHITE);

    static Sprite* createCircle(int radius, const std::string& text, int fontSize = 16,
                                Color3B bgColor = Color3B(128, 128, 128),
                                Color3B textColor = Color3B::WHITE);

private:
    PlaceHolder(); // for autorelease consideration, make ctor private
};

#endif
