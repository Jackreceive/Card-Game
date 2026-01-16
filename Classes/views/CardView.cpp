#include "CardView.h"

CardView* CardView::create(CardModel* model) {
    CardView* view = new (std::nothrow) CardView();
    if (view && view->init(model)) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool CardView::init(CardModel* model) {
    if (!Node::init()) return false;
    _cardId = model->getId();

    std::string suitName;
    bool isRed = false;

    switch (model->getSuit()) {
    case 0: suitName = "diamond"; isRed = true;  break;
    case 1: suitName = "club";    isRed = false; break;
    case 2: suitName = "heart";   isRed = true;  break;
    case 3: suitName = "spade";   isRed = false; break;
    default: suitName = "spade";  isRed = false; break;
    }

    std::string faceStr;
    int f = (int)model->getFace();
    if (f == 1)       faceStr = "A";
    else if (f == 11) faceStr = "J";
    else if (f == 12) faceStr = "Q";
    else if (f == 13) faceStr = "K";
    else              faceStr = std::to_string(f);


    auto bg = cocos2d::Sprite::create("res/card_general.png");

    if (bg) {
        float targetWidth = 200.0f;
        float scale = targetWidth / bg->getContentSize().width;
        bg->setScale(scale);
        bg->setPosition(cocos2d::Vec2(0, 0));
        this->addChild(bg);


        std::string colorStr = isRed ? "red" : "black";
        std::string numFile = "res/number/small_" + colorStr + "_" + faceStr + ".png";
        std::string suitFile = "res/suits/" + suitName + ".png";

        auto numSprite = cocos2d::Sprite::create(numFile);
        if (numSprite) {
            numSprite->setAnchorPoint(cocos2d::Vec2(0, 1)); 
            numSprite->setNormalizedPosition(cocos2d::Vec2(0.08f, 0.90f));  //用归一化坐标
            bg->addChild(numSprite);
        }

        auto suitSprite = cocos2d::Sprite::create(suitFile);
        if (suitSprite) {
            suitSprite->setAnchorPoint(cocos2d::Vec2(1, 1));
            suitSprite->setNormalizedPosition(cocos2d::Vec2(0.92f, 0.90f));
            suitSprite->setScale(0.6f);
            bg->addChild(suitSprite);
        }

        auto centerSuit = cocos2d::Sprite::create(suitFile);
        if (centerSuit) {
            centerSuit->setAnchorPoint(cocos2d::Vec2(0.5, 0.5)); 
            centerSuit->setNormalizedPosition(cocos2d::Vec2(0.5f, 0.5f));
            centerSuit->setScale(2.0f);
            bg->addChild(centerSuit);
        }

    }
    else { //路径下读取不到正确文件就会执行这里
        auto whiteBg = cocos2d::LayerColor::create(cocos2d::Color4B::WHITE, 260, 360);
        whiteBg->setPosition(cocos2d::Vec2(-130, -180));
        this->addChild(whiteBg);
        auto label = cocos2d::Label::createWithSystemFont(faceStr, "Arial", 100);
        label->setColor(cocos2d::Color3B::BLACK);
        this->addChild(label, 1);
    }


    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this, bg](cocos2d::Touch* touch, cocos2d::Event* event) {
        cocos2d::Vec2 p = this->convertTouchToNodeSpace(touch);

        if (bg) {
            if (bg->getBoundingBox().containsPoint(p)) return true;
        }
        else {
            if (std::abs(p.x) < 130 && std::abs(p.y) < 180) return true;
        }
        return false;
        };

    listener->onTouchEnded = [this](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (_callback) _callback(_cardId);
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void CardView::setClickCallback(const std::function<void(int)>& callback) {
    _callback = callback;
}