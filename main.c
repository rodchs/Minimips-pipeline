#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pipeline.h"
#include "pipeline.c"

Mem_p mem_p[256];
Mem_d mem_d[256];
int pc = 0;


void executar_instrucao(char* bin_instr, BancoRegistradores *BR, Mem_d *mem_d) {
    Instrucao inst = decod(bin_instr);
    int pulou = 0;
    
    printf("inst: %s\n", bin_instr);
    printf("opcode: %d\n", inst.opcode);
    printf("tipo: %d\n", inst.tipo);
    printf("rs: %d\n", inst.rs);
    printf("rt: %d\n", inst.rt);
    printf("im: %d\n", inst.immediate);
    
    switch(inst.tipo) {
        case 1: // R
        if (inst.funct == 0) { // and
            BR->reg[inst.rd] = ula(BR->reg[inst.rs], BR->reg[inst.rt], 0);
        } else if (inst.funct == 1) { // or
            BR->reg[inst.rd] = ula(BR->reg[inst.rs], BR->reg[inst.rt], 1);
        } else if (inst.funct == 2) { // add
            BR->reg[inst.rd] = ula(BR->reg[inst.rs], BR->reg[inst.rt], 2);
        } else if (inst.funct == 3) { // sub
            BR->reg[inst.rd] = ula(BR->reg[inst.rs], BR->reg[inst.rt], 4);
        }
        break;
        
        case 2: // I
        if (inst.opcode == 8) { // beq
            if (BR->reg[inst.rs] == BR->reg[inst.rt]){
                pc += (inst.immediate*2);
                pulou = 1;
            }
        } else if (inst.opcode == 11) { // lw
            BR->reg[inst.rt] = mem_d[BR->reg[inst.rs] + inst.immediate].dado;
        } else if (inst.opcode == 15) { // sw
            int val = BR->reg[inst.rt];
            mem_d[BR->reg[inst.rs] + inst.immediate].dado = BR->reg[inst.rt];
        } else if(inst.opcode == 4){
            BR->reg[inst.rt] = BR->reg[inst.rs];
        }
        break;
        case 3: // J
        pc = inst.address;
        break;
    }
    if (!pulou) {
        pc++; // Atualiza PC para a próxima instrução
    }
}


int main() {
    initMemorias(&mem_p, &mem_d);
    BancoRegistradores BR = {{0}};
    int c = 1;
    while (c) {
        int m;
        
        printf("\n MINI-MIPS 8 BITS - UNIPAMPA\n"); 
        printf("1. Carregar memoria\n"); 
        printf("2. Carregar memoria de dados\n");
        printf("3. Imprimir memoria \n");
        printf("4. Imprimir memoria de dados\n");
        printf("5. Imprimir registradores \n"); 
        printf("6. Imprimir todo o simulador \n"); 
        printf("7. Salvar .asm \n"); 
        printf("8. Salvar .data \n"); 
        printf("9. Executa Programa (run)\n"); 
        printf("10. Executa uma instrucao (Step)\n"); 
        printf("11. Volta uma instrucao (Back)\n"); 
        printf("0. Sair \n"); 
        printf("Escolha uma opcao: "); 
        setbuf(stdin, NULL);
        scanf("%d", &m);
        
        switch(m){
            case 1: //carregar memroia
                carregarMemoria(&mem_p);
                break;
            case 2: //carregar memoria de dados
                carregarMemoriaDados(&mem_d);
                break;
            case 3: //imprimir memoria
                imprimirMemoria(&mem_p);
                break;
            case 4: //imprimir memoria de dados
                ImprimirMemoriaDados(&mem_d); 
                break;
            case 5: //imprimir registradores
                imprimirSimulador(&BR); 
                break;
            case 6: // imprimir tudo
                imprimirMemoria(&mem_p);
                ImprimirMemoriaDados(&mem_d);
                imprimirSimulador(&BR);
                break;
            case 7: //Salvar .asm
                salvar_asm(&mem_p);
                break;
            case 8: //Salvar .data
                salvar_data(&mem_d);
                break;
            case 9: //run
                while (pc < 256 && strlen(mem_p[pc].inst) > 0) {
                    executar_instrucao(mem_p[pc].inst, &BR, &mem_d);
                }
                break;
            case 10: // step
            if (strlen(mem_p[pc].inst) > 0) {
                executar_instrucao(mem_p[pc].inst, &BR, &mem_d);  // Executa uma instrução
                printf("Executando instrucao no PC = %d\n", pc);
            }
            break;
            case 0:
                printf("Programa finalizado.\n");
                c = 0;
                break;
            default:
                printf("Opcao invalida.\n");
        }
    }
    return 0;
}
