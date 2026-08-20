#pragma once
#ifndef WINTYPES_HPP
#define WINTYPES_HPP

#include "mahjong.hpp"

// winChecker.hpp(몸통 탐색)와 calcScore.hpp(판수/부수 계산)가 공통으로 쓰는
// 자료구조들을 모아둔 헤더. calcScore.hpp가 WinInfo를 써야 하는데
// winChecker.hpp는 반대로 calcScore.hpp의 calcHan/calcFu를 써야 해서
// 순환 include가 생기므로, 두 파일이 공유하는 타입만 여기로 분리했다.
namespace winChecker {

struct WinInfo {
    unsigned int state = 0;

    mahjong::Meld melds[mahjong::PAIR_MAX]{};
    int meldCnt = 0;

    mahjong::Tile head = 0;

    // 화료패
    mahjong::Tile winTile = 0;

    // 화료패를 얻은 방법
    mahjong::WGet winGet = mahjong::WGet::SELF;

    // -2 : 머리
    // -1 : 없음
    // 0~3 : 해당 몸통
    int winMeld = -1;

    unsigned int yaku = 0;
    unsigned int yakuman = 0;

    int dora = 0;
};

enum class YokuMask : unsigned int {
    LICHI = 1,                 // 리치
    IPPATSU = 1 << 1,          // 일발
    PINGHU = 1 << 2,           // 핑후
    TANYAO = 1 << 3,           // 탕야오
    IPEKO = 1 << 4,            // 이페코
    MENZENTSUMO = 1 << 5,      // 멘젠쯔모
    RINSHAN = 1 << 6,          // 영상개화
    LASTTILEDRAW = 1 << 7,     // 해저로월
    LASTCARDDISCARD = 1 << 8,  // 하저로어
    CHANKAN = 1 << 9,          // 창깡
    // 역패는 따로 체크. 바이트

    // 2판
    CHITOITSU = 1 << 10,       // 치또이츠
    DOUBLELICHI = 1 << 11,     // 더블리치
    IKKITSUKAN = 1 << 12,      // 일기통관
    SANSHOKUDOUJUN = 1 << 13,  // 삼색동순
    SANSHOKUDOUKOU = 1 << 14,  // 삼색동각
    CHANTA = 1 << 15,          // 찬타
    HONROUTOU = 1 << 16,       // 혼노두
    SHOUSANGEN = 1 << 17,      // 소삼원
    TOITOIHOU = 1 << 18,       // 또이또이
    SANANKOU = 1 << 19,        // 삼안커
    SANKANTSU = 1 << 20,       // 삼깡즈
    // 3판
    HONITSU = 1 << 21,     // 혼일색
    JUNCHANTA = 1 << 22,   // 쥰찬타
    RYANPEIKOU = 1 << 23,  // 량페코
    // 6판
    CHINITSU = 1 << 24  // 청일색
};
enum class YokuManMask : unsigned int {
    KOKUSHI = 1,          // 국사무쌍
    TENHOU = 1 << 1,      // 천화
    CHIIHOU = 1 << 2,     // 지화
    CHUUREN = 1 << 3,     // 구련보등
    RYUUIISOU = 1 << 4,   // 녹일색
    DAISANGEN = 1 << 5,   // 대삼원
    SHOUSUUSHI = 1 << 6,  // 소사희
    CHINROUTOU = 1 << 7,  // 청노두
    TSUUIISOU = 1 << 8,   // 자일색
    SUUANKOU = 1 << 9,    // 스안커
    SUUKANTSU = 1 << 10,  // 스깡쯔
    // 더블 역만
    KOKUSHI13 = 1 << 11,      // 국사무쌍 13명 대기
    JUNSEICHUUREN = 1 << 12,  // 순정구련보등
    DAISUUSHI = 1 << 13,      // 대사희
    SUUANKOUTANKI = 1 << 14   // 스안커 단기
};

struct DFSState {
    unsigned char cnt[mahjong::TILE_MAX]{};

    mahjong::Meld melds[mahjong::PAIR_MAX]{};
    int meldCnt = 0;

    mahjong::Tile head = 0;

    // 화료패
    mahjong::Tile winTile = 0;

    // -2 : 머리에 화료패가 들어감
    // -1 : 아직 화료패가 들어간 위치를 찾지 못함
    // 0~3 : melds[0] ~ melds[3]
    int winMeld = -1;

    unsigned int state = 0;
};

}  // namespace winChecker

#endif  // WINTYPES_HPP