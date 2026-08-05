#ifndef CHECKYOKUNYOKUMAN_HPP
#define CHECKYOKUNYOKUMAN_HPP
#include "mahjong.hpp"
#include "player.hpp"

// 치또이츠 : 머리가 7개.
bool isChiitoitsu(const mahjong::Tile handCard[15]) {  // 치또이츠 판별
    int pairCnt = 0;
    for (int i = 0; i < 14; i+=2) {// 정렬되어있다 가정
        if (handCard[i] == handCard[i + 1])// 머리 발견
            pairCnt++;
    }
    return pairCnt == 7;
}

// 모든 몸통이 슌쯔, 머리는 객풍패일때, 머리 두개로 양면대기 시 성립
bool isPinfu(const mahjong::Tile handCard[15]) {  // 핑후 판별
    mahjong::Tile head;

    for (int i = 0; i < 14; i++) {
        if (handCard[i] == handCard[i + 1]) {  // 머리 발견
            head = handCard[i];
            break;
        }
    }

    return true;
}

#endif  // CHECKYOKUNYOKUMAN_HPP