#pragma once
#ifndef CHECKYOKUNYOKUMAN_HPP
#define CHECKYOKUNYOKUMAN_HPP

#include "mahjong.hpp"

namespace yoku{
    // 치또이츠 : 머리가 7개.
bool isChiitoitsu(
    const mahjong::Tile handCard[13],
    mahjong::Tile winTile) {
    unsigned char cnt[mahjong::TILE_MAX]{};

    // 13장
    for (int i = 0; i < 13; ++i) {
        if (handCard[i] < mahjong::TILE_MAX)
            ++cnt[handCard[i]];
    }

    // 화료패
    if (winTile >= mahjong::TILE_MAX)
        return false;

    ++cnt[winTile];

    int pairCnt = 0;

    for (int i = 0; i < mahjong::TILE_MAX; ++i) {
        if (cnt[i] == 2) {
            ++pairCnt;
        } else if (cnt[i] != 0) {
            return false;
        }
    }

    return pairCnt == 7;
}

    // 모든 몸통이 슌쯔, 머리는 객풍패일때, 머리 두개로 양면대기 시 성립
    bool isPinfu(const mahjong::Tile handCard[mahjong::HAND_MAX]) {  // 핑후 판별
        mahjong::Tile head{};

        for (int i = 0; i < mahjong::HAND_MAX - 1; i++) {
            if (handCard[i] == handCard[i + 1]) {  // 머리 발견
                head = handCard[i];
                break;
            }
        }

        // TODO: 실제 핑후 조건(전부 슌쯔 + 객풍 머리 + 양면대기) 판별 필요.
        // 현재는 자리만 잡아둔 스텁으로, winChecker의 몸통 분해 결과(WinInfo)를
        // 받아 판정하도록 옮기는 편이 자연스럽다 (여기서는 손패만 보고는
        // 몸통이 전부 슌쯔인지 알 수 없음).
        (void)head;
        return true;
    }
    // 요구패를 사용하지 않고 화료시 성립
    bool isTangyao(const mahjong::Tile handCard[mahjong::HAND_MAX]){
        for(int i=0;i<mahjong::HAND_MAX;i++){
            if(){
                return false;
            }
        }
        return true;
    }
    bool is(const mahjong::Tile handCard[mahjong::HAND_MAX]) {
        ;
    }
}

namespace yokuMan{
    // 국사무쌍 : 19패(각 수트 1,9) + 자패 7종을 모두 1장 이상, 그 중 1종류는 2장(페어) 보유
bool isKokushi(
    const mahjong::Tile handCard[13],
    mahjong::Tile winTile) {
    unsigned char cnt[mahjong::TILE_MAX]{};

    for (int i = 0; i < 13; ++i) {
        if (handCard[i] < mahjong::TILE_MAX)
            ++cnt[handCard[i]];
    }

    if (winTile >= mahjong::TILE_MAX)
        return false;

    ++cnt[winTile];

    constexpr mahjong::Tile terminals[] = {
        mahjong::M1, mahjong::M9,
        mahjong::S1, mahjong::S9,
        mahjong::T1, mahjong::T9,
        mahjong::E,
        mahjong::S,
        mahjong::W,
        mahjong::N,
        mahjong::Wh,
        mahjong::G,
        mahjong::R
    };

    bool pair = false;

    for (mahjong::Tile tile : terminals) {
        if (cnt[tile] == 0)
            return false;

        if (cnt[tile] >= 2)
            pair = true;
    }

    return pair;
}

    // 구련보등 : 한 수트로만 구성 + 1112345678999 형태 + 아무 패나 1장 추가
    bool isChuuren(const mahjong::Tile handCard[mahjong::HAND_MAX]) {
        auto suitOf = [](mahjong::Tile t) -> int {
            if (t >= mahjong::M1 && t <= mahjong::M9) return 0;
            if (t >= mahjong::S1 && t <= mahjong::S9) return 1;
            if (t >= mahjong::T1 && t <= mahjong::T9) return 2;
            return -1;  // 자패 -> 구련보등 불가
        };
        static const mahjong::Tile suitBase[3] = {
            mahjong::MAHSKII::M1, mahjong::MAHSKII::S1, mahjong::MAHSKII::T1
        };

        const int suit = suitOf(handCard[0]);
        if (suit == -1) return false;

        int cnt[9] = {0};
        for (int i = 0; i < mahjong::HAND_MAX; i++) {
            if (suitOf(handCard[i]) != suit) return false;  // 혼일색이 아니면 불가
            int n = handCard[i] - suitBase[suit];            // 0~8 (1~9)
            cnt[n]++;
        }

        for (int n = 0; n < 9; n++) {
            int need = (n == 0 || n == 8) ? 3 : 1;  // 1, 9는 최소 3장, 2~8은 최소 1장
            if (cnt[n] < need) return false;
        }
        return true;  // 남는 1장은 자연히 텐파이 확장패로 처리됨
    }
}

#endif  // CHECKYOKUNYOKUMAN_HPP