#pragma once
#ifndef CALCSCORE_HPP
#define CALCSCORE_HPP
#include "mahjong.hpp"
#include "winTypes.hpp"
//몸통 분해 결과(WinInfo)하나를 놓고 판수(calcHan)/부수(calcFu)를 계산하는
//헤더. winChecker::findBestWin이 find_dfs로 찾아낸 여러 후보 분해 중 어떤
//것이 가장 점수가 높은지 비교할 때 이 함수들을 사용한다.
namespace winChecker{
    //----타일 판별용 자잘한 헬퍼-------------------------------------------
    namespace detail{
        inline bool isHonor(mahjong::Tile t){
            return t==mahjong::MAHSKII::E||t==mahjong::MAHSKII::S||
                t==mahjong::MAHSKII::W||t==mahjong::MAHSKII::N||
                t==mahjong::MAHSKII::Wh||t==mahjong::MAHSKII::G||
                t==mahjong::MAHSKII::R;
        }
        inline bool isTerminal(mahjong::Tile t){
            return t==mahjong::MAHSKII::M1||t==mahjong::MAHSKII::M9||
                t==mahjong::MAHSKII::S1||t==mahjong::MAHSKII::S9||
                t==mahjong::MAHSKII::T1||t==mahjong::MAHSKII::T9;
        }
        inline bool isTerminalOrHonor(mahjong::Tile t){
            return isHonor(t)||isTerminal(t);
        }
        //0=만수,1=삭수,2=통수,-1=자패
        inline int suitOf(mahjong::Tile t){
            if(t>=mahjong::MAHSKII::M1&&t<=mahjong::MAHSKII::M9)return 0;
            if(t>=mahjong::MAHSKII::S1&&t<=mahjong::MAHSKII::S9)return 1;
            if(t>=mahjong::MAHSKII::T1&&t<=mahjong::MAHSKII::T9)return 2;
            return-1;
        }
        //숫자패의 시작값(해당 수트의 1)-몸통 카드값에서 빼면 0~8(1~9)이 된다
        inline mahjong::Tile suitBase(int suit){
            static const mahjong::Tile base[3]={
                mahjong::MAHSKII::M1,mahjong::MAHSKII::S1,mahjong::MAHSKII::T1
            };
            return base[suit];
        }
        inline int honorIndex(mahjong::Tile t){
            switch(t){
                case mahjong::MAHSKII::E:
                    return 0;
                case mahjong::MAHSKII::S:
                    return 1;
                case mahjong::MAHSKII::W:
                    return 2;
                case mahjong::MAHSKII::N:
                    return 3;
                case mahjong::MAHSKII::Wh:
                    return 4;
                case mahjong::MAHSKII::G:
                    return 5;
                case mahjong::MAHSKII::R:
                    return 6;
            }
            return-1;
        }
        inline bool isDragon(mahjong::Tile t){
            return t==mahjong::MAHSKII::Wh||t==mahjong::MAHSKII::G||
                t==mahjong::MAHSKII::R;
        }
        //커쯔/깡쯔 계열(치퐁 없이 스스로 모은 것)인지
        inline bool isTripletType(mahjong::MType m){
            return m==mahjong::MType::KOUT||m==mahjong::MType::KAN||
                m==mahjong::MType::INK||m==mahjong::MType::SKN;
        }
        //암커/암깡(부르지 않고 스스로 만든 몸통)인지. WGet::SELF=쯔모로
        //채우거나 안깡한 경우이므로 암(暗)으로 취급한다.
        inline bool isConcealedTriplet(const mahjong::Meld&m){
            return isTripletType(m.mType)&&m.wGet==mahjong::WGet::SELF;
        }
    }//namespace detail
    //----판수 계산---------------------------------------------------------
    //WinInfo(머리+몸통 4개)를 보고 판별 가능한 역들을 계산해 info.yaku
    //비트마스크에 채워 넣고,총 판수를 반환한다.
    //주의:리치/일발/영상개화/해저/하저/창깡처럼 그 판(局)의 진행 상황이
    //있어야 아는 역들은 손패 모양만으로는 알 수 없으므로 이 함수 범위 밖이다
    //(필요하면 별도 플래그를 인자로 추가해서 처리).
    inline int calcHan(WinInfo&info,bool tsumo,bool menzen){
        using namespace detail;
        unsigned int yaku=0;
        int han=0;
        auto addYaku=[&](YokuMask mask,int openHan,int menzenHan){
            han+=menzen?menzenHan:openHan;
            yaku|=static_cast<unsigned int>(mask);
        };
        const int meldCnt=info.meldCnt;
        if(meldCnt!=mahjong::PAIR_MAX-1){
            //몸통 4개가 갖춰지지 않은 분해(비정상 입력)는 역 없음 처리
            info.yaku=0;
            return 0;
        }
        int shunCnt=0,koutCnt=0,concealedKoutCnt=0;
        bool allSimple=!isTerminalOrHonor(info.head);
        bool allTerminalOrHonor=isTerminalOrHonor(info.head);
        bool everyMeldHasTerminalOrHonor=isTerminalOrHonor(info.head);
        bool everyMeldHasNoHonor=!isHonor(info.head);//준찬타 판정용(자패 섞이면 안 됨)
        int suitSeen=suitOf(info.head);//-1 이면 아직 없음(자패였음)
        bool singleSuit=true;
        bool anyHonorTile=isHonor(info.head);
        bool shunStart[3][7]={};     //[suit][번호(0=1~..6=7시작)]
        int shunStartCount[3][7]={};//같은 슌쯔가 몇 번 나왔는지(이페코/량페코용)
        int koutNumber[3][9]={};     //[suit][번호(0=1~8=9)]커쯔 개수
        int koutHonorCount[7]={};    //자패별 커쯔 개수(E,S,W,N,Wh,G,R)
        for(int i=0;i<meldCnt;i++){
            const mahjong::Meld&m=info.melds[i];
            const mahjong::Tile t=m.card;
            const int suit=suitOf(t);
            if(suit==-1)anyHonorTile=true;
            if(suit!=-1){
                if(suitSeen==-1)
                    suitSeen=suit;
                else if(suitSeen!=suit)
                    singleSuit=false;
            }
            if(m.mType==mahjong::MType::SHUN){
                shunCnt++;
                bool hasTermHere=isTerminalOrHonor(t)||isTerminalOrHonor(t+2);
                if(hasTermHere){
                    allSimple=false;
                }else{
                    everyMeldHasTerminalOrHonor=false;
                }
                allTerminalOrHonor=false;//슌쯔가 있으면 혼노두/자일색류 불가
                if(suit!=-1){
                    int num=t-suitBase(suit);//0~6(1~7 시작)
                    if(num>=0&&num<7){
                        shunStart[suit][num]=true;
                        shunStartCount[suit][num]++;
                    }
                }
            }else{//KOUT/KAN/INK/SKN->커쯔/깡쯔
                koutCnt++;
                if(isTerminalOrHonor(t)){
                    //혼노두/자일색 후보 유지,준찬타 조건 갱신
                    if(isHonor(t))everyMeldHasNoHonor=false;
                }else{
                    allTerminalOrHonor=false;
                    everyMeldHasTerminalOrHonor=false;
                }
                if(isTerminalOrHonor(t)){
                    /*allSimple 유지 처리 아래에서*/
                }
                if(isHonor(t)||isTerminal(t))allSimple=false;
                if(isConcealedTriplet(m))concealedKoutCnt++;
                if(suit!=-1){
                    int num=t-suitBase(suit);//0~8(1~9)
                    koutNumber[suit][num]++;
                }else{
                    int hi=honorIndex(t);
                    if(hi>=0)koutHonorCount[hi]++;
                }
            }
        }
        //----각 역 판정-----------------------------------------------------
        //멘젠쯔모
        if(tsumo&&menzen){
            addYaku(YokuMask::MENZENTSUMO,0,1);
        }
        //탕야오:머리/몸통 전부 2~8 숫자패
        if(allSimple){
            addYaku(YokuMask::TANYAO,1,1);
        }
        //핑후:전부 슌쯔+머리가 자패(용/바람)가 아님(멘젠 한정)
        if(menzen&&shunCnt==4&&!isDragon(info.head)&&
            !(isHonor(info.head))){
            addYaku(YokuMask::PINGHU,0,1);
        }
        //이페코/량페코(멘젠 한정):완전히 같은 슌쯔가 2개(이페코)또는
        //서로 다른 짝으로 2세트(량페코)
        if(menzen){
            int pairSets=0;//짝을 이룬 슌쯔 세트 수
            for(int s=0;s<3;s++)
                for(int n=0;n<7;n++)
                    if(shunStartCount[s][n]>=2)pairSets++;
            if(pairSets>=2){
                addYaku(YokuMask::RYANPEIKOU,0,3);
            }else if(pairSets==1){
                addYaku(YokuMask::IPEKO,0,1);
            }
        }
        //삼색동순:같은 번호의 슌쯔가 만/삭/통 세 수트에 모두 존재
        for(int n=0;n<7;n++){
            if(shunStart[0][n]&&shunStart[1][n]&&shunStart[2][n]){
                addYaku(YokuMask::SANSHOKUDOUJUN,1,2);
                break;
            }
        }
        //삼색동각:같은 번호의 커쯔가 세 수트에 모두 존재
        for(int n=0;n<9;n++){
            if(koutNumber[0][n]&&koutNumber[1][n]&&koutNumber[2][n]){
                addYaku(YokuMask::SANSHOKUDOUKOU,2,2);
                break;
            }
        }
        //일기통관:한 수트에서 123/456/789 슌쯔가 모두 존재
        for(int s=0;s<3;s++){
            if(shunStart[s][0]&&shunStart[s][3]&&shunStart[s][6]){
                addYaku(YokuMask::IKKITSUKAN,1,2);
                break;
            }
        }
        //찬타/준찬타:머리+몸통 전부 터미널/자패를 하나씩 포함
        if(everyMeldHasTerminalOrHonor){
            if(everyMeldHasNoHonor){
                addYaku(YokuMask::JUNCHANTA,2,3);
            }else{
                addYaku(YokuMask::CHANTA,1,2);
            }
        }
        //혼노두:머리+몸통 전부 터미널/자패이면서 전부 커쯔(슌쯔 없음)
        if(allTerminalOrHonor&&shunCnt==0){
            addYaku(YokuMask::HONROUTOU,2,2);
        }
        //또이또이:전부 커쯔/깡쯔
        if(koutCnt==4){
            addYaku(YokuMask::TOITOIHOU,2,2);
        }
        //삼안커:암커/암깡 3개 이상
        if(concealedKoutCnt>=3){
            addYaku(YokuMask::SANANKOU,2,2);
        }
        //소삼원:용패 두 종류는 커쯔,나머지 한 종류는 머리
        {
            int dragonKoutCnt=0;
            for(int hi=4;hi<=6;hi++)
                if(koutHonorCount[hi])dragonKoutCnt++;
            bool headIsDragon=isDragon(info.head);
            if(dragonKoutCnt==2&&headIsDragon){
                addYaku(YokuMask::SHOUSANGEN,2,2);
            }
        }
        //혼일색/청일색:한 수트+(자패 있으면 혼일색,없으면 청일색)
        if(singleSuit&&suitSeen!=-1){
            if(anyHonorTile){
                addYaku(YokuMask::HONITSU,2,3);
            }else{
                addYaku(YokuMask::CHINITSU,5,6);
            }
        }
        info.yaku=yaku;
        return han;
    }
    //----부수 계산----------------------------------------------------------
    //TODO:커쯔/깡쯔(명/암,노두/중장)가산,머리(역패)가산,
    //       대기 형태(변짜/간짜/단기)가산,쯔모/론 가산 등 구현.
    //지금은 자리만 잡아둔 스텁(기본 20부 고정)이며,calcScore가 이 값을
    //그대로 후보 비교에 사용한다.
    inline int calcFu(const WinInfo&info,bool tsumo,bool menzen){
        int fu=20;//기본 20부
        (void)info;
        (void)tsumo;
        (void)menzen;
        return fu;
    }
    //비교용 스코어:판을 부보다 우선하도록 가중치를 둠.
    //(실제 점수표 환산은 calcFu가 채워진 뒤 별도로 구현)
    inline int calcScore(WinInfo&info,bool tsumo,bool menzen){
        int han=calcHan(info,tsumo,menzen);
        int fu=calcFu(info,tsumo,menzen);
        return han*10000+fu;
    }
}//namespace winChecker
#endif //CALCSCORE_HPP