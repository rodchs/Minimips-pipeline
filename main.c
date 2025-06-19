#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pipeline.h"
#include "pipeline.c"


int main() {
    Mem_d mem_d[256];
    Mem_p mem_p[256];
    
    initMemorias(&mem_p, &mem_d);
    BancoRegistradores BR = {{0}};
    int c = 1;
    
    Pc pc;
    pc.endereco = 0;
    Pipeline_estagio_1 estagio1;
    Pipeline_estagio_2 estagio2;
    Pipeline_estagio_3 estagio3;
    Pipeline_estagio_4 estagio4;

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
                estagio_writeback(&BR, &estagio4); 
                printf("ESTASGIO WB\n");         
                printf("DADO_LIDO: %d|ULA_OUT: %d|REG_WRITE: %d|REG_MEM: %d| RD: %d|\n", estagio4.dado_lido, estagio4.ULA_out, estagio4.reg_write, estagio4.reg_mem, estagio4.rd); 
                estagio_memoria(&estagio3, &estagio4, &pc, &mem_d);
                printf("ESTASGIO MEM\n");         
                printf("PC_NOVO: %d|DADO_LIDO: %d|ULA_OUT: %d|REG_WRITE: %d|REG_MEM: %d| RD: %d|\n", pc, estagio4.dado_lido, estagio4.ULA_out, estagio4.reg_write, estagio4.reg_mem, estagio4.rd); 
                estagio_exec(&estagio3, &estagio2);
                printf("ESTASGIO EXE\n");         
                printf("PC: %d|ULA_OUT: %d|F_ZERO: %d|F_JMP: %d| F_BRANCH: %d|\n", estagio3.pc, estagio3.ULA_out, estagio3.f_zero, estagio3.f_jump, estagio3.f_branch); 
                estagio_decod(&estagio1, &estagio2, &BR);  
                printf("ESTASGIO DECOD\n");         
                printf("PC: %d|A: %d|B: %d|F_JMP: %d|ULAOp: %d|ULAFonte: %d|MEM_WRITE: %d\n", estagio2.pc, estagio2.A, estagio2.B, estagio2.f_jump, estagio2.ULAOp, estagio2.ULAFonte, estagio2.mem_write); 
                estagio_busca(&mem_p, &pc, &estagio1);
                printf("ESTASGIO BUSCA\n");         
                printf("PC: %d| INST: %s\n", estagio1.pc, estagio1.inst);
                break;
            case 10: // step

                // ESSE CASO ESTA SENDO USADO PARA TESTE// RODA OS 5 ESTAGIOS DE UMA UNICA INSTRUCAO DE UMA VEZ SO PARA TESTAR SE A INSTRUCAO ESTA FUNCIONANDO
                estagio_busca(&mem_p, &pc, &estagio1);
                printf("PC: %d| INST: %s\n", estagio1.pc, estagio1.inst);
                estagio_decod(&estagio1, &estagio2, &BR);  
                printf("PC: %d|A: %d|B: %d|F_JMP: %d|ULAOp: %d|ULAFonte: %d|MEM_WRITE: %d\n", estagio2.pc, estagio2.A, estagio2.B, estagio2.f_jump, estagio2.ULAOp, estagio2.ULAFonte, estagio2.mem_write); 
                estagio_exec(&estagio3, &estagio2);
                printf("PC: %d|ULA_OUT: %d|F_ZERO: %d|F_JMP: %d| F_BRANCH: %d|\n", estagio3.pc, estagio3.ULA_out, estagio3.f_zero, estagio3.f_jump, estagio3.f_branch); 
                estagio_memoria(&estagio3, &estagio4, &pc, &mem_d);
                printf("PC_NOVO: %d|DADO_LIDO: %d|ULA_OUT: %d|REG_WRITE: %d|REG_MEM: %d| RD: %d|\n", pc, estagio4.dado_lido, estagio4.ULA_out, estagio4.reg_write, estagio4.reg_mem, estagio4.rd); 
                estagio_writeback(&BR, &estagio4);         
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
