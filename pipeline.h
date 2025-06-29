#ifndef PIPELINE_H__
#define PIPELINE_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct{
    int endereco;
} Pc;

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

} Pipeline_estagio_1;

typedef struct{
    int pc;
    int A;
    int B;
    int sel_A;
    int sel_B;
    int f_jump;
    int f_branch;
    int immediate;
    int address;
    int RegDst;
    int reg_write;
    int reg_mem;
    int mem_write;
    int mem_read;
    int ULAOp;
    int rd;
    int rt;
    int ULAFonte;
    int branch_address;

} Pipeline_estagio_2;

typedef struct{
    int pc;
    int address;
    int ULA_out;
    int f_zero;
    int f_branch;
    int mem_write;
    int mem_read;
    int rd;
    int branch_address;
    int dado_escrita;
    int reg_write;
    int reg_mem;
    int f_jump;

} Pipeline_estagio_3;

typedef struct{
    int dado_lido;
    int reg_write;
    int reg_mem;
    int ULA_out;
    int rd;

} Pipeline_estagio_4;

typedef struct{
    int dado;
}Forward;

struct Node{
    Pc pc;
    Mem_d mem_d[256];
    BancoRegistradores pBanco;
    Pipeline_estagio_1 estagio1;
    Pipeline_estagio_2 estagio2;
    Pipeline_estagio_3 estagio3;
    Pipeline_estagio_4 estagio4;
    struct Node *prox;
};


typedef struct Node nodo;


Instrucao decod(char* inst);
void carregarMemoria(Mem_p *mem_p);
void carregarMemoriaDados(Mem_d *mem_d);
void ImprimirMemoriaDados(Mem_d *mem_d);
void imprimirSimulador(BancoRegistradores *BR);
void imprimirMemoria(Mem_p *mem_p);
void imprimir_instrucao(Instrucao *i);
void imprimirPipeline(Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2, Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4, Mem_p *mem_p);
void conv_asm(FILE* arquivo_asm, Instrucao inst);
void salvar_asm(Mem_p *mem_p);
void salvar_data(Mem_d *mem_d);
void initMemorias(Mem_p *mem_p, Mem_d *mem_d);
void estagio_busca(Mem_p *mem_p, Pc *pc, Pipeline_estagio_1 *estagio1);
void estagio_decod(Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2, Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4, BancoRegistradores *banco);
void estagio_exec(Pipeline_estagio_3 *estagio3, Pipeline_estagio_2 *estagio2, Pipeline_estagio_4 *estagio4, Forward *forward);
void estagio_memoria(Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4, Pc *pc, Mem_d *mem_d, Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2);
void estagio_writeback(BancoRegistradores *banco, Pipeline_estagio_4 *estagio4, Forward *forward);
void pipelineInit(Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2, Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4);
void push(nodo *p, Pc *pPc, Mem_d *pMem_d, BancoRegistradores *pBanco, Pipeline_estagio_1 *pEstagio1, Pipeline_estagio_2 *pEstagio2, Pipeline_estagio_3 *pEstagio3, Pipeline_estagio_4 *pEstagio4);
nodo *pop(nodo *p);
nodo *alocaNodo();
int estaVazia(nodo *p);


#endif /* PIPELINE_H__*/
