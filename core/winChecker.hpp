#pragma once
#ifndef WINCHECKER_HPP
#define WINCHECKER_HPP
#include<vector>
#include "calcScore.hpp" //calcHan/calcFu/calcScore
#include "checkYOKUnYOKUMAN.hpp"
#include "mahjong.hpp"
#include "winTypes.hpp" //WinInfo/YokuMask/YokuManMask/DFSState
namespace winChecker{//주어진 손패+화료패+멘츠 정보를 보고 화료 가능한 형태인지 판단한다.
    void find_dfs(
        DFSState&dfs,
        std::vector<WinInfo>&result){
        constexpr int NEED_MELD=mahjong::PAIR_MAX-1;
        //------------------------------------------------------------
        //종료 조건
        //------------------------------------------------------------
        if(dfs.meldCnt==NEED_MELD){
            for(int i=0;i<mahjong::TILE_MAX;++i){
                if(dfs.cnt[i]!=0)
                    return;
            }
            //화료패가 실제 완성 형태에 들어가지 않았다면 실패
            if(dfs.winMeld==-1)
                return;
            WinInfo info;
            info.state=dfs.state;
            info.head=dfs.head;
            info.winTile=dfs.winTile;
            info.winMeld=dfs.winMeld;
            info.meldCnt=dfs.meldCnt;
            for(int i=0;i<dfs.meldCnt;++i)
                info.melds[i]=dfs.melds[i];
            result.push_back(info);
            return;
        }
        //------------------------------------------------------------
        //가장 작은 남은 패
        //------------------------------------------------------------
        int first=-1;
        for(int i=0;i<mahjong::TILE_MAX;++i){
            if(dfs.cnt[i]!=0){
                first=i;
                break;
            }
        }
        //패가 없는데 몸통이 부족함
        if(first==-1)
            return;
        const mahjong::Tile tile=
            static_cast<mahjong::Tile>(first);
        //------------------------------------------------------------
        //1. 커쯔
        //------------------------------------------------------------
        if(dfs.cnt[first]>=3){
            dfs.cnt[first]-=3;
            const int idx=dfs.meldCnt++;
            dfs.melds[idx]={
                tile,
                mahjong::MType::KOUT,
                mahjong::WGet::SELF
            };
            //현재 분기의 winMeld 저장
            const int oldWinMeld=dfs.winMeld;
            //아직 화료패의 위치를 찾지 못했고
            //현재 커쯔가 화료패를 포함한다면 기록
            if(dfs.winMeld==-1&&
                dfs.winTile==tile){
                dfs.winMeld=idx;
            }
            find_dfs(dfs,result);
            //백트래킹
            dfs.winMeld=oldWinMeld;
            --dfs.meldCnt;
            dfs.cnt[first]+=3;
        }
        //------------------------------------------------------------
        //2. 슌쯔
        //------------------------------------------------------------
        if(mahjong::canShun(tile)&&
            dfs.cnt[first+1]!=0&&
            dfs.cnt[first+2]!=0){
            --dfs.cnt[first];
            --dfs.cnt[first+1];
            --dfs.cnt[first+2];
            const int idx=dfs.meldCnt++;
            dfs.melds[idx]={
                tile,
                mahjong::MType::SHUN,
                mahjong::WGet::SELF
            };
            const int oldWinMeld=dfs.winMeld;
            //현재 슌쯔가 화료패를 포함하는가?
            if(dfs.winMeld==-1&&
                (dfs.winTile==static_cast<mahjong::Tile>(first)||
                dfs.winTile==static_cast<mahjong::Tile>(first+1)||
                dfs.winTile==static_cast<mahjong::Tile>(first+2))){
                dfs.winMeld=idx;
            }
            find_dfs(dfs,result);
            //백트래킹
            dfs.winMeld=oldWinMeld;
            --dfs.meldCnt;
            ++dfs.cnt[first];
            ++dfs.cnt[first+1];
            ++dfs.cnt[first+2];
        }
    }
    int __countCard(const mahjong::Tile handCard[mahjong::HAND_MAX],mahjong::Tile card){
        int cnt=0;
        for(int i=0;i<mahjong::HAND_MAX;i++){
            if(handCard[i]==card)cnt++;
        }
        return cnt;
    }
    //handCard 전체(14장)를 보고 가능한 몸통 분해들 중 가장 점수가 높은
    //것을 찾아 반환한다. 분해가 하나도 없으면(=일반형으로 화료 불가)state가
    //그대로 0인 WinInfo가 반환되므로,호출부에서 meldCnt==0&&head==0 등으로
    //판정하거나 isWin()의 chiitoitsu/kokushi 비트로 대체 판단하면 된다.
    WinInfo findBestWin(const mahjong::Tile handCard[mahjong::HAND_MAX],mahjong::Tile winTile,
                        bool tsumo=true,bool menzen=true){
        unsigned char baseCnt[mahjong::TILE_MAX]{};
        //------------------------------------------------------------
        //13장 손패
        //------------------------------------------------------------
        for(int i=0;i<13;++i){
            const mahjong::Tile tile=handCard[i];
            if(tile<mahjong::TILE_MAX)
                ++baseCnt[tile];
        }
        //------------------------------------------------------------
        //화료패
        //------------------------------------------------------------
        if(winTile>=mahjong::TILE_MAX)
            return{};
        ++baseCnt[winTile];
        std::vector<WinInfo>results;
        //------------------------------------------------------------
        //머리 후보
        //------------------------------------------------------------
        for(int t=0;t<mahjong::TILE_MAX;++t){
            if(baseCnt[t]<2)
                continue;
            DFSState dfs;
            for(int i=0;i<mahjong::TILE_MAX;++i)
                dfs.cnt[i]=baseCnt[i];
            //머리 제거
            dfs.cnt[t]-=2;
            dfs.head=
                static_cast<mahjong::Tile>(t);
            //DFS가 반드시 알고 있어야 하는 화료패
            dfs.winTile=winTile;
            //화료패가 머리로 사용됨
            if(dfs.head==dfs.winTile)
                dfs.winMeld=-2;
            find_dfs(dfs,results);
        }
        //------------------------------------------------------------
        //가장 좋은 분해 선택
        //------------------------------------------------------------
        WinInfo best;
        int bestScore=-1;
        for(auto&info:results){
            const int score=
                calcScore(info,tsumo,menzen);
            if(score>bestScore){
                bestScore=score;
                best=info;
            }
        }
        return best;
    }
    unsigned int isWin(
        const mahjong::Tile handCard[13],
        mahjong::Tile winTile,
        bool tsumo=true,
        bool menzen=true){
        unsigned int scoreMask=0;
        //------------------------------------------------------------
        //화료패 유효성
        //------------------------------------------------------------
        if(winTile>=mahjong::TILE_MAX)
            return 0;
        //------------------------------------------------------------
        //정렬된 13장 손패 생성
        //------------------------------------------------------------
        mahjong::Tile sortedHand[13];
        for(int i=0;i<13;++i)
            sortedHand[i]=handCard[i];
        mahjong::PrioritySort(sortedHand,13);
        //------------------------------------------------------------
        //치또이츠
        //------------------------------------------------------------
        if(yaku::isChiitoitsu(sortedHand,winTile)){
            scoreMask|=
                static_cast<unsigned int>(
                    YokuMask::CHITOITSU);
        }
        //------------------------------------------------------------
        //국사무쌍
        //------------------------------------------------------------
        if(yakuMan::isKokushi(sortedHand,winTile)){
            scoreMask|=
                static_cast<unsigned int>(
                    YokuManMask::KOKUSHI);
        }
        //------------------------------------------------------------
        //일반형
        //------------------------------------------------------------
        WinInfo best=
            findBestWin(
                sortedHand,
                winTile,
                tsumo,
                menzen);
        if(best.meldCnt==
            mahjong::PAIR_MAX-1){
            scoreMask|=best.yaku;
        }
        return scoreMask;
    }    
}//namespace winChecker
#endif //WINCHECKER_HPP