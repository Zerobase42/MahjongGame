#pragma once
#ifndef CHECKYOKUNYOKUMAN_HPP
#define CHECKYOKUNYOKUMAN_HPP

#include <algorithm>
#include "mahjong.hpp"

namespace yoku{
    // 치또이츠 : 머리가 7개.
    bool isChiitoitsu(const mahjong::Tile handCard[mahjong::HAND_MAX]) {  // 치또이츠 판별
        int pairCnt = 0;
        for (int i = 0; i < 14; i += 2) {        // 정렬되어있다 가정
            if (handCard[i] == handCard[i + 1])  // 머리 발견
                pairCnt++;
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
}

namespace yokuMan{
    // 국사무쌍 : 19패(각 수트 1,9) + 자패 7종을 모두 1장 이상, 그 중 1종류는 2장(페어) 보유
    bool isKokushi(const mahjong::Tile handCard[mahjong::HAND_MAX]) {
        static const mahjong::Tile kokushiTiles[13] = {
            mahjong::MAHSKII::M1, mahjong::MAHSKII::M9,
            mahjong::MAHSKII::T1, mahjong::MAHSKII::T9,
            mahjong::MAHSKII::S1, mahjong::MAHSKII::S9,
            mahjong::MAHSKII::E, mahjong::MAHSKII::S,
            mahjong::MAHSKII::W, mahjong::MAHSKII::N,
            mahjong::MAHSKII::Wh, mahjong::MAHSKII::G, mahjong::MAHSKII::R
        };

        bool hasPair = false;
        for (auto tile : kokushiTiles) {
            int cnt = 0;
            for (int i = 0; i < mahjong::HAND_MAX; i++) {
                if (handCard[i] == tile) cnt++;
            }
            if (cnt == 0) return false;   // 요구 패 중 하나라도 없으면 국사무쌍 아님
            if (cnt >= 2) hasPair = true; // 페어가 되는 패가 하나라도 있어야 함
        }
        return hasPair;
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