#ifndef CHECKYOKUNYOKUMAN_HPP
#define CHECKYOKUNYOKUMAN_HPP
#include "mahjong.hpp"
#include "player.hpp"

// 치또이츠 : 머리가 7개.
bool isChiitoitsu(const mahjong::Tile handCard[15]) {  // 치또이츠 판별
    int pairCnt = 0;
    for (int i = 0; i < 14; i++) {// 정렬되어있다 가정
        if (handCard[i] == handCard[i + 1])// 머리 발견
            pairCnt++;
    }
    return pairCnt == 7;
}
bool isPinfu() {
    ;//...
    return true;
}

#endif  // CHECKYOKUNYOKUMAN_HPP