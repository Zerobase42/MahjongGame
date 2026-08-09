#pragma once
#ifndef WINCHECKER_HPP
#define WINCHECKER_HPP

#include<vector>
#include "mahjong.hpp"
#include "checkYOKUnYOKUMAN.hpp"

namespace winChecker {  // 주어진 손패 + 화료패 + 멘츠 정보를 보고 화료 가능한 형태인지 판단한다.

    struct WinInfo {
        unsigned int state = 0;
        // 화료 시 사용된 몸통
        mahjong::Meld melds[mahjong::PAIR_MAX]{};
        int meldCnt = 0;
        // 머리
        mahjong::Tile head = 0;
        // 화료패
        mahjong::Tile winTile = 0;
        // 화료패를 얻은 방법
        mahjong::WGet winGet = mahjong::WGet::SELF;
        // 화료패가 들어간 몸통
        // -1 : 머리 또는 아직 없음
        int winMeld = -1;
        // 역
        unsigned int yaku = 0;
        // 역만
        unsigned int yakuman = 0;
        // 도라, 적도라, 우라도리
        int dora=0;
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

    struct DFSState {
        unsigned char cnt[mahjong::TILE_MAX]{};

        mahjong::Meld melds[mahjong::PAIR_MAX]{};
        int meldCnt = 0;

        mahjong::Tile head = 0;

        int winMeld = -1;

        unsigned int state = 0;
    };

    void find_dfs(
        // 백트래킹으로 가능한 몸통 탐색
        DFSState& dfs,
        std::vector<WinInfo>& result) {
        // 모든 몸통을 만들었으면 남은 패가 없어야 완성
        if (dfs.meldCnt == mahjong::PAIR_MAX - 1) {
            for (int i = 0; i < mahjong::TILE_MAX; ++i) {
                if (dfs.cnt[i] != 0)
                    return;
            }

            WinInfo info;
            info.state = dfs.state;
            info.head = dfs.head;
            info.meldCnt = dfs.meldCnt;
            info.winMeld = dfs.winMeld;

            for (int i = 0; i < dfs.meldCnt; ++i)
                info.melds[i] = dfs.melds[i];

            result.push_back(info);
            return;
        }

        // 가장 작은 남은 타일을 찾는다.
        int first = -1;
        for (int i = 0; i < mahjong::TILE_MAX; ++i) {
            if (dfs.cnt[i]) {
                first = i;
                break;
            }
        }

        // 패가 없는데 몸통 수가 부족하면 실패
        if (first == -1)
            return;

        // 1. 커쯔
        if (dfs.cnt[first] >= 3) {
            dfs.cnt[first] -= 3;

            int idx = dfs.meldCnt++;
            dfs.melds[idx] = {
                static_cast<mahjong::Tile>(first),
                mahjong::MType::KOUT,
                mahjong::WGet::SELF
            };

            find_dfs(dfs, result);

            --dfs.meldCnt;
            dfs.cnt[first] += 3;
        }

        // 2. 슌쯔
        // 숫자패인지 확인
        bool canShun =
            (first >= mahjong::M1 && first <= mahjong::M7) ||
            (first >= mahjong::S1 && first <= mahjong::S7) ||
            (first >= mahjong::T1 && first <= mahjong::T7);

        if (canShun &&
            dfs.cnt[first + 1] &&
            dfs.cnt[first + 2]) {
            --dfs.cnt[first];
            --dfs.cnt[first + 1];
            --dfs.cnt[first + 2];

            int idx = dfs.meldCnt++;

            dfs.melds[idx] = {
                static_cast<mahjong::Tile>(first),
                mahjong::MType::SHUN,
                mahjong::WGet::SELF
            };

            find_dfs(dfs, result);

            --dfs.meldCnt;

            ++dfs.cnt[first];
            ++dfs.cnt[first + 1];
            ++dfs.cnt[first + 2];
        }
    }

    int __countCard(const mahjong::Tile handCard[mahjong::HAND_MAX],mahjong::Tile card){
        int cnt=0;
        for(int i=0;i<mahjong::HAND_MAX;i++){
            if(handCard[i]==card)cnt++;
        }
        return cnt;
    }

    // ---- 판수 / 부수 계산 -------------------------------------------------
    // 같은 손패라도 몸통을 나누는 방법이 여러 개일 수 있으므로(예: 이페코 vs
    // 다른 슌쯔 조합), find_dfs가 찾아낸 각 WinInfo 후보마다 이 함수들로
    // 점수를 매겨서 가장 높은 것을 채택한다. 실제 역 판별/부수 가산 로직은
    // 아직 구현되지 않은 자리(스텁)이며, 추후 여기를 채워 넣으면 된다.
    int calcHan(const WinInfo& info, bool tsumo, bool menzen) {
        int han = 0;
        // TODO: 리치/멘젠쯔모/탕야오/이페코/일기통관/혼일색/청일색 ... 등
        //       info.melds / info.head / info.winTile / info.winGet 을 보고
        //       역별로 판정하여 han += n 형태로 누적.
        (void)info; (void)tsumo; (void)menzen;
        return han;
    }

    int calcFu(const WinInfo& info, bool tsumo, bool menzen) {
        int fu = 20;  // 기본 20부
        // TODO: 커쯔/깡쯔(명/암, 노두/중장) 가산, 머리(역패) 가산,
        //       대기 형태(변짜/간짜/단기) 가산, 쯔모/론 가산 등 구현.
        (void)info; (void)tsumo; (void)menzen;
        return fu;
    }

    // 비교용 임시 스코어: 판을 부보다 우선하도록 가중치를 둠.
    // (실제 점수표 환산은 calcHan/calcFu가 채워진 뒤 별도로 구현)
    int calcScore(const WinInfo& info, bool tsumo, bool menzen) {
        int han = calcHan(info, tsumo, menzen);
        int fu = calcFu(info, tsumo, menzen);
        return han * 10000 + fu;
    }

    // handCard 전체(14장)를 보고 가능한 몸통 분해들 중 가장 점수가 높은
    // 것을 찾아 반환한다. 분해가 하나도 없으면(=일반형으로 화료 불가) state가
    // 그대로 0인 WinInfo가 반환되므로, 호출부에서 meldCnt==0 && head==0 등으로
    // 판정하거나 isWin()의 chiitoitsu/kokushi 비트로 대체 판단하면 된다.
    WinInfo findBestWin(const mahjong::Tile handCard[mahjong::HAND_MAX],
                         bool tsumo = true, bool menzen = true) {
        // 손패를 타일별 개수 배열로 변환 (find_dfs / 머리 탐색용)
        unsigned char baseCnt[mahjong::TILE_MAX]{};
        for (int i = 0; i < mahjong::HAND_MAX; i++) {
            if (handCard[i] < mahjong::TILE_MAX)  // 255(빈 슬롯) 방어
                baseCnt[handCard[i]]++;
        }

        std::vector<WinInfo> results;

        // 머리 후보: 2장 이상 있는 모든 종류의 패에 대해 각각 시도
        for (int t = 0; t < mahjong::TILE_MAX; t++) {
            if (baseCnt[t] < 2) continue;

            DFSState dfs;
            for (int i = 0; i < mahjong::TILE_MAX; i++)
                dfs.cnt[i] = baseCnt[i];

            dfs.cnt[t] -= 2;                          // 머리 2장 제외
            dfs.head = static_cast<mahjong::Tile>(t);  // 남은 12장으로 몸통 4개 탐색

            find_dfs(dfs, results);
        }

        WinInfo best;  // 기본 생성자 -> state/meldCnt 등 0
        int bestScore = -1;
        for (const auto& info : results) {
            int s = calcScore(info, tsumo, menzen);
            if (s > bestScore) {
                bestScore = s;
                best = info;
            }
        }
        return best;
    }

    unsigned int isWin(const mahjong::Tile handCard[mahjong::HAND_MAX],
                        bool tsumo = true, bool menzen = true) {
        // 역 포함 여부를 판단하는 로직을 구현
        // 비트마스킹으로 가능한 역을 계산하여 반환
        // 역만도 마찬가지
        unsigned int scoreMask=0;

        // 기저 사례 : 치또이쯔, 국사무쌍 (몸통 4개+머리 형태가 아닌 특수 형태)
        if (yoku::isChiitoitsu(handCard)) {
            scoreMask|=static_cast<unsigned int>(YokuMask::CHITOITSU);
        }
        if(yokuMan::isKokushi(handCard)){
            scoreMask|=static_cast<unsigned int>(YokuManMask::KOKUSHI); // 국사무쌍은 치또이츠와 동일한 판정으로 처리
        }

        // 일반형(몸통 4개 + 머리) : 머리를 바꿔가며 find_dfs로 가능한 모든
        // 몸통 조합을 탐색한 뒤, calcHan/calcFu로 점수를 매겨 가장 높은
        // 조합을 채택한다.
        WinInfo best = findBestWin(handCard, tsumo, menzen);
        if (best.meldCnt == mahjong::PAIR_MAX - 1) {  // 몸통 4개를 다 채운 유효한 분해가 존재
            scoreMask |= best.yaku;
        }

        return scoreMask;  // 가능한 역의 비트마스크 반환
    }
}  // namespace winChecker

#endif  // WINCHECKER_HPP