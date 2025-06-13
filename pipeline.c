#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pipeline.h"

void estagio_busca(Mem_p *mem_p, int *pc, pipeline_estagio_1 *estagio1){ 
    strncpy(estagio1->inst, mem_p[*pc].inst, 17);
    pc++;
    estagio1->pc = pc;
}

void estagio_decod(int *pc, pipeline_estagio_1 *estagio1, pipeline_estagio_2 *estagio2, BancoRegistradores *banco){
    estagio2->inst = decod(estagio1->inst);
    estagio2->A = banco->reg[estagio2->inst.rs];
    estagio2->B = banco->reg[estagio2->inst.rt];
    estagio2->pc = pc;
    estagio2->f_jump = (estagio2->inst.tipo == 3) ? 1 : 0;
}

void estagio_exec(pipeline_estagio_3 *estagio3, pipeline_estagio_2 *estagio2){
    estagio3->ULA_out = ula(estagio2->A, estagio2->B, estagio2->inst.funct);
    estagio3->address = estagio2->inst.address;
    estagio3->branch_address = estagio2->inst.address + estagio2->pc;
    estagio3->f_jump = estagio2->f_jump;
    estagio3->pc = estagio2->pc;
    estagio3->rd = (estagio2->inst.opcode = 0) ? estagio2->inst.rd : estagio2->inst.rt;
    estagio3->dado_escrita = estagio2->B;

    if(estagio2->inst.tipo = 2){
        estagio3->ULA_out = ula(estagio2->A, estagio2->inst.immediate, estagio2->inst.funct);    
        estagio3->f_zero = ula(estagio2->A, estagio2->inst.immediate, 4);  
        if(estagio2->inst.opcode == 8){
            estagio3->f_branch = 1;
        }  
    }
    else{
        estagio3->ULA_out = ula(estagio2->A, estagio2->B, estagio2->inst.funct);
        estagio3->f_zero = ula(estagio2->A, estagio2->B, 4);
    }
}

void estagio_memoria(pipeline_estagio_3 *estagio3, pipeline_estagio_4 *estagio4, int *pc){
    if(estagio3->f_jump == 1){
        if(estagio3->f_branch == 1 && estagio3->f_zero == 1){
            *pc = estagio3->branch_address;
            return;
        }
        *pc = estagio3->address;
        return;
    }
    estagio4->dado_lido = mem_d[estagio3->ULA_out];
        if(estagio3->f_memwrite == 1){
            mem_d[estagio3->ULA_out = estagio3->dado_escrita];
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
        case 4:
            return ((a - b == 0)) ? 1 : 0;
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
