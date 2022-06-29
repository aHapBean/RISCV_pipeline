#ifndef BUFFER_STAGES_HPP
#define BUFFEr_StAGES_HPP
#include  <bits/stdc++.h>
#include "global.hpp"

#define funct3B 0b00000000000000000111000000000000   //define funct 3
//0b00000000'00000000'01110000'00000000
#define fs  printf(""); //printf("forwarding success");

using u32 = unsigned int;
using u8  = unsigned char;


extern u8 mem[500000];
extern u32 reg[32];
extern u32 PC;//不能乱声明！ 
extern u32 clk;
extern bool eesc;
u32 discard_clk;

namespace STAGE{
    u32 STALL_post_D_F_W_bubble,discard_flag;        //初值已赋 ! ! ! ! !
    object_num out_obn; //被挤掉的obn

    struct PC_IF_buffer {
        u32 PC;
        object_num obn;
        PC_IF_buffer& operator=(const PC_IF_buffer &t){
            PC = t.PC;
            obn = t.obn;
        }
    }PC_IF,res_PC_IF;

    struct IF_ID_buffer {
        object_num obn;
        u32 code,iniPC,predPC;
        IF_ID_buffer& operator=(const IF_ID_buffer &t){
            iniPC = t.iniPC;
            predPC = t.predPC;
            obn = t.obn;
            code = t.code;
        }
    }IF_ID,res_IF_ID;
//bubble 可以考虑全局bool写  ! 妙 !

    struct ID_EX_buffer {   //ID give the value to it
        object_num obn;
        u32 rd,rs1,rs2;     //编号
        u32 iniPC,predPC;               
        u32 regd,reg1,reg2,imm,shamt,Mfregd,Mfrd,Efregd,Efrd,Wfregd,Wfrd,ld_flag; //操作数
        OPflag opflag;      //OPflag
        ID_EX_buffer(){rs1 = rs2 = rd = imm = Mfregd = Mfrd = Efregd = Wfregd = Wfrd = ld_flag = Efrd = 0;}
        ID_EX_buffer& operator=(const ID_EX_buffer &t){
            obn = t.obn;
            rd  = t.rd;
            rs1 = t.rs1;
            rs2 = t.rs2;

            iniPC = t.iniPC;
            predPC = t.predPC;

            regd = t.regd;
            reg1 = t.reg1;
            reg2 = t.reg2;
            imm = t.imm;
            shamt = t.shamt;
            opflag = t.opflag;      //res_buffer里面没有 forwarding的内容 !
            //没必要有
        }
    }ID_EX,res_ID_EX;

    struct EX_MEM_buffer {
        object_num obn;
        u32 esc_flag;
        u32 rd,rs1,rs2;     //TO delete !     
        u32 regd,iniPC,predPC,imm,reg2;
        u32 ld_dest,st_dest,ld_flag,st_flag;//打标记，看是否有内存读写操作
        OPflag opflag;
        EX_MEM_buffer& operator=(const EX_MEM_buffer &t){//只有一周期bubble就一次bubble就直接改回去 !  bubble的一种可行办法
            obn = t.obn;
            rd  = t.rd;
            rs1 = t.rs1;
            rs2 = t.rs2;
            //esc_flag = t.esc_flag; //!!
            regd = t.regd;
            iniPC = t.iniPC;
            predPC = t.predPC;
            imm = t.imm;reg2 = t.reg2;
            ld_dest = t.ld_dest;st_dest = t.st_dest;
            ld_flag = t.ld_flag;st_flag = t.st_flag;
            opflag = t.opflag;
        }
    }EX_MEM,res_EX_MEM;

    struct MEM_WB_buffer{
        object_num obn;
        OPflag opflag;
        u32 regd,rd,iniPC,predPC;
        u32 esc_flag;
        MEM_WB_buffer& operator=(const EX_MEM_buffer &t){
            obn = t.obn;
            opflag = t.opflag;
            regd = t.regd;rd  = t.rd;
            esc_flag = t.esc_flag;
            iniPC = t.iniPC;
            predPC = t.predPC;
        }
    }MEM_WB,res_MEM_WB;

void printID_EX_Buffer(STAGE::ID_EX_buffer &t);

void RES_PC_IF_up(object_num obn){
    //res_PC_IF.PC = PC;
    res_PC_IF.obn = obn;
}

    /*forwarding*/
void Epreforwarding(u32 regd,u32 rd,u32 ld_flag){
    //假如我每次结束后立即更新buffer则这里可以直接判断
    //顺序问题，此时仍未更新 ！
    ID_EX.Efregd = regd;
    ID_EX.Efrd   = rd;
    ID_EX.ld_flag = ld_flag;    //每次回传都需要重新赋值 !!!
}

void Mpreforwarding(u32 regd,u32 rd){
    ID_EX.Mfregd = regd;
    ID_EX.Mfrd   = rd;
}

void Wpreforwarding(u32 regd,u32 rd){
    ID_EX.Wfrd = rd;
    ID_EX.Wfregd = regd;
}


//rd != 0;

//FUCK !!!!!! here 1

void forwarding(){
    //STALL 后的forwarding

//然而这并不是stall后的forwarding
    if(STALL_post_D_F_W_bubble){            //两次forwarding值都保留了 ？？？？
        //如果刚刚执行过stall
        //留下forwarding的数据
        if(ID_EX.Mfrd != 0 && ID_EX.rs1 == ID_EX.Mfrd)ID_EX.reg1 = ID_EX.Mfregd, fs 
        if(ID_EX.Mfrd != 0 && ID_EX.rs2 == ID_EX.Mfrd)ID_EX.reg2 = ID_EX.Mfregd,fs    //forwarding
        STALL_post_D_F_W_bubble = 0;        //只需要一个回合，那么这里应该没有问题
        //ld_flag 在每一次cycle会更新！（下面）
        return ;
    }
//如果有停顿，这里赋值也不要紧
    if(ID_EX.Wfrd != 0 && ID_EX.rs1 == ID_EX.Wfrd)ID_EX.reg1 = ID_EX.Wfregd,fs 
    if(ID_EX.Wfrd != 0 && ID_EX.rs2 == ID_EX.Wfrd)ID_EX.reg2 = ID_EX.Wfregd,fs    //forwarding
                                                                                        //0特判 !
    if(ID_EX.Mfrd != 0 && ID_EX.rs1 == ID_EX.Mfrd)ID_EX.reg1 = ID_EX.Mfregd,fs
    if(ID_EX.Mfrd != 0 && ID_EX.rs2 == ID_EX.Mfrd)ID_EX.reg2 = ID_EX.Mfregd,fs    //forwarding

    if(ID_EX.ld_flag && ID_EX.Efrd != 0 && (ID_EX.rs1 == ID_EX.Efrd || ID_EX.rs2 == ID_EX.Efrd)) {
        //printf("\nSTALL_bubble_here ! \n");                                       //?????这是什么鬼 ？？又错了
		STALL_post_D_F_W_bubble = 1;//确实需要停顿
    } else {    //假如stall过了，那么留下的是两组 stall过的，此时MEM传过来的反而有用
        if(ID_EX.Efrd != 0 && ID_EX.rs1 == ID_EX.Efrd)ID_EX.reg1 = ID_EX.Efregd,fs
        if(ID_EX.Efrd != 0 && ID_EX.rs2 == ID_EX.Efrd)ID_EX.reg2 = ID_EX.Efregd,fs    //forwarding
        STALL_post_D_F_W_bubble = 0;    //remember to do this
    }//EX一定保持在后面更新
}


/*执行完成后 update buffer*/
void updateALL(){
    //stall 时的 forwarding ? 
    //此处更新可能打乱我的none ! ! ! !
    if(discard_flag == 2 || STALL_post_D_F_W_bubble && discard_flag)printf("FUCKKKKKKKKKKKK\n\n\n");
    if(discard_flag && discard_clk == clk){
    	PC = res_EX_MEM.iniPC;//
        res_IF_ID.obn = none;
        res_ID_EX.obn = none;//ID EX
        //printf("\nJUMP/BRANCH here !\n");
    }
    
    if(discard_flag && discard_clk == clk - 1){
        //res_IF_ID.obn = none;
        //res_ID_EX.obn = none;// EX MEM
        res_EX_MEM.obn = none;
    }
    
    if(discard_flag && discard_clk + 2 == clk){
        //res_EX_MEM.obn = none;// MEM WB
        //理论上这两条可以去掉，中间和下面的第一条
        res_MEM_WB.obn = none;//最后一个不用管，因为MEM没跑，它没更新
        discard_flag = 0;//控制下一个周期，可以重新开始
    }

    if(STALL_post_D_F_W_bubble){EX_MEM.obn = none;}//MEM不操作，IF_ID，"PC_IF",ID_EX不动 ! !
    //停滞
    // ID_EX 不动， PC_IF 没什么用 , 而EX_MEM已经操作过了 ! ! ! !

    //注意修改回传
    else {
        IF_ID = res_IF_ID;//是这三个操作停滞所以没有输出
        ID_EX = res_ID_EX;
        EX_MEM= res_EX_MEM;//set none !一周期的停滞，不然重复操作了
    }
//我的STALL是一个标记
    forwarding();   //forwarding处理，针对ID_EX_buffer

    //if(STALL_post_D_F_W_bubble == 1)++STALL_post_D_F_W_bubble;
    
    PC_IF = res_PC_IF;//update !

    MEM_WB= res_MEM_WB;//update ！
    
    res_PC_IF.obn = none;
    res_IF_ID.obn = none;
    res_ID_EX.obn = none;
    res_EX_MEM.obn= none;
    res_MEM_WB.obn= none;
}


u32 predict(u32 &PC){
    PC += 4;
    return PC;
}

void RES_IF_ID_up(u32 iniPC,u32 predPC,u32 code,object_num obn){
    res_IF_ID.obn  = obn;
    res_IF_ID.iniPC   = iniPC;
    res_IF_ID.predPC = predPC;
    res_IF_ID.code = code;
}

void IF(){
    //in the mem the info is stored in the form of byte  |u8 !
    object_num obn = PC_IF.obn;     //PC由全局来做统领
    if(obn == none)obn = one;   //不对吧，我之前不是加了吗
    if(STALL_post_D_F_W_bubble || obn == none){out_obn = obn;return ;}//这个编号都无所谓了，只要是在跑,还是先留一个接口吧，
    //主要原因是 PC_IFbuffer无实际意义 ！！ PC又不用你的，用全局

    u32 code = 0b0,iniPC = PC,predPC;
	IF_ID.code = 0b0;
    for(int i = 0;i < 4; ++i){
        code |= (mem[PC + 3 - i] & 0b11111111);//机器内部其实就是二进制形式的简单赋值，16进制数在内存也以0 1 形式体现
        if(i != 3)code <<= 8;       //     不能左移四次，因为第一次不用移！！ 
    }//逆读
    predPC = predict(PC);        //change the global PC !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
                                    //静态预测
    RES_IF_ID_up(iniPC,predPC,code,obn);
}

//放到前面！
void RES_ID_EX_up(  object_num obn,
                u32 rd = 0,u32 rs1 = 0,u32 rs2 = 0,
                u32 imm = 0,u32 shamt = 0,
                u32 regd = 0,u32 reg1 = 0,u32 reg2 = 0,
                u32 iniPC = 0,u32 predPC = 0,
                OPflag opflag = LUI     //add NONE ????????
                ){

    res_ID_EX.obn = obn;

    res_ID_EX.opflag = opflag;
    res_ID_EX.imm = imm;
    res_ID_EX.rd = rd;
    res_ID_EX.rs1 = rs1;
    res_ID_EX.rs2 = rs2;    //为了data hazard做准备
    res_ID_EX.opflag = opflag;
    res_ID_EX.shamt = shamt;

    res_ID_EX.regd = reg[rd];
    res_ID_EX.reg1 = reg[rs1];
    res_ID_EX.reg2 = reg[rs2];
    
    res_ID_EX.iniPC = iniPC;
    res_ID_EX.predPC = predPC;

    //printID_EX_Buffer(res_ID_EX);
}


    /*Instuction_Decode*/
void ID(){
    object_num obn = IF_ID.obn;
	
    if(STALL_post_D_F_W_bubble || obn == none){return ;}

    u32 funct3 = 0b0,funct7 = 0b0,code = IF_ID.code;
    u32 opcode = (code & 0b1111111);
    OPflag opflag;

    //give value to the opflag
    switch (opcode) {
        case 0b0110111:
            opflag = LUI;break;
        case 0b0010111:
            opflag = AUIPC;break;
        case 0b1101111:
            opflag = JAL;break;
        case 0b1100111://it has a funct3 用处 ? ?仅用于区分吗
			opflag = JALR;break;
        case 0b1100011:
            funct3 = (code & (funct3B));// funct3
            funct3 >>= 12;      // ?
            switch (funct3) {
                case 0b000:
                    opflag = BEQ;break;
                case 0b001:
                    opflag = BNE;break;
                case 0b100:
                    opflag = BLT;break;
                case 0b101:
                    opflag = BGE;break;
                case 0b110:
                    opflag = BLTU;break;
                case 0b111:
                    opflag = BGEU;break;
            }
            break;
        case 0b0000011:
            funct3 = (code & funct3B);// funct3
            funct3 >>= 12;
            switch (funct3) {
                case 0b000:
                    opflag = LB;break;
                case 0b001:
                    opflag = LH;break;
                case 0b010:
                    opflag = LW;break;
                case 0b100:
                    opflag = LBU;break;
                case 0b101:
                    opflag = LHU;break;
            }
            break;
        case 0b0100011:
            funct3 = (code & funct3B);// funct3
            funct3 >>= 12;
            switch (funct3) {
                case 0b000:
                    opflag = SB;break;
                case 0b001:
                    opflag = SH;break;
                case 0b010:
                    opflag = SW;break;
            }
            break;
        case 0b0010011:
            funct3 = (code & funct3B);// funct3
            funct3 >>= 12;
            switch (funct3) {
                case 0b000:
                    opflag = ADDI;break;
                case 0b010:
                    opflag = SLTI;break;
                case 0b011:
                    opflag = SLTIU;break;
                case 0b100:
                    opflag = XORI;break;
                case 0b110:
                    opflag = ORI;break;
                case 0b111:
                    opflag = ANDI;break;
                case 0b001:
                    opflag = SLLI;break;//it has funct7
                case 0b101:
                    u32 funct7 = (code & 0b11111110000000000000000000000000);//0b11111110'00000000'00000000'00000000
                    funct7 >>= 25;
                    switch (funct7) {
                        case 0b0000000:
                            opflag = SRLI;break;
                        case 0b0100000:
                            opflag = SRAI;break;
                    }
                    break;
            }
            break;
        case 0b0110011:
            funct3 = (code & funct3B);// funct3
            funct3 >>= 12;
            switch(funct3){
                case 0b000:
                    funct7 = (code & 0b11111110000000000000000000000000);
                        funct7 >>= 25;
                        switch (funct7) {
                            case 0b0000000:
                                opflag = ADD;break;
                            case 0b0100000:
                                opflag = SUB;break;
                        }
                        break;
                case 0b001:
                    opflag = SLL;break;
                case 0b010:
                    opflag = SLT;break;
                case 0b011:
                    opflag = SLTU;break;
                case 0b100:
                    opflag = XOR;break;
                case 0b101:
                    funct7 = (code & 0b11111110000000000000000000000000);
                        funct7 >>= 25;
                        switch (funct7) {
                            case 0b0000000:
                                opflag = SRL;break;
                            case 0b0100000:
                                opflag = SRA;break;
                        }
                        break;
                case 0b110:
                    opflag = OR;break;
                case 0b111:
                    opflag = AND;break;
            }
            break;
    }
    //puts("");
    printOP(opflag);
    

    u32 rd = 0b0,rs1 = 0b0,rs2 = 0b0,imm = 0b0,flag = 0b0,shamt = 0b0;

    //几种解码
    switch (opflag) {

//U
        case LUI:case AUIPC:
            imm = (code & 0b11111111111111111111000000000000);		//0b11111111'11111111'11110000'00000000
            // 扩展
            rd  = (code & 0b00000000000000000000111110000000);		//0b00000000'00000000'00001111'10000000
            rd >>= 7;
            //std::cout<<std::endl<<"imm:"<<imm<<std::endl;
            break;

//UJ
        case JAL:
        	char *tp;
            u32 imm20,imm10_1,imm11,imm19_12;
            imm20    = (code & 0b10000000000000000000000000000000);	//0b10000000'00000000'00000000'00000000
			imm20   >>= (31 - 20);
			//printf("imm_binary: %s\n\n",itoa(imm20,tp,2));
            
            imm10_1  = (code & 0b01111111111000000000000000000000);	//0b01111111'11100000'00000000'00000000
			imm10_1 >>= (30 - 10);
			//printf("imm_binary: %s\n\n",itoa(imm10_1,tp,2));

            imm11    = (code & 0b00000000000100000000000000000000);	//0b00000000'00010000'00000000'00000000
            
			imm11 >>= (20 - 11);
			//printf("imm_binary: %s\n\n",itoa(imm11,tp,2));

            imm19_12 = (code & 0b00000000000011111111000000000000);	//0b00000000'00001111'11110000'00000000
			//printf("imm_binary: %s\n\n",itoa(imm19_12,tp,2));
			
            rd       = (code & 0b00000000000000000000111110000000);	//0b00000000'00000000'00001111'10000000
            rd >>= 7;
            
            imm |= imm20;imm |= imm10_1;imm |= imm11;imm |= imm19_12;

            flag = (code & 0b10000000000000000000000000000000);		//0b10000000'00000000'00000000'00000000
            if(flag != 0b0){//符号扩展
                imm |= (0b11111111111100000000000000000000);		//0b11111111'11110000'00000000'00000000
            }
            //std::cout<<std::endl<<"imm:"<<imm<<std::endl;
            break;

//SB
        case BEQ:case BNE:case BLT:case BGE:case BLTU:case BGEU:
            rs1 = (code & 0b00000000000011111000000000000000);		//0b00000000'00001111'10000000'00000000
            rs1 >>= 15;
            rs2 = (code & 0b00000001111100000000000000000000);		//0b00000001'11110000'00000000'00000000
            rs2 >>= 20;

            u32 imm12,imm10_5,imm4_1;//imm11;
            imm12   = (code & 0b10000000000000000000000000000000);	//0b10000000'00000000'00000000'00000000
            imm12 >>= (31 - 12);//31 -> 12

            imm10_5 = (code & 0b01111110000000000000000000000000);	//0b01111110'00000000'00000000'00000000
            imm10_5 >>= (30 - 10);
            
            imm4_1  = (code & 0b00000000000000000000111100000000);	//0b00000000'00000000'00001111'00000000
            imm4_1 >>= (8 - 1);

            imm11   = (code & 0b00000000000000000000000010000000);	//0b00000000'00000000'00000000'10000000
            imm11 <<= (11 - 7);

            imm = 0b0;
            imm |= imm4_1;imm |= imm10_5;imm |= imm11;imm |= imm12;
            flag = (code & 0b10000000000000000000000000000000);		//0b10000000'00000000'00000000'00000000
            if(flag != 0){//从12位开始符号扩展
                imm |= (0b11111111111111111110000000000000);		//0b11111111'11111111'11100000'00000000
            }
            //std::cout<<std::endl<<"imm:"<<imm<<std::endl;
            break;

//I
        case JALR:case LB:case LH:case LW:case LBU:case LHU:case ADDI:
        case SLTI:case SLTIU:case XORI:case ORI:case ANDI: 
            imm  = (code & 0b11111111111100000000000000000000);		//0b11111111'11110000'00000000'00000000
            flag = (code & 0b10000000000000000000000000000000);	//0b10000000'00000000'00000000'00000000
            imm >>= 20;//???change 6.23
            if(flag != 0b0){//高20位符号位扩展1
            //change Here
                imm |= (0b11111111111111111111000000000000);		//0b11111111'11111111'11110000'00000000
            }
            rd  = (code & 0b00000000000000000000111110000000);		//0b00000000'00000000'00001111'10000000
            rd >>= 7;//

            rs1 = (code & 0b00000000000011111000000000000000);		//0b00000000'00001111'10000000'00000000
            rs1 >>= 15;

            //std::cout<<std::endl<<"imm:"<<imm<<std::endl;
            break;

//S
        case SB:case SH:case SW://TODO
            u32 t1,t2;
            t1 = (code & 0b11111110000000000000000000000000);		//0b11111110'00000000'00000000'00000000
            t2 = (code & 0b00000000000000000000111110000000);		//0b00000000'00000000'00001111'10000000
            t2 >>= 7;
            t1 >>= (25 - 5);

            t1 |= t2;
            imm = t1;

            flag = (code & 0b10000000000000000000000000000000);	//0b10000000'00000000'00000000'00000000
            if(flag != 0b0){//符号扩展
                imm |= (0b11111111111111111111000000000000);		//0b11111111'11111111'11110000'00000000
            }//扩展20位，上面有扩展19位的

            rs1 = (code & 0b00000000000011111000000000000000);		//0b00000000'00001111'10000000'00000000
            rs1 >>= 15;

            rs2 = (code & 0b00000001111100000000000000000000);		//0b00000001'11110000'00000000'00000000
            rs2 >>= 20;
            //std::cout<<std::endl<<"imm:"<<imm<<std::endl;
            break;

//R
        case SLLI:case SRLI:case SRAI:case ADD:case SUB:case SLL:case SLT:
        case SLTU:case XOR:case SRL:case SRA:case OR:case AND:
            rs1 = (code & 0b00000000000011111000000000000000);		//0b00000000'00001111'10000000'00000000
            rs1 >>= 15;

            rs2 = (code & 0b00000001111100000000000000000000);		//0b00000001'11110000'00000000'00000000
            rs2 >>= 20;

            if(opflag == SLLI || opflag == SRLI || opflag == SRAI){
                shamt = rs2;                                        //注意看shamt可否用rs2代替
            }
            rd  = (code & 0b00000000000000000000111110000000);		//0b00000000'00000000'00001111'10000000
            rd >>= 7;
            //std::cout<<std::endl<<"imm:"<<imm<<std::endl;
            break;
    }

    RES_ID_EX_up(obn,rd,rs1,rs2,imm,shamt,reg[rd],reg[rs1],reg[rs2],IF_ID.iniPC,IF_ID.predPC,opflag);
}

void RES_EX_MEM_up(object_num obn,
                u32 ld_dest = 0,u32 ld_flag = 0,
                u32 st_dest = 0,u32 st_flag = 0,
                u32 regd = 0,u32 reg2 = 0,
                u32 rd = 0,u32 rs1 = 0,u32 rs2 = 0,
                u32 iniPC = 0,u32 predPC = 0,
                OPflag opflag = LUI){
    res_EX_MEM.obn = obn;		//FUCK
    res_EX_MEM.ld_dest = ld_dest;res_EX_MEM.st_flag = st_flag;              //FUCKKKKKKKKKKKKKKKKKKK
    res_EX_MEM.ld_flag = ld_flag;res_EX_MEM.st_dest = st_dest;
    res_EX_MEM.regd = regd;res_EX_MEM.opflag = opflag;res_EX_MEM.rd = rd;
    res_EX_MEM.rs1 = rs1;res_EX_MEM.rs2 = rs2;//no opcode ! 
    res_EX_MEM.reg2 = reg2;
    res_EX_MEM.iniPC = iniPC;           //??**
    res_EX_MEM.predPC= predPC;                
}


    /*Execute*/
void EX(){
    object_num obn = ID_EX.obn;

    if(STALL_post_D_F_W_bubble || obn == none){return ;}

    OPflag opflag = ID_EX.opflag;
    u32 rd = ID_EX.rd,
        rs1 = ID_EX.rs1,
        rs2 = ID_EX.rs2,
        shamt = ID_EX.shamt,
        imm = ID_EX.imm,
        regd = ID_EX.regd,
        reg1 = ID_EX.reg1,
        reg2 = ID_EX.reg2,
        iniPC = ID_EX.iniPC,  //PC !!!!!!!
        predPC = ID_EX.predPC;

    u32 ld_dest = 0b0,st_dest = 0b0,ld_flag = 0,st_flag = 0;
    EX_MEM.esc_flag = 0;
    bool ok = false;

    switch (opflag) {
        case LUI:       
            //fdb
            regd = imm;break;
        case AUIPC:      
            //fdb
            regd = (iniPC) + imm;//  change ! PC -= 4;PC += imm;break;
            break;          //change !
        case JAL:       
            //fdb
            //PC -= 4;
            ok = true;
            regd = iniPC + 4;iniPC += imm;break;          //下一个地址链接到regd中，而你静态预测已经是默认+4 ，，YC ! !
        case JALR:      
            //fdb
            //PC -= 4;
            ok = true;
            regd = iniPC + 4;iniPC = reg1 + imm;          //这个PC是全局PC!
			break;            //*
        case BEQ:
            //fdb
            if((reg1) == (reg2))ok = true,iniPC = iniPC + imm;break;    //*
        case BNE:
            //fdb
            if((reg1) != (reg2))ok = true,iniPC = iniPC + imm;break;    //*
        case BLT:
            //fdb
            if((int)reg1 < (int)reg2)ok = true,iniPC = iniPC + imm;break;       //*
        case BGE:
            //fdb
            if((int)reg1 >= (int)reg2)ok = true,iniPC = iniPC + imm;break;      //*
        case BLTU:
            //fdb
            if(reg1 < reg2)ok = true,iniPC = iniPC + imm;break;         //*
        case BGEU:
            //fdb
            if(reg1 >= reg2)ok = true,iniPC = iniPC + imm;break;        //*change the PC

        case LB:case LH:case LW:case LBU:case LHU:
            //fdb
            ld_dest = 0b0;ld_dest += (u32)(imm);ld_dest += reg1;    //这个imm需要符号吗
            //printf("reg a5 : %d\n",reg1);
			//printf("imm : %d\n",imm);
			ld_flag = 1;
            break;

        case SB:case SH:case SW:
            //fdb
            st_dest = reg1 + imm;
            st_flag = 1;
            break;

        case ADDI:
            //load imm to rd !
            //fdb
            if(imm == 255 && rd == 10){
                EX_MEM.esc_flag = 1;break;//
            }
            regd = reg1 + imm;
            break;

        case SLTI:      //set less than imm
            //fdb
            if((int)reg1 < (int)imm)regd = 1;
            else regd = 0;
            break;

        case SLTIU:     //set less than imm unsigned
            //fdb
            if(reg1 < imm)regd = 1;
            else regd = 0;
            break;

        case XORI:
            //fdb
            regd = reg1 ^ imm;
            break;
        case ORI:
            //fdb
            regd = reg1 | imm;
            break;
        case ANDI:
            //fdb
            regd = reg1 & imm;
            break;
            
        case SLLI:      //shift left logical imm   
            //fdb
            regd = reg1 << shamt;
            break;      //这个地方有个什么要算术移动，对，算术移动
        case SRLI:
            //fdb
            regd = reg1 >> shamt;
            break;
//??
        case SRAI:		//change here
            //fdb
            regd = (u32)((int)reg1 >> shamt);//
            break;
        case ADD:
            //fdb
            regd = reg1 + reg2;
            break;
        case SUB:
            //fdb
            regd = reg1 - reg2;//???符号要紧吗？？
            break;
        case SLL:       //shift left locical
            //fdb
            regd = reg1 << reg2;
            break;
        case SLT:       //set less than
            //fdb
            regd = ((int)reg1 < (int)reg2) ? 1 : 0;
            break;
        case SLTU:
            //fdb
            regd = (reg1 < reg2) ? 1 : 0;
            break;
        case XOR:
            //fdb
            regd = reg1 ^ reg2;
            break;
        case SRL:       //shift right locical
            //fdb
            regd = reg1 >> reg2;
            break;
        case OR:
            //fdb
            regd = reg1 | reg2;
            break;
        case AND:
            //fdb
            regd = reg1 & reg2;
            break;
    }
    
    //iniPC 就是真实的PC,经过操作后的PC
    //OK来标记是否已经对 iniPC操作
    if(!ok)iniPC += 4;
    if(iniPC != predPC){
        //IF_ID.obn = none;//!!!!不能 ！ ！
        //ID_EX.obn = none;
        ++discard_flag;
        discard_clk = clk;
        //停掉正下方两个，重新来
        //PC = iniPC;//真实操作时需要的正确值
        //此时乱序执行可能还没IF那一个 !!!!!!!! 
    }
	printID_EX_Buffer(res_ID_EX);
    /*
	if(rd == 15){
		bool ok = true;
		if(ok){
			int a,b;
			a = b;
		}
	}
    */
    Epreforwarding(regd,rd,ld_flag);
    
    RES_EX_MEM_up(obn,ld_dest,ld_flag,st_dest,st_flag,regd,reg2,rd,rs1,rs2,iniPC,predPC,opflag);
}

void RES_MEM_WB_up(object_num obn,
                u32 esc_flag = 0,OPflag opflag = LUI,
                u32 iniPC = 0,u32 predPC = 0,                             //这里给PC = 0会有问题吗
                u32 rd = 0,u32 regd = 0){
    res_MEM_WB.obn = obn;
    res_MEM_WB.esc_flag = esc_flag;     //仅仅是EX_MEM直接赋值给 真实 buffer
    //其他buffer,比如 MEM_WB buffer 还是遵守正常流程的
    res_MEM_WB.opflag = opflag;
    res_MEM_WB.iniPC = iniPC;
    res_MEM_WB.predPC= predPC;
    res_MEM_WB.rd = rd;
    res_MEM_WB.regd = regd;
}

void printSTORE(u32 st_dest,u32 reg2){
	printf("st_dest: %d reg2 : %d \n",st_dest,reg2);
}

   /*Memrory access*/
void MEM(){// L S
    object_num obn = EX_MEM.obn;
    //if(discard_flag == 2 || discard_flag == 3)obn = none,EX_MEM.obn = none,res_EX_MEM.obn = none;
    //if(discard_flag == 1)++discard_flag;    //考虑到有乱序的问题，这里也有问题 ! ! ! !
    //if(discard_flag && discard_clk + 2 == clk)obn = none,EX_MEM.obn = none;

    if(obn == none){return ;}

    OPflag opflag = EX_MEM.opflag;
    u32 ld_dest = EX_MEM.ld_dest,
        ld_flag = EX_MEM.ld_flag,
        st_dest = EX_MEM.st_dest,
        st_flag = EX_MEM.st_flag,
        regd = EX_MEM.regd,//缓冲区里面regd可能有数据也可能没有数据
        rd = EX_MEM.rd,
        reg2 = EX_MEM.reg2,
        iniPC = EX_MEM.iniPC,
        predPC = EX_MEM.predPC;     //*
    u32 tmp;
    
    if(ld_flag){
        //读内存
        switch (opflag) {
            case LB:    //load binary
                regd = (u32)mem[ld_dest];        //?????u8 -> u32需要处理？？
                if(((u32)mem[ld_dest] & 0b10000000)){
                    regd |= 0b11111111111111111111111100000000;//0b11111111'11111111'11111111'00000000;
                }//符号位扩展
                break;

            case LH:    //load halfword
                regd = (u32)mem[ld_dest + 1];regd <<= 8;
                regd |= (u32)mem[ld_dest];
                if(((u32)regd & 0b1000000000000000)){//two byte
                    regd |= 0b11111111111111110000000000000000;//0b11111111'11111111'00000000'00000000;
                }//符号位扩展
                break;

            case LW:
                regd = 0b0;
                for(int i = 0;i < 4; ++i){//右边的先下来 
                    regd |= mem[ld_dest + 3 - i];       
                    if(i != 3)regd <<= 8;
                }
                //printOP(opflag);
                //printf("reg:a5 :%d\n",reg[15]);
                //printf("LW : ld_dest: %d",ld_dest);
                break;

            case LBU:
                regd = (u32)mem[ld_dest];
                break;

            case LHU://右边的先下来 
                regd = (u32)mem[ld_dest + 1];regd <<= 8;
                regd |= (u32)mem[ld_dest];
                break;
        }
    }
	if(st_flag) {
        switch (opflag) {
            case SB:
                tmp = 0b0;tmp = (reg2 & 0b11111111);//取后八位
                mem[st_dest] = (u8)tmp;
                break;

            case SH:
                tmp = 0b0;tmp = (reg2 & 0b1111111111111111);//取后16位
                //右边放在内存左边
				mem[st_dest] = (u8)(tmp);tmp >>= 8;
                mem[st_dest + 1] = (u8)(tmp);                        //????here ????
                break;
                
            case SW:
                tmp = 0b0;tmp = (reg2);             //取后16位
                for(int i = 0;i < 4; ++i){
                    mem[st_dest + i] = (u8)(tmp);
                    tmp >>= 8;
                }
                //printSTORE(st_dest,reg2);
                break;
        }
    }
    
    Mpreforwarding(regd,rd);

    RES_MEM_WB_up(obn,EX_MEM.esc_flag,opflag,EX_MEM.iniPC,EX_MEM.predPC,rd,regd);
}

    /*write back to the register*/
    //discard_flag置零后代表 discard已经处理完毕，可以正常运行
void WB(){
    object_num obn = MEM_WB.obn;
    //讲道理这个res 由 MEM 提供
    //if(discard_flag == 3)obn = none,MEM_WB.obn = none,res_MEM_WB.obn = none,discard_flag = 0;//代表已经跑过reg
    //if(discard_flag == 2)++discard_flag;
    //if(discard_flag && discard_clk + 3 == clk)obn = none,MEM_WB.obn = none,discard_flag = 0;//代表已经跑过reg
    //每次操作 “清空” res 那么这里就可以不用加上，要不然明明传了，你都置0，会有问题


    if(obn == none){return ;}              //全局PC ???????????

    OPflag opflag = MEM_WB.opflag;
    u32 regd = MEM_WB.regd,
        rd   = MEM_WB.rd,
        esc_flag = MEM_WB.esc_flag;

    Wpreforwarding(regd,rd);
    switch (opflag) {
        case BEQ:case BNE:case BLT:case BGE:case BLTU:		//AUIPC!!! 
        case BGEU:case SB:case SH:case SW:
            break;
        default:
            if (esc_flag) {     //PC跳了以后是不是不用PC += 4;特判？
                //reg[rd] = regd;//???需要存储？？ no need to store
                //actually it doesn't matters
                eesc = true;
                //forwarding(); ?no forwarding
                //printf("对应阶段执行完成后reg[10]的值\n");
				//printOP(opflag);
                //printf("\n reg[10] : %d \n",reg[10]);
                printf("%d\n",(regd & 255u));
                return ;
            }
            reg[rd] = regd;
    }
   // printf("对应阶段执行完成后reg[10]的值\n");
				//printOP(opflag);
                //printf("\n reg[10] : %d \n",reg[10]);
    reg[0] = 0;//       big fault!
    RES_PC_IF_up(obn);
}

void printID_EX_Buffer(STAGE::ID_EX_buffer &t){
	return ;
    printf("imm: %d rd: %d rs1: %d rs2 %d\n",t.imm,t.rd,t.rs1,t.rs2);
    printf("寄存器编号 rd:");
    printREG(t.rd);
    printf(" ");
    printf("rs1: ");
    printREG(t.rs1);
    printf(" ");
    printf("rs2 ");
    printREG(t.rs2);
    printf("\n reg : ra :%d ",reg[1]);
	puts(" \n");
    char tp[32];
    //printf("imm_binary: %s\n\n",itoa(t.imm,tp,2));
}


}

#endif //BUFFER_STAGES_HPP