// Classes/controllers/GameController.h
#pragma once
#include "cocos2d.h"
#include "views/CardView.h" 
#include "models/CardModel.h" 
struct GameMoveStep {
    int movedCardId;         // 哪张牌移动了
    cocos2d::Vec2 startPos;  // 它原来在哪里
    int startZOrder;        
    int previousStackId;     // 移动前的“旧底牌”是谁
};

class GameController : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    bool handleCardClick(int cardId);

    void onUndoClicked(cocos2d::Ref* sender);

    CREATE_FUNC(GameController);

private:
    CardModel* createCard(int id, int face, int suit, float x, float y);
    void generateLevel();
    bool isCardBlocked(CardModel* targetCard);
    cocos2d::Vector<CardModel*> _models;
    cocos2d::Layer* _playFieldLayer;
    int _currentStackCardId;
    std::vector<int> _stockCardIds;

    std::vector<GameMoveStep> _historyStack;
};