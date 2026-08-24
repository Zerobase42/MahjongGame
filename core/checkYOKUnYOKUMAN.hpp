#pragma once
#ifndef CHECKYOKUNYOKUMAN_HPP
#define CHECKYOKUNYOKUMAN_HPP
#include "mahjong.hpp"
namespace yaku{
    struct YakuContext{
        //머리
        mahjong::Tile head=0;
        //울어서 만든 몸통
        const mahjong::Meld*openMelds=nullptr;
        int openMeldCnt=0;
        //손패에서 DFS로 만든 몸통
        const mahjong::Meld*handMelds=nullptr;
        int handMeldCnt=0;
        //화료패
        mahjong::Tile winTile=0;
        //SELF/RON
        mahjong::WGet winGet=mahjong::WGet::SELF;
        //멘젠 여부
        bool menzen=true;
    };
    //============================================================
    //공통 몸통 순회
    //============================================================
    template<typename Func>
    bool allMeld(const YakuContext&ctx,Func func){
        for(int i=0;i<ctx.openMeldCnt;++i){
            if(!func(ctx.openMelds[i]))
                return false;
        }
        for(int i=0;i<ctx.handMeldCnt;++i){
            if(!func(ctx.handMelds[i]))
                return false;
        }
        return true;
    }
    template<typename Func>
    bool anyMeld(const YakuContext&ctx,Func func){
        for(int i=0;i<ctx.openMeldCnt;++i){
            if(func(ctx.openMelds[i]))
                return true;
        }
        for(int i=0;i<ctx.handMeldCnt;++i){
            if(func(ctx.handMelds[i]))
                return true;
        }
        return false;
    }
    //손패에서 DFS로 만든 몸통만 검사
    template<typename Func>
    bool allHandMeld(const YakuContext&ctx,Func func){
        for(int i=0;i<ctx.handMeldCnt;++i){
            if(!func(ctx.handMelds[i]))
                return false;
        }
        return true;
    }
    //============================================================
    //몸통 개수
    //============================================================
    inline int meldCnt(const YakuContext&ctx){
        return ctx.openMeldCnt+ctx.handMeldCnt;
    }
    //============================================================
    //슌쯔/커쯔 검사
    //============================================================
    inline bool isShun(const mahjong::Meld&meld){
        return meld.type==mahjong::MType::SHUN;
    }
    inline bool isKoutsu(const mahjong::Meld&meld){
        return meld.type==mahjong::MType::KOUT||
               meld.type==mahjong::MType::PON||
               meld.type==mahjong::MType::INK||
               meld.type==mahjong::MType::KAN||
               meld.type==mahjong::MType::SKN;
    }
    //============================================================
    //역패
    //============================================================
    inline bool isYakuhaiTile(mahjong::Tile tile,mahjong::Tile seatWind,mahjong::Tile roundWind){
        if(tile==mahjong::Wh||
            tile==mahjong::G||
            tile==mahjong::R)
            return true;
        if(tile==seatWind)
            return true;
        if(tile==roundWind)
            return true;
        return false;
    }
    inline bool hasYakuhai(const YakuContext&ctx,mahjong::Tile seatWind,mahjong::Tile roundWind){
        return anyMeld(ctx,[&](const mahjong::Meld&meld){
            return isKoutsu(meld)&&
                   isYakuhaiTile(
                       meld.tile,
                       seatWind,
                       roundWind);
        });
    }
    //============================================================
    //탕야오
    //============================================================
    inline bool isTanyao(const YakuContext&ctx){
        if(mahjong::isTerminalOrHonor(ctx.head))
            return false;
        return allMeld(ctx,[](const mahjong::Meld&meld){
            //슌쯔는 시작패만 검사해도
            //123/789를 구분해야 하므로
            //실제 mahjong.hpp의 함수에 맞춰 수정
            return !mahjong::isTerminalOrHonor(meld.tile);
        });
    }
    //============================================================
    //또이또이
    //============================================================
    inline bool isToitoi(const YakuContext&ctx){
        if(meldCnt(ctx)!=mahjong::PAIR_MAX-1)
            return false;
        return allMeld(ctx,[](const mahjong::Meld&meld){
            return isKoutsu(meld);
        });
    }
    //============================================================
    //이페코
    //============================================================
    inline bool isIipeiko(const YakuContext&ctx){
        //멘젠 한정
        if(!ctx.menzen)
            return false;
        //울은 몸통이 있으면 불가능
        if(ctx.openMeldCnt!=0)
            return false;
        for(int i=0;i<ctx.handMeldCnt;++i){
            if(!isShun(ctx.handMelds[i]))
                continue;
            for(int j=i+1;j<ctx.handMeldCnt;++j){
                if(!isShun(ctx.handMelds[j]))
                    continue;
                if(ctx.handMelds[i].tile==
                    ctx.handMelds[j].tile)
                    return true;
            }
        }
        return false;
    }
    //============================================================
    //량페코
    //============================================================
    inline bool isRyanpeikou(const YakuContext&ctx){
        if(!ctx.menzen)
            return false;
        if(ctx.openMeldCnt!=0)
            return false;
        int pairCnt=0;
        for(int i=0;i<ctx.handMeldCnt;++i){
            if(!isShun(ctx.handMelds[i]))
                continue;
            for(int j=i+1;j<ctx.handMeldCnt;++j){
                if(!isShun(ctx.handMelds[j]))
                    continue;
                if(ctx.handMelds[i].tile==
                    ctx.handMelds[j].tile)
                    ++pairCnt;
            }
        }
        return pairCnt==2;
    }
    //============================================================
    //핑후
    //============================================================
    inline bool isPinfu(const YakuContext&ctx){
        //멘젠 한정
        if(!ctx.menzen)
            return false;
        if(ctx.openMeldCnt!=0)
            return false;
        //모든 몸통 슌쯔
        if(!allHandMeld(ctx,[](const mahjong::Meld&meld){
                return isShun(meld);
            }))
            return false;
        //머리가 역패면 핑후 불가
        //실제 자풍/장풍은 YakuContext에 추가해서 검사하는 것을 권장
        if(ctx.head==mahjong::E||
            ctx.head==mahjong::S||
            ctx.head==mahjong::W||
            ctx.head==mahjong::N||
            ctx.head==mahjong::Wh||
            ctx.head==mahjong::G||
            ctx.head==mahjong::R)
            return false;
        //TODO:
        //winTile을 이용한 양면대기 판정
        if(!isRyanmenWait(ctx))
            return false;
        return true;
    }
    //============================================================
    //혼일색
    //============================================================
    inline bool isHonitsu(const YakuContext&ctx){
        int suit=-1;
        bool honor=false;
        auto check=[&](mahjong::Tile tile){
            if(mahjong::isHonor(tile)){
                honor=true;
                return true;
            }
            int s=mahjong::getSuit(tile);
            if(suit==-1){
                suit=s;
                return true;
            }
            return suit==s;
        };
        if(!check(ctx.head))
            return false;
        if(!allMeld(ctx,[&](const mahjong::Meld&meld){
                return check(meld.tile);
            }))
            return false;
        return honor;
    }
    //============================================================
    //청일색
    //============================================================
    inline bool isChinitsu(const YakuContext&ctx){
        int suit=-1;
        auto check=[&](mahjong::Tile tile){
            if(mahjong::isHonor(tile))
                return false;
            int s=mahjong::getSuit(tile);
            if(suit==-1){
                suit=s;
                return true;
            }
            return suit==s;
        };
        if(!check(ctx.head))
            return false;
        return allMeld(ctx,[&](const mahjong::Meld&meld){
            return check(meld.tile);
        });
    }
    //============================================================
    //YakuContext 생성
    //============================================================
    inline YakuContext makeYakuContext(
        const mahjong::Tile head,
        const mahjong::Meld*openMelds,
        int openMeldCnt,
        const mahjong::Meld*handMelds,
        int handMeldCnt,
        mahjong::Tile winTile,
        mahjong::WGet winGet,
        bool menzen){
        YakuContext ctx;
        ctx.head=head;
        ctx.openMelds=openMelds;
        ctx.openMeldCnt=openMeldCnt;
        ctx.handMelds=handMelds;
        ctx.handMeldCnt=handMeldCnt;
        ctx.winTile=winTile;
        ctx.winGet=winGet;
        ctx.menzen=menzen;
        return ctx;
    }
}
namespace yakuMan{
        //국사무쌍:19패(각 수트 1,9)+자패 7종을 모두 1장 이상,그 중 1종류는 2장(페어)보유
    bool isKokushi(
        const mahjong::Tile handCard[13],
        mahjong::Tile winTile){
        unsigned char cnt[mahjong::TILE_MAX]{};
        for(int i=0;i<13;++i){
            if(handCard[i]<mahjong::TILE_MAX)
                ++cnt[handCard[i]];
        }
        if(winTile>=mahjong::TILE_MAX)
            return false;
        ++cnt[winTile];
        constexpr mahjong::Tile terminals[]={
            mahjong::M1,mahjong::M9,
            mahjong::S1,mahjong::S9,
            mahjong::T1,mahjong::T9,
            mahjong::E,
            mahjong::S,
            mahjong::W,
            mahjong::N,
            mahjong::Wh,
            mahjong::G,
            mahjong::R
        };
        bool pair=false;
        for(mahjong::Tile tile:terminals){
            if(cnt[tile]==0)
                return false;
            if(cnt[tile]>=2)
                pair=true;
        }
        return pair;
    }
    //구련보등:한 수트로만 구성+1112345678999 형태+아무 패나 1장 추가
    bool isChuuren(const mahjong::Tile handCard[mahjong::HAND_MAX]){
        auto suitOf=[](mahjong::Tile t)->int {
            if(t>=mahjong::M1&&t<=mahjong::M9)return 0;
            if(t>=mahjong::S1&&t<=mahjong::S9)return 1;
            if(t>=mahjong::T1&&t<=mahjong::T9)return 2;
            return-1;//자패->구련보등 불가
        };
        static const mahjong::Tile suitBase[3]={
            mahjong::MAHSKII::M1,mahjong::MAHSKII::S1,mahjong::MAHSKII::T1
        };
        const int suit=suitOf(handCard[0]);
        if(suit==-1)return false;
        int cnt[9]={0};
        for(int i=0;i<mahjong::HAND_MAX;i++){
            if(suitOf(handCard[i])!=suit)return false;//혼일색이 아니면 불가
            int n=handCard[i]-suitBase[suit];            //0~8(1~9)
            cnt[n]++;
        }
        for(int n=0;n<9;n++){
            int need=(n==0||n==8)?3:1;//1,9는 최소 3장,2~8은 최소 1장
            if(cnt[n]<need)return false;
        }
        return true;//남는 1장은 자연히 텐파이 확장패로 처리됨
    }
}
#endif//CHECKYOKUNYOKUMAN_HPP