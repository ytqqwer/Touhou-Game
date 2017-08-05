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
    addChild(bg, 0,1);
    /*background2*/
    auto bg_2 =
        PlaceHolder::createRect(Size(_visibleSize.width * 0.859, _visibleSize.height * 0.78), "",
                                16, Color3B(91, 155, 213));
    bg_2->setPosition(_visibleSize / 2);
    addChild(bg_2, 0,2);
    /*background3*/
    auto bg_3 = PlaceHolder::createRect(
        Size(_visibleSize.width * 0.595, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
    bg_3->setAnchorPoint(Vec2(0, 0));
    bg_3->setPosition(Vec2(_visibleSize.width * 0.29, _visibleSize.height * 0.141));
    addChild(bg_3, 0,3);

    /*backButton*/
    auto backButton = Button::create("InventoryScene/p1.png", "", "");
    backButton->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    backButton->setTitleText("返回");
    backButton->setTitleColor(Color3B::WHITE);
    backButton->setContentSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.15));
    backButton->addTouchEventListener(
        [](Ref* pSender, Widget::TouchEventType type) { Director::getInstance()->popScene(); });
    addChild(backButton,0,4);

    /*normalProps*/
    auto normalProps = Button::create("InventoryScene/p2.png", "", "");
    normalProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.45));
    normalProps->setTitleText("普通道具");
    normalProps->setTitleFontSize(15);
    normalProps->setTitleColor(Color3B(91, 155, 213));
    normalProps->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    addChild(normalProps, 0,5);
    normalProps->addTouchEventListener(CC_CALLBACK_2(InventoryScene::getitems, this,1));
    normalProps->setTouchEnabled(true);

    /*strongProps*/
    auto strongProps = Button::create("InventoryScene/p2.png", "", "");
    strongProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.6));
    strongProps->setTitleText("强化道具");
    strongProps->setTitleFontSize(15);
    strongProps->setTitleColor(Color3B(91, 155, 213));
    strongProps->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    addChild(strongProps, 0,6);
    strongProps->addTouchEventListener(CC_CALLBACK_2(InventoryScene::getitems, this,2));
    strongProps->setTouchEnabled(true);

    /*specialProps*/
    auto specialProps = Button::create("InventoryScene/p2.png", "", "");
    specialProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.75));
    specialProps->setTitleText("特殊道具");
    specialProps->setTitleFontSize(15);
    specialProps->setTitleColor(Color3B(91, 155, 213));
    specialProps->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    addChild(specialProps,0,7);
    specialProps->addTouchEventListener(CC_CALLBACK_2(InventoryScene::getitems, this,3));
    specialProps->setTouchEnabled(true);
    return true;
}



void
InventoryScene::update(float dt)
{
}



void 
InventoryScene::getitems(Ref* ref, Widget::TouchEventType type, int n)
{
	if (type != Widget::TouchEventType::ENDED)
		return;
	if (type != Widget::TouchEventType::ENDED)
		return;
	for (int i = 8; i <= sum; i++) {
		removeChildByTag(i);
	}
	sum = 8;

	auto bg_3 = PlaceHolder::createRect(
		Size(_visibleSize.width * 0.595, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
	bg_3->setAnchorPoint(Vec2(0, 0));
	bg_3->setPosition(Vec2(_visibleSize.width * 0.29, _visibleSize.height * 0.141));
	addChild(bg_3, 0, sum++);
	float high = 0;
	auto people = gamedata->getAvailableCharacterList();//人物列表
	auto props = gamedata->getAvailableItemList();//可用装备列表
	//for (int i = 1; i < people.size(); i++)
	//{
	//	peopleitem=gamedata->getCharacterItemList(people[i].tag);
	//	// peopleprops = gamedata->getCharacterItemList(people[i].tag);//人物装备的道具
	//}

	for (int i = 0; i < props.size(); i++) {
		if (props[i].first.type == Item::Type::STRENGTHEN) {
			typeflag[i] = 2;
		}
		else if (props[i].first.type == Item::Type::NORMAL) {
			typeflag[i] = 1;
		}
		else if (props[i].first.type == Item::Type::SPECIAL) {
			typeflag[i] = 3;
		}
	}
	/*道具背景、图标*/
	for (int i = 0; i < props.size(); i++) {
		if (typeflag[i] == 2 && n == 2) {
			auto box = PlaceHolder::createRect(
				Size(_visibleSize.width * 0.590, _visibleSize.height * 0.07), "", 16,
				Color3B(91, 155, 213));
			box->setAnchorPoint(Vec2(0, 1));
			box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688 - high));
			bg_3->addChild(box, 0, sum++);
			high = high + _visibleSize.height * 0.1;
			/*图标*/
			auto pic = PlaceHolder::createRect(
				Size(_visibleSize.height * 0.07, _visibleSize.height * 0.07), "", 16,
				Color3B::BLACK);
			pic->setAnchorPoint(Vec2(0, 0));
			pic->setPosition(Vec2(0, 0));
			box->addChild(pic, 0, sum++);

			auto tool = Sprite::create(props[i].first.icon);
			tool->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.04));
			tool->setAnchorPoint(Vec2(0, 0));
			pic->addChild(tool, 0, sum++);
		}
		else if (typeflag[i] == 1 && n == 1) {
			auto box = PlaceHolder::createRect(
				Size(_visibleSize.width * 0.590, _visibleSize.height * 0.07), "", 16,
				Color3B(91, 155, 213));
			box->setAnchorPoint(Vec2(0, 1));
			box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688 - high));
			bg_3->addChild(box, 0, sum++);
			high = high + _visibleSize.height * 0.1;
			/*图标*/
			auto pic = PlaceHolder::createRect(
				Size(_visibleSize.height * 0.07, _visibleSize.height * 0.07), "", 16,
				Color3B::BLACK);
			pic->setAnchorPoint(Vec2(0, 0));
			pic->setPosition(Vec2(0, 0));
			box->addChild(pic, 0, sum++);

			auto tool = Sprite::create(props[i].first.icon);
			tool->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.04));
			tool->setAnchorPoint(Vec2(0, 0));
			pic->addChild(tool, 0, sum++);
		}
		else if (typeflag[i] == 3 && n == 3) {
			auto box = PlaceHolder::createRect(
				Size(_visibleSize.width * 0.590, _visibleSize.height * 0.07), "", 16,
				Color3B(91, 155, 213));
			box->setAnchorPoint(Vec2(0, 1));
			box->setPosition(Vec2(_visibleSize.width * 0.003, _visibleSize.height * 0.688 - high));
			bg_3->addChild(box, 0, sum++);
			high = high + _visibleSize.height * 0.1;
			/*图标*/
			auto pic = PlaceHolder::createRect(
				Size(_visibleSize.height * 0.07, _visibleSize.height * 0.07), "", 16,
				Color3B::BLACK);
			pic->setAnchorPoint(Vec2(0, 0));
			pic->setPosition(Vec2(0, 0));
			box->addChild(pic, 0, sum++);

			auto tool = Sprite::create(props[i].first.icon);
			tool->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.04));
			tool->setAnchorPoint(Vec2(0, 0));
			pic->addChild(tool, 0, sum++);
		}
	}
	/*道具名称*/
	Label* name[10];
	double height = 0;
	for (int i = 0; i < props.size(); i++) {
		if (typeflag[i] == 2 && n == 2) {
			name[i] = Label::createWithTTF(props[i].first.name, "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.073, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;
		}
		else if (typeflag[i] == 1 && n == 1) {
			name[i] = Label::createWithTTF(props[i].first.name, "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.073, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;
		}
		else if (typeflag[i] == 3 && n == 3) {
			name[i] = Label::createWithTTF(props[i].first.name, "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.073, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;
		}
	}

	/*道具功能描述*/

	height = 0;
	for (int i = 0; i < props.size(); i++) {
		if (typeflag[i] == 2 && n == 2) {
			name[i] = Label::createWithTTF(props[i].first.description, "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.203, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;
		}
		else if (typeflag[i] == 1 && n == 1) {
			name[i] = Label::createWithTTF(props[i].first.description, "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.203, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;
		}
		else if (typeflag[i] == 3 && n == 3) {
			name[i] = Label::createWithTTF(props[i].first.description, "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.203, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;
		}
	}

	/*使用者*/
	for (int i = 0; i < props.size(); i++) {
		for (int k = 0; k < people.size(); k++) {
			auto peopleitem = gamedata->getCharacterItemList(people[k].tag);
			for (int j = 0; j < peopleitem.size(); j++) {
				if (props[i].first.tag == peopleitem[j].tag) {
					userflag[i] = people[k].name;
				}
				else
				{
					userflag[i] = "无人使用";
				}

			}
		}
	}
	height = 0;
	for (int i = 0; i < props.size(); i++) {
		if (typeflag[i] == 2 && n == 2) {

			name[i] = Label::createWithTTF(userflag[i], "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;

		}
		else if (typeflag[i] == 1 && n == 1) {
			name[i] = Label::createWithTTF(userflag[i], "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;
			
		}
		else if (typeflag[i] == 3 && n == 3) {

			name[i] = Label::createWithTTF(userflag[i], "fonts/dengxian.ttf", 20);
			name[i]->setAnchorPoint(Vec2(0, 1));
			name[i]->setPosition(
				Vec2(_visibleSize.width * 0.403, _visibleSize.height * 0.670 - height));
			name[i]->setColor(Color3B::BLACK);
			bg_3->addChild(name[i], 0, sum++);
			height = height + _visibleSize.height * 0.1;
			
		}
	}
}
