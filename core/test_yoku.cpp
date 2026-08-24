//===========================================================================
//마작 역 판별 로직 테스트용 코드
//
//빌드:
//  g++-std=c++17-Wall-Wextra-o test_yoku test_yoku.cpp
//실행:
//  ./test_yoku
//
//외부 테스트 프레임워크 없이 간단한 assert 스타일로 작성했습니다.
//각 TEST_CASE는 실패해도 프로그램이 멈추지 않고 다음 케이스로 넘어가며,
//마지막에 통과/실패 개수를 요약해서 보여줍니다.
//
//[변경사항]isChiitoitsu/isKokushi/findBestWin/isWin이 화료패(winTile)를
//손패(13장)와 분리해서 받는 형태로 바뀌었습니다. 이 테스트 코드도 그에 맞춰
//"13장 손패 + 화료패" 형태로 손패를 나눠 지정하도록 수정했고,화료패 지정에
//따라 결과가 달라지는 케이스(대기 형태,13면대기,잘못된 화료패 등)를
//추가했습니다.
//===========================================================================
#include<windows.h>
#include<cstdio>
#include<cstring>
#include<string>
#include "checkYOKUnYOKUMAN.hpp"
#include "winChecker.hpp"
#define SET_CODE_KO SetConsoleCP(CP_UTF8),SetConsoleOutputCP(CP_UTF8)
//---------------------------------------------------------------------------
//아주 작은 테스트 유틸
//---------------------------------------------------------------------------
static int g_pass=0;
static int g_fail=0;
#define CHECK(cond,msg)                                       \
    do{                                                       \
        if(cond){                                            \
            ++g_pass;                                          \
            printf("  [PASS] %s\n",msg);                      \
        }else{                                               \
            ++g_fail;                                          \
            printf("  [FAIL] %s  (line %d)\n",msg,__LINE__);\
        }                                                      \
    }while(0)
#define SECTION(title)printf("\n== %s ==\n",title)
//역 비트마스크를 사람이 읽을 수 있는 이름들로 출력
static void printYaku(unsigned int yaku){
    struct Entry{
        winChecker::YokuMask mask;
        const char*name;
    };
    static const Entry table[]={
        {winChecker::YokuMask::LICHI,"lichi"},
        {winChecker::YokuMask::IPPATSU,"ippatsu"},
        {winChecker::YokuMask::PINGHU,"pinfu"},
        {winChecker::YokuMask::TANYAO,"tanyao"},
        {winChecker::YokuMask::IPEKO,"iipeikou"},
        {winChecker::YokuMask::MENZENTSUMO,"menzen_tsumo"},
        {winChecker::YokuMask::CHITOITSU,"chiitoitsu"},
        {winChecker::YokuMask::IKKITSUKAN,"ittsuu"},
        {winChecker::YokuMask::SANSHOKUDOUJUN,"sanshoku_doujun"},
        {winChecker::YokuMask::SANSHOKUDOUKOU,"sanshoku_doukou"},
        {winChecker::YokuMask::CHANTA,"chanta"},
        {winChecker::YokuMask::HONROUTOU,"honroutou"},
        {winChecker::YokuMask::SHOUSANGEN,"shousangen"},
        {winChecker::YokuMask::TOITOIHOU,"toitoi"},
        {winChecker::YokuMask::SANANKOU,"sananko"},
        {winChecker::YokuMask::HONITSU,"honitsu"},
        {winChecker::YokuMask::JUNCHANTA,"junchanta"},
        {winChecker::YokuMask::RYANPEIKOU,"ryanpeikou"},
        {winChecker::YokuMask::CHINITSU,"chinitsu"},
    };
    bool any=false;
    for(auto&e:table){
        if(yaku&static_cast<unsigned int>(e.mask)){
            printf("%s%s",any?", ":"",e.name);
            any=true;
        }
    }
    if(!any)printf("(none)");
}
static bool has(unsigned int yaku,winChecker::YokuMask m){
    return(yaku&static_cast<unsigned int>(m))!=0;
}
//손패(13장)+화료패 1장을 findBestWin으로 분석하고,결과 요약을 출력한 뒤
//WinInfo를 반환한다.
//(winMeld도 함께 출력:-2=머리로 화료,-1=분해 실패,0~3=해당 몸통으로 화료)
static winChecker::WinInfo analyze(const char*label,
                                   const mahjong::Tile hand[13],
                                   mahjong::Tile winTile,
                                   bool tsumo=true,bool menzen=true){
    auto best=winChecker::findBestWin(hand,winTile,tsumo,menzen);
    printf("[%s] meldCnt=%d head=%d winMeld=%d yaku=",
           label,best.meldCnt,best.head,best.winMeld);
    printYaku(best.yaku);
    printf("\n");
    return best;
}
//---------------------------------------------------------------------------
//손패를 손으로 나열하기 편하게 해주는 매크로
//(정렬 여부는 상관없음:find_dfs 등은 카운트 기반이라 무관하지만,
// isChiitoitsu는 카운트 기반으로 바뀌었으므로 순서는 더 이상 중요하지 않음)
//---------------------------------------------------------------------------
using mahjong::Tile;
using namespace mahjong;//M1..M9,S1..S9,T1..T9,E,S,W,N,Wh,G,R(unscoped enum이라 mahjong 네임스페이스에 바로 들어있음)
//TILE_MAX(34)이상인,명백히 잘못된 화료패 값
static constexpr Tile INVALID_TILE=255;
int main(){
    SET_CODE_KO;
    //-----------------------------------------------------------------
    SECTION("치또이츠 / 국사무쌍 / 구련보등 기본 판별");
    {
        //13장+화료패(마지막 페어를 완성시키는 패)
        Tile chiitoi[13]={
            M1,M1,M3,M3,M5,M5,M7,M7,S2,S2,S4,S4,E //6쌍+외톨이 E
        };
        CHECK(yoku::isChiitoitsu(chiitoi,E),"화료패(E)로 마지막 쌍을 채우면 치또이츠(7쌍) 판정");
        //같은 13장인데 화료패가 페어를 완성시키지 못하면(S)치또이츠 아님
        CHECK(!yoku::isChiitoitsu(chiitoi,S),"화료패가 짝을 못 채우면(다른 패) 치또이츠 아님");
        //화료패가 유효 범위를 벗어나면 무조건 실패해야 함
        CHECK(!yoku::isChiitoitsu(chiitoi,INVALID_TILE),"화료패 값이 잘못되면 치또이츠 판정 실패");
        //이미 손패에 같은 패가 3장 있는 상태에서 화료패로 그 패를 또 뽑으면
        //(쿠사낑)그 패는 4장(깡)이 되어 짝이 아니므로 치또이츠가 깨져야 함
        Tile almostQuad[13]={
            M1,M1,M3,M3,M5,M5,M5,S2,S2,S4,S4,E,E //M5가 이미 3장
        };
        CHECK(!yoku::isChiitoitsu(almostQuad,M5),"화료패로 같은 패의 4번째 장을 뽑으면(깡) 치또이츠 아님");
        //13면대기 국사무쌍:13종 요구패를 1장씩 들고 있는 상태(어떤 요구패로도 화료 가능)
        Tile kokushi13[13]={
            M1,M9,S1,S9,T1,T9,E,S,W,N,Wh,G,R //13종 각 1장
        };
        CHECK(yokuMan::isKokushi(kokushi13,M1),"국사무쌍 13면대기: 화료패=M1이어도 국사무쌍");
        CHECK(yokuMan::isKokushi(kokushi13,R),"국사무쌍 13면대기: 화료패=R이어도 국사무쌍(다른 패로도 화료 가능)");
        //요구패가 아닌 패로는 국사무쌍이 될 수 없음(13종은 다 있어도 짝이 안 생김)
        CHECK(!yokuMan::isKokushi(kokushi13,M2),"화료패가 요구패가 아니면(M2) 국사무쌍 아님");
        //화료패 값이 잘못된 경우
        CHECK(!yokuMan::isKokushi(kokushi13,INVALID_TILE),"화료패 값이 잘못되면 국사무쌍 판정 실패");
        //구련보등은 아직 화료패를 따로 받지 않으므로(헤더 미변경)기존처럼 14장으로 확인
        Tile chuuren[mahjong::HAND_MAX]={
            M1,M1,M1,M2,M3,M4,M5,M6,M7,M8,M9,M9,M9,M5 //1112345678999+5
        };
        CHECK(yokuMan::isChuuren(chuuren),"구련보등 기본형 판정");
        Tile notChuuren[mahjong::HAND_MAX]={
            M1,M1,M1,M2,M3,M4,M5,M6,M7,M8,M9,M9,M9,S1 //혼일색 깨짐
        };
        CHECK(!yokuMan::isChuuren(notChuuren),"타 수트 섞이면 구련보등 아님");
    }
    //-----------------------------------------------------------------
    SECTION("몸통 탐색 (find_dfs / findBestWin) 기본 동작");
    {
        //123m 456s 789t 456m+7m(화료패로 7m 페어 완성)(전부 슌쯔)
        Tile hand[13]={
            M1,M2,M3,S4,S5,S6,T7,T8,T9,M4,M5,M6,M7
        };
        auto best=analyze("일반형 4슌쯔+머리",hand,/*winTile=*/M7);
        CHECK(best.meldCnt==4,"몸통 4개를 모두 채워 유효한 분해를 찾음");
        CHECK(best.head==M7,"머리로 7m 페어를 선택함");
        CHECK(best.winMeld==-2,"화료패가 머리에서 완성되면 winMeld == -2");
        //몸통 분해가 불가능한 엉터리 손패(4-3-3-2-1 같은 형태)
        Tile impossible[13]={
            M1,M2,M4,S1,S3,S5,T1,T4,T7,M9,S9,T9,E
        };
        auto badBest=winChecker::findBestWin(impossible,/*winTile=*/S,true,true);
        CHECK(badBest.meldCnt!=4,"분해 불가능한 손패는 4몸통을 채우지 못함");
        //화료패가 아예 유효 범위를 벗어나면 빈 WinInfo(meldCnt==0)가 반환되어야 함
        auto invalidWin=winChecker::findBestWin(hand,INVALID_TILE,true,true);
        CHECK(invalidWin.meldCnt==0,"화료패 값이 잘못되면 findBestWin이 빈 결과를 반환");
    }
    //-----------------------------------------------------------------
    SECTION("화료패 지정에 따른 대기 형태(샨퐁) 결과 차이");
    {
        //234m 456s 567t+8m8m+2t2t(13장):슌쯔 3개+페어 2개인 "샨퐁 대기"
        //화료패로 8m을 뽑으면 8m이 커쯔가 되고 2t가 머리,반대로 2t를 뽑으면
        //그 반대가 되어야 한다. 즉 같은 13장이라도 화료패에 따라 몸통 분해가
        //달라짐을 확인한다.
        Tile shanpon[13]={
            M2,M3,M4,S4,S5,S6,T5,T6,T7,M8,M8,T2,T2
        };
        auto winByM8=analyze("샨퐁 대기, 화료패=8m",shanpon,/*winTile=*/M8);
        CHECK(winByM8.meldCnt==4,"8m으로 화료 시 몸통 4개 완성");
        CHECK(winByM8.head==T2,"8m으로 화료하면 2t가 머리가 됨");
        auto winByT2=analyze("샨퐁 대기, 화료패=2t",shanpon,/*winTile=*/T2);
        CHECK(winByT2.meldCnt==4,"2t로 화료 시 몸통 4개 완성");
        CHECK(winByT2.head==M8,"2t로 화료하면 8m이 머리가 됨");
        //두 화료패 모두 또이또이는 아님(슌쯔 3개가 남아있으므로)
        CHECK(!has(winByM8.yaku,winChecker::YokuMask::TOITOIHOU),
              "샨퐁이라도 슌쯔가 섞여있으면 또이또이 아님");
    }
    //-----------------------------------------------------------------
    SECTION("탕야오 / 핑후 / 멘젠쯔모");
    {
        //234m 456s 567t 234t+8m(화료패로 88m 완성)(전부 2~8 슌쯔,자패/터미널 없음)
        Tile hand[13]={
            M2,M3,M4,S4,S5,S6,T5,T6,T7,T2,T3,T4,M8
        };
        auto best=analyze("탕야오+핑후+멘젠쯔모",hand,/*winTile=*/M8,/*tsumo=*/true,/*menzen=*/true);
        CHECK(has(best.yaku,winChecker::YokuMask::TANYAO),"탕야오 인식");
        CHECK(has(best.yaku,winChecker::YokuMask::PINGHU),"핑후 인식");
        CHECK(has(best.yaku,winChecker::YokuMask::MENZENTSUMO),"멘젠쯔모 인식");
        //같은 손패/화료패인데 론(tsumo=false)이면 멘젠쯔모는 빠져야 함
        auto ron=winChecker::findBestWin(hand,/*winTile=*/M8,/*tsumo=*/false,/*menzen=*/true);
        CHECK(!has(ron.yaku,winChecker::YokuMask::MENZENTSUMO),"론이면 멘젠쯔모 제외");
        //자패 머리(핑후 조건 위반)면 핑후가 빠져야 함
        Tile withHonorHead[13]={
            M2,M3,M4,S4,S5,S6,T5,T6,T7,T2,T3,T4,E
        };
        auto noPinfu=analyze("자패 머리라 핑후 아님",withHonorHead,/*winTile=*/E);
        CHECK(!has(noPinfu.yaku,winChecker::YokuMask::PINGHU),"자패 머리면 핑후 제외");
    }
    //-----------------------------------------------------------------
    SECTION("또이또이 / 삼안커");
    {
        //222m 555s 777t 999t+3m(화료패로 33m 완성)(전부 커쯔)
        Tile hand[13]={
            M2,M2,M2,S5,S5,S5,T7,T7,T7,T9,T9,T9,M3
        };
        auto best=analyze("또이또이+삼안커",hand,/*winTile=*/M3);
        CHECK(has(best.yaku,winChecker::YokuMask::TOITOIHOU),"또이또이 인식");
        CHECK(has(best.yaku,winChecker::YokuMask::SANANKOU),"삼안커(암커 3개+) 인식");
    }
    //-----------------------------------------------------------------
    SECTION("혼일색 / 청일색 / 일기통관");
    {
        //123m 456m 789m EEE+9m(화료패로 99m 완성)(만수+자패->혼일색,123/456/789->일기통관)
        Tile honitsu[13]={
            M1,M2,M3,M4,M5,M6,M7,M8,M9,E,E,E,M9
        };
        auto h=analyze("혼일색+일기통관",honitsu,/*winTile=*/M9);
        CHECK(has(h.yaku,winChecker::YokuMask::HONITSU),"혼일색 인식");
        CHECK(has(h.yaku,winChecker::YokuMask::IKKITSUKAN),"일기통관 인식");
        CHECK(!has(h.yaku,winChecker::YokuMask::CHINITSU),"자패 섞였으므로 청일색 아님");
        //123m 456m 789m 234m+5m(화료패로 55m 완성)(만수만 사용->청일색)
        Tile chinitsu[13]={
            M1,M2,M3,M4,M5,M6,M7,M8,M9,M2,M3,M4,M5
        };
        auto c=analyze("청일색",chinitsu,/*winTile=*/M5);
        CHECK(has(c.yaku,winChecker::YokuMask::CHINITSU),"청일색 인식");
        CHECK(!has(c.yaku,winChecker::YokuMask::HONITSU),"청일색이면 혼일색과 중복 표시 안 함");
    }
    //-----------------------------------------------------------------
    SECTION("삼색동순 / 삼색동각");
    {
        //123m 123s 123t 456m+7m(화료패로 77m 완성)(세 수트 모두 123 슌쯔)
        Tile sanshokuShun[13]={
            M1,M2,M3,S1,S2,S3,T1,T2,T3,M4,M5,M6,M7
        };
        auto s1=analyze("삼색동순",sanshokuShun,/*winTile=*/M7);
        CHECK(has(s1.yaku,winChecker::YokuMask::SANSHOKUDOUJUN),"삼색동순 인식");
        //555m 555s 555t 123m+7m(화료패로 77m 완성)(세 수트 모두 5 커쯔)
        Tile sanshokuKou[13]={
            M5,M5,M5,S5,S5,S5,T5,T5,T5,M1,M2,M3,M7
        };
        auto s2=analyze("삼색동각",sanshokuKou,/*winTile=*/M7);
        CHECK(has(s2.yaku,winChecker::YokuMask::SANSHOKUDOUKOU),"삼색동각 인식");
    }
    //-----------------------------------------------------------------
    SECTION("찬타 / 준찬타 / 혼노두");
    {
        //123m 789s 123t 999m+1t(화료패로 11t 완성)머리(모든 몸통+머리에 터미널 포함,자패는 없음)
        Tile junchanta[13]={
            M1,M2,M3,S7,S8,S9,T1,T2,T3,M9,M9,M9,T1
        };
        auto j=analyze("준찬타(자패 없이 터미널만)",junchanta,/*winTile=*/T1);
        CHECK(has(j.yaku,winChecker::YokuMask::JUNCHANTA),"준찬타 인식");
        CHECK(!has(j.yaku,winChecker::YokuMask::CHANTA),"준찬타면 찬타 중복 표시 안 함");
        //123m 789s EEE 999m+W(화료패로 WW 완성)머리(자패가 섞인 터미널/자패형->찬타)
        Tile chanta[13]={
            M1,M2,M3,S7,S8,S9,E,E,E,M9,M9,M9,W
        };
        auto c=analyze("찬타(자패 포함)",chanta,/*winTile=*/W);
        CHECK(has(c.yaku,winChecker::YokuMask::CHANTA),"찬타 인식");
        CHECK(!has(c.yaku,winChecker::YokuMask::JUNCHANTA),"자패 섞이면 준찬타 아님");
        //111m 999s EEE WWW+9t(화료패로 99t 완성)머리(전부 터미널/자패 커쯔->혼노두)
        Tile honroutou[13]={
            M1,M1,M1,S9,S9,S9,E,E,E,W,W,W,T9
        };
        auto hr=analyze("혼노두",honroutou,/*winTile=*/T9);
        CHECK(has(hr.yaku,winChecker::YokuMask::HONROUTOU),"혼노두 인식");
        CHECK(has(hr.yaku,winChecker::YokuMask::TOITOIHOU),"혼노두는 전부 커쯔이므로 또이또이도 동반");
    }
    //-----------------------------------------------------------------
    SECTION("소삼원");
    {
        //WhWhWh GGG 123m 456s+R(화료패로 RR 완성)머리(용패 2종 커쯔+용패 머리)
        Tile shousangen[13]={
            Wh,Wh,Wh,G,G,G,M1,M2,M3,S4,S5,S6,R
        };
        auto s=analyze("소삼원",shousangen,/*winTile=*/R);
        CHECK(has(s.yaku,winChecker::YokuMask::SHOUSANGEN),"소삼원 인식");
    }
    //-----------------------------------------------------------------
    SECTION("이페코 / 량페코 (멘젠 한정)");
    {
        //112233m 456s 789t+9t(화료패로 T789 슌쯔 완성 후 99t 머리)(11 22 33m 슌쯔가 완전히 겹침->이페코 1세트)
        Tile iipeikou[13]={
            M1,M2,M3,M1,M2,M3,S4,S5,S6,T7,T8,T9,T9
        };
        auto i=analyze("이페코",iipeikou,/*winTile=*/T9);
        CHECK(has(i.yaku,winChecker::YokuMask::IPEKO),"이페코 인식");
        CHECK(!has(i.yaku,winChecker::YokuMask::RYANPEIKOU),"이페코 1세트만 있으면 량페코 아님");
        //112233m 112233s+7p(화료패로 77p 완성)(슌쯔 짝이 두 세트->량페코)
        Tile ryanpeikou[13]={
            M1,M2,M3,M1,M2,M3,S1,S2,S3,S1,S2,S3,T7
        };
        auto r=analyze("량페코",ryanpeikou,/*winTile=*/T7);
        CHECK(has(r.yaku,winChecker::YokuMask::RYANPEIKOU),"량페코 인식");
        CHECK(!has(r.yaku,winChecker::YokuMask::IPEKO),"량페코면 이페코와 중복 표시 안 함");
    }
    //-----------------------------------------------------------------
    SECTION("isWin() 종합 (치또이츠/국사무쌍 + 일반형 스코어 마스크 병합 확인)");
    {
        Tile chiitoi[13]={
            M1,M1,M3,M3,M5,M5,M7,M7,S2,S2,S4,S4,E
        };
        unsigned int mask=winChecker::isWin(chiitoi,/*winTile=*/E);
        CHECK(mask&static_cast<unsigned int>(winChecker::YokuMask::CHITOITSU),
              "isWin()이 치또이츠 비트를 포함");
        Tile normal[13]={
            M2,M3,M4,S4,S5,S6,T5,T6,T7,T2,T3,T4,M8
        };
        unsigned int normalMask=winChecker::isWin(normal,/*winTile=*/M8,true,true);
        CHECK((normalMask&static_cast<unsigned int>(winChecker::YokuMask::TANYAO))!=0,
              "isWin()이 일반형 분해에서 나온 탕야오 비트도 포함");
        //화료패가 잘못되면(범위 밖)어떤 역도 성립하지 않아야 함(0 반환)
        unsigned int invalidMask=winChecker::isWin(normal,INVALID_TILE,true,true);
        CHECK(invalidMask==0,"isWin()은 화료패 값이 잘못되면 0을 반환");
    }
    /*
    SECTION("자체 생성 테케");
    {
        Tile aa[13]={
            M1,M1,M1,M1,M2,M3,M4,M4,M4,M4,T9,T9,T9
        };
        CHECK()
    }
    */
    //-----------------------------------------------------------------
    printf("\n===========================================\n");
    printf("결과: %d개 통과 / %d개 실패 (총 %d개)\n",g_pass,g_fail,g_pass+g_fail);
    printf("===========================================\n");
    return g_fail==0?0:1;
}