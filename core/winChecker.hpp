#pragma once
#ifndef WINCHECKER_HPP
#define WINCHECKER_HPP

#include "mahjong.hpp"
#include "checkYOKUnYOKUMAN.hpp"

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

    enum class YokuMask : unsigned int {
        LICHI=1,               // 리치
        IPPATSU=1<<1,          // 일발
        PINGHU=1<<2,           // 핑후
        TANYAO=1<<3,           // 탕야오
        IPEKO=1<<4,            // 이페코
        MENZENTSUMO=1<<5,      // 멘젠쯔모
        RINSHAN=1<<6,          // 영상개화
        LASTTILEDRAW=1<<7,     // 해저로월
        LASTCARDDISCARD=1<<8,  // 하저로어
        CHANKAN=1<<9,          // 창깡
        // 역패는 따로 체크. 바이트

        // 2판
        CHITOITSU=1<<10,       // 치또이츠
        DOUBLELICHI=1<<11,     // 더블리치
        IKKITSUKAN=1<<12,      // 일기통관
        SANSHOKUDOUJUN=1<<13,  // 삼색동순
        SANSHOKUDOUKOU=1<<14,  // 삼색동각
        CHANTA=1<<15,          // 찬타
        HONROUTOU=1<<16,       // 혼노두
        SHOUSANGEN=1<<17,      // 소삼원
        TOITOIHOU=1<<18,       // 또이또이
        SANANKOU=1<<19,        // 삼안커
        SANKANTSU=1<<20,       // 삼깡즈
        // 3판
        HONITSU=1<<21,         // 혼일색
        JUNCHANTA=1<<22,       // 쥰찬타
        RYANPEIKOU=1<<23,      // 량페코
        // 6판
        CHINITSU=1<<24         // 청일색
    };
    enum class YokuManMask : unsigned int {
        KOKUSHI=1,             // 국사무쌍
        TENHOU=1<<1,           // 천화
        CHIIHOU=1<<2,          // 지화
        CHUUREN=1<<3,          // 구련보등
        RYUUIISOU=1<<4,        // 녹일색
        DAISANGEN=1<<5,        // 대삼원
        SHOUSUUSHI=1<<6,       // 소사희
        CHINROUTOU=1<<7,       // 청노두
        TSUUIISOU=1<<8,        // 자일색
        SUUANKOU=1<<9,         // 스안커
        SUUKANTSU=1<<10,       // 스깡쯔
        // 더블 역만
        KOKUSHI13=1<<11,       // 국사무쌍 13명 대기
        JUNSEICHUUREN=1<<12,   // 순정구련보등
        DAISUUSHI=1<<13,       // 대사희
        SUUANKOUTANKI=1<<14    // 스안커 단기
    };


void find_dfs() {
    // 손패를 분해하여 가능한 멘츠 조합을 찾는 깊이 우선 탐색 알고리즘 구현
    // 각 멘츠 조합에 대해 가능한 역을 계산하고 YokuMask에 반영
}


unsigned int isWin(const mahjong::Tile handCard[15]) {
    // 역 포함 여부를 판단하는 로직을 구현
    // 비트마스킹으로 가능한 역을 계산하여 반환
    // 역만도 마찬가지
    unsigned int scoreMask=0;

    // 기저 사례 : 치또이쯔, 국사무쌍
    if (yoku::isChiitoitsu(handCard)) {
        scoreMask|=static_cast<unsigned int>(YokuMask::CHITOITSU);
    }
    if(yokuMan::isKokushi(handCard)){
        scoreMask|=static_cast<unsigned int>(YokuManMask::KOKUSHI); // 국사무쌍은 치또이츠와 동일한 판정으로 처리
    }

    find_dfs();

    return scoreMask;  // 가능한 역의 비트마스크 반환
}
}  // namespace winChecker

#endif  // WINCHECKER_HPP