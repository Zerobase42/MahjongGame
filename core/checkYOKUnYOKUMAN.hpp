// calc score

#include "player.hpp"

bool isChiitoitsu(const mahjong::Hand& handCard) {
    int pairCnt = 0;
    for (int i = 0; i < 14; i++) {
        if (handCard.card[i] == handCard.card[i + 1])
            pairCnt++;
    }
    return pairCnt == 7;
}
