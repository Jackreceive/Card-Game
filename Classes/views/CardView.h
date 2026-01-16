// Classes/views/CardView.h
#pragma once
#include "cocos2d.h"
#include "models/CardModel.h"

class CardView : public cocos2d::Node {
public:
    static CardView* create(CardModel* model);
    bool init(CardModel* model);

    // 设置点击回调函数 [cite: 362]
    void setClickCallback(const std::function<void(int)>& callback);
    
    // 获取绑定的 Model ID
    int getCardId() const { return _cardId; }

private:
    int _cardId;
    std::function<void(int)> _callback;
    cocos2d::Sprite* _bgSprite;
    cocos2d::Label* _faceLabel; // 暂时用文字代替图片
};