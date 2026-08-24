#pragma once
#ifndef MAHJONG_HPP
#define MAHJONG_HPP

#ifndef VERSION
#define VERSION 100
#endif // VERSION

#include<algorithm>
namespace mahjong{
    inline constexpr int PLAYER_MAX=4;
    inline constexpr int TILE_MAX=34;
    inline constexpr int PAIR_MAX=5;
    inline constexpr int HAND_MAX=14;
    using Tile=unsigned char;
    inline unsigned char used_tile[TILE_MAX]{};
    enum MAHSKII:unsigned char{
        M1=1,M2,M3,M4,M5,M6,M7,M8,M9,
        S1=11,S2,S3,S4,S5,S6,S7,S8,S9,
        T1=21,T2,T3,T4,T5,T6,T7,T8,T9,
        E=0,S=10,W=20,N=30,
        Wh=31,G=32,R=33
    };
    enum class MType:unsigned char{
        CHI,//치->슌쯔
        PON,//퐁->커쯔
        KAN,//대밍깡
        INK,//안깡
        SKN,//소밍깡
        HEAD,//머리
        SHUN,//슌쯔
        KOUT,//커쯔
    };
    enum class WGet:unsigned char{
        LEFT,
        OPPOSITE,
        RIGHT,
        //^ 치퐁깡
        SELF//안깡,내가 만든 슌쯔,커쯔
    };
    struct Meld{
        Tile card;
        MType mType;
        WGet wGet;
    };
    struct Hand{
        Tile card[HAND_MAX];//손패
        Meld melds[PAIR_MAX];//치퐁깡 모아놓은거. 회료시 확인
        int meldCnt;//치퐁깡 개수
    };
    inline static constexpr unsigned char priority[34]={
        28,
        1,2,3,4,5,6,7,8,9,29,
        10,11,12,13,14,15,16,17,18,30,
        19,20,21,22,23,24,25,26,27,31,
        32,33,34
    };
    inline void PrioritySort(Tile*card,int size=HAND_MAX){
        std::sort(card,card+size,
            [](Tile a,Tile b){
                if(a==255||b==255)return a<b;
                return priority[a]<priority[b];
            });
    }
    inline constexpr bool isShun(Tile tile){
        return(M1<=tile&&tile<=M7)||
              (S1<=tile&&tile<=S7)||
              (T1<=tile&&tile<=T7);
    }
    inline bool isHonor(Tile tile) {
        return tile >= E && tile <= R;
    }
    inline bool isTerminal(Tile tile) {
        return (tile >= M1 && tile <= M9 &&
                (tile == M1 || tile == M9)) ||
               (tile >= S1 && tile <= S9 &&
                (tile == S1 || tile == S9)) ||
               (tile >= T1 && tile <= T9 &&
                (tile == T1 || tile == T9));
    }
    inline bool isTerminalOrHonor(Tile tile) {
        return isTerminal(tile) || isHonor(tile);
    }
}//namespace majong
#endif//MAHJONG_HPP