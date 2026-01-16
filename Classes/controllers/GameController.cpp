#include "GameController.h"
#include <algorithm> 
#include <random>  
#include <vector>  
#include <ctime>    

cocos2d::Scene* GameController::createScene() {
    return GameController::create();
}

// 辅助函数：快速创建一张牌
CardModel* GameController::createCard(int id, int face, int suit, float x, float y) {
    auto model = CardModel::create(id, (CardFace)face, (CardSuit)suit);
    model->setPosition(cocos2d::Vec2(x, y));
    return model;
}

void GameController::generateLevel() {
    // 1.准备一副完整的扑克牌数据
    struct CardData {
        int face;
        int suit;
    };
    std::vector<CardData> deck;
    // 循环生成 52 张牌 (4种花色 * 13个点数)
    for (int s = 0; s < 4; s++) {
        for (int f = 1; f <= 13; f++) {
            deck.push_back({ f, s });
        }
    }
    // 2.洗牌
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));

    // 3.开始发牌
    int deckIndex = 0; // 记录发到了第几张
    int idCounter = 100; // 卡牌ID计数器


    std::vector<cocos2d::Vec2> pyramidPositions = {//给主牌区发牌
        {540, 1400},//第一行一张
        {480, 1300}, {600, 1300},//第二行两张
        {420, 1200}, {540, 1200}, {660, 1200}//第三行三张
    };

    for (auto pos : pyramidPositions) {
        CardData data = deck[deckIndex++];// 从洗好的牌里拿一张
        _models.pushBack(createCard(idCounter++, data.face, data.suit, pos.x, pos.y));
    }

    //B. 发一张底牌
    CardData bottomData = deck[deckIndex++];
    auto topCard = createCard(1, bottomData.face, bottomData.suit, 540, 400);// ID 设为 1，放在底牌区
    _models.pushBack(topCard);
    _currentStackCardId = 1;

    _stockCardIds.clear(); // 清空旧数据

    // 把剩下的牌都发过去 (为了演示，你可以只发10张，也可以全部发完)
    // 这里我们发 20 张作为备用牌
    for (int i = 0; i < 20; i++) {
        if (deckIndex >= deck.size()) break; // 防止越界

        CardData stockData = deck[deckIndex++];
        int stockId = 200 + i;
             
        auto stockCard = createCard(stockId, stockData.face, stockData.suit, 310, 400);// 创建备用牌堆，位置固定在左下角
        _models.pushBack(stockCard);
        _stockCardIds.push_back(stockId);
    }
}

bool GameController::init() {
    if (!Scene::init()) return false;
    auto background = cocos2d::LayerColor::create(cocos2d::Color4B(0, 100, 0, 255));
    this->addChild(background, -100);   //创建背景

    _playFieldLayer = cocos2d::Layer::create();
    this->addChild(_playFieldLayer, 0);

    auto undoLabel = cocos2d::Label::createWithSystemFont("UNDO", "Arial", 60);
    undoLabel->setColor(cocos2d::Color3B::YELLOW); // 黄色比较显眼

    auto undoItem = cocos2d::MenuItemLabel::create(undoLabel, CC_CALLBACK_1(GameController::onUndoClicked, this));
    undoItem->setPosition(cocos2d::Vec2(900, 200)); // 放在屏幕右下角

    auto menu = cocos2d::Menu::create(undoItem, nullptr);
    menu->setPosition(cocos2d::Vec2::ZERO); // 必须归零，否则坐标很难算
    this->addChild(menu, 200); // 层级设高一点，保证能点到

    this->generateLevel();

    for (auto model : _models) { //把牌放到视图中
        auto view = CardView::create(model);
        view->setPosition(model->getPosition());

        // 绑定点击
        view->setClickCallback([this](int id) {
            this->handleCardClick(id);
            });

        _playFieldLayer->addChild(view, 0, model->getId());
    }

    return true;
}

// 辅助函数：判断一张牌是否被挡住了
bool GameController::isCardBlocked(CardModel* targetCard) {
    float cardW = 140.0f; // 卡牌宽
    float cardH = 200.0f; // 卡牌高

    // 遍历所有牌
    for (auto otherCard : _models) {
        if (otherCard == targetCard) continue;

        if (otherCard->getPosition().y < 800) continue; //忽略底牌区的牌

        float dy = targetCard->getPosition().y - otherCard->getPosition().y;
        float dx = std::abs(targetCard->getPosition().x - otherCard->getPosition().x);

        if (dy > 0 && dy < 150 && dx < 70) {
            return true; 
        }
    }
    return false;
}

//接受所点击的卡片编号，处理点击逻辑
bool GameController::handleCardClick(int cardId) {
    cocos2d::Vec2 wastePilePos(540, 400);

    bool isStockCard = false;
    for (int id : _stockCardIds) {
        if (id == cardId) {
            isStockCard = true;
            break;
        }
    }

    CardModel* cardToMove = nullptr;
    bool isValidMove = false;

    //逻辑分支 A: 点击备用牌堆
    if (isStockCard) {
        // 找到这张牌的模型
        for (auto m : _models) {
            if (m->getId() == cardId) {
                cardToMove = m;
                break;
            }
        }
        if (cardToMove) isValidMove = true;
    }
    //逻辑分支 B: 点击桌面卡牌 (进行数值匹配)
    else {
        if (cardId == _currentStackCardId) return false;
        CardModel* clickedCard = nullptr;
        CardModel* stackCard = nullptr;
        // 查找模型
        for (auto model : _models) {
            if (model->getId() == cardId) clickedCard = model;
            if (model->getId() == _currentStackCardId) stackCard = model;
        }

        if (clickedCard && isCardBlocked(clickedCard)) {
            cocos2d::log("Card is Blocked! ID: %d", cardId);

            auto view = _playFieldLayer->getChildByTag(cardId);
            if (view) {
                auto move = cocos2d::MoveBy::create(0.05f, cocos2d::Vec2(10, 0));
                view->runAction(cocos2d::Sequence::create(move, move->reverse(), nullptr));
            }
            return false;
        }

        if (clickedCard && stackCard) {
            // 计算点数差
            int face1 = (int)clickedCard->getFace();
            int face2 = (int)stackCard->getFace();
            int diff = std::abs(face1 - face2);

            if (diff == 1 ) {
                cocos2d::log("Match Success!");
                cardToMove = clickedCard;
                isValidMove = true;
            }
        }
    }

    if (isValidMove && cardToMove) {
        GameMoveStep step;
        step.movedCardId = cardToMove->getId();
        step.startPos = cardToMove->getPosition(); // 记下它现在的位置(老家)
        step.previousStackId = _currentStackCardId; // 记下谁是被覆盖的旧底牌

        // 获取 View 来记录层级
        auto view = _playFieldLayer->getChildByTag(cardToMove->getId());
        if (view) {
            step.startZOrder = view->getLocalZOrder();
            _historyStack.push_back(step);
        }

        if (view) {
            view->setLocalZOrder(100); // 提至最上层，防止飞行时被遮挡
            auto moveTo = cocos2d::MoveTo::create(0.2f, wastePilePos); // 飞向底牌区
            view->runAction(moveTo);
        }

        cardToMove->setPosition(wastePilePos);

        _currentStackCardId = cardToMove->getId();

        return true;
    }

    return false;
}

void GameController::onUndoClicked(cocos2d::Ref* sender) {
    // 1. 如果没有历史记录，直接返回
    if (_historyStack.empty()) {
        cocos2d::log("No more history to undo!");
        return;
    }

    // 2. 取出最后一步操作
    GameMoveStep lastStep = _historyStack.back();
    _historyStack.pop_back();

    cocos2d::log("Undoing Card ID: %d returning to (%f, %f)",
        lastStep.movedCardId, lastStep.startPos.x, lastStep.startPos.y);

    // 3. 找到那张飞走的牌 (View 和 Model)
    auto cardView = _playFieldLayer->getChildByTag(lastStep.movedCardId);
    CardModel* cardModel = nullptr;
    for (auto m : _models) {
        if (m->getId() == lastStep.movedCardId) {
            cardModel = m;
            break;
        }
    }

    if (cardView && cardModel) {
        auto moveBack = cocos2d::MoveTo::create(0.2f, lastStep.startPos);

        auto finish = cocos2d::CallFunc::create([cardView, lastStep]() {
            cardView->setLocalZOrder(lastStep.startZOrder);
            });

        cardView->runAction(cocos2d::Sequence::create(moveBack, finish, nullptr));

        cardModel->setPosition(lastStep.startPos);

        _currentStackCardId = lastStep.previousStackId;
    }
}