#include "CardModel.h"

CardModel* CardModel::create(int id, CardFace face, CardSuit suit) {
    CardModel* model = new (std::nothrow) CardModel();
    if (model && model->init(id, face, suit)) {
        model->autorelease();
        return model;
    }
    CC_SAFE_DELETE(model);
    return nullptr;
}

bool CardModel::init(int id, CardFace face, CardSuit suit) {
    _id = id;
    _face = face;
    _suit = suit;
    return true;
}