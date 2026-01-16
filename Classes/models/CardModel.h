// Classes/models/CardModel.h
#pragma once
#include "cocos2d.h"
#include "configs/GameConfig.h"

class CardModel : public cocos2d::Ref {
public:
    static CardModel* create(int id, CardFace face, CardSuit suit);
    bool init(int id, CardFace face, CardSuit suit);

    int getId() const { return _id; }
    CardFace getFace() const { return _face; }
    CardSuit getSuit() const { return _suit; }
    
    // 记录卡牌当前的位置数据
    void setPosition(cocos2d::Vec2 pos) { _position = pos; }
    cocos2d::Vec2 getPosition() const { return _position; }

private:
    int _id;
    CardFace _face;
    CardSuit _suit;
    cocos2d::Vec2 _position;
};