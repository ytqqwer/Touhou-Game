#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "InventoryScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "ui/CocosGUI.h"
// #include "resources.h.dir/inventory.h"
#include <string>

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string InventoryScene::TAG{ "InventoryScene" };

InventoryScene::InventoryScene()
{
    _visibleSize = _director->getVisibleSize();
    int sum = 0;
}

bool
InventoryScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("InventoryScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  3. PlaceHolder */

    /*auto p = PlaceHolder::createCircle(100, "InventoryScene");
    p->setPosition(_visibleSize / 2);
    this->addChild(p);*/

    /*  4. schedule */

    this->scheduleUpdate();
    /*background*/
    auto bg = Sprite::create("InventoryScene/bg.png");
    bg->setContentSize(Size(_visibleSize.width, _visibleSize.height));
    bg->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.5));
    this->addChild(bg, ceng);
    /*background2*/
    auto bg_2 =
        PlaceHolder::createRect(Size(_visibleSize.width * 0.859, _visibleSize.height * 0.78), "",
                                16, Color3B(91, 155, 213));
    bg_2->setPosition(_visibleSize / 2);
    addChild(bg_2, ceng);
    /*background3*/
    auto bg_3 = PlaceHolder::createRect(
        Size(_visibleSize.width * 0.595, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
    bg_3->setAnchorPoint(Vec2(0, 0));
    bg_3->setPosition(Vec2(_visibleSize.width * 0.29, _visibleSize.height * 0.141));
    addChild(bg_3, ceng);

    /*backButton*/
    auto backButton = Button::create("InventoryScene/p1.png", "", "");
    backButton->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    backButton->setTitleText("返回");
    backButton->setTitleColor(Color3B::WHITE);
    backButton->setContentSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.15));
    backButton->addTouchEventListener(
        [](Ref* pSender, Widget::TouchEventType type) { Director::getInstance()->popScene(); });
    this->addChild(backButton);

    /*normalProps*/
    auto normalProps = Button::create("InventoryScene/p2.png", "", "");
    normalProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.45));
    normalProps->setTitleText("普通道具");
    normalProps->setTitleFontSize(15);
    normalProps->setTitleColor(Color3B(91, 155, 213));
    normalProps->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    this->addChild(normalProps, ceng);
    normalProps->addTouchEventListener(CC_CALLBACK_2(InventoryScene::getNoramlitem, this));
    normalProps->setTouchEnabled(true);

    /*strongProps*/
    auto strongProps = Button::create("InventoryScene/p2.png", "", "");
    strongProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.6));
    strongProps->setTitleText("强化道具");
    strongProps->setTitleFontSize(15);
    strongProps->setTitleColor(Color3B(91, 155, 213));
    strongProps->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    this->addChild(strongProps, ceng);
    strongProps->addTouchEventListener(CC_CALLBACK_2(InventoryScene::getStrengitem, this));
    strongProps->setTouchEnabled(true);

    /*specialProps*/
    auto specialProps = Button::create("InventoryScene/p2.png", "", "");
    specialProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.75));
    specialProps->setTitleText("特殊道具");
    specialProps->setTitleFontSize(15);
    specialProps->setTitleColor(Color3B(91, 155, 213));
    specialProps->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    this->addChild(specialProps, ceng);
    specialProps->addTouchEventListener(CC_CALLBACK_2(InventoryScene::getSpecialitem, this));
    specialProps->setTouchEnabled(true);

    /*初始普通道具*/
    // float high = 0;
    // auto people = gamedata->getAvailableCharacterList();
    // auto props = gamedata->getAvailableItemList();
    // auto peopleprops1 = gamedata->getCharacterItemList(people[0].tag);
    // auto peopleprops2 = gamedata->getCharacterItemList(people[1].tag);
    // auto peopleprops3 = gamedata->getCharacterItemList(people[2].tag);
    // for (int i = 0; i < props.size(); i++)
    //{
    //	if (props[i].first.type == Item::Type::NORMAL)
    //	{
    //		typeflag[i] = 1;
    //	}
    //}
    // for (int i = 0; i < props.size(); i++)
    //{
    //	if (typeflag[i] == 1)
    //	{
    //		auto box =
    //			PlaceHolder::createRect(Size(_visibleSize.width * 0.590, _visibleSize.height * 0.07),
    //"",
    //				16, Color3B(91, 155, 213));
    //		box->setAnchorPoint(Vec2(0, 1));
    //		box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688 -
    //high));
    //		bg_3->addChild(box, ceng);
    //		high = high + _visibleSize.height*0.1;
    //		/*图标*/
    //		auto pic = PlaceHolder::createRect(Size(_visibleSize.height * 0.07,
    //_visibleSize.height*0.07), "",
    //			16, Color3B::BLACK);
    //		pic->setAnchorPoint(Vec2(0, 0));
    //		pic->setPosition(Vec2(0, 0));
    //		box->addChild(pic, ceng);

    //		auto tool = Sprite::create(props[i].first.icon);
    //		tool->setContentSize(
    //			Size(_visibleSize.width *0.04, _visibleSize.height * 0.04));
    //		tool->setAnchorPoint(Vec2(0, 0));
    //		pic->addChild(tool, ceng);
    //	}
    //}
    ///*道具名称*/
    // Label* name[10];
    // double height = 0;
    // for (int i = 0; i < props.size(); i++)
    //{
    //	if (typeflag[i] == 1)
    //	{
    //		name[i] = Label::createWithTTF(props[i].first.name, "fonts/dengxian.ttf", 20);
    //		name[i]->setAnchorPoint(Vec2(0, 1));
    //		name[i]->setPosition(Vec2(_visibleSize.width * 0.073, _visibleSize.height * 0.670 -
    //height));
    //		name[i]->setColor(Color3B::BLACK);
    //		bg_3->addChild(name[i], ceng);
    //		height = height + _visibleSize.height*0.1;
    //	}
    //}

    ///*道具功能描述*/
    // Label* describe[10];
    // height = 0;
    // for (int i = 0; i < props.size(); i++)
    //{
    //	if (typeflag[i] == 1)
    //	{
    //		name[i] = Label::createWithTTF(props[i].first.description, "fonts/dengxian.ttf",
    //20);
    //		name[i]->setAnchorPoint(Vec2(0, 1));
    //		name[i]->setPosition(Vec2(_visibleSize.width * 0.203, _visibleSize.height * 0.670 -
    //height));
    //		name[i]->setColor(Color3B::BLACK);
    //		bg_3->addChild(name[i], ceng);
    //		height = height + _visibleSize.height*0.1;
    //	}
    //}

    ///*使用者*/
    // for (int i = 0; i < props.size(); i++)
    //{
    //	for (int j = 0; j < peopleprops1.size(); j++)
    //	{
    //		if (props[i].first.tag == peopleprops1[j].tag)
    //		{
    //			userflag[i] = 1;
    //		}
    //	}
    //	for (int j = 0; j < peopleprops2.size(); j++)
    //	{
    //		if (props[i].first.tag == peopleprops2[j].tag)
    //		{
    //			userflag[i] = 2;
    //		}
    //	}
    //	for (int j = 0; j < peopleprops3.size(); j++)
    //	{
    //		if (props[i].first.tag == peopleprops3[j].tag)
    //		{
    //			userflag[i] = 3;
    //		}
    //	}
    //}
    // Label* user[30];
    // string content[4] = { "使用者：Hakurei Reimu", "使用者：Kirisame Marisa", "使用者：Alice"
    // ,"无使用者"};
    // height = 0;
    // for (int i = 0; i < props.size(); i++)
    //{
    //	if (typeflag[i] == 1)
    //	{
    //		if (userflag[i] == 1)
    //		{
    //			name[i] = Label::createWithTTF(content[0], "fonts/dengxian.ttf", 20);
    //			name[i]->setAnchorPoint(Vec2(0, 1));
    //			name[i]->setPosition(Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 -
    //height));
    //			name[i]->setColor(Color3B::BLACK);
    //			bg_3->addChild(name[i], ceng);
    //			height = height + _visibleSize.height*0.1;
    //		}
    //		else if (userflag[i] == 2)
    //		{
    //			name[i] = Label::createWithTTF(content[1], "fonts/dengxian.ttf", 20);
    //			name[i]->setAnchorPoint(Vec2(0, 1));
    //			name[i]->setPosition(Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 -
    //height));
    //			name[i]->setColor(Color3B::BLACK);
    //			bg_3->addChild(name[i], ceng);
    //			height = height + _visibleSize.height*0.1;
    //		}
    //		else if (userflag[i] == 3)
    //		{
    //			name[i] = Label::createWithTTF(content[2], "fonts/dengxian.ttf", 20);
    //			name[i]->setAnchorPoint(Vec2(0, 1));
    //			name[i]->setPosition(Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 -
    //height));
    //			name[i]->setColor(Color3B::BLACK);
    //			bg_3->addChild(name[i], ceng);
    //			height = height + _visibleSize.height*0.1;
    //		}
    //		else
    //		{
    //			name[i] = Label::createWithTTF(content[3], "fonts/dengxian.ttf", 20);
    //			name[i]->setAnchorPoint(Vec2(0, 1));
    //			name[i]->setPosition(Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 -
    //height));
    //			name[i]->setColor(Color3B::BLACK);
    //			bg_3->addChild(name[i], ceng);
    //			height = height + _visibleSize.height*0.1;
    //		}

    //	}
    //}

    ///*获得道具*/
    // auto people= gamedata->getAvailableCharacterList();
    // auto peopleprops= gamedata->getCharacterItemList(people[0].tag);
    // float high = 0;
    // for (int i = 0; i < peopleprops.size(); i++)
    //{
    //	/*auto tool = Sprite::create(peopleprops[0].icon);
    //	if (peopleprops[0].type == Item::Type::NORMAL)
    //	{
    //		tool->setContentSize(
    //			Size(_visibleSize.width *0.05, _visibleSize.height * 0.05));
    //		tool->setColor(Color3B::WHITE);
    //		tool->setPosition(Vec2(_visibleSize.width * 0.30, _visibleSize.height * 0.142));
    //		addChild(tool, 1);
    //	}*/
    //	auto box =
    //		PlaceHolder::createRect(Size(_visibleSize.width * 0.590, _visibleSize.height * 0.07),
    //"",
    //			16, Color3B(91, 155, 213));
    //	box->setAnchorPoint(Vec2(0,1));
    //	box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688-high));
    //	bg_3->addChild(box,0);
    //	high=high+ _visibleSize.height*0.1;
    //	/*图标*/
    //	auto pic= PlaceHolder::createRect(Size(_visibleSize.height * 0.07, _visibleSize.height*0.07
    //), "",
    //		16, Color3B::BLACK);
    //	pic->setAnchorPoint(Vec2(0, 0));
    //	pic->setPosition(Vec2(0, 0));
    //	box->addChild(pic, 1);

    //	auto tool = Sprite::create(peopleprops[i].icon);
    //	tool->setContentSize(
    //		Size(_visibleSize.width *0.04, _visibleSize.height * 0.04));
    //	tool->setAnchorPoint(Vec2(0, 0));
    //	pic->addChild(tool, 2);
    //}
    ///*道具名称*/
    // Label* name[10];
    // double height = 0;
    // for (int i = 0; i < peopleprops.size(); i++)
    //{
    //	name[i] = Label::createWithTTF(peopleprops[i].name, "fonts/dengxian.ttf", 20);
    //	name[i]->setAnchorPoint(Vec2(0, 1));
    //	name[i]->setPosition(Vec2(_visibleSize.width * 0.073, _visibleSize.height * 0.670 -
    //height));
    //	name[i]->setColor(Color3B::BLACK);
    //	bg_3->addChild(name[i],2);
    //	height=height+_visibleSize.height*0.1;
    //}

    ///*道具功能描述*/
    // Label* describe[10];
    // height = 0;
    // for (int i = 0; i < peopleprops.size(); i++)
    //{
    //	name[i] = Label::createWithTTF(peopleprops[i].description, "fonts/dengxian.ttf", 20);
    //	name[i]->setAnchorPoint(Vec2(0, 1));
    //	name[i]->setPosition(Vec2(_visibleSize.width * 0.203, _visibleSize.height * 0.670 -
    //height));
    //	name[i]->setColor(Color3B::BLACK);
    //	bg_3->addChild(name[i], 2);
    //	height = height + _visibleSize.height*0.1;
    //}

    /*test*/
    // auto props = gamedata->getAvailableItemList();
    // auto people = gamedata->getAvailableCharacterList();
    // auto peopleprops1 = gamedata->getCharacterItemList(people[0].tag);

    // for (int i = 0; i < props.size(); i++)
    //{
    //	for (int j = 0; j < peopleprops1.size(); j++)
    //	{
    //		if (props[i].first.tag == peopleprops1[j].tag)
    //			usenumber[i] = 1;
    //	}
    //	if (props[i].first.type == Item::Type::NORMAL)
    //	{
    //		typeflag[i] = 1;
    //	}
    //	else if(props[i].first.type==Item::Type::STRENGTHEN)
    //	{
    //		typeflag[i] = 2;
    //	}
    //	else
    //	{
    //		typeflag[i] = 3;
    //	}
    //}

    return true;
}

// void
// InventoryScene::getNoramlitem(Ref* ref, Widget::TouchEventType type)
//{
//
//	if (type != Widget::TouchEventType::ENDED)
//		return;
//	for (int i = 1; i <= sum; i++) {
//		removeChildByTag(i);
//	}
//	sum = 0;
//	auto props = gamedata->getAvailableItemList();
//	auto people = gamedata->getAvailableCharacterList();
//	auto peopleprops1 = gamedata->getCharacterItemList(people[0].tag);
//	auto peopleprops2 = gamedata->getCharacterItemList(people[1].tag);
//	for (int i = 0; i < props.size(); i++)
//	{
//		for (int j = 0; j < peopleprops1.size(); j++)
//		{
//			if (props[i].first.tag == peopleprops1[j].tag)
//			{
//				usenumber[i] = 1;
//			}
//			else if (props[i].first.tag == peopleprops2[j].tag)
//			{
//				usenumber[i] = 2;
//			}
//		}
//		if (props[i].first.type == Item::Type::NORMAL)
//		{
//			typeflag[i] = 1;
//		}
//
//	}
//	float high = 0;
//	for (int i = 0; i < props.size(); i++)
//	{
//		auto box =
//			PlaceHolder::createRect(Size(_visibleSize.width * 0.590, _visibleSize.height *
//0.07), "",
//				16, Color3B(91, 155, 213));
//		box->setAnchorPoint(Vec2(0, 1));
//		box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688 -
//high));
//		addChild(box, 0,++sum);
//		high = high + _visibleSize.height*0.1;
//		/*图标*/
//		auto pic = PlaceHolder::createRect(Size(_visibleSize.height * 0.07,
//_visibleSize.height*0.07), "",
//			16, Color3B::BLACK);
//		pic->setAnchorPoint(Vec2(0, 0));
//		pic->setPosition(Vec2(0, 0));
//		box->addChild(pic,1,++sum);
//
//		auto tool = Sprite::create(props[i].first.icon);
//		tool->setContentSize(
//			Size(_visibleSize.width *0.04, _visibleSize.height * 0.04));
//		tool->setAnchorPoint(Vec2(0, 0));
//		pic->addChild(tool, 2,++sum);
//
//	}
//
//}

void
InventoryScene::update(float dt)
{
}

/*普通道具*/
void
InventoryScene::getNoramlitem(Ref* ref, Widget::TouchEventType type)
{
    ceng = ceng + 5;
    ;
    if (type != Widget::TouchEventType::ENDED)
        return;
    auto bg_3 = PlaceHolder::createRect(
        Size(_visibleSize.width * 0.595, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
    bg_3->setAnchorPoint(Vec2(0, 0));
    bg_3->setPosition(Vec2(_visibleSize.width * 0.29, _visibleSize.height * 0.141));
    addChild(bg_3, ceng);
    float high = 0;
    auto people = gamedata->getAvailableCharacterList();
    auto props = gamedata->getAvailableItemList();
    auto peopleprops1 = gamedata->getCharacterItemList(people[0].tag);
    auto peopleprops2 = gamedata->getCharacterItemList(people[1].tag);
    auto peopleprops3 = gamedata->getCharacterItemList(people[2].tag);
    for (int i = 0; i < props.size(); i++) {
        if (props[i].first.type == Item::Type::NORMAL) {
            typeflag[i] = 1;
        }
    }
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 1) {
            auto box = PlaceHolder::createRect(
                Size(_visibleSize.width * 0.590, _visibleSize.height * 0.07), "", 16,
                Color3B(91, 155, 213));
            box->setAnchorPoint(Vec2(0, 1));
            box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688 - high));
            bg_3->addChild(box, ceng + 1);
            high = high + _visibleSize.height * 0.1;
            /*图标*/
            auto pic = PlaceHolder::createRect(
                Size(_visibleSize.height * 0.07, _visibleSize.height * 0.07), "", 16,
                Color3B::BLACK);
            pic->setAnchorPoint(Vec2(0, 0));
            pic->setPosition(Vec2(0, 0));
            box->addChild(pic, ceng + 1);

            auto tool = Sprite::create(props[i].first.icon);
            tool->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.04));
            tool->setAnchorPoint(Vec2(0, 0));
            pic->addChild(tool, ceng + 1);
        }
    }
    /*道具名称*/
    Label* name[10];
    double height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 1) {
            name[i] = Label::createWithTTF(props[i].first.name, "fonts/dengxian.ttf", 20);
            name[i]->setAnchorPoint(Vec2(0, 1));
            name[i]->setPosition(
                Vec2(_visibleSize.width * 0.073, _visibleSize.height * 0.670 - height));
            name[i]->setColor(Color3B::BLACK);
            bg_3->addChild(name[i], ceng + 1);
            height = height + _visibleSize.height * 0.1;
        }
    }

    /*道具功能描述*/
    Label* describe[10];
    height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 1) {
            name[i] = Label::createWithTTF(props[i].first.description, "fonts/dengxian.ttf", 20);
            name[i]->setAnchorPoint(Vec2(0, 1));
            name[i]->setPosition(
                Vec2(_visibleSize.width * 0.203, _visibleSize.height * 0.670 - height));
            name[i]->setColor(Color3B::BLACK);
            bg_3->addChild(name[i], ceng + 1);
            height = height + _visibleSize.height * 0.1;
        }
    }

    /*使用者*/
    for (int i = 0; i < props.size(); i++) {
        for (int j = 0; j < peopleprops1.size(); j++) {
            if (props[i].first.tag == peopleprops1[j].tag) {
                userflag[i] = 1;
            }
        }
        for (int j = 0; j < peopleprops2.size(); j++) {
            if (props[i].first.tag == peopleprops2[j].tag) {
                userflag[i] = 2;
            }
        }
        for (int j = 0; j < peopleprops3.size(); j++) {
            if (props[i].first.tag == peopleprops3[j].tag) {
                userflag[i] = 3;
            }
        }
    }
    Label* user[30];
    string content[4] = { "使用者：Hakurei Reimu", "使用者：Kirisame Marisa", "使用者：Alice",
                          "无使用者" };
    height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 1) {
            if (userflag[i] == 1) {
                name[i] = Label::createWithTTF(content[0], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], ceng + 1);
                height = height + _visibleSize.height * 0.1;
            } else if (userflag[i] == 2) {
                name[i] = Label::createWithTTF(content[1], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], ceng + 1);
                height = height + _visibleSize.height * 0.1;
            } else if (userflag[i] == 3) {
                name[i] = Label::createWithTTF(content[2], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], ceng + 1);
                height = height + _visibleSize.height * 0.1;
            } else {
                name[i] = Label::createWithTTF(content[3], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], ceng + 1);
                height = height + _visibleSize.height * 0.1;
            }
        }
    }
}

/*特殊道具*/
void
InventoryScene::getSpecialitem(Ref* ref, Widget::TouchEventType type)
{
    ceng = ceng + 5;
    if (type != Widget::TouchEventType::ENDED)
        return;
    auto bg_3 = PlaceHolder::createRect(
        Size(_visibleSize.width * 0.595, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
    bg_3->setAnchorPoint(Vec2(0, 0));
    bg_3->setPosition(Vec2(_visibleSize.width * 0.29, _visibleSize.height * 0.141));
    addChild(bg_3, ceng);
    float high = 0;
    auto people = gamedata->getAvailableCharacterList();
    auto props = gamedata->getAvailableItemList();
    auto peopleprops1 = gamedata->getCharacterItemList(people[0].tag);
    auto peopleprops2 = gamedata->getCharacterItemList(people[1].tag);
    auto peopleprops3 = gamedata->getCharacterItemList(people[2].tag);
    for (int i = 0; i < props.size(); i++) {
        if (props[i].first.type == Item::Type::SPECIAL) {
            typeflag[i] = 3;
        }
    }
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 3) {
            auto box = PlaceHolder::createRect(
                Size(_visibleSize.width * 0.590, _visibleSize.height * 0.07), "", 16,
                Color3B(91, 155, 213));
            box->setAnchorPoint(Vec2(0, 1));
            box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688 - high));
            bg_3->addChild(box, 0);
            high = high + _visibleSize.height * 0.1;
            /*图标*/
            auto pic = PlaceHolder::createRect(
                Size(_visibleSize.height * 0.07, _visibleSize.height * 0.07), "", 16,
                Color3B::BLACK);
            pic->setAnchorPoint(Vec2(0, 0));
            pic->setPosition(Vec2(0, 0));
            box->addChild(pic, 1);

            auto tool = Sprite::create(props[i].first.icon);
            tool->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.04));
            tool->setAnchorPoint(Vec2(0, 0));
            pic->addChild(tool, 2);
        }
    }
    /*道具名称*/
    Label* name[10];
    double height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 3) {
            name[i] = Label::createWithTTF(props[i].first.name, "fonts/dengxian.ttf", 20);
            name[i]->setAnchorPoint(Vec2(0, 1));
            name[i]->setPosition(
                Vec2(_visibleSize.width * 0.073, _visibleSize.height * 0.670 - height));
            name[i]->setColor(Color3B::BLACK);
            bg_3->addChild(name[i], 2);
            height = height + _visibleSize.height * 0.1;
        }
    }

    /*道具功能描述*/
    Label* describe[10];
    height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 3) {
            name[i] = Label::createWithTTF(props[i].first.description, "fonts/dengxian.ttf", 20);
            name[i]->setAnchorPoint(Vec2(0, 1));
            name[i]->setPosition(
                Vec2(_visibleSize.width * 0.203, _visibleSize.height * 0.670 - height));
            name[i]->setColor(Color3B::BLACK);
            bg_3->addChild(name[i], 2);
            height = height + _visibleSize.height * 0.1;
        }
    }

    /*使用者*/
    for (int i = 0; i < props.size(); i++) {
        for (int j = 0; j < peopleprops1.size(); j++) {
            if (props[i].first.tag == peopleprops1[j].tag) {
                userflag[i] = 1;
            }
        }
        for (int j = 0; j < peopleprops2.size(); j++) {
            if (props[i].first.tag == peopleprops2[j].tag) {
                userflag[i] = 2;
            }
        }
        for (int j = 0; j < peopleprops3.size(); j++) {
            if (props[i].first.tag == peopleprops3[j].tag) {
                userflag[i] = 3;
            }
        }
    }
    Label* user[30];
    string content[4] = { "使用者：Hakurei Reimu", "使用者：Kirisame Marisa", "使用者：Alice",
                          "无使用者" };
    height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 3) {
            if (userflag[i] == 1) {
                name[i] = Label::createWithTTF(content[0], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], 2);
                height = height + _visibleSize.height * 0.1;
            } else if (userflag[i] == 2) {
                name[i] = Label::createWithTTF(content[1], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], 2);
                height = height + _visibleSize.height * 0.1;
            } else if (userflag[i] == 3) {
                name[i] = Label::createWithTTF(content[2], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], 2);
                height = height + _visibleSize.height * 0.1;
            } else {
                name[i] = Label::createWithTTF(content[3], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], 2);
                height = height + _visibleSize.height * 0.1;
            }
        }
    }
}

/*强化道具*/
void
InventoryScene::getStrengitem(Ref* ref, Widget::TouchEventType type)
{
    ceng = ceng + 5;
    if (type != Widget::TouchEventType::ENDED)
        return;
    auto bg_3 = PlaceHolder::createRect(
        Size(_visibleSize.width * 0.595, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
    bg_3->setAnchorPoint(Vec2(0, 0));
    bg_3->setPosition(Vec2(_visibleSize.width * 0.29, _visibleSize.height * 0.141));
    addChild(bg_3, ceng);
    float high = 0;
    auto people = gamedata->getAvailableCharacterList();
    auto props = gamedata->getAvailableItemList();
    auto peopleprops1 = gamedata->getCharacterItemList(people[0].tag);
    auto peopleprops2 = gamedata->getCharacterItemList(people[1].tag);
    auto peopleprops3 = gamedata->getCharacterItemList(people[2].tag);
    for (int i = 0; i < props.size(); i++) {
        if (props[i].first.type == Item::Type::STRENGTHEN) {
            typeflag[i] = 2;
        }
    }
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 2) {
            auto box = PlaceHolder::createRect(
                Size(_visibleSize.width * 0.590, _visibleSize.height * 0.07), "", 16,
                Color3B(91, 155, 213));
            box->setAnchorPoint(Vec2(0, 1));
            box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688 - high));
            bg_3->addChild(box, ceng + 1);
            high = high + _visibleSize.height * 0.1;
            /*图标*/
            auto pic = PlaceHolder::createRect(
                Size(_visibleSize.height * 0.07, _visibleSize.height * 0.07), "", 16,
                Color3B::BLACK);
            pic->setAnchorPoint(Vec2(0, 0));
            pic->setPosition(Vec2(0, 0));
            box->addChild(pic, ceng + 1);

            auto tool = Sprite::create(props[i].first.icon);
            tool->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.04));
            tool->setAnchorPoint(Vec2(0, 0));
            pic->addChild(tool, ceng + 1);
        }
    }
    /*道具名称*/
    Label* name[10];
    double height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 2) {
            name[i] = Label::createWithTTF(props[i].first.name, "fonts/dengxian.ttf", 20);
            name[i]->setAnchorPoint(Vec2(0, 1));
            name[i]->setPosition(
                Vec2(_visibleSize.width * 0.073, _visibleSize.height * 0.670 - height));
            name[i]->setColor(Color3B::BLACK);
            bg_3->addChild(name[i], ceng + 1);
            height = height + _visibleSize.height * 0.1;
        }
    }

    /*道具功能描述*/
    Label* describe[10];
    height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 2) {
            name[i] = Label::createWithTTF(props[i].first.description, "fonts/dengxian.ttf", 20);
            name[i]->setAnchorPoint(Vec2(0, 1));
            name[i]->setPosition(
                Vec2(_visibleSize.width * 0.203, _visibleSize.height * 0.670 - height));
            name[i]->setColor(Color3B::BLACK);
            bg_3->addChild(name[i], ceng + 1);
            height = height + _visibleSize.height * 0.1;
        }
    }

    /*使用者*/
    for (int i = 0; i < props.size(); i++) {
        for (int j = 0; j < peopleprops1.size(); j++) {
            if (props[i].first.tag == peopleprops1[j].tag) {
                userflag[i] = 1;
            }
        }
        for (int j = 0; j < peopleprops2.size(); j++) {
            if (props[i].first.tag == peopleprops2[j].tag) {
                userflag[i] = 2;
            }
        }
        for (int j = 0; j < peopleprops3.size(); j++) {
            if (props[i].first.tag == peopleprops3[j].tag) {
                userflag[i] = 3;
            }
        }
    }
    Label* user[30];
    string content[4] = { "使用者：Hakurei Reimu", "使用者：Kirisame Marisa", "使用者：Alice",
                          "无使用者" };
    height = 0;
    for (int i = 0; i < props.size(); i++) {
        if (typeflag[i] == 2) {
            if (userflag[i] == 1) {
                name[i] = Label::createWithTTF(content[0], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], ceng + 1);
                height = height + _visibleSize.height * 0.1;
            } else if (userflag[i] == 2) {
                name[i] = Label::createWithTTF(content[1], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], ceng + 1);
                height = height + _visibleSize.height * 0.1;
            } else if (userflag[i] == 3) {
                name[i] = Label::createWithTTF(content[2], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], ceng + 1);
                height = height + _visibleSize.height * 0.1;
            } else {
                name[i] = Label::createWithTTF(content[3], "fonts/dengxian.ttf", 20);
                name[i]->setAnchorPoint(Vec2(0, 1));
                name[i]->setPosition(
                    Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
                name[i]->setColor(Color3B::BLACK);
                bg_3->addChild(name[i], ceng + 1);
                height = height + _visibleSize.height * 0.1;
            }
        }
    }
}
