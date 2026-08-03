#pragma once
#ifndef MAHJONG_HPP
#define MAHJONG_HPP
namespace mahjong{
inline constexpr int PLAYER_MAX=4;
inline constexpr int TILE_MAX=34;
inline constexpr int PAIR_MAX=5;
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
    HEAD//머리
};
enum class WGet:unsigned char{
    LEFT,
    OPPOSITE,
    RIGHT,
    SELF
};
struct Meld{
    Tile card;
    MType mType;
    WGet wGet;
};
struct Hand{
    Tile card[15];
    Meld melds[PAIR_MAX];
    int meldCnt;
};
}//namespace majong
#endif //MAHJONG_HPP