#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pipeline.h"


nodo *alocaNodo(){
    nodo *novo = (nodo *) malloc(sizeof(nodo));
    if(!novo){
        printf("Erro ao alocar nodo");
    }else{
        novo->prox = NULL;
        return novo;
    }
}

int estaVazia(nodo *p){
    if(p->prox == NULL) return 1;

    else return 0;
}

void push(nodo *p, Pc *pPc, Mem_d *pMem_d, BancoRegistradores *pBanco, Pipeline_estagio_1 *pEstagio1, Pipeline_estagio_2 *pEstagio2, Pipeline_estagio_3 *pEstagio3, Pipeline_estagio_4 *pEstagio4){

    nodo *novo = alocaNodo();
    novo->prox = NULL;
    for(int z = 0; z<255; z++){
    novo->mem_d[z].dado = pMem_d[z].dado;
    }
    novo->pBanco = *pBanco;
    novo->pc = *pPc;
    novo->estagio1 = *pEstagio1;
    novo->estagio2 = *pEstagio2;
    novo->estagio3 = *pEstagio3;
    novo->estagio4 = *pEstagio4;

    if(estaVazia(p)){
        p->prox = novo;
    } else{
        nodo *tmp = p->prox;
        while(tmp->prox != NULL){
            tmp = tmp->prox;
        }
        tmp->prox = novo;
    }
}

nodo *pop(nodo *p){
    if(p->prox == NULL){
        return NULL;
    }else{
        nodo *ult = p->prox, *penult = p;
        
        while(ult->prox != NULL){
            penult = ult;
            ult = ult->prox;
        }
        
        penult->prox = NULL;
        return ult;
    }
}

void pipelineInit(Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2, Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4){
    estagio1->pc = 0;
    estagio2->A  = 0;
    estagio2->B  = 0;
    estagio2->pc = 0;
    estagio2->rt  = 0;
    estagio2->immediate  = 0;
    estagio2->rd  = 0;
    estagio2->address  = 0;
    estagio2->sel_A = 0 ;
    estagio2->sel_B = 0 ;
    estagio2->ULAOp = 0 ;
    estagio2->ULAFonte = 0 ;
    estagio2->RegDst = 0 ;
    estagio2->f_jump = 0 ;
    estagio2->f_branch = 0 ;
    estagio2->mem_write = 0 ;
    estagio2->reg_write = 0 ;
    estagio2->reg_mem = 0 ;
    estagio3->address = 0;
    estagio3->branch_address = 0;
    estagio3->f_jump = 0;
    estagio3->pc = 0;
    estagio3->rd = 0;
    estagio3->dado_escrita = 0;
    estagio3->reg_write = 0;
    estagio3->mem_write = 0;
    estagio3->reg_mem = 0;
    estagio3->f_branch = 0;
    estagio3->f_zero = 0;
    estagio3->ULA_out = 0;
    estagio4->dado_lido = 0;
    estagio4->reg_write = 0;
    estagio4->reg_mem = 0;
    estagio4->ULA_out = 0;
    estagio4->rd = 0;


}

void estagio_busca(Mem_p *mem_p, Pc *pc, Pipeline_estagio_1 *estagio1){ 
    strncpy(estagio1->inst, mem_p[pc->endereco].inst, 17);
    estagio1->pc = pc->endereco;
    pc->endereco++;
    printf("----------------Executando estagio IF----------------\n");
    printf("Instrucao Buscada: %s\n", estagio1->inst);
    printf("PC: %d\n", pc->endereco);
    printf("-----------------------------------------------------\n");
    printf("\n");
    
}

void estagio_decod(Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2, Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4,BancoRegistradores *banco){
    
    int nop;
    int hazard = 0;
    // DECODER
    if(strcmp(estagio1->inst, "0000000000000000") == 0){
        nop = 1;
    } else {
        nop = 0;
    }
    Instrucao inst = decod(estagio1->inst);
    estagio2->A = banco->reg[inst.rs];
    estagio2->B = banco->reg[inst.rt];
    estagio2->pc = estagio1->pc;
    estagio2->rt = inst.rt;
    estagio2->immediate = inst.immediate;
    estagio2->rd = inst.rd;
    estagio2->address = inst.address;

    // CONTROLE
    
    if(estagio3->rd == inst.rs){
        estagio2->sel_A = 1;
        hazard = 1;
    } else  if(estagio4->rd == inst.rs){
        estagio2->sel_A = 2;
    } else {
        estagio2->sel_A = 0;
    }

    if(estagio3->rd == inst.rt){
        estagio2->sel_B = 1;
        hazard = 1;
    } else if(estagio4->rd == inst.rt){
        estagio2->sel_B = 2;
    } else{
        estagio2->sel_B = 0;
    }

    estagio2->branch_address = (estagio2->pc + 1) + inst.immediate;
    estagio2->ULAOp =(inst.tipo == 1) ? inst.funct : 0;
    estagio2->ULAFonte = (inst.tipo == 2 && inst.opcode != 8) ? 1 : 0;
    estagio2->RegDst = (inst.opcode == 0) ? 1 : 0;
    estagio2->f_jump = (inst.tipo == 3) ? 1 : 0;
    estagio2->f_branch = (inst.opcode == 8) ? 1 : 0;
    estagio2->mem_write = (inst.opcode == 15) ? 1 : 0;
    estagio2->mem_read = (inst.opcode == 11) ? 1 : 0;
    estagio2->reg_write = (inst.tipo == 1 || inst.opcode == 4 || inst.opcode == 11 && nop == 0) ? 1 : 0;
    estagio2->reg_mem = (inst.opcode == 11) ? 1 : 0;

    printf("--------------------- Executando estagio ID --------------------\n");
    printf("Instrucao Decodificada\n");
    if(hazard == 1){
        printf("-----------HAZARD DE DADOS DETECTADO!------------\n");
    }
    printf("-----------------------------------------------------\n");
    imprimir_instrucao(&inst);
    printf("-----------------------------------------------------\n");
    printf("ULAop: %d | ULAFonte: %d | RegDst: %d | F_JUMP: %d | F_BRANCH: %d |\n MEM_WRITE: %d | REG_WRITE: %d | REG_MEM: %d\n",
        estagio2->ULAOp, estagio2->ULAFonte, estagio2->RegDst, estagio2->f_jump, estagio2->f_branch, estagio2->mem_write, estagio2->reg_write, estagio2->reg_mem);
        printf("PC: %d|\n", estagio2->pc);
        printf("-----------------------------------------------------\n");
    printf("\n");

}

void estagio_exec(Pipeline_estagio_3 *estagio3, Pipeline_estagio_2 *estagio2, Pipeline_estagio_4 *estagio4, Forward *forward ){

    int A, B;
    switch(estagio2->sel_A){
        case 0: 
            A = estagio2->A;
            break;
        case 1:
            A = estagio3->ULA_out;
            break;
        case 2:
            A = forward->dado;
            break;
        }


    switch(estagio2->sel_B){
        case 0: 
            B = estagio2->B;
            break;
        case 1:
            B = estagio3->ULA_out;
            break;
        case 2:
            B = forward->dado;
            break;
    }

    estagio3->dado_escrita = B;
    
    if(estagio2->ULAFonte == 1){
        B = estagio2->immediate;
    }

    estagio3->ULA_out = ula(A, B, estagio2->ULAOp);
    estagio3->address = estagio2->address;
    estagio3->branch_address = estagio2->branch_address;
    estagio3->f_jump = estagio2->f_jump;
    estagio3->pc = estagio2->pc;
    estagio3->rd = (estagio2->RegDst == 1) ? estagio2->rd: estagio2->rt;
    estagio3->reg_write = estagio2->reg_write;
    estagio3->mem_write = estagio2->mem_write;
    estagio3->mem_read = estagio2->mem_read;
    estagio3->reg_mem = estagio2->reg_mem;
    estagio3->f_branch = estagio2->f_branch;
    estagio3->f_zero = ula(A, B, 5); 

    printf("----------------Executando estagio EX----------------\n");
    printf("ULA_OUT: %d| A: %d| B: %d| RD: %d| ULAOP: %d| ULAFONTE: %d|\n", estagio3->ULA_out, A, B, estagio3->rd, estagio2->ULAOp, estagio2->ULAFonte);
    printf("F_JUMP: %d| F_BRANCH: %d | MEM_WRITE: %d | MEM_READ : %d| REG_WRITE: %d | REG_MEM: %d\n",
        estagio3->f_jump, estagio3->f_branch, estagio3->mem_write, estagio3->mem_read, estagio3->reg_write, estagio3->reg_mem);
        printf("PC: %d|\n", estagio3->pc);
        printf("-----------------------------------------------------\n");
    printf("\n");

}

void estagio_memoria(Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4, Pc *pc, Mem_d *mem_d, Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2){
    
    // ACESSO A MEMORIA

    if(estagio3->mem_read == 1){
        estagio4->dado_lido = mem_d[estagio3->ULA_out].dado;
    }
    
    if(estagio3->mem_write == 1){
        mem_d[estagio3->ULA_out].dado = estagio3->dado_escrita;
    }
    
    estagio4->reg_write = estagio3->reg_write;
    estagio4->reg_mem = estagio3->reg_mem;
    estagio4->ULA_out = estagio3->ULA_out;
    estagio4->rd = estagio3->rd;
    
    printf("----------------Executando estagio MEM---------------\n");
    if(estagio3->mem_read == 1){
        printf("Acesso de leitura a memoria\n");
        printf("Dado lido: %d\n", estagio4->dado_lido);
    } else if(estagio3->mem_write == 1){
        printf("Acesso de escrita a memoria\n");
        printf("Dado escrito: %d no endereco %d\n", estagio3->dado_escrita, estagio3->ULA_out);
    } 
    else if(estagio3->f_branch == 1 && estagio3->f_zero == 0){
        printf("Desvio condicional nao tomado!\n");
    }
    else{
        printf("Nao houve acesso a memoria!\n");
    }
    printf("RD %d\n", estagio3->rd);
    printf("BRANCH_ADDRESS: %d|F_BRANCH: %d|F_ZERO: %d| JUMP_ADDRESS: %d|F_JUMP: %d|\n", estagio3->branch_address, estagio3->f_branch, estagio3->f_zero, estagio3->address, estagio3->f_jump);
    
    // DESVIO
    
    if(estagio3->f_branch == 1 && estagio3->f_zero == 1){
        pc->endereco = estagio3->branch_address;
        printf("Desvio condicional tomado para o endereco: %d\n", estagio3->branch_address);
        flushPipeline(estagio1, estagio2, estagio3);
        return;
    }
    
    if(estagio3->f_jump == 1){
        pc->endereco = estagio3->address;
        printf("Desvio incondicional para o endereco: %d\n", estagio3->address);
        flushPipeline(estagio1, estagio2, estagio3);
        return;
    }
    
    printf("-----------------------------------------------------\n");
    printf("\n");


}

void estagio_writeback(BancoRegistradores *banco, Pipeline_estagio_4 *estagio4, Forward *forward){

    forward->dado = estagio4->ULA_out;

    printf("----------------Executando estagio WB---------------\n");
    if(estagio4->reg_write == 1){
        if(estagio4->reg_mem == 1){
            printf("Escrita no registrador R%d: %d\n", estagio4->rd, estagio4->dado_lido);
            banco->reg[estagio4->rd] = estagio4->dado_lido;
        }
        else{
            printf("Escrita no registrador R%d: %d\n", estagio4->rd, estagio4->ULA_out);
            banco->reg[estagio4->rd] = estagio4->ULA_out;
        }
    } else{
        printf("Nao houve escrita nos registradores!\n");
    }


    printf("RD: %d| DADO_LIDO: %d| ULA_OUT: %d| REG_WRITE %d| REG_MEM %d|\n", estagio4->rd, estagio4->dado_lido, estagio4->ULA_out, estagio4->reg_write, estagio4->reg_mem);
    printf("-----------------------------------------------------\n");

}

void flushPipeline(Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2, Pipeline_estagio_3 *estagio3){

    estagio1->pc = 0;
    strncpy(estagio1->inst, "0000000000000000\n", 17);
    estagio2->A  = 0;
    estagio2->B  = 0;
    estagio2->pc = 0;
    estagio2->rt  = 0;
    estagio2->immediate  = 0;
    estagio2->rd = 0;
    estagio2->address  = 0;
    estagio2->sel_A = 0;
    estagio2->sel_B = 0;
    estagio2->ULAOp = 0;
    estagio2->ULAFonte = 0;
    estagio2->RegDst = 0;
    estagio2->f_jump = 0;
    estagio2->f_branch = 0;
    estagio2->mem_write = 0;
    estagio2->reg_write = 0;
    estagio2->reg_mem = 0 ;
    estagio3->address = 0;
    estagio3->branch_address = 0;
    estagio3->f_jump = 0;
    estagio3->pc = 0;
    estagio3->rd = 0;
    estagio3->dado_escrita = 0;
    estagio3->reg_write = 0;
    estagio3->mem_write = 0;
    estagio3->reg_mem = 0;
    estagio3->f_branch = 0;
    estagio3->f_zero = 0;
    estagio3->ULA_out = 0;

}

void imprimirPipeline(Pipeline_estagio_1 *estagio1, Pipeline_estagio_2 *estagio2, Pipeline_estagio_3 *estagio3, Pipeline_estagio_4 *estagio4, Mem_p *mem_p){
    printf("-------------------- estagio IF ---------------------\n");
    printf("Instrucao Buscada: %s\n", estagio1->inst);
    printf("PC: %d\n", estagio1->pc);
    printf("-----------------------------------------------------\n");
    printf("\n");
    printf("--------------------- estagio ID --------------------\n");
    printf("Instrucao Decodificada\n");
    Instrucao inst = decod(mem_p[estagio2->pc].inst);
    imprimir_instrucao(&inst);
    printf("ULAop: %d | ULAFonte: %d | RegDst: %d | F_JUMP: %d | F_BRANCH: %d |\n MEM_WRITE: %d | REG_WRITE: %d | REG_MEM: %d\n",
    estagio2->ULAOp, estagio2->ULAFonte, estagio2->RegDst, estagio2->f_jump, estagio2->f_branch, estagio2->mem_write, estagio2->reg_write, estagio2->reg_mem);
    printf("PC: %d|\n", estagio2->pc);
    printf("-----------------------------------------------------\n");
    printf("\n");
    printf("--------------------- estagio EX --------------------\n");
    printf("ULA_OUT: %d| A: %d| B: %d| RD: %d|\n", estagio3->ULA_out, estagio2->A, estagio2->B, estagio3->rd);
    printf("F_JUMP: %d| F_BRANCH: %d | MEM_WRITE: %d | MEM_READ : %d| REG_WRITE: %d | REG_MEM: %d\n",
        estagio3->f_jump, estagio3->f_branch, estagio3->mem_write, estagio3->mem_read, estagio3->reg_write, estagio3->reg_mem);
    printf("PC: %d|\n", estagio3->pc);
    printf("-----------------------------------------------------\n");
    printf("\n");
    printf("--------------------- estagio MEM -------------------\n");
    printf("Dado lido: %d\n", estagio4->dado_lido);
    printf("Dado escrito: %d no endereco %d\n", estagio3->dado_escrita, estagio3->ULA_out);
    printf("RD %d\n", estagio3->rd);
    printf("BRANCH_ADDRESS: %d|F_BRANCH: %d|F_ZERO: %d| JUMP_ADDRESS: %d|F_JUMP: %d|\n", estagio3->branch_address, estagio3->f_branch, estagio3->f_zero, estagio3->address, estagio3->f_jump);
    printf("-----------------------------------------------------\n");
    printf("\n");
    printf("---------------------- estagio WB -------------------\n");
    printf("RD: %d| DADO_LIDO: %d| ULA_OUT: %d| REG_WRITE %d| REG_MEM %d|\n", estagio4->rd, estagio4->dado_lido, estagio4->ULA_out, estagio4->reg_write, estagio4->reg_mem);
    printf("-----------------------------------------------------\n");

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
        case 2: //sub
            return a - b;
            break;
        case 3: //and
            return a & b;
            break;
        case 4: //or
            return a | b;
            break;
        case 5:
            printf("a: %d, b: %d\n",a ,b);
            return ((a - b) == 0) ? 1 : 0;
            break;
    }
}

void initMemorias(Mem_p *mem_p, Mem_d *mem_d){
    for(int i = 0; i < 256; i++){
        strcpy(mem_p[i].inst, "0000000000000000\0");
        mem_d[i].dado = 0;
    }
}

void imprimir_instrucao(Instrucao *i) {
    printf("Tipo: %d | Opcode: %d | RS: %d | RT: %d | RD: %d |\n Funct: %d | Immediate: %d | Address: %d\n",
        i->tipo, i->opcode, i->rs, i->rt, i->rd, i->funct, i->immediate, i->address);
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
        printf("Mem_d[%d]: %d\n", i, mem_d[i].dado);
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
        printf("Mem_p[%i]: %s\n", i, mem_p[i].inst);
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
