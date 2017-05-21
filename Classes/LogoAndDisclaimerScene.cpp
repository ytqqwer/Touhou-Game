#include "LogoAndDisclaimerScene.h"
#include "GameData.h"
#include "JumpTableScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "resources.h.dir/logo.h"
#include <iostream>
#include <string>
using namespace std;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string LogoAndDisclaimerScene::TAG{ "LogoAndDisclaimerScene" };

LogoAndDisclaimerScene::LogoAndDisclaimerScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
LogoAndDisclaimerScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

    testGameData();

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("LogoAndDisclaimerScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  3. PlaceHolder */

    auto p = PlaceHolder::createCircle(100, "LogoAndDisclaimerScene");
    p->setPosition(_visibleSize / 2);
    this->addChild(p);

    /*  4. schedule */

    this->scheduleUpdate();

    return true;
}

void
LogoAndDisclaimerScene::update(float dt)
{
    /*  1. 现在是否该切换到下一个场景？ */

    // if (switchToJumpTable) {
    if (true) {
        switchToJumpTableScene();
    }
}

void
LogoAndDisclaimerScene::switchToJumpTableScene()
{
    auto s = JumpTableScene::create();
    _director->replaceScene(s);
}

void
LogoAndDisclaimerScene::testGameData()
{
    /* 1. 看其运行时会不会崩溃 */

    auto ptr = GameData::getInstance();

    auto get_current_save_tag = ptr->getCurrentSaveTag();
    auto vector_save = ptr->getSaveList();
    ptr->saveSave();
    // gdptr->deleteSave(get_current_save_tag); // too dangerous
    // gdptr->switchSave(get_current_save_tag); // too dangerous
    ptr->setConversationSpeed(1.1);
    auto get_conversation_speed = ptr->getConversationSpeed();
    auto get_conversation_indicator_list = ptr->getConversationIndicatorList("Hakurei Jinja");
    auto get_conversation = ptr->getConversation("1");
    auto get_current_location = ptr->getCurrentLocation();
    auto get_location_list = ptr->getLocationList();
    auto get_unlocked_location_tag_list = ptr->getUnlockedLocationTagList();
    ptr->switchLocation("Hakurei Jinja");
    auto get_round_list = ptr->getRoundList("Hakurei Jinja");
    auto get_avail_characters = ptr->getAvailableCharacterList();
    auto get_onstage_char_list = ptr->getOnStageCharacterTagList();
    ptr->switchOnStageCharacter(0, "Reimu");
    ptr->changeItem("Reimu", 0, "I2"); // two same items in one character
    ptr->changeSpellCard("Reimu", 0, "C2");
    auto get_character_item_list = ptr->getCharacterItemList("Reimu");
    auto get_character_card_list = ptr->getCharacterSpellCardList("Reimu");
    auto get_attack_list = ptr->getAttackList("Reimu");
    ptr->switchAttackType("Reimu", Character::Attack::Type::FOCUS);
    auto get_money_num = ptr->getMoneyNum();
    ptr->increaseMoney(+1024);
    get_money_num = ptr->getMoneyNum();
    auto get_avail_item_list = ptr->getAvailableItemList();
    auto get_item_list_in_store_1 = ptr->getItemListInStore("ArmsStore");
    auto get_item_list_in_store_2 = ptr->getItemListInStore("Kourindou");
    auto get_avail_card_list = ptr->getAvailableSpellCardList();

    /* 2. 测试其返回的结果是否正确 */

    log("\n==================== GameData Test Begin ====================");

    log(">> GetSaveList:");
    for (auto const& s : vector_save) {
        cout << "  tag: " << s.tag << endl;
        cout << "  name: " << s.name << endl;
        cout << "  time: " << s.time << endl;
        cout << "  locationTag: " << s.locationTag << endl;
    }

    log(">> getConversationIndicatorList:");
    for (auto const& i : get_conversation_indicator_list) {
        cout << "  conversationTag: " << i.conversationTag << endl;
        cout << "  icon: " << i.icon << endl;
        cout << "  name: " << i.name << endl;
    }

    log(">> getConversation:");
    for (auto const& d : get_conversation) {
        cout << "  characterName: " << d.characterName << endl;
        cout << "  characterNameWrodArt" << d.characterNameWordArt << endl;
        cout << "  characterPicPosition"
             << (d.characterPicPosition == Dialogue::PicturePosition::LEFT
                     ? "LEFT"
                     : d.characterPicPosition == Dialogue::PicturePosition::MID ? "MID" : "RIGHT")
             << endl;
        cout << "  content: " << d.content << endl;
        cout << "  backgrouondPicture: " << d.backgroundPicture << endl;
        cout << "  backgroundMusic: " << d.backgroundMusic << endl;
        cout << "  soundEffect: " << d.soundEffect << endl;
        cout << "  screenEffect: "
             << (d.screenEffect == Dialogue::ScreenEffect::NONE ? "NONE" : "SHAKE");
    }

    log(">> getCurrentLocation: ");
    auto l = get_current_location;
    cout << "  tag: " << l.tag << endl;
    cout << "  name: " << l.name << endl;
    cout << "  previewPicture: " << l.previewPicture << endl;
    cout << "  backgroundPicture: " << l.backgroundPicture << endl;
    cout << "  backgroundMusic: " << l.backgroundMusic << endl;
    cout << "  passedRound: " << l.passedRound << endl;
    cout << "  totalRound: " << l.totalRound << endl;

    log(">> getLocationList: ");
    for (auto const& l : get_location_list) {
        cout << "  tag: " << l.tag << endl;
        cout << "  name: " << l.name << endl;
        cout << "  previewPicture: " << l.previewPicture << endl;
        cout << "  backgroundPicture: " << l.backgroundPicture << endl;
        cout << "  backgroundMusic: " << l.backgroundMusic << endl;
        cout << "  passedRound: " << l.passedRound << endl;
        cout << "  totalRound: " << l.totalRound << endl;
    }

    log(">> getUnlockedLocationTagList: ");
    for (auto const& t : get_unlocked_location_tag_list) {
        cout << "  tag: " << t << endl;
    }

    log(">> getAvailCharacterList: ");
    for (auto const& c : get_avail_characters) {
        cout << "  tag: " << c.tag << endl;
        cout << "  name: " << c.name << endl;
        cout << "  avatar: " << c.name << endl;
        cout << "  itemSlotNum: " << c.itemSlotNum << endl;
        cout << "  spellCardSlotNum: " << c.spellCardSlotNum << endl;
        cout << "  healthPointBase: " << c.healthPointBase << endl;
        cout << "  healthPointInc: " << c.healthPointInc << endl;
        cout << "  manaBase: " << c.manaBase << endl;
        cout << "  manaInc: " << c.manaInc << endl;
        cout << "  ..." << endl;
    }

    log(">> getOnStageCharacterTagList: ");
    for (auto const& c : get_onstage_char_list) {
        cout << "  tag: " << c << endl;
    }

    log(">> getCharacterItemList: ");
    for (auto const& i : get_character_item_list) {
        cout << "  tag: " << i.tag << endl;
        cout << "  name: " << i.name << endl;
        cout << "  icon: " << i.icon << endl;
        cout << "  description: " << i.description << endl;
        cout << "  type: " << (i.type == Item::Type::NORMAL
                                   ? "NORMAL"
                                   : i.type == Item::Type::STRENGTHEN ? "STRENGTHEN" : "SPECIAL")
             << endl;
        cout << "  isCarriable: " << i.isCarriable << endl;
        cout << "  isUseable: " << i.isUseable << endl;
        cout << "  maxUseCount: " << i.maxUseCount << endl;
        cout << "  healthPointCost: " << i.healthPointCost << endl;
        cout << "  manaCost: " << i.manaCost << endl;
        cout << "  coolDown: " << i.cooldown << endl;
        cout << "  healthPointInc: " << i.healthPointInc << endl;
        cout << "  manaInc: " << i.manaInc << endl;
        cout << "  walkSpeedInc: " << i.walkSpeedInc << endl;
        cout << "  walkAccelerationInc: " << i.walkAccelerationInc << endl;
        cout << "  dashAccelerationInc: " << i.dashAccelerationInc << endl;
        cout << "  price: " << i.price << endl;
    }

    log(">> getCharacterSpellCardList: ");
    for (auto const& c : get_character_card_list) {
        cout << "  tag: " << c.tag << endl;
        cout << "  ..." << endl;
    }

    log(">> getAvailItemList:");
    for (auto const& p : get_avail_item_list) {
        cout << "  tag: " << p.first.tag << endl;
        cout << "  amount: " << p.second << endl;

        cout << "  ..." << endl;
    }

    log(">> getAvailSpellCardList:");
    for (auto const& p : get_avail_card_list) {
        cout << "  tag: " << p.first.tag << endl;
        cout << "  amount: " << p.second << endl;
        cout << "  ..." << endl;
    }

    log(">> getItemListInStore(\"ArmsStore\"):");
    for (auto const& i : get_item_list_in_store_1) {
        cout << "  tag: " << i.tag << endl;
        cout << "  ..." << endl;
    }

    log(">> getItemListInStore(\"Kourindou\")");
    for (auto const& i : get_item_list_in_store_2) {
        cout << "  tag: " << i.tag << endl;
        cout << "  ..." << endl;
    }

    log("==================== GameData Test End ====================");
}
