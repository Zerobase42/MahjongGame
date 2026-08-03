#pragma once
#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "mahjong.hpp"
class Player {
   public:
    Player() = default;
    ~Player() = default;
    void setHand(const mahjong::Hand& hand);
    const mahjong::Hand& getHand() const;
    void setScore(int score);
    int getScore() const;

    void popCard(mahjong::MAHSKII card) {
        ;  // 해당하는 패 지우고 우선순위 정렬
    }

    void inpCard(mahjong::MAHSKII card) {
        ;  // -1 에 넣고 우선순위 정렬
    }

   public:
    mahjong::Hand handCard;
    mahjong::Tile winTile{};      // 대기->화료패
    bool tsumo = false;           // 쯔모/론
    bool menzen = true;           // 멘젠 확인 -> 치퐁깡 넣을때 false로 바꾸기
    mahjong::Tile seatWind{};     // 자리풍
    mahjong::Tile roundWind{};    // 본국 풍
    unsigned long long YOKU = 0;  // 가능한 역 비트마스킹
    unsigned long long YOKUMAN = 0;

   private:
    void calcYoku() const {
        if (menzen)
    }

    int calcFu() const {
        int score = 20;  // 기본 20부 시작
        bool isHydra = true;
        for (const auto& [card, type, from] : handCard.melds) {
            if (type != mahjong::MType::HEAD) {
                isHydra = false;
                break;
            }
        }
        if (isHydra) {   // 치또이쯔면 25부 시작
            score += 5;  // score=25
        }
        if (tsumo)
            score += 2;
        else
            score += 10;
        // 핑후 판정
        return score;
    }
};
#endif  // PLAYER_HPP