#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pipeline.h"


void controle(int *estagio_inst, pipeline_estagio_1 *estagio_1, pipeline_estagio_2 *estagio_2, int *pc, Mem_p *mem_p, BancoRegistradores *BR) {
    if(*estagio_inst == 0){
        estagio_1->inst = inst;
        estagio_1->pc = *pc;
    } else (*estagio_inst == 1) {
        Instrucao inst = estagio_1->inst;
        estagio_2->inst_decodificada = inst;
        estagio_2->pc = estagio_1->pc;

         if (inst.tipo == 1 || inst.tipo == 2) {
            estagio_2->valor_rs = BR->reg[inst.rs];
            estagio_2->valor_rt = BR->reg[inst.rt];
        } else {
            estagio_2->valor_rs = 0;
            estagio_2->valor_rt = 0;
        }

        *estagio_inst = 2; 
    }
}

Instrucao decod(char* inst) {
    Instrucao i;
    char buffer[17];

    strncpy(buffer, inst, 4);
    buffer[4] = '\0';
    i.opcode = strtol(buffer, NULL, 2);

    if (i.opcode == 0) {
        i.tipo = 1;
        strncpy(buffer, inst + 4, 3); buffer[3] = '\0';
        i.rs = strtol(buffer, NULL, 2);

        strncpy(buffer, inst + 7, 3); buffer[3] = '\0';
        i.rt = strtol(buffer, NULL, 2);

        strncpy(buffer, inst + 10, 3); buffer[3] = '\0';
        i.rd = strtol(buffer, NULL, 2);

        strncpy(buffer, inst + 13, 3); buffer[3] = '\0';
        i.funct = strtol(buffer, NULL, 2);
    } else if (i.opcode == 2) {
        i.tipo = 3;
        strncpy(buffer, inst + 4, 12); buffer[12] = '\0';
        i.address = strtol(buffer, NULL, 2);
    } else {
        i.tipo = 2;
        strncpy(buffer, inst + 4, 3); buffer[3] = '\0';
        i.rs = strtol(buffer, NULL, 2);

        strncpy(buffer, inst + 7, 3); buffer[3] = '\0';
        i.rt = strtol(buffer, NULL, 2);

        strncpy(buffer, inst + 10, 6); buffer[6] = '\0';
        int imm = strtol(buffer, NULL, 2);
        if (inst[10] == '1') imm -= 64;
        i.immediate = imm;
    }

    return i;
}

int ula(int a, int b, int op) {
    switch (op) {
        case 0: //add
            return a + b;
        case 1: //sub
            return a - b;
        case 2: //and
            return a & b;
        case 3: //or
            return a | b;
        default:
            printf("Operacao invalida.\n"); 
            return 0;
    }
}

void initMemorias(Mem_p *mem_p, Mem_d *mem_d){
    for(int i = 0; i < 256; i++){
        strcpy(mem_p[i].inst, "0000000000000000\0");
        mem_d[i].dado = 0;
    }
}

void carregarMemoria(Mem_p *mem_p) {
    char arquivo[50];
    printf("Digite o nome do arquivo que deseja abrir: ");
    scanf("%s", arquivo);

    FILE *f = fopen(arquivo, "r");
    if (f == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    int i = 0;
    char linha[17];
    while (fgets(linha, 16 + 1, f)) { 
        if(linha[0] == '\n') continue;

        char *pos; // Ponteiro para a posição do caractere de nova linha
        if ((pos = strchr(linha, '\n')) != NULL) {
            *pos = '\0'; 
        }
        strncpy(mem_p[i].inst, linha, 17); // Copia a linha para a memória de programa
        i++;
    }
    fclose(f);
    printf("Memoria de programa carregada com sucesso\n");
}

void carregarMemoriaDados(Mem_d *mem_d){
    char arquivo[50];
    printf("Digite o nome do arquivo que deseja abrir: ");
    scanf("%s", arquivo);

    FILE *f = fopen(arquivo, "r");
    if (f == NULL){
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivo);
        return;
    }
    int valor;
    int i = 0;
    while((fscanf(f, "%d\n", &valor)) == 1) {
        mem_d[i].dado = valor;
        i++;
    }
}

void ImprimirMemoriaDados(Mem_d *mem_d){
    for(int i = 0; i < 256; i++) {
        printf("%d: %d\n", i, mem_d[i].dado);
    }
}

void imprimirSimulador(BancoRegistradores *BR) {
    printf("Conteudo do banco de registradores:\n");
    for (int i = 0; i < 8; i++) {
        printf("R%d: %d\n", i, BR->reg[i]);
    }
}

void imprimirMemoria(Mem_p *mem_p) {
    for (int i = 0; i < 256; i++) {
        printf("mem_p[%i]: %s\n", i, mem_p[i].inst);
    }
}

void conv_asm(FILE* arquivo_asm, Instrucao inst){
    if(inst.rd == 0 && inst.rt == 0 && inst.rs == 0){
        return;
    }
    switch (inst.tipo) {
        case 1:
            switch (inst.funct) {
                case 0:
                    fprintf(arquivo_asm, "add $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
                    break;
                case 2:
                    fprintf(arquivo_asm, "sub $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
                    break;
                case 4:
                    fprintf(arquivo_asm, "and $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
                    break;
                case 5:
                    fprintf(arquivo_asm, "or $r%d, $r%d, $r%d", inst.rd, inst.rs, inst.rt);
                    break;
                default:
                    fprintf(arquivo_asm, "Funcao R nao reconhecida: %d", inst.funct);
                    break;
            }
            break;
        case 2:
            switch (inst.opcode) {
                case 4:
                    fprintf(arquivo_asm, "addi $r%d, $r%d, %d", inst.rt, inst.rs, inst.immediate);
                    break;
                case 11:
                    fprintf(arquivo_asm, "lw $r%d, %d($r%d)", inst.rt, inst.immediate, inst.rs);
                    break;
                case 15:
                    fprintf(arquivo_asm, "sw $r%d, %d($r%d)", inst.rt, inst.immediate, inst.rs);
                    break;
                case 8:
                    fprintf(arquivo_asm, "beq $r%d, $r%d, %d", inst.rt, inst.rs, inst.immediate);
                    break;
                default:
                    fprintf(arquivo_asm, "Opcode I nao reconhecido: %d", inst.opcode);
                    break;
            }
            break;
        case 3:
            fprintf(arquivo_asm, "j %d", inst.address);
            break;
    }

}

void salvar_asm(Mem_p *mem_p) {
    FILE *arquivo_asm = fopen("programa.asm", "w"); 
    if (arquivo_asm == NULL) {
        printf("Erro ao criar o arquivo\n");
        return;
    }
    for (int i = 0; i < 256; i++) {
        conv_asm(arquivo_asm, decod(mem_p[i].inst));
        fprintf(arquivo_asm, "\n");
    }
    fclose(arquivo_asm);
    printf("Arquivo .asm salvo com sucesso!\n");
}

void salvar_data(Mem_d *mem_d) {
    FILE *arquivo_memoria = fopen("programa.data", "w");
    if (arquivo_memoria == NULL) {
        printf("Erro ao criar o arquivo\n");
        return;
    }

    for (int i = 0; i < 256; i++) {
        fprintf(arquivo_memoria, "Endereco de memoria[%d]: %s\n", i, mem_d[i].dado);
    }

    fclose(arquivo_memoria);
    printf("Arquivo .data salvo com sucesso!\n");
}
