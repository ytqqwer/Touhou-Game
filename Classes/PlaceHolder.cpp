#include "PlaceHolder.h"
#include <string>

PlaceHolder::PlaceHolder()
{
}

Sprite*
PlaceHolder::createRect(const Size& size, const std::string& text, int fontSize, Color3B bgColor,
                        Color3B textColor)
{
    /*  1. bgRect */

    auto bgRect = Sprite::create();
    bgRect->initWithFile("place_holder.png");
    bgRect->setContentSize(size);
    bgRect->setColor(bgColor);

    /*  2. text label  */

    auto textLabel = LabelTTF::create(text, "font/arial.ttf", fontSize);
    textLabel->setColor(textColor);
    textLabel->setPosition(size / 2);
    bgRect->addChild(textLabel);

    return bgRect;
}

Sprite*
PlaceHolder::createCircle(int radius, const std::string& text, int fontSize, Color3B bgColor,
                          Color3B textColor)
{

    /*  0. contrainer sprite */
    auto containerSprite = Sprite::create();
    containerSprite->setContentSize(Size(2 * radius, 2 * radius));

    /*  1. background circle */

    auto drawNode = DrawNode::create();
    drawNode->drawSolidCircle(Vec2(radius, radius), radius, 360, 128, Color4F(bgColor));
    containerSprite->addChild(drawNode);

    /*  2. text label */

    auto textLabel = LabelTTF::create(text, "font/arial.ttf", fontSize);
    textLabel->setColor(textColor);
    textLabel->setPosition(Vec2(radius, radius));
    containerSprite->addChild(textLabel);

    return containerSprite;
}
