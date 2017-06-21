#include "FirstEmitter.h"
#define Pi 3.1415926

FirstEmitter::FirstEmitter(Node* character) {
	this->character = character;
	this->mode = 1;
}

void FirstEmitter::createBullet(float dt) {
	
    mode = (mode % 3) + 1;
	
	switch (mode)
	{
	case 1:this->unschedule(schedule_selector(FirstEmitter::shootBullet3));
		   number = 7;
		   count = 0;
		   startAngle = 0.5;
		   endAngle = 1.5;
		   varAngle = 0.0;
		   this->schedule(schedule_selector(FirstEmitter::shootBullet1), 1.0f);
		   break;
	case 2:this->unschedule(schedule_selector(FirstEmitter::shootBullet1));
		   number = 8;
		   count = 0;
		   startAngle = 0.5;
		   endAngle = 1.5;
		   varAngle = 0.0;
		   this->schedule(schedule_selector(FirstEmitter::shootBullet2), 0.3f);
		   break;
	case 3:this->unschedule(schedule_selector(FirstEmitter::shootBullet2));
		   number = 8;
		   count = 0;
		   startAngle = 0.5;
		   endAngle = 1.0;
		   varAngle = 0.0;
		   this->schedule(schedule_selector(FirstEmitter::shootBullet3), 0.2f); 
		   break;
	}
	
}

void FirstEmitter::shootBullet1(float dt) {
	count++;
	varAngle += 0;

	Size winSize = Director::getInstance()->getWinSize();
	auto characterPos = character->getPosition();
	float angle = (endAngle - startAngle)*Pi / (number - 1);
	float s = sqrt(winSize.width*winSize.width + winSize.height*winSize.height);

	for (int i = 0; i < number; i++) {

		Sprite* spriteBullet;

		switch (count % 7)
		{
		case 1:spriteBullet = Bullet::create("Bullets/Bursts/RedBurst.png");
			break;
		case 2:spriteBullet = Bullet::create("Bullets/Bursts/OrangeBurst.png");
			break;
		case 3:spriteBullet = Bullet::create("Bullets/Bursts/YellowBurst.png");
			break;
		case 4:spriteBullet = Bullet::create("Bullets/Bursts/GreenBurst.png");
			break;
		case 5:spriteBullet = Bullet::create("Bullets/Bursts/CyanBurst.png");
			break;
		case 6:spriteBullet = Bullet::create("Bullets/Bursts/PurpleBurst.png");
			break;
		default:spriteBullet = Bullet::create("Bullets/Bursts/PinkBurst.png");
			break;
		}

		bullets.pushBack(spriteBullet);
		spriteBullet->setAnchorPoint(Vec2(0.5, 0.0));
		spriteBullet->setRotation(-startAngle - (i + varAngle)*angle);
		spriteBullet->setPosition(characterPos);
		addChild(spriteBullet);

		Vec2 deltaP = Vec2(s*cos((startAngle + 0.5)*Pi + (i + varAngle)*angle), s*sin((startAngle + 0.5)*Pi + (i + varAngle)*angle));
		auto actionMoveBy = MoveBy::create(6, deltaP);
		auto actionInOut = EaseInOut::create(actionMoveBy, 1);
		auto actionDone = CallFuncN::create(CC_CALLBACK_1(FirstEmitter::removeBullet, this));
		Sequence* sequence = Sequence::create(actionInOut, actionDone, NULL);
		spriteBullet->runAction(sequence);
	}
}

void FirstEmitter::shootBullet2(float dt) {
	count++;
	
	if (count % 4 == 0) {
		if (this->number < 20) {
			this->number++;
		}
	}

	if (count % 2 == 0) {
		varAngle += 0.3;
	}
	else {
		varAngle -= 0.3;
	}

	Size winSize = Director::getInstance()->getWinSize();
	auto characterPos = character->getPosition();
	float angle = (endAngle - startAngle)*Pi / (number - 1);
	float s = sqrt(winSize.width*winSize.width + winSize.height*winSize.height);

	for (int i = 0; i < number; i++) {

		Sprite* spriteBullet;

		switch (count % 4)
		{
		case 1:spriteBullet = Bullet::create("Bullets/Stars/YellowStar.png");
			break;
		case 2:spriteBullet = Bullet::create("Bullets/Stars/GreenStar.png");
			break;
		case 3:spriteBullet = Bullet::create("Bullets/Stars/BlueStar.png");
			break;
		default:spriteBullet = Bullet::create("Bullets/Stars/PurpleStar.png");
			break;
		}

		bullets.pushBack(spriteBullet);
		spriteBullet->setAnchorPoint(Vec2(0.5, 0.0));
		spriteBullet->setRotation(-startAngle - (i + varAngle)*angle);
		spriteBullet->setPosition(characterPos);
		addChild(spriteBullet);

		Vec2 deltaP = Vec2(s*cos((startAngle + 0.5)*Pi + (i + varAngle)*angle), s*sin((startAngle + 0.5)*Pi + (i + varAngle)*angle));
		auto actionMoveBy = MoveBy::create(6, deltaP);
		auto actionInOut = EaseSineInOut::create(actionMoveBy);
		auto actionDone = CallFuncN::create(CC_CALLBACK_1(FirstEmitter::removeBullet, this));
		Sequence* sequence = Sequence::create(actionInOut, actionDone, NULL);
		spriteBullet->runAction(sequence);
	}
}

void FirstEmitter::shootBullet3(float dt) {
	count++;
	varAngle += 1.5;

	Size winSize = Director::getInstance()->getWinSize();
	auto characterPos = character->getPosition();
	float angle = (endAngle - startAngle)*Pi / (number - 1);
	float s = sqrt(winSize.width*winSize.width + winSize.height*winSize.height);

	for (int i = 0; i < number; i++) {

		Sprite* spriteBullet;

		switch (count%8)
		{
		case 1:spriteBullet = Bullet::create("Bullets/Dots/RedDot.png");
			break;
		case 2:spriteBullet = Bullet::create("Bullets/Dots/OrangeDot.png");
			break;
		case 3:spriteBullet = Bullet::create("Bullets/Dots/YellowDot.png");
			break;
		case 4:spriteBullet = Bullet::create("Bullets/Dots/GreenDot.png");
			break;
		case 5:spriteBullet = Bullet::create("Bullets/Dots/CyanDot.png");
			break;
		case 6:spriteBullet = Bullet::create("Bullets/Dots/BlueDot.png");
			break;
		case 7:spriteBullet = Bullet::create("Bullets/Dots/PurpleDot.png");
			break;
		default:spriteBullet = Bullet::create("Bullets/Dots/PinkDot.png");
			break;
		}
			
		bullets.pushBack(spriteBullet);
		spriteBullet->setAnchorPoint(Vec2(0.5, 0.0));
		spriteBullet->setRotation(-startAngle - (i + varAngle)*angle);
		spriteBullet->setPosition(characterPos);
		addChild(spriteBullet);

		Vec2 deltaP = Vec2(s*cos((startAngle + 0.5)*Pi + (i + varAngle)*angle), s*sin((startAngle + 0.5)*Pi + (i + varAngle)*angle));
		auto actionMoveBy = MoveBy::create(6, deltaP);
		auto actionInOut = EaseInOut::create(actionMoveBy, 1);
		//auto actionInOut = EaseExponentialInOut::create(actionMoveBy);
		//auto actionInOut = EaseElasticInOut::create(actionMoveBy,0.8);
		//auto actionInOut = EaseBounceInOut::create(actionMoveBy);
		//auto actionInOut = EaseBackInOut::create(actionMoveBy);
		auto actionDone = CallFuncN::create(CC_CALLBACK_1(FirstEmitter::removeBullet, this));
		Sequence* sequence = Sequence::create(actionInOut, actionDone, NULL);
		spriteBullet->runAction(sequence);
	}
}