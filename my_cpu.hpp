//decided by Path !
//namely the Path is responsible for find the include lib
//cpu.hpp

#ifndef CPU_HPP
#define CPU_HPP

#include <bits/stdc++.h>
#include "buffer_stages.hpp"
#include "global.hpp"

using u32 = unsigned int;
using u8  = unsigned char;

extern u8 mem[500000];     //全局变量
extern u32 reg[32];
extern u32 PC;
extern u32 clk;

u32 all_empty_flag;

//int 和进制扯不上关系
class cpu{
private:
    u32 sz;
    bool isRun[6];
public:
cpu(){
    memset(mem,0,sizeof(mem));
    memset(reg,0,sizeof(reg));
    memset(isRun,0,sizeof(isRun));
    STAGE::STALL_post_D_F_W_bubble = 0;
    STAGE::discard_flag = 0;
    PC = 0;
    sz = 0;
    clk = 0;
    all_empty_flag = 1;
}

~cpu(){}

void init(std::istream &in){
    std::string s = "";
    u32 ptr = 0;
    in>>s;
    while(!in.fail() && s != "###"){
        if(s == "###")break;
        if(s[0] == '@'){
        	char *tmp;
            ptr = strtoul(s.substr(1,8).c_str(),&tmp,16);
        } else {
        	char *tmp;
            mem[ptr] = strtoul(s.c_str(),&tmp,16);//自然分界读写可以
            //printf("[%s,%d] \n", s.c_str(), mem[ptr]);
			ptr++;
        }
        in>>s;
    }
    sz = ptr;
}
void DEBUGrun(){
	while(1){
        clk++;
		STAGE::IF();
		STAGE::ID();
        STAGE::EX();
        STAGE::MEM();
        STAGE::WB();
        if(STAGE::MEM_WB.esc_flag != 0)break;
	}
}
/*
                                    1 F->D->E->M->W
                                    2    F->D->E->M->W
                                    3       F->D->E->M->W
                                    4          F->D->E->M->W
                                    5             F->D->E->M->W
                                    1 F->D->E->O->O->M->W
                                    2    F->D->O->O->E->M->W
*/
/*
五级流水设计流程：
1.无branch 及其他任何考虑的五级流水  √ -> 2.有branch,这东西是不是和predictor在一起会好些,因为我这里只有静态预测，先用rand()代替可以
3.

即下阶段任务：处理两种hazard !
1.data hazard 写了forwarding 这种 hazard可消失！(MEM变为3个阶段则不确定)
//目测完成 data hazard的处理
//可能可以实现乱序执行

2.control hazard ,根据我的预测值比对，不同的话直接 discard ! ! !
*/

void set(){
	STAGE::IF_ID.obn = STAGE::ID_EX.obn = STAGE::EX_MEM.obn = STAGE::MEM_WB.obn = none;
    STAGE::res_IF_ID.obn = STAGE::res_ID_EX.obn = STAGE::res_EX_MEM.obn = STAGE::res_MEM_WB.obn = none;
}
void pipeRUN(){
    set();
    extern bool eesc;
	eesc = false;
    while(1){
        clk++;
        STAGE::MEM();
        STAGE::WB();
        //MEM/WB
        STAGE::ID();
        STAGE::EX();        
        STAGE::IF();
        STAGE::updateALL();
        if(eesc)break;
    }
}
//单指令执行本质:
/*
while(1){
    if(i == 0){
        IF();
    } else if(){
        ID();
    } else if(){
        EX();
    } else if(){
        MEM();
    } else if(){
        WB();
        if(STAGE::MEM_WB.esc_flag)break;
    }
}
*/

bool empty(){return all_empty_flag == 1;}

};
#endif //CPP_HPP
