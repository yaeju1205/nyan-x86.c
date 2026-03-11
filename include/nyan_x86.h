// 구현구조
// x86_64 명령어의 경우 rex 를 먼저 생성합니다
// 그후 opcode 를 추가합니다
// 그다음 modR/M 을 추가합니다
// 추가로 sib 혹은 imm 이 추가된다면 추가합니다
#ifndef NYAN_ASM_X86_CODE_H
#define NYAN_ASM_X86_CODE_H

#include <stdint.h>

// x86 의 mod 입니다
// modR/M 을 구성할때 사용합니다 
typedef const enum {
    X86_MOD_REG_PTR = 0b00,
    X86_MOD_MEM_BYTE = 0b01,
    X86_MOD_MEM_DWORD = 0b10,
    X86_MOD_REG_REG = 0b11,
}  X86_Mod;

// GROUP1 은 뒤에 붙는 ModR/M 에 따라 동작이 다릅니다
// 예를들어 ModR/M 에서 reg 가 0 이라면 add 를 합니다
typedef enum {
    X86_GROUP1_ADD = 0,
    X86_GROUP1_OR  = 1,
    X86_GROUP1_ADC = 2,
    X86_GROUP1_SBB = 3,
    X86_GROUP1_AND = 4,
    X86_GROUP1_SUB = 5,
    X86_GROUP1_XOR = 6,
    X86_GROUP1_CMP = 7
} X86_OP_GROUP1;

// GROUP3 은 뒤에 붙는 ModR/M 에 따라 연산이 정해집니다
// 예를들어 ModR/M 에서 reg 가 5 라면 imul 입니다 
typedef enum {
    X86_GROUP3_NOT  = 2,
    X86_GROUP3_NEG  = 3,
    X86_GROUP3_MUL  = 4,
    X86_GROUP3_IMUL = 5,
    X86_GROUP3_DIV  = 4,
    X86_GROUP3_IDIV = 5,
} X86_OP_GROUP3;

// x86 의 opcode 들입니다 mov add 등이 있습니다
typedef const enum {
    X86_GROUP1 = (uint8_t)0x81,
    X86_GROUP3 = (uint8_t)0xF7,
    X86_ADD    = (uint8_t)0x01,
    X86_SUB    = (uint8_t)0x29,
    X86_CQO    = (uint8_t)0x99,
    X86_AND    = (uint8_t)0x21,
    X86_OR     = (uint8_t)0x09,
    X86_XOR    = (uint8_t)0x31,
    X86_CMP    = (uint8_t)0x39,
    X86_LOAD   = (uint8_t)0x8b,
    X86_STORE  = (uint8_t)0x89,
    X86_RET    = (uint8_t)0xc3,
} X86_OPCode;

// x86 의 레지스터 코드들 입니다
// x86 함수들에서 사용 가능합니다
typedef const enum {
    X86_EAX = (uint8_t)0b000, // 0x0
    X86_ECX = (uint8_t)0b001, // 0x1
    X86_EDX = (uint8_t)0b010, // 0x2
    X86_EBX = (uint8_t)0b011, // 0x3
    X86_ESP = (uint8_t)0b100, // 0x4
    X86_EBP = (uint8_t)0b101, // 0x5
    X86_ESI = (uint8_t)0b110, // 0x6
    X86_EDI = (uint8_t)0b111, // 0x7
} X86_Register;

// x86_64 의 확장 레지스터 코드들 입니다
// x86_64 함수들에서 사용 가능합니다
typedef const enum {
    X86_64_RAX = (uint8_t)0b000,  // 0x0
    X86_64_RCX = (uint8_t)0b001,  // 0x1
    X86_64_RDX = (uint8_t)0b010,  // 0x2
    X86_64_RBX = (uint8_t)0b011,  // 0x3
    X86_64_RSP = (uint8_t)0b100,  // 0x4
    X86_64_RBP = (uint8_t)0b101,  // 0x5
    X86_64_RSI = (uint8_t)0b110,  // 0x6
    X86_64_RDI = (uint8_t)0b111,  // 0x7
    X86_64_R8 =  (uint8_t)0b1000, // 0x8
    X86_64_R9 =  (uint8_t)0b1001, // 0x9
    X86_64_R10 = (uint8_t)0b1010, // 0xA
    X86_64_R11 = (uint8_t)0b1011, // 0xB
    X86_64_R12 = (uint8_t)0b1100, // 0xC
    X86_64_R13 = (uint8_t)0b1101, // 0xD
    X86_64_R14 = (uint8_t)0b1110, // 0xE
    X86_64_R15 = (uint8_t)0b1111, // 0xF
} X86_64_Register;

// modR/M 바이트를 생성하는 함수입니다
// modR/M 에 대하여
// opcode 를 앞에 hex 로 두고 그 뒤에 modR/M 코드를 두면 됩니다
// modR/M 의 구성은 mod (2bit) source (3bit) dest (3bit)
// 이때의 mod 구성은 11 이면 레지스터, 레지스터 00 이면 메모리 메모리 이런식입니다
// source 와 dst 에는 메모리 주소 혹은 레지스터 번호가 포함됩니다
// mod(2bit) | Reg(3bit) | R/M(3bit)
static inline uint8_t x86_make_modrm(X86_Mod mod, X86_Register reg, uint8_t rm) {
    // 0x7(0111) & reg 를 하여 3 자리 까지만 보고 다 지워버립니다
    // 왜냐하면 rex 플레그를 넣고 확장 레지스터를 사용할경우
    // 3bit 가 아닌 4bit 가 들어오기에 잘라내야합니다
    // 0x3(0011) & mod 또한 같은 이유입니다
    // -> mod: 2bit, reg: 3bit, rm: 3bit
    return (uint8_t)(((mod & 0x3) << 6) | ((reg & 0x7) << 3) | (rm & 0x7));
}

// modR/M 에서 mod 를 0b00 으로 지정한 상태입니다
// 즉 레지스터의 포인터를 사용하는 상태입니다
// [reg] 형태
uint8_t x86_emit_opcode_reg_ptr(uint8_t* buffer, int* pos, X86_OPCode opcode, X86_Register reg, uint8_t ptr);

// modR/M 에서 mod 를 0b01 으로 지정한 상태입니다
// 즉 레지스터 포인터 + 바이트입니다
// [reg + 8bit] 형태
uint8_t x86_emit_opcode_mem_byte(uint8_t* buffer, int* pos, X86_OPCode opcode, X86_Register reg, uint8_t offset);

// modR/M 에서 mod 를 0b10 으로 지정한 상태입니다
// 즉 레지스터 포인터 + 더블워드입니다
// [reg + 32bit] 형태
uint8_t x86_emit_opcode_mem_dword(uint8_t* buffer, int* pos, X86_OPCode opcode, X86_Register reg, uint8_t rm, uint32_t offset);

// modR/M 에서 mod 를 0b11 으로 지정한 상태입니다
// 즉 레지스터와 레지스터입니다
uint8_t x86_emit_opcode_reg_reg(uint8_t* buffer, int* pos, X86_OPCode opcode, X86_Register reg, X86_Register rm);

// 레지스터 간 이동
// x86_64 에서는 확장을 위해 rex(Register Extension) 을 사용합니다
// src 혹은 dst 가 r8, r9 ... 들을 사용 한다면 rex 플래그로 줘야합니다
// rex 는 8bit 에 x86_64 를 사용하는지 (0x08)
// 4비트에 src 가 x86_64 를 사용하는지 (0x04)
// 1비트에 dst 가 x86_64 를 사용하는지 (0x01)
// 이렇게 플래그를 받습니다
void x86_emit_mov_rr(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src);
void x86_64_emit_mov_rr(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src);

// 레지스터에 상수 넣기
void x86_emit_mov_ri32(uint8_t* buffer, int* pos, X86_Register dst, uint32_t imm);
void x86_64_emit_mov_ri64(uint8_t* buffer, int* pos, X86_64_Register dst, uint64_t imm);

// 메모리에서 읽기 (mov dst [src])
// rsp 혹은 rbp 에 sib 를 추가합니다
// sib 는 scale(2bit) index(3bit) base(3bit) 로 1byte 로 구현됩니다
// 포인터 계산구조 [ base + index * scale + disp ] 를 따릅니다
// esp ebp 등의 레지스터들에 우회구현, 특수구현에 사용됩니다
// [ebp] 가 고정되어 얻어짐 -> [ebp + 0(disp)] 으로 획득
// [esp] 에는 index 를 0b100 으로 넣을시 base 만을 따름
// 이때의 base 는 100 즉 4(esp)
void x86_emit_mov_rm(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src);
void x86_64_emit_mov_rm(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src);

// 메모리에서 레지스터로 로드
void x86_emit_mov_mr(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src);
void x86_64_emit_mov_mr(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src);

// ALU(Reg, Reg) or ALU(Reg, IMM)
// 레지스터와 레지스터의 값을 더합니다
void x86_emit_add_rr(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src);
void x86_64_emit_add_rr(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src);
void x86_emit_add_ri32(uint8_t* buffer, int* pos, X86_Register dst, uint32_t imm);
void x86_64_emit_add_ri32(uint8_t* buffer, int* pos, X86_64_Register dst, uint32_t imm);

// 레지스터와 레지스터의 값을 뺍니다
void x86_emit_sub_rr(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src);
void x86_64_emit_sub_rr(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src);
void x86_emit_sub_ri32(uint8_t* buffer, int* pos, X86_Register dst, uint32_t imm);
void x86_64_emit_sub_ri32(uint8_t* buffer, int* pos, X86_64_Register dst, uint32_t imm);

// eax 와 src 를 곱합니다
// group3 에서 5(imul) 을 사용합니다
// eax 와 ebx 에 저장됩니다
void x86_emit_imul(uint8_t* buffer, int* pos, X86_Register src);
void x86_64_emit_imul(uint8_t* buffer, int* pos, X86_64_Register src);

// rax 데이터를 rdx 로 옮깁니다
void x86_64emit_cqo(uint8_t* buffer, int* pos);

// eax 와 src 를 나눕니다
// group3 에서 7(idiv) 을 사용합니다
// 몫은 rax 나머지는 rdx 에 저장됩니다
void x86_emit_idiv(uint8_t* buffer, int* pos, X86_Register src);
void x86_64_emit_idiv(uint8_t* buffer, int* pos, X86_64_Register src);

void x86_emit_ret(uint8_t* buffer, int* pos);

#endif