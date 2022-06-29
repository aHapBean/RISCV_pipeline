#ifndef GLOBAL_HPP
#define GLOBAL_HPP
using u32 = unsigned int;
using u8  = unsigned char;

u8 mem[500000];     //全局变量
u32 reg[32];
u32 PC;
u32 clk;
bool eesc;
enum OPflag {
    LUI,
    AUIPC,      //add uppper imm to PC
    JAL,        //jump and link
    JALR,       //jump and link reg
    BEQ,        //branch equal  

    BNE,        
    BLT,
    BGE,
    BLTU,
    BGEU,
    
    LB,
    LH,
    LW,
    LBU,
    LHU,

    SB,
    SH,
    SW,
    ADDI,
    SLTI,

    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,

    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,

    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,

    OR,
    AND
};

enum object_num{
    none,
    one,
};

struct IF_ID_buffer {
    object_num obn;
    u32 code,iniPC,predPC;
    IF_ID_buffer& operator=(const IF_ID_buffer &t){
        iniPC = t.iniPC;
        predPC = t.predPC;
        obn = t.obn;
        code = t.code;
    }
};

struct ID_EX_buffer {
    object_num obn;
    u32 rd,rs1,rs2;   
    u32 iniPC,predPC;               
    u32 regd,reg1,reg2,imm,shamt,Mfregd,Mfrd,Efregd,Efrd,Wfregd,Wfrd,ld_flag;
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
        opflag = t.opflag;
    }
};

struct EX_MEM_buffer {
    object_num obn;
    u32 esc_flag;
    u32 rd,rs1,rs2;  
    u32 regd,iniPC,predPC,imm,reg2;
    u32 ld_dest,st_dest,ld_flag,st_flag;
    OPflag opflag;
    EX_MEM_buffer& operator=(const EX_MEM_buffer &t){
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
};

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
};


void printOP(OPflag flag){
	return ;
    switch (flag) {
        case 0:std::cout<<"LUI"<<std::endl;break;
        case 1:std::cout<<"AUIPC"<<std::endl;break;
        case 2:std::cout<<"JAL"<<std::endl;break;
        case 3:std::cout<<"JALR"<<std::endl;break;
        case 4:std::cout<<"BEQ"<<std::endl;break;
        case 5:std::cout<<"BNE"<<std::endl;break;
        case 6:std::cout<<"BLT"<<std::endl;break;
        case 7:std::cout<<"BGE"<<std::endl;break;
        case 8:std::cout<<"BLTU"<<std::endl;break;
        case 9:std::cout<<"BGEU"<<std::endl;break;
        case 10:std::cout<<"LB"<<std::endl;break;
        case 11:std::cout<<"LH"<<std::endl;break;
        case 12:std::cout<<"LW"<<std::endl;break;
        case 13:std::cout<<"LBU"<<std::endl;break;
        case 14:std::cout<<"LHU"<<std::endl;break;
        case 15:std::cout<<"SB"<<std::endl;break;
        case 16:std::cout<<"SH"<<std::endl;break;
        case 17:std::cout<<"SW"<<std::endl;break;
        case 18:std::cout<<"ADDI"<<std::endl;break;
        case 19:std::cout<<"SLTI"<<std::endl;break;
        case 20:std::cout<<"SLTIU"<<std::endl;break;
        case 21:std::cout<<"XORI"<<std::endl;break;
        case 22:std::cout<<"ORI"<<std::endl;break;
        case 23:std::cout<<"ANDI"<<std::endl;break;
        case 24:std::cout<<"SLLI"<<std::endl;break;
        case 25:std::cout<<"SRLI"<<std::endl;break;
        case 26:std::cout<<"SRAI"<<std::endl;break;
        case 27:std::cout<<"ADD"<<std::endl;break;
        case 28:std::cout<<"SUB"<<std::endl;break;
        case 29:std::cout<<"SLL"<<std::endl;break;
        case 30:std::cout<<"SLT"<<std::endl;break;
        case 31:std::cout<<"SLTU"<<std::endl;break;
        case 32:std::cout<<"XOR"<<std::endl;break;
        case 33:std::cout<<"SRL"<<std::endl;break;
        case 34:std::cout<<"SRA"<<std::endl;break;
        case 35:std::cout<<"OR"<<std::endl;break;
        case 36:std::cout<<"AND"<<std::endl;break;
    }
}

void printREG(u32 rd){
	return ;
    switch (rd) {
        case 0:std::cout<<"zero";break;
        case 1:std::cout<<"ra";break;
        case 2:std::cout<<"sp";break;
        case 3:std::cout<<"qp";break;
        case 4:std::cout<<"tp";break;
        case 5:std::cout<<"t0";break;
        case 6:std::cout<<"t1";break;
        case 7:std::cout<<"t2";break;
        case 8:std::cout<<"s0/fp";break;
        case 9:std::cout<<"s1";break;
        case 10:std::cout<<"a0";break;
        case 11:std::cout<<"a1";break;
        case 12:std::cout<<"a2";break;
        case 13:std::cout<<"a3";break;
        case 14:std::cout<<"a4";break;
        case 15:std::cout<<"a5";break;
        case 16:std::cout<<"a6";break;
        case 17:std::cout<<"a7";break;
        case 18:std::cout<<"s2";break;
        case 19:std::cout<<"s3";break;
        case 20:std::cout<<"s4";break;
        case 21:std::cout<<"s5";break;
        case 22:std::cout<<"s6";break;
        case 23:std::cout<<"s7";break;
        case 24:std::cout<<"s8";break;
        case 25:std::cout<<"s9";break;
        case 26:std::cout<<"s10";break;
        case 27:std::cout<<"s11";break;
        case 28:std::cout<<"t3";break;
        case 29:std::cout<<"t4";break;
        case 30:std::cout<<"t5";break;
        case 31:std::cout<<"t6";break;
    }
}

void printSTORE(u32 st_dest,u32 reg2){
	printf("st_dest: %d reg2 : %d \n",st_dest,reg2);
}

void printID_EX_Buffer(ID_EX_buffer &t){
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
}

#endif