#pragma once
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <algorithm>  // sort

#include "checkYOKUnYOKUMAN.hpp"
#include "mahjong.hpp"

class Player {
   public:  // 기본 선언 값
    mahjong::Hand handCard;
    mahjong::Tile winTile{};  // 대기->화료패
    bool tsumo = false;       // 쯔모/론
    bool menzen = true;       // 멘젠 확인 -> 치퐁깡 넣을때 false로 바꾸기

    unsigned int YOKU = 0;     // 가능한 역 비트마스킹
    unsigned int YOKUMAN = 0;  // 가능한 역만 비트마스킹

   public:
    Player() = default;
    ~Player() = default;
    /*
    void setHand(const mahjong::Hand& hand);
    const mahjong::Hand& getHand() const;
    void setScore(int score);
    int getScore() const;
    */

   private:
    static constexpr unsigned char priority[34] = {
        28,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 29,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 30,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 31,
        32, 33, 34
    };
    void PrioritySort() {  // 우선순위 정렬 -> 만 통 삭 바람패 삼원패 순으로 정렬
        std::sort(handCard.card, handCard.card + mahjong::HAND_MAX,
                  [this](mahjong::Tile a, mahjong::Tile b) {
                      if (a == 255 || b == 255) return a < b;
                      return priority[a] < priority[b];
                  });
    }

   public:
    void popCard(mahjong::MAHSKII card) {  // 마작 패 버리기
        for (int i = 0; i < mahjong::HAND_MAX; i++) {
            if (handCard.card[i] == card) {
                for (int j = i; j < mahjong::HAND_MAX-1; j++)
                    handCard.card[j] = handCard.card[j + 1];
                handCard.card[mahjong::HAND_MAX-1] = 255;
                PrioritySort();
                return;
            }
        }
    }

    void inpCard(mahjong::MAHSKII card) {  // 마작 패 받기
        handCard.card[mahjong::HAND_MAX-1] = card;
        PrioritySort();
    }

   private:
    void calcYoku() const {
        if (menzen) {
            ;
        }
    }

    int calcFu() const {
        int score = 20;  // 기본 20부 시작
        // 기저사례 처리
        if (tsumo && yoku::isPinfu(handCard.card))  // 핑후면 20부 고정
            return 20;
        if (yoku::isChiitoitsu(handCard.card))  // 치또이쯔면 25부 시작
            return 25;

        if (tsumo)  // 쯔모면 +2
            score += 2;
        else  // 론이면 +10
            score += 10;
        // 핑후 판정
        ;  //...
        return score;
    }
};

#endif  // PLAYER_HPP