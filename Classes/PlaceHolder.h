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
    PlaceHolder();
};

// 为了更灵活的配置 placeholder ，我们创建 PlaceHolderGenerator 类
// 它包含 PlaceHolder 类的所有功能，但更灵活同时也更复杂

class PlaceHolderGenerator
{
public:
    enum class PlaceHolderShape
    {
        RECT,
        CIRCLE,
    };

public:
    PlaceHolderGenerator();

    // 配置 placeholder
    // 其中 shape，size/radius 是必配置项
    PlaceHolderGenerator& setShape(PlaceHolderShape shape);
    PlaceHolderGenerator& setSize(Size s);    // for rect use only
    PlaceHolderGenerator& setRadius(float r); // for circle use only
    PlaceHolderGenerator& setEnableBorder(bool trueOrFalse);
    PlaceHolderGenerator& setBorderColor(const Color3B& borderColor);
    PlaceHolderGenerator& setText(const std::string& text);
    PlaceHolderGenerator& setFontSize(int size);
    PlaceHolderGenerator& setBgColor(const Color3B& bgColor);
    PlaceHolderGenerator& setTextColor(const Color3B& textColor);

    // 生成 placeholder
    Sprite* generate();

private:
    PlaceHolderShape shape;
    Size size;
    float radius;
    bool enableBorder;
    Color3B borderColor;
    std::string text;
    int fontSize;
    Color3B bgColor;
    Color3B textColor;
};

#endif
