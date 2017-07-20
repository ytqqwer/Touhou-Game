#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Player.h"
#include "GameplayScene.h"

bool
Player::init()
{
    if (!Node::init())
        return false;

	auto characterTagList = GameData::getInstance()->getOnStageCharacterTagList();

	p1RunAnimation = Animation::create();
	p1JumpAnimation = Animation::create();
	p1DashAnimation = Animation::create();

	p2RunAnimation = Animation::create();
	p2JumpAnimation = Animation::create();
	p2DashAnimation = Animation::create();

	//定义两角色的序列帧动画。因为此处要经常切换纹理，所以不能用AnimationCache
	if (characterTagList[0] == "Reimu")
	{
		p1PlayerTextureName = "gameplayscene/Reimu/walkFront000.png";
		for (int i = 0; i <= 7; i++)
			p1RunAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/walkFront00" + std::to_string(i) +
				".png");
		p1RunAnimation->setDelayPerUnit(0.15f);

		for (int i = 0; i <= 8; i++)
			p1JumpAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/jump00" + std::to_string(i) +
				".png");
		p1JumpAnimation->setDelayPerUnit(0.11f);

		for (int i = 0; i <= 7; i++)
			p1DashAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/dashFront00" + std::to_string(i) +
				".png");
		p1DashAnimation->setDelayPerUnit(0.07f);
	}
	else if (characterTagList[0] == "Marisa")
	{
		p1PlayerTextureName = "gameplayscene/Marisa/walkFront000.png";
		for (int i = 0; i <= 7; i++)
			p1RunAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/walkFront00" + std::to_string(i) +
				".png");
		p1RunAnimation->setDelayPerUnit(0.15f);

		for (int i = 0; i <= 9; i++)
			p1JumpAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jumpFront00" + std::to_string(i) +
				".png");
		p1JumpAnimation->setDelayPerUnit(0.1f);

		for (int i = 0; i <= 8; i++)
			p1DashAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/dashFront00" + std::to_string(i) +
				".png");
		p1DashAnimation->setDelayPerUnit(0.07f);
	}
	if (characterTagList[1] == "Reimu")
	{
		//p2PlayerTextureName = "gameplayscene/Reimu/walkFront000.png";
		for (int i = 0; i <= 7; i++)
			p2RunAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/walkFront00" + std::to_string(i) +
				".png");
		p2RunAnimation->setDelayPerUnit(0.15f);

		for (int i = 0; i <= 8; i++)
			p2JumpAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/jump00" + std::to_string(i) +
				".png");
		p2JumpAnimation->setDelayPerUnit(0.11f);

		for (int i = 0; i <= 7; i++)
			p2DashAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/dashFront00" + std::to_string(i) +
				".png");
		p2DashAnimation->setDelayPerUnit(0.07f);
	}
	else if (characterTagList[1] == "Marisa")
	{
		//p2PlayerTextureName = "gameplayscene/Marisa/walkFront000.png";
		for (int i = 0; i <= 7; i++)
			p2RunAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/walkFront00" + std::to_string(i) +
				".png");
		p2RunAnimation->setDelayPerUnit(0.15f);

		for (int i = 0; i <= 9; i++)
			p2JumpAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jumpFront00" + std::to_string(i) +
				".png");
		p2JumpAnimation->setDelayPerUnit(0.1f);

		for (int i = 0; i <= 8; i++)
			p2DashAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/dashFront00" + std::to_string(i) +
				".png");
		p2DashAnimation->setDelayPerUnit(0.07f);
	}

	AnimationCache::getInstance()->addAnimation(p1RunAnimation, "p1RunAnimation");
	AnimationCache::getInstance()->addAnimation(p1JumpAnimation, "p1JumpAnimation");
	AnimationCache::getInstance()->addAnimation(p1DashAnimation, "p1DashAnimation");

	AnimationCache::getInstance()->addAnimation(p2RunAnimation, "p2RunAnimation");
	AnimationCache::getInstance()->addAnimation(p2JumpAnimation, "p2JumpAnimation");
	AnimationCache::getInstance()->addAnimation(p2DashAnimation, "p2DashAnimation");

	auto body = PhysicsBody::createBox(Size(50, 75));
	body->setDynamic(true);
	body->setMass(1);
	body->setGravityEnable(true);
	body->setRotationEnable(false);
	body->getFirstShape()->setDensity(0);
	body->getFirstShape()->setFriction(0.2);
	body->getFirstShape()->setRestitution(0);
	body->setCategoryBitmask(playerCategory);
	body->setCollisionBitmask(groundCategory | enemyCategory);
	body->setContactTestBitmask(groundCategory | enemyCategory);

	//留空，需要创建两个刚体

	this->setPhysicsBody(body);
	this->setTag(playerTag);
	this->setName("player");

	playerSprite = Sprite::create(p1PlayerTextureName); //此处必须初始化一张角色纹理，否则后面无法切换纹理
	playerSprite->setName("playerSprite");
	this->addChild(playerSprite);

	playerAnimation = p1RunAnimation;
	playerAnim = Animate::create(playerAnimation);
	playerSprite->runAction(RepeatForever::create(playerAnim)); //初始时刻角色在奔跑

	return true;
}

void
Player::playerRun(float dt)
{
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();

    if (this->playerDirection.compare("right")) {
        Vec2 impluse = Vec2(0, 0);
        // Vec2 impluse = Vec2(-20.0f, 0.0f);
        // body->applyForce(Vec2(-100.0f, 0.0f));

        if (velocity.x > 10) {
            body->setVelocity(Vec2(-100, velocity.y));
        }

        if (velocity.x > -MAX_SPEED) {
            impluse.x = -std::min(MAX_SPEED / ACCELERATE_TIME * dt, MAX_SPEED + velocity.x);
        }
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(0, 0);
        // Vec2 impluse = Vec2(20.0f, 0.0f);
        // body->applyForce(Vec2(100.0f,0.0f));

        if (velocity.x < -10) {
            body->setVelocity(Vec2(100, velocity.y));
        }

        if (velocity.x < MAX_SPEED) {
            impluse.x = std::min(MAX_SPEED / ACCELERATE_TIME * dt, MAX_SPEED - velocity.x);
        }
        body->applyImpulse(impluse);
    }
}

void
Player::playerJump()
{
    if (this->jumpCounts == 0) {
        return;
    }
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0)); //再次跳跃时，重置Y轴速度为0

    //留空，在空中时不再接受水平加速，只有惯性

    Vec2 impluse = Vec2(0.0f, 500.0f);
    body->applyImpulse(impluse);

	Animate* animate;
	if (this->currentPlayer == 0)
	{
		animate = Animate::create(AnimationCache::getInstance()->getAnimation("p1JumpAnimation"));
	}
	else
	{
		animate = Animate::create(AnimationCache::getInstance()->getAnimation("p2JumpAnimation"));
	}

	//留空，对于不同的角色机制应有不同

	playerSprite->stopAllActions();
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Player::resetAction, this));
    auto sequence = Sequence::create(Repeat::create(animate, 1), actionDone, NULL);
    playerSprite->runAction(sequence);

    this->jumpCounts--;
}

void
Player::playerDash()
{
    //留空，阻止连续dash，可增加角色状态标志

    if (this->dashCounts == 0) {
        return;
    }
    auto body = this->getPhysicsBody();
    auto velocity = body->getVelocity();
    body->setVelocity(Vec2(velocity.x, 0)); // dash时，重置Y轴速度为0

    //留空，将y轴速度短暂锁定为0，可以使角色不受重力

    if (this->playerDirection.compare("right")) //当比对相等时返回false,wtf???
    {
        Vec2 impluse = Vec2(-350.0f, 0.0f);
        body->applyImpulse(impluse);
    } else {
        Vec2 impluse = Vec2(350.0f, 0.0f);
        body->applyImpulse(impluse);
    }

	Animate* animate;
	if (this->currentPlayer == 0)
	{
		animate = Animate::create(AnimationCache::getInstance()->getAnimation("p1DashAnimation"));
	}
	else
	{
		animate = Animate::create(AnimationCache::getInstance()->getAnimation("p2DashAnimation"));
	}

	//留空，对于不同的角色机制应有不同

	playerSprite->stopAllActions();
    auto actionDone = CallFuncN::create(CC_CALLBACK_1(Player::resetAction, this));
    auto sequence = Sequence::create(Repeat::create(animate, 1), actionDone, NULL);
    playerSprite->runAction(sequence);

    this->dashCounts--;
}

void
Player::switchCharacter(int currentPlayer)
{
	playerSprite->stopAllActions();
	if (this->currentPlayer == 0)	//第一个角色
	{
		playerAnimation = p2RunAnimation;
		this->currentPlayer = 1;//切换到第二个角色

								//留空，切换刚体
	}
	else
	{
		playerAnimation = p1RunAnimation;
		this->currentPlayer = 0;

		//留空，切换刚体
	}
	playerAnim = Animate::create(playerAnimation);
	playerSprite->runAction(RepeatForever::create(playerAnim)); //初始时刻角色在奔跑

}

void
Player::resetAction(Node* node)
{
	playerSprite->stopAllActions();

	if (this->currentPlayer == 0)
	{
		playerSprite->runAction(RepeatForever::create(
			Animate::create(AnimationCache::getInstance()->getAnimation("p1RunAnimation"))));
	}
	else
	{
		playerSprite->runAction(RepeatForever::create(
			Animate::create(AnimationCache::getInstance()->getAnimation("p2RunAnimation"))));
	}
}

void
Player::regainDashCounts(float dt)
{
    this->dashCounts++;
}
