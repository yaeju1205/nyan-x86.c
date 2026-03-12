#include "nyan_x86.h"
#include <stdint.h>

uint8_t x86_emit_opcode_reg_ptr(
    uint8_t* buffer,
    int* pos,
    X86_OPCode opcode,
    X86_Register reg,
    uint8_t ptr
) {
    buffer[(*pos)++] = opcode;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, reg, ptr);
}

uint8_t x86_emit_opcode_mem_byte(
    uint8_t* buffer,
    int* pos,
    X86_OPCode opcode,
    X86_Register reg,
    uint8_t offset
) {
    buffer[(*pos)++] = opcode;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_MEM_BYTE, reg, offset);
}

uint8_t x86_emit_opcode_mem_dword(
    uint8_t* buffer,
    int* pos,
    X86_OPCode opcode,
    X86_Register reg,
    uint8_t rm,
    uint32_t offset
) {
    buffer[(*pos)++] = opcode;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_MEM_DWORD, reg, rm);
    *(uint32_t*)(&buffer[*pos]) = offset; 
    *pos += 4;
}

uint8_t x86_emit_opcode_reg_reg(
    uint8_t* buffer,
    int* pos,
    X86_OPCode opcode,
    X86_Register reg,
    X86_Register rm
) {
    buffer[(*pos)++] = opcode;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, reg, rm);
}

void x86_emit_mov_rr(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src) {
    x86_emit_opcode_reg_reg(buffer, pos, X86_LOAD, src, dst);
}

void x86_64_emit_mov_rr(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src) {
    uint8_t rex = 0x48;
    if (src >= 8) rex |= 0x04;
    if (dst >= 8) rex |= 0x01;
    buffer[(*pos)++] = rex;
    x86_emit_opcode_reg_reg(buffer, pos, X86_LOAD, src, dst);
}

void x86_emit_mov_ri32(uint8_t* buffer, int* pos, X86_Register dst, uint32_t imm) {
    buffer[(*pos)++] = 0xB8 + dst; 
    *(uint32_t*)(&buffer[*pos]) = imm;
    *pos += 4;
}

void x86_64_emit_mov_ri64(uint8_t* buffer, int* pos, X86_64_Register dst, uint64_t imm) {
    uint8_t rex = 0x48;
    if (dst >= 8) rex |= 0x01;
    buffer[(*pos)++] = rex;
    buffer[(*pos)++] = 0xB8 + dst;
    *(uint64_t*)(&buffer[*pos]) = imm;
    *pos += 8;
}

void x86_emit_mov_rm(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src) {
    buffer[(*pos)++] = X86_LOAD;
    if (src == 4) {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, dst, 4);
        buffer[(*pos)++] = 0x24;
    } 
    else if (src == 5) {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_MEM_BYTE, dst, 5);
        buffer[(*pos)++] = 0x00;
    } 
    else {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, dst, src);
    }
}

void x86_64_emit_mov_rm(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src) {
    uint8_t rex = 0x48;
    if (src >= 8) rex |= 0x04;
    if (dst >= 8) rex |= 0x01;
    buffer[(*pos)++] = rex;
    buffer[(*pos)++] = X86_LOAD;
    if (src == 4) {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, dst, 4);
        buffer[(*pos)++] = 0x24;
    } 
    else if (src == 5) {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_MEM_BYTE, dst, 5);
        buffer[(*pos)++] = 0x00;
    } 
    else {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, dst, src);
    }
}

void x86_emit_mov_mr(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src) {
    buffer[(*pos)++] = X86_STORE;
    if (src == 4) {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, dst, 4);
        buffer[(*pos)++] = 0x24;
    } 
    else if (src == 5) {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_MEM_BYTE, dst, 5);
        buffer[(*pos)++] = 0x00;
    } 
    else {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, dst, src);
    }
}

void x86_64_emit_mov_mr(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src) {
    uint8_t rex = 0x48;
    if (src >= 8) rex |= 0x04;
    if (dst >= 8) rex |= 0x01;
    buffer[(*pos)++] = rex;
    buffer[(*pos)++] = X86_STORE;
    if (src == 4) {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, dst, 4);
        buffer[(*pos)++] = 0x24;
    } 
    else if (src == 5) {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_MEM_BYTE, dst, 5);
        buffer[(*pos)++] = 0x00;
    } 
    else {
        buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_PTR, dst, src);
    }
}

void x86_emit_add_rr(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src) {
    buffer[(*pos)++] = X86_ADD;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, src, dst);
}

void x86_64_emit_add_rr(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src) {
    uint8_t rex = 0x48;
    if (src >= 8) rex |= 0x04;
    if (dst >= 8) rex |= 0x01;
    buffer[(*pos)++] = rex;
    buffer[(*pos)++] = X86_ADD;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, src, dst);
}

void x86_emit_add_ri32(uint8_t* buffer, int* pos, X86_Register dst, uint32_t imm) {
    buffer[(*pos)++] = X86_GROUP1;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, X86_GROUP1_ADD, dst);
    if ((int32_t)imm >= -128 && (int32_t)imm <= 127) {
        buffer[(*pos)++] = (uint8_t)imm;
    }
    else {
        *(uint32_t*)(&buffer[*pos]) = imm;
        *pos += 4;
    }
}

void x86_64_emit_add_ri32(uint8_t* buffer, int* pos, X86_64_Register dst, uint32_t imm) {
    uint8_t rex = 0x48;
    if (dst >= 8) rex |= 0x01;
    buffer[(*pos)++] = rex;
    buffer[(*pos)++] = X86_GROUP1;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, X86_GROUP1_ADD, dst);
    if ((int32_t)imm >= -128 && (int32_t)imm <= 127) {
        buffer[(*pos)++] = (uint8_t)imm;
    }
    else {
        *(uint32_t*)(&buffer[*pos]) = imm;
        *pos += 4;
    }
}

void x86_emit_sub_rr(uint8_t* buffer, int* pos, X86_Register dst, X86_Register src) {
    buffer[(*pos)++] = X86_SUB;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, src, dst);
}

void x86_64_emit_sub_rr(uint8_t* buffer, int* pos, X86_64_Register dst, X86_64_Register src) {
    uint8_t rex = 0x48;
    if (src >= 8) rex |= 0x04;
    if (dst >= 8) rex |= 0x01;
    buffer[(*pos)++] = rex;
    buffer[(*pos)++] = X86_SUB;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, src, dst);
}

void x86_emit_sub_ri32(uint8_t* buffer, int* pos, X86_Register dst, uint32_t imm) {
    buffer[(*pos)++] = X86_GROUP1;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, X86_GROUP1_SUB, dst);
    if ((int32_t)imm >= -128 && (int32_t)imm <= 127) {
        buffer[(*pos)++] = (uint8_t)imm;
    }
    else {
        *(uint32_t*)(&buffer[*pos]) = imm;
        *pos += 4;
    }
}

void x86_64_emit_sub_ri32(uint8_t* buffer, int* pos, X86_64_Register dst, uint32_t imm) {
    uint8_t rex = 0x48;
    if (dst >= 8) rex |= 0x01;
    buffer[(*pos)++] = rex;
    buffer[(*pos)++] = X86_GROUP1;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, X86_GROUP1_SUB, dst);
    if ((int32_t)imm >= -128 && (int32_t)imm <= 127) {
        buffer[(*pos)++] = (uint8_t)imm;
    }
    else {
        *(uint32_t*)(&buffer[*pos]) = imm;
        *pos += 4;
    }
}

void x86_emit_imul(uint8_t* buffer, int* pos, X86_Register src) {
    buffer[(*pos)++] = X86_GROUP3;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, 5, src);
}

void x86_64_emit_imul(uint8_t* buffer, int* pos, X86_64_Register src) {
    uint8_t rex = 0x48;
    if (src >= 8) rex |= 0x04;
    buffer[(*pos)++] = rex;
    buffer[(*pos)++] = X86_GROUP3;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, 5, src);
}

void x86_64emit_cqo(uint8_t* buffer, int* pos) {
    buffer[(*pos)++] = 0x48;
    buffer[(*pos)++] = X86_CQO;
}

void x86_emit_idiv(uint8_t* buffer, int* pos, X86_Register src) {
    buffer[(*pos)++] = X86_GROUP3;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, 7, src);
}

void x86_64_emit_idiv(uint8_t* buffer, int* pos, X86_64_Register src) {
    uint8_t rex = 0x48;
    if (src >= 8) rex |= 0x01;
    buffer[(*pos)++] = X86_GROUP3;
    buffer[(*pos)++] = x86_make_modrm(X86_MOD_REG_REG, 7, src);
}

void x86_emit_ret(uint8_t* buffer, int* pos) {
    buffer[(*pos)++] = X86_RET;
}