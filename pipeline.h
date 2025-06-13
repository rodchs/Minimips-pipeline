#ifndef PIPELINE_H__
#define PIPELINE_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct{
    char inst[17];
} Mem_p;

typedef struct{
    int dado;
} Mem_d;

typedef struct {
    int reg[8];
} BancoRegistradores;

typedef struct {
    int tipo;         // 1 = R, 2 = I, 3 = J
    int opcode;
    int rs;
    int rt;
    int rd;
    int funct;
    int immediate;
    int address;
} Instrucao;

typedef struct{
    int pc;
    char inst[17];
} pipeline_estagio_1;

typedef struct{
    int pc;
    int A;
    int B;
    int f_jump;
    Instrucao inst;
} pipeline_estagio_2;

typedef struct{
    int pc;
    int address;
    int ULA_out;
    int f_zero;
    int f_jump;
    int f_branch;
    int rd;
    int branch_address;
    int dado_escrita;
} pipeline_estagio_3;

void controle(int *estagio_inst, pipeline_estagio_1 *estagio_1, pipeline_estagio_2 *estagio_2, int *pc, Mem_p *mem_p, BancoRegistradores *BR);int ula(int a, int b, int op);
void carregarMemoria(Mem_p *mem_p);
void carregarMemoriaDados(Mem_d *mem_d);
void ImprimirMemoriaDados(Mem_d *mem_d);
void imprimirSimulador(BancoRegistradores *BR);
void imprimirMemoria(Mem_p *mem_p);
void conv_asm(FILE* arquivo_asm, Instrucao inst);
void salvar_asm(Mem_p *mem_p);
void salvar_data(Mem_d *mem_d);
void initMemorias(Mem_p *mem_p, Mem_d *mem_d);


#endif /* PIPELINE_H__*/
