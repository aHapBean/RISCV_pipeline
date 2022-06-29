//decided by Path !
//namely the Path is responsible for find the include lib
//cpu.hpp

#ifndef CPU_HPP
#define CPU_HPP

#include <bits/stdc++.h>
#include "buffer_stages.hpp"
#include "global.hpp"

#define fdb std::cout<<"FUCK"<<std::endl;
using u32 = unsigned int;
using u8  = unsigned char;


//建立五个pipeline,然后流水执行？
//对每一个过程传入对象，可以有一个计数的单位！
//有道理，独立出一个stage hpp !
//根据示意图更改buffer的值，然后上OJ测试


extern u8 mem[500000];     //全局变量
extern u32 reg[32];
extern u32 PC;
extern u32 clk;

u32 all_empty_flag;

//int 和进制扯不上关系
class cpu{
//buffer
public:
    //u8 mem[500000];
private:
    //u32 reg[32];
    //u32 PC;
    u32 sz;
    object_num pipe_ob[6] = {none,one,two,three,four,five};
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

~cpu(){
	//delete mem
}

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

//at most five objects
//called 1 , 2 , 3 , 4 , 5

//这个对象常处于buffer内，缓冲区内跟着它的就是他的信息，如果需要重来是不是需要存储初始信息？

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

void inirun(){
    STAGE::PC_IF.obn = one;STAGE::IF();
    STAGE::PC_IF.obn = two;STAGE::ID();STAGE::IF();
    STAGE::PC_IF.obn = three;STAGE::EX();STAGE::ID();STAGE::IF();
    if(all_empty_flag)return ;//先跑完当前这个，放弃掉之后的,
    STAGE::PC_IF.obn = four;STAGE::MEM();STAGE::EX();STAGE::ID();STAGE::IF();
    if(all_empty_flag)return ;//先跑完当前这个，放弃掉之后的,
    STAGE::PC_IF.obn = five;STAGE::WB();STAGE::MEM();STAGE::EX();STAGE::ID();STAGE::IF();//后面不能用全局PC ! 甚至PC_IF
    if(all_empty_flag)return ;//先跑完当前这个，放弃掉之后的,
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
    STAGE::PC_IF.obn = STAGE::IF_ID.obn = STAGE::ID_EX.obn = STAGE::EX_MEM.obn = STAGE::MEM_WB.obn = none;
    STAGE::res_PC_IF.obn = STAGE::res_IF_ID.obn = STAGE::res_ID_EX.obn = STAGE::res_EX_MEM.obn = STAGE::res_MEM_WB.obn = none;
}
void pipeRUN(){
    set();
    extern bool eesc;
	eesc = false;
    while(1){
        clk++;
        /*
        if(empty()){
            inirun();
        } else {
            */
            //记一个res buffer就行
            //五个阶段全部结束后再更新正规buffer
            STAGE::WB();
            STAGE::MEM();
            //MEM/WB
            STAGE::EX();
            STAGE::ID();
            STAGE::IF();
            STAGE::updateALL();
            if(eesc)break;
        //}
    }
}
//原来那个单指令执行的本质:
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
