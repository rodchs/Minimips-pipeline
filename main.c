#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pipeline.h"
#include "pipeline.c"


int main() {
    Mem_d mem_d[256];
    Mem_p mem_p[256];
    
    BancoRegistradores BR = {{0}};

    nodo *p = alocaNodo();
    nodo *tmp;
    
    Pc pc;

    Forward forward;
    
    Pipeline_estagio_1 estagio1;
    Pipeline_estagio_2 estagio2;
    Pipeline_estagio_3 estagio3;
    Pipeline_estagio_4 estagio4;
    
    int c = 1;
    int m, j, i = 0;
    pc.endereco = 0;
    forward.dado = 0;
    initMemorias(&mem_p, &mem_d);
    pipelineInit(&estagio1, &estagio2, &estagio3, &estagio4);

    while (c) {
        
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
                // imprimirMemoria(&mem_p);
                // ImprimirMemoriaDados(&mem_d);
                // imprimirSimulador(&BR);
                imprimirPipeline(&estagio1, &estagio2, &estagio3, &estagio4, &mem_p);
                break;
            case 7: //Salvar .asm
                salvar_asm(&mem_p);
                break;
            case 8: //Salvar .data
                salvar_data(&mem_d);
                break;
            case 9: //run
                for(i = 0; i < 255; i++){
                    push(p, &pc, &mem_d, &BR, &estagio1, &estagio2, &estagio3, &estagio4);
                    estagio_writeback(&BR, &estagio4, &forward); 
                    estagio_memoria(&estagio3, &estagio4, &pc, &mem_d, &estagio1, &estagio2);
                    estagio_exec(&estagio3, &estagio2, &estagio4, &forward);
                    estagio_decod(&estagio1, &estagio2, &estagio3, &estagio4, &BR);  
                    estagio_busca(&mem_p, &pc, &estagio1);
                }
                break;
                case 10: // step
                push(p, &pc, &mem_d, &BR, &estagio1, &estagio2, &estagio3, &estagio4);
                estagio_writeback(&BR, &estagio4, &forward); 
                estagio_memoria(&estagio3, &estagio4, &pc, &mem_d, &estagio1, &estagio2);
                estagio_exec(&estagio3, &estagio2, &estagio4, &forward);
                estagio_decod(&estagio1, &estagio2, &estagio3, &estagio4, &BR);  
                estagio_busca(&mem_p, &pc, &estagio1);
                break;
            case 11: //back
                tmp = pop(p);
                if(tmp != NULL){
                    pc = tmp->pc;
                    BR = tmp->pBanco;
                    for(int z = 0; z<255; z++){
                        mem_d[z].dado = tmp->mem_d[z].dado;
                    }
                    estagio1 = tmp->estagio1;
                    estagio2 = tmp->estagio2;
                    estagio3 = tmp->estagio3;
                    estagio4 = tmp->estagio4;
                } else {
                    printf("A pilha esta vazia e nao podemos mais voltar.\n");
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
