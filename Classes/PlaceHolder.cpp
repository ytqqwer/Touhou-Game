#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "PlaceHolder.h"
#include <string>

using std::string;

////////////////////////////////////////////////////////////////////////////////
// PlaceHolder

PlaceHolder::PlaceHolder()
{
}

Sprite*
PlaceHolder::createRect(const Size& size, const std::string& text, int fontSize, Color3B bgColor,
                        Color3B textColor)
{
    PlaceHolderGenerator gr;

    gr.setShape(PlaceHolderGenerator::PlaceHolderShape::RECT)
        .setSize(size)
        .setText(text)
        .setFontSize(fontSize)
        .setBgColor(bgColor)
        .setTextColor(textColor);

    return gr.generate();
}

Sprite*
PlaceHolder::createCircle(int radius, const std::string& text, int fontSize, Color3B bgColor,
                          Color3B textColor)
{
    PlaceHolderGenerator gr;
    gr.setShape(PlaceHolderGenerator::PlaceHolderShape::CIRCLE)
        .setRadius(radius)
        .setText(text)
        .setFontSize(fontSize)
        .setBgColor(bgColor)
        .setTextColor(textColor);

    return gr.generate();
}

////////////////////////////////////////////////////////////////////////////////
// PlaceHolderGenerator

PlaceHolderGenerator::PlaceHolderGenerator()
    : shape(PlaceHolderShape::RECT)
    , size(200, 100)
    , radius(80.0)
    , enableBorder(false)
    , borderColor(Color3B::ORANGE)
    , fontSize(16)
    , bgColor(Color3B(128, 128, 128))
    , textColor(Color3B::WHITE)
{
}

// 重复性的东西让编译器帮我们生成

/* 1. 定义宏 */

#ifdef APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_SET
#undef APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_SET
#endif
#define APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_SET(varType, varName, funName)                        \
    PlaceHolderGenerator& PlaceHolderGenerator::set##funName(varType param)                        \
    {                                                                                              \
        varName = param;                                                                           \
        return *this;                                                                              \
    }

/* 2. 使用宏 */

APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_SET(PlaceHolderShape, shape, Shape)
APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_SET(Size, size, Size)
APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_SET(float, radius, Radius)
APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_SET(bool, enableBorder, EnableBorder)
APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_SET(int, fontSize, FontSize)

/* 1. 定义宏 */

#ifdef APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_REF_SET
#undef APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_REF_SET
#endif
#define APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_REF_SET(varType, varName, funName)                    \
    PlaceHolderGenerator& PlaceHolderGenerator::set##funName(const varType& param)                 \
    {                                                                                              \
        varName = param;                                                                           \
        return *this;                                                                              \
    }

/* 2. 使用宏 */

APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_REF_SET(string, text, Text)
APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_REF_SET(Color3B, bgColor, BgColor)
APP_SYNTHESIZE_PLACEHOLDER_GENERATOR_REF_SET(Color3B, textColor, TextColor)

Sprite*
PlaceHolderGenerator::generate()
{
    /*  0. contrainer sprite */

    auto containerSprite = Sprite::create();
    if (shape == PlaceHolderShape::RECT) {
        containerSprite->setContentSize(size);
    } else {
        containerSprite->setContentSize(Size(2 * radius, 2 * radius));
    }

    /*  bgRect/bgCircle & border */

    auto bgDrawer = DrawNode::create();

    if (shape == PlaceHolderShape::RECT) {
        bgDrawer->drawSolidRect(Vec2(0, 0), Vec2(size), Color4F(bgColor));
        if (enableBorder) {
            bgDrawer->drawRect(Vec2(0, 0), Vec2(size), Color4F(borderColor));
        }
    } else {
        bgDrawer->drawSolidCircle(Vec2(radius, radius), radius, 360, 128, Color4F(bgColor));
        if (enableBorder) {
            bgDrawer->drawCircle(Vec2(radius, radius), radius, 360, 128, false,
                                 Color4F(borderColor));
        }
    }

    containerSprite->addChild(bgDrawer);

    /*  text */

    auto textLabel = Label::createWithTTF(text, "fonts/dengxian.ttf", fontSize);
    textLabel->setColor(textColor);

    if (shape == PlaceHolderShape::RECT) {
        textLabel->setPosition(size / 2);
    } else {
        textLabel->setPosition(Vec2(radius, radius));
    }

    containerSprite->addChild(textLabel);

    return containerSprite;
}
