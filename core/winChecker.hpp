#pragma once
#ifndef WINCHECKER_HPP
#define WINCHECKER_HPP

#include "mahjong.hpp"

namespace winChecker {  // 주어진 손패 + 화료패 + 멘츠 정보를 보고 화료 가능한 형태인지 판단한다.
enum class WinShape : unsigned int {
    NONE = 0,

    // 화료 형태
    NORMAL = 1u << 0,      // 4면자 + 1머리
    CHIITOITSU = 1u << 1,  // 치또이츠
    KOKUSHI = 1u << 2,     // 국사무쌍

    // 일반형 구성
    HEAD = 1u << 3,
    SHUN = 1u << 4,
    KOUT = 1u << 5,
    KAN = 1u << 6,
};

unsigned int isWin(const mahjong::Tile handCard[15]) {
    // 역 포함 여부를 판단하는 로직을 구현
    // 비트마스킹으로 가능한 역을 계산하여 반환
    // 역만도 마찬가지

    return 0;  // 임시로 항상 0 반환
}
}  // namespace winChecker

#endif  // WINCHECKER_HPP