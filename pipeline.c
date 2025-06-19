#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pipeline.h"

void estagio_busca(Mem_p *mem_p, Pc *pc, Pipeline_estagio_1 *estagio1){ 
    strncpy(estagio1->inst, mem_p[pc->endereco].inst, 17);
    estagio1->pc = pc->endereco;
    estagio1->check = 1;
    pc->endereco++;
}

void estagio_decod(Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2, BancoRegistradores *banco){
    if(estagio1->check != 1) return;

    // DECODER
    Instrucao inst = decod(estagio1->inst);
    estagio2->A = banco->reg[inst.rs];
    estagio2->B = banco->reg[inst.rt];
    estagio2->dado_escrita = banco->reg[inst.rt];
    estagio2->pc = estagio1->pc;
    estagio2->rt = inst.rt;
    estagio2->immediate = inst.immediate;
    estagio2->rd = inst.rd;
    estagio2->address = inst.address;
    // CONTROLE
    estagio2->ULAOp =(inst.tipo == 1) ? inst.funct : 0;
    estagio2->ULAFonte = (inst.tipo == 2) ? 1 : 0;
    estagio2->RegDst = (inst.opcode == 0) ? 1 : 0;
    estagio2->f_jump = (inst.tipo == 3) ? 1 : 0;
    estagio2->f_branch = (inst.opcode == 8) ? 1 : 0;
    estagio2->mem_write = (inst.opcode == 15) ? 1 : 0;
    estagio2->reg_write = (inst.tipo == 1 || inst.opcode == 4 || inst.opcode == 11) ? 1 : 0;
    estagio2->reg_mem = (inst.opcode == 11) ? 1 : 0;

    estagio2->check = 1;

}

void estagio_exec(Pipeline_estagio_3 *estagio3, Pipeline_estagio_2 *estagio2){

    if(estagio2->check != 1) return;

    if(estagio2->ULAFonte == 1){
        estagio3->ULA_out = ula(estagio2->A, estagio2->immediate, estagio2->ULAOp);
    }else{
        estagio3->ULA_out = ula(estagio2->A, estagio2->B, estagio2->ULAOp);
    }
    estagio3->address = estagio2->address;
    estagio3->branch_address = estagio2->immediate;
    estagio3->f_jump = estagio2->f_jump;
    estagio3->pc = estagio2->pc;
    estagio3->rd = (estagio2->RegDst = 1) ? estagio2->rt : estagio2->rd;
    estagio3->dado_escrita = estagio2->dado_escrita;
    estagio3->reg_write = estagio2->reg_write;
    estagio3->mem_write = estagio2->mem_write;
    estagio3->reg_mem = estagio2->reg_mem;
    estagio3->f_branch = estagio2->f_branch;
    estagio3->f_zero = ula(estagio2->A, estagio2->B, 4); 
    
    estagio3->check = 1;
}

void estagio_memoria(Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4, Pc *pc, Mem_d *mem_d){
    if(estagio3->check != 1) return;
    
    // ACESSO A MEMORIA
    
    estagio4->dado_lido = mem_d[estagio3->ULA_out].dado;
    
    if(estagio3->mem_write == 1){
        mem_d[estagio3->ULA_out].dado = estagio3->dado_escrita;
    }
    
    estagio4->reg_write = estagio3->reg_write;
    estagio4->reg_mem = estagio3->reg_mem;
    estagio4->ULA_out = estagio3->ULA_out;
    estagio4->rd = estagio3->rd;

    estagio4->check = 1;

    // DESVIO

    if(estagio3->f_branch == 1 && estagio3->f_zero == 1){
        pc->endereco = estagio3->branch_address;
        return;
    }

    if(estagio3->f_jump == 1){
        pc->endereco = estagio3->address;
        return;
    }
}

void estagio_writeback(BancoRegistradores *banco, Pipeline_estagio_4 *estagio4){

    if(estagio4->check != 1) return;

    if(estagio4->reg_write == 1){
        if(estagio4->reg_mem == 1){
            banco->reg[estagio4->rd] = estagio4->dado_lido;
        }
        else{
            banco->reg[estagio4->rd] = estagio4->ULA_out;
        }
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

        strncpy(buffer, inst + 4, 3); buffer[3] = '\0';
        i.rs = strtol(buffer, NULL, 2);

        strncpy(buffer, inst + 7, 3); buffer[3] = '\0';
        i.rt = strtol(buffer, NULL, 2);
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
            break;
        case 1: //sub
            return a - b;
            break;
        case 2: //and
            return a & b;
            break;
        case 3: //or
            return a | b;
            break;
        case 4:
            return ((a - b == 0)) ? 1 : 0;
            break;
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
