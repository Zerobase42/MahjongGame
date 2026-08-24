#pragma once
#ifndef PLAYER_HPP
#define PLAYER_HPP
#include<algorithm>//sort
#include "checkYOKUnYOKUMAN.hpp"
#include "mahjong.hpp"
class Player{
   public://기본 선언 값
    mahjong::Hand handCard;
    mahjong::Tile winTile{};//대기->화료패
    bool tsumo=false;      //쯔모/론
    bool menzen=true;      //멘젠 확인->치퐁깡 넣을때 false로 바꾸기
    unsigned int YOKU=0;    //가능한 역 비트마스킹
    unsigned int YOKUMAN=0;//가능한 역만 비트마스킹
   public:
    Player()=default;
    ~Player()=default;
    /*
    void setHand(const mahjong::Hand&hand);
    const mahjong::Hand&getHand()const;
    void setScore(int score);
    int getScore()const;
    */
   public:
    void popCard(mahjong::MAHSKII card){//마작 패 버리기
        for(int i=0;i<mahjong::HAND_MAX;i++){
            if(handCard.card[i]==card){
                for(int j=i;j<mahjong::HAND_MAX-1;j++)
                    handCard.card[j]=handCard.card[j+1];
                handCard.card[mahjong::HAND_MAX-1]=255;
                mahjong::PrioritySort(handCard.card);
                return;
            }
        }
    }
    void inpCard(mahjong::MAHSKII card){//마작 패 받기
        handCard.card[mahjong::HAND_MAX-1]=card;
        mahjong::PrioritySort(handCard.card);
    }
   private:
    void calcYoku()const{
        if(menzen){
            ;
        }
    }
    int calcFu()const{
        int score=20;//기본 20부 시작
        //기저사례 처리
        if(tsumo&&yaku::isPinfu(handCard.card))//핑후면 20부 고정
            return 20;
        if(yaku::isChiitoitsu(handCard.card))//치또이쯔면 25부 시작
            return 25;
        if(tsumo)//쯔모면+2
            score+=2;
        else //론이면+10
            score+=10;
        //핑후 판정
        ;//...
        return score;
    }
};
#endif //PLAYER_HPP