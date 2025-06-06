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
    Instrucao inst;
} pipeline_estagio_1;

ontrole(int *estagio_inst, pipeline_estagio_1 *estagio_1, int *pc, Instrucao inst);
int ula(int a, int b, int op);
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