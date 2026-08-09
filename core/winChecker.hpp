#pragma once
#ifndef WINCHECKER_HPP
#define WINCHECKER_HPP

#include <vector>

#include "calcScore.hpp"  // calcHan / calcFu / calcScore
#include "checkYOKUnYOKUMAN.hpp"
#include "mahjong.hpp"
#include "winTypes.hpp"  // WinInfo / YokuMask / YokuManMask / DFSState

namespace winChecker {  // 주어진 손패 + 화료패 + 멘츠 정보를 보고 화료 가능한 형태인지 판단한다.

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

int __countCard(const mahjong::Tile handCard[mahjong::HAND_MAX], mahjong::Tile card) {
    int cnt = 0;
    for (int i = 0; i < mahjong::HAND_MAX; i++) {
        if (handCard[i] == card) cnt++;
    }
    return cnt;
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

        dfs.cnt[t] -= 2;                           // 머리 2장 제외
        dfs.head = static_cast<mahjong::Tile>(t);  // 남은 12장으로 몸통 4개 탐색

        find_dfs(dfs, results);
    }

    WinInfo best;  // 기본 생성자 -> state/meldCnt 등 0
    int bestScore = -1;
    for (auto& info : results) {  // calcScore가 info.yaku를 채워 넣으므로 비-const
        int s = calcScore(info, tsumo, menzen);
        if (s > bestScore) {
            bestScore = s;
            best = info;  // yaku가 채워진 이후 상태를 복사
        }
    }
    return best;
}

unsigned int isWin(const mahjong::Tile handCard[mahjong::HAND_MAX],
                   bool tsumo = true, bool menzen = true) {
    // 역 포함 여부를 판단하는 로직을 구현
    // 비트마스킹으로 가능한 역을 계산하여 반환
    // 역만도 마찬가지
    unsigned int scoreMask = 0;

    // 기저 사례 : 치또이쯔, 국사무쌍 (몸통 4개+머리 형태가 아닌 특수 형태)
    if (yoku::isChiitoitsu(handCard)) {
        scoreMask |= static_cast<unsigned int>(YokuMask::CHITOITSU);
    }
    if (yokuMan::isKokushi(handCard)) {
        scoreMask |= static_cast<unsigned int>(YokuManMask::KOKUSHI);  // 국사무쌍은 치또이츠와 동일한 판정으로 처리
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