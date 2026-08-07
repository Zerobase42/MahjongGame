#pragma once
#ifndef WINCHECKER_HPP
#define WINCHECKER_HPP

#include "mahjong.hpp"

namespace winChecker {  // 주어진 손패 + 화료패 + 멘츠 정보를 보고 화료 가능한 형태인지 판단한다.

struct WinInfo {
    // 화료 가능한 형태인지 여부
    // 손패
    // 울어서 만든 패(바꾸기 불가)
    // 화료패
    // 화료패를 얻은 방법(쯔모, 론)
    // 화료패를 얻은 위치(치,퐁,깡,자기)
    // 역패로 얻은 역 비트마스킹
    // 삼원패 자풍패 장풍패 연풍패
};

    enum class WinShape : unsigned int {
    LICHI=1, // 리치
    IPPATSU=2, // 일발
    PINGHU=4, // 핑후
    TANYAO=8, // 탕야오
    IPEKO=16, // 이페코
    MENZENTSUMO=32, // 멘젠쯔모
    RINSHAN=64, // 영상개화
    LASTTILEDRAW=128, // 해저로월
    LASTCARDDISCARD=256, // 하저로어
    CHANKAN=512, // 창깡
    // 2판
    CHITOITSU=1024, // 치또이츠
    DOUBLELICHI=2048, // 더블리치
    IKKITSUKAN=4096, // 일기통관
    SANSHOKUDOUJUN=8192, // 삼색동순
    SANSHOKUDOUKOU=16384, // 삼색동각
    CHANTA=32768, // 찬타
    HONROUTOU=65536, // 혼노두
    SHOUSANGEN=131072, // 소삼원
    TOITOIHOU=262144, // 토이토이
    SANANKOU=524288, // 삼안커
    SANKANTSU=1048576, // 삼깡즈
    // 3판
    HONISSOU=2097152, // 혼일색
    JUNCHAN=4194304, // 쥰찬타
    RYANPEIKOU=8388608, // 량페코
    // 6판
    CHINITSU=16777216 // 청일색
};

unsigned int isWin(const mahjong::Tile handCard[15]) {
    // 역 포함 여부를 판단하는 로직을 구현
    // 비트마스킹으로 가능한 역을 계산하여 반환
    // 역만도 마찬가지

    return 0;  // 임시로 항상 0 반환
}
}  // namespace winChecker

#endif  // WINCHECKER_HPP