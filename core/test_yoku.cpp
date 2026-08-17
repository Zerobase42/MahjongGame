// ===========================================================================
// 마작 역 판별 로직 테스트용 코드
//
// 빌드:
//   g++ -std=c++17 -Wall -Wextra -o test_mahjong test_mahjong.cpp
// 실행:
//   ./test_mahjong
//
// 외부 테스트 프레임워크 없이 간단한 assert 스타일로 작성했습니다.
// 각 TEST_CASE는 실패해도 프로그램이 멈추지 않고 다음 케이스로 넘어가며,
// 마지막에 통과/실패 개수를 요약해서 보여줍니다.
// ===========================================================================

#include <cstdio>
#include <cstring>
#include <string>

#include<windows.h>

#include "checkYOKUnYOKUMAN.hpp"
#include "winChecker.hpp"

#define SET_CODE_KO SetConsoleCP(CP_UTF8),SetConsoleOutputCP(CP_UTF8)

// ---------------------------------------------------------------------------
// 아주 작은 테스트 유틸
// ---------------------------------------------------------------------------
static int g_pass = 0;
static int g_fail = 0;

#define CHECK(cond, msg)                                       \
    do {                                                       \
        if (cond) {                                            \
            ++g_pass;                                          \
            printf("  [PASS] %s\n", msg);                      \
        } else {                                               \
            ++g_fail;                                          \
            printf("  [FAIL] %s  (line %d)\n", msg, __LINE__); \
        }                                                      \
    } while (0)

#define SECTION(title) printf("\n== %s ==\n", title)

// 역 비트마스크를 사람이 읽을 수 있는 이름들로 출력
static void printYaku(unsigned int yaku) {
    struct Entry {
        winChecker::YokuMask mask;
        const char* name;
    };
    static const Entry table[] = {
        { winChecker::YokuMask::LICHI, "lichi" },
        { winChecker::YokuMask::IPPATSU, "ippatsu" },
        { winChecker::YokuMask::PINGHU, "pinfu" },
        { winChecker::YokuMask::TANYAO, "tanyao" },
        { winChecker::YokuMask::IPEKO, "iipeikou" },
        { winChecker::YokuMask::MENZENTSUMO, "menzen_tsumo" },
        { winChecker::YokuMask::CHITOITSU, "chiitoitsu" },
        { winChecker::YokuMask::IKKITSUKAN, "ittsuu" },
        { winChecker::YokuMask::SANSHOKUDOUJUN, "sanshoku_doujun" },
        { winChecker::YokuMask::SANSHOKUDOUKOU, "sanshoku_doukou" },
        { winChecker::YokuMask::CHANTA, "chanta" },
        { winChecker::YokuMask::HONROUTOU, "honroutou" },
        { winChecker::YokuMask::SHOUSANGEN, "shousangen" },
        { winChecker::YokuMask::TOITOIHOU, "toitoi" },
        { winChecker::YokuMask::SANANKOU, "sananko" },
        { winChecker::YokuMask::HONITSU, "honitsu" },
        { winChecker::YokuMask::JUNCHANTA, "junchanta" },
        { winChecker::YokuMask::RYANPEIKOU, "ryanpeikou" },
        { winChecker::YokuMask::CHINITSU, "chinitsu" },
    };
    bool any = false;
    for (auto& e : table) {
        if (yaku & static_cast<unsigned int>(e.mask)) {
            printf("%s%s", any ? ", " : "", e.name);
            any = true;
        }
    }
    if (!any) printf("(none)");
}

static bool has(unsigned int yaku, winChecker::YokuMask m) {
    return (yaku & static_cast<unsigned int>(m)) != 0;
}

// 손패(14장) 하나를 findBestWin으로 분석하고, 결과 요약을 출력한 뒤 WinInfo를 반환
static winChecker::WinInfo analyze(const char* label,
                                   const mahjong::Tile hand[mahjong::HAND_MAX],
                                   bool tsumo = true, bool menzen = true) {
    auto best = winChecker::findBestWin(hand, tsumo, menzen);
    printf("[%s] meldCnt=%d head=%d yaku=", label, best.meldCnt, best.head);
    printYaku(best.yaku);
    printf("\n");
    return best;
}

// ---------------------------------------------------------------------------
// 손패를 손으로 나열하기 편하게 해주는 매크로
// (정렬 여부는 상관없음: find_dfs/isChiitoitsu 등은 카운트 기반이라 무관하지만,
//  isChiitoitsu는 "정렬되어있다 가정"하는 구현이라 페어끼리 인접하게 적어줍니다)
// ---------------------------------------------------------------------------
using mahjong::Tile;
using namespace mahjong;  // M1..M9, S1..S9, T1..T9, E,S,W,N,Wh,G,R (unscoped enum이라 mahjong 네임스페이스에 바로 들어있음)

int main() {
    SET_CODE_KO;
    // -----------------------------------------------------------------
    SECTION("치또이츠 / 국사무쌍 / 구련보등 기본 판별");
    {
        Tile chiitoi[mahjong::HAND_MAX] = {
            M1, M1, M3, M3, M5, M5, M7, M7, S2, S2, S4, S4, E, E  // 7쌍
        };
        CHECK(yoku::isChiitoitsu(chiitoi), "정상적인 치또이츠(7쌍) 판정");

        Tile notChiitoi[mahjong::HAND_MAX] = {
            M1, M1, M3, M3, M5, M5, M7, M7, S2, S2, S4, S4, E, S  // 마지막 쌍이 깨짐
        };
        CHECK(!yoku::isChiitoitsu(notChiitoi), "쌍이 깨지면 치또이츠 아님");

        Tile notChiitoi2[mahjong::HAND_MAX] = {
            M1, M1, M3, M3, M5, M5, M5, M5, S2, S2, S4, S4, E, E  // 똑같은 쌍이 2개
        };
        CHECK(!yoku::isChiitoitsu(notChiitoi2), "똑같은 머리 2개는 치또이츠 아님(깡)");

        Tile kokushi[mahjong::HAND_MAX] = {
            M1, M9, S1, S9, T1, T9, E, S, W, N, Wh, G, R, M1  // 13종 + M1 페어
        };
        CHECK(yokuMan::isKokushi(kokushi), "국사무쌍 13종+페어 판정");

        Tile notKokushi[mahjong::HAND_MAX] = {
            M1, M9, S1, S9, T1, T9, E, S, W, N, Wh, G, R, M2  // 요구패 아닌 잉여패
        };
        CHECK(!yokuMan::isKokushi(notKokushi), "요구패 미충족 시 국사무쌍 아님");

        Tile chuuren[mahjong::HAND_MAX] = {
            M1, M1, M1, M2, M3, M4, M5, M6, M7, M8, M9, M9, M9, M5  // 1112345678999 + 5
        };
        CHECK(yokuMan::isChuuren(chuuren), "구련보등 기본형 판정");

        Tile notChuuren[mahjong::HAND_MAX] = {
            M1, M1, M1, M2, M3, M4, M5, M6, M7, M8, M9, M9, M9, S1  // 혼일색 깨짐
        };
        CHECK(!yokuMan::isChuuren(notChuuren), "타 수트 섞이면 구련보등 아님");
    }

    // -----------------------------------------------------------------
    SECTION("몸통 탐색 (find_dfs / findBestWin) 기본 동작");
    {
        // 123m 456s 789t 456m + 77m 머리 (전부 슌쯔)
        Tile hand[mahjong::HAND_MAX] = {
            M1, M2, M3, S4, S5, S6, T7, T8, T9, M4, M5, M6, M7, M7
        };
        auto best = analyze("일반형 4슌쯔+머리", hand);
        CHECK(best.meldCnt == 4, "몸통 4개를 모두 채워 유효한 분해를 찾음");
        CHECK(best.head == M7, "머리로 7m 페어를 선택함");

        // 몸통 분해가 불가능한 엉터리 손패(4-3-3-2-1 같은 형태)
        Tile impossible[mahjong::HAND_MAX] = {
            M1, M2, M4, S1, S3, S5, T1, T4, T7, M9, S9, T9, E, S
        };
        auto badBest = winChecker::findBestWin(impossible, true, true);
        CHECK(badBest.meldCnt != 4, "분해 불가능한 손패는 4몸통을 채우지 못함");
    }

    // -----------------------------------------------------------------
    SECTION("탕야오 / 핑후 / 멘젠쯔모");
    {
        // 234m 456s 567t 234t + 88m (전부 2~8 슌쯔, 자패/터미널 없음)
        Tile hand[mahjong::HAND_MAX] = {
            M2, M3, M4, S4, S5, S6, T5, T6, T7, T2, T3, T4, M8, M8
        };
        auto best = analyze("탕야오+핑후+멘젠쯔모", hand, /*tsumo=*/true, /*menzen=*/true);
        CHECK(has(best.yaku, winChecker::YokuMask::TANYAO), "탕야오 인식");
        CHECK(has(best.yaku, winChecker::YokuMask::PINGHU), "핑후 인식");
        CHECK(has(best.yaku, winChecker::YokuMask::MENZENTSUMO), "멘젠쯔모 인식");

        // 같은 손패인데 론(tsumo=false)이면 멘젠쯔모는 빠져야 함
        auto ron = winChecker::findBestWin(hand, /*tsumo=*/false, /*menzen=*/true);
        CHECK(!has(ron.yaku, winChecker::YokuMask::MENZENTSUMO), "론이면 멘젠쯔모 제외");

        // 자패 머리(핑후 조건 위반)면 핑후가 빠져야 함
        Tile withHonorHead[mahjong::HAND_MAX] = {
            M2, M3, M4, S4, S5, S6, T5, T6, T7, T2, T3, T4, E, E
        };
        auto noPinfu = analyze("자패 머리라 핑후 아님", withHonorHead);
        CHECK(!has(noPinfu.yaku, winChecker::YokuMask::PINGHU), "자패 머리면 핑후 제외");
    }

    // -----------------------------------------------------------------
    SECTION("또이또이 / 삼안커");
    {
        // 222m 555s 777t 999t + 33m (전부 커쯔)
        Tile hand[mahjong::HAND_MAX] = {
            M2, M2, M2, S5, S5, S5, T7, T7, T7, T9, T9, T9, M3, M3
        };
        auto best = analyze("또이또이+삼안커", hand);
        CHECK(has(best.yaku, winChecker::YokuMask::TOITOIHOU), "또이또이 인식");
        CHECK(has(best.yaku, winChecker::YokuMask::SANANKOU), "삼안커(암커 3개+) 인식");
    }

    // -----------------------------------------------------------------
    SECTION("혼일색 / 청일색 / 일기통관");
    {
        // 123m 456m 789m EEE + 99m  (만수 + 자패 -> 혼일색, 123/456/789 -> 일기통관)
        Tile honitsu[mahjong::HAND_MAX] = {
            M1, M2, M3, M4, M5, M6, M7, M8, M9, E, E, E, M9, M9
        };
        auto h = analyze("혼일색+일기통관", honitsu);
        CHECK(has(h.yaku, winChecker::YokuMask::HONITSU), "혼일색 인식");
        CHECK(has(h.yaku, winChecker::YokuMask::IKKITSUKAN), "일기통관 인식");
        CHECK(!has(h.yaku, winChecker::YokuMask::CHINITSU), "자패 섞였으므로 청일색 아님");

        // 123m 456m 789m 234m + 55m (만수만 사용 -> 청일색)
        Tile chinitsu[mahjong::HAND_MAX] = {
            M1, M2, M3, M4, M5, M6, M7, M8, M9, M2, M3, M4, M5, M5
        };
        auto c = analyze("청일색", chinitsu);
        CHECK(has(c.yaku, winChecker::YokuMask::CHINITSU), "청일색 인식");
        CHECK(!has(c.yaku, winChecker::YokuMask::HONITSU), "청일색이면 혼일색과 중복 표시 안 함");
    }

    // -----------------------------------------------------------------
    SECTION("삼색동순 / 삼색동각");
    {
        // 123m 123s 123t 456m + 77m (세 수트 모두 123 슌쯔)
        Tile sanshokuShun[mahjong::HAND_MAX] = {
            M1, M2, M3, S1, S2, S3, T1, T2, T3, M4, M5, M6, M7, M7
        };
        auto s1 = analyze("삼색동순", sanshokuShun);
        CHECK(has(s1.yaku, winChecker::YokuMask::SANSHOKUDOUJUN), "삼색동순 인식");

        // 555m 555s 555t 123m + 77m (세 수트 모두 5 커쯔)
        Tile sanshokuKou[mahjong::HAND_MAX] = {
            M5, M5, M5, S5, S5, S5, T5, T5, T5, M1, M2, M3, M7, M7
        };
        auto s2 = analyze("삼색동각", sanshokuKou);
        CHECK(has(s2.yaku, winChecker::YokuMask::SANSHOKUDOUKOU), "삼색동각 인식");
    }

    // -----------------------------------------------------------------
    SECTION("찬타 / 준찬타 / 혼노두");
    {
        // 123m 789s 123t 999m + 11p 머리 (모든 몸통+머리에 터미널 포함, 자패는 없음)
        Tile junchanta[mahjong::HAND_MAX] = {
            M1, M2, M3, S7, S8, S9, T1, T2, T3, M9, M9, M9, T1, T1
        };
        auto j = analyze("준찬타(자패 없이 터미널만)", junchanta);
        CHECK(has(j.yaku, winChecker::YokuMask::JUNCHANTA), "준찬타 인식");
        CHECK(!has(j.yaku, winChecker::YokuMask::CHANTA), "준찬타면 찬타 중복 표시 안 함");

        // 123m 789s EEE 999m + WW 머리 (자패가 섞인 터미널/자패형 -> 찬타)
        Tile chanta[mahjong::HAND_MAX] = {
            M1, M2, M3, S7, S8, S9, E, E, E, M9, M9, M9, W, W
        };
        auto c = analyze("찬타(자패 포함)", chanta);
        CHECK(has(c.yaku, winChecker::YokuMask::CHANTA), "찬타 인식");
        CHECK(!has(c.yaku, winChecker::YokuMask::JUNCHANTA), "자패 섞이면 준찬타 아님");

        // 111m 999s EEE WWW + 99t 머리 (전부 터미널/자패 커쯔 -> 혼노두)
        Tile honroutou[mahjong::HAND_MAX] = {
            M1, M1, M1, S9, S9, S9, E, E, E, W, W, W, T9, T9
        };
        auto hr = analyze("혼노두", honroutou);
        CHECK(has(hr.yaku, winChecker::YokuMask::HONROUTOU), "혼노두 인식");
        CHECK(has(hr.yaku, winChecker::YokuMask::TOITOIHOU), "혼노두는 전부 커쯔이므로 또이또이도 동반");
    }

    // -----------------------------------------------------------------
    SECTION("소삼원");
    {
        // WhWhWh GGG 123m 456s + RR 머리 (용패 2종 커쯔 + 용패 머리)
        Tile shousangen[mahjong::HAND_MAX] = {
            Wh, Wh, Wh, G, G, G, M1, M2, M3, S4, S5, S6, R, R
        };
        auto s = analyze("소삼원", shousangen);
        CHECK(has(s.yaku, winChecker::YokuMask::SHOUSANGEN), "소삼원 인식");
    }

    // -----------------------------------------------------------------
    SECTION("이페코 / 량페코 (멘젠 한정)");
    {
        // 112233m 456s 789t + 99t (11 22 33m 슌쯔가 완전히 겹침 -> 이페코 1세트)
        Tile iipeikou[mahjong::HAND_MAX] = {
            M1, M2, M3, M1, M2, M3, S4, S5, S6, T7, T8, T9, T9, T9
        };
        auto i = analyze("이페코", iipeikou);
        CHECK(has(i.yaku, winChecker::YokuMask::IPEKO), "이페코 인식");
        CHECK(!has(i.yaku, winChecker::YokuMask::RYANPEIKOU), "이페코 1세트만 있으면 량페코 아님");

        // 112233m 112233s + 77p (슌쯔 짝이 두 세트 -> 량페코)
        Tile ryanpeikou[mahjong::HAND_MAX] = {
            M1, M2, M3, M1, M2, M3, S1, S2, S3, S1, S2, S3, T7, T7
        };
        auto r = analyze("량페코", ryanpeikou);
        CHECK(has(r.yaku, winChecker::YokuMask::RYANPEIKOU), "량페코 인식");
        CHECK(!has(r.yaku, winChecker::YokuMask::IPEKO), "량페코면 이페코와 중복 표시 안 함");
    }

    // -----------------------------------------------------------------
    SECTION("isWin() 종합 (치또이츠/국사무쌍 + 일반형 스코어 마스크 병합 확인)");
    {
        Tile chiitoi[mahjong::HAND_MAX] = {
            M1, M1, M3, M3, M5, M5, M7, M7, S2, S2, S4, S4, E, E
        };
        unsigned int mask = winChecker::isWin(chiitoi);
        CHECK(mask & static_cast<unsigned int>(winChecker::YokuMask::CHITOITSU),
              "isWin()이 치또이츠 비트를 포함");

        Tile normal[mahjong::HAND_MAX] = {
            M2, M3, M4, S4, S5, S6, T5, T6, T7, T2, T3, T4, M8, M8
        };
        unsigned int normalMask = winChecker::isWin(normal, true, true);
        CHECK((normalMask & static_cast<unsigned int>(winChecker::YokuMask::TANYAO)) != 0,
              "isWin()이 일반형 분해에서 나온 탕야오 비트도 포함");
    }

    // -----------------------------------------------------------------
    printf("\n===========================================\n");
    printf("결과: %d개 통과 / %d개 실패 (총 %d개)\n", g_pass, g_fail, g_pass + g_fail);
    printf("===========================================\n");

    return g_fail == 0 ? 0 : 1;
}