#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"

/* SOLUTION CODE BELOW */
const int TWO_POW_SEVENTEEN = 131072;    // 2^17
const int TWO_POW_FIFTEEN = 32768; //2^15

static char* create_copy_of_str(const char* str) {
    size_t len = strlen(str) + 1;
    char *buf = (char *) malloc(len);
    if (!buf) {
        allocation_failed();
    }
    strncpy(buf, str, len);
    return buf;
}
/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li and other pseudoinstructions. Your pseudoinstruction
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addiu instruction. Otherwise, expand it into
        a lui-ori pair.

   If you are going to use the $zero or $0, use $0, not $zero.

   MARS has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if MARS behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
    if (strcmp(name, "li") == 0) {//name: instruction  must contain right num of arguments
        if (num_args != 2) //only two arguments
            return 0;
        long int imm;
        int rd = translate_reg(args[0]);
        int err = translate_num(&imm, args[1], INT32_MIN, INT32_MAX);
        if (err==-1||rd==-1) {
            printf("Argument translation failed\n");
            return 0;
        }
        if (((imm >= 0) && (imm < TWO_POW_FIFTEEN)) || ((imm < 0) && (imm >= -TWO_POW_FIFTEEN))) { //solve in one instruction
            char** newInstr = (char**)calloc(3, sizeof(char*));
            if (newInstr == NULL)
                allocation_failed();
            int num_new = 3;
            char* newName = create_copy_of_str("addiu");
            /*newInstr[0] = (char*)malloc((strlen(args[0])+1)*sizeof(char));
               newInstr[1] = (char*)malloc((strlen(args[1])+1)*sizeof(char));
               newInstr[2] = (char*)malloc(2*sizeof(char));
               if(newInstr[2] == NULL || newInstr[1] == NULL || newInstr[0] == NULL || newName == NULL )
                allocation_failed();*/
            newInstr[0] = create_copy_of_str(args[0]);
            newInstr[1] = create_copy_of_str("$0");
            newInstr[2] = create_copy_of_str(args[1]);
            //*(newInstr[2] + 2) = create_copy_of_str(args[1]);

            //newName = "addiu";

            write_inst_string(output,newName,newInstr,num_new);
            for(int j = 0; j<3; j++)
                free(*(newInstr+j));
            free(newInstr);//R2S (for removed 2 stars)
            free(newName);
            return 1;
        } else {
            int addr_lui = imm >> 16;
            int addr_ori = imm & 0xFFFF;
            char** newInstr1 =(char **) calloc(2,sizeof(char*));
            char** newInstr2 =(char **) calloc(3,sizeof(char*));// file out the lui part
            if (newInstr1 == NULL || newInstr2 ==NULL)
                allocation_failed();
            /*char * newName1 = (char*)malloc(sizeof(char)*4);
               char * newName2 = (char*)malloc(sizeof(char)*4);*/
            int num_new1 = 2;
            int num_new2 = 3;
            newInstr1[0] = create_copy_of_str("$at");

            /*newInstr1[0] = (char*)malloc(4*sizeof(char));
               newInstr1[1] = (char*)malloc(17);
               newInstr2[0] = (char*)malloc((strlen(args[0])+1)*sizeof(char));
               newInstr2[1] = (char*)malloc(4*sizeof(char));
               newInstr2[2] = (char*)malloc(17);
               if(newInstr1[2] == NULL || newInstr1[1] == NULL || newInstr1[0] == NULL || newName1 == NULL )
                  allocation_failed();
               if(newInstr2[2] == NULL || newInstr2[1] == NULL || newInstr2[0] == NULL || newName2 == NULL )
                  allocation_failed();
               newInstr1[0] = "$at";*/
            //itoa(addr_lui,newInstr1[1],16);
            char * temp = (char*)calloc(17,sizeof(char));
            sprintf(temp, "%x", addr_lui);
            newInstr1[1] = calloc(17, sizeof(char));
            char * zeroex = "0x";
            strcpy(newInstr1[1], zeroex);
            char * tempp0 = newInstr1[1];
            tempp0 += 2;
            for (size_t i = 0; i < strlen(temp)+1; i++) {
              tempp0[i]=temp[i];
            }
            free(temp);
            newInstr2[0] = create_copy_of_str(args[0]);
            newInstr2[1] = create_copy_of_str("$at");
            //itoa(addr_ori,newInstr1[2],16);
            char * temp2 = (char*)calloc(17,sizeof(char));
            sprintf(temp2, "%x", addr_ori);
            newInstr2[2] = calloc(17, sizeof(char));
            strcpy(newInstr2[2], zeroex);
            char * tempp = newInstr2[2];
            tempp += 2;
            for (size_t i = 0; i < strlen(temp2)+1; i++) {
              tempp[i]=temp2[i];
            }
            free(temp2);
            char * newName1 = create_copy_of_str("lui");
            char * newName2 = create_copy_of_str("ori");
            write_inst_string(output,newName1,newInstr1,num_new1);
            write_inst_string(output,newName2,newInstr2,num_new2);
            for(int j = 0; j<2; j++)
                free(newInstr1[j]);
            for(int j = 0; j<3; j++)
                free(newInstr2[j]);
            free(newInstr1);//R2S
            free(newInstr2);//R2S
            free(newName1);
            free(newName2);
            return 2;
        }

    } else if (strcmp(name, "move") == 0) {
        if (num_args != 2)
            return 0;
        int rd = translate_reg(args[0]);
        int rs = translate_reg(args[1]);
        if (rs==-1||rd==-1) {
            printf("Argument translation failed\n");
            return 0;
        }
        char** newInstr = (char**)malloc(3*sizeof(char*));
        if (newInstr == NULL)
            allocation_failed();
        int num_new = 3;
        char* newName = create_copy_of_str("addiu");
        /*newInstr[0] = malloc(sizeof(char)*(strlen(args[0])+1));
           newInstr[1] = malloc(sizeof(char)*(strlen(args[0])+1));
           newInstr[2] = malloc(sizeof(char)*2);
           if(newInstr[2] == NULL || newInstr[1] == NULL || newInstr[0] == NULL || newName == NULL )
                allocation_failed();*/

        newInstr[0] = create_copy_of_str(args[0]);
        newInstr[1] = create_copy_of_str(args[1]);
        newInstr[2] = create_copy_of_str("0");
        //newName = "addiu";
        write_inst_string(output,newName,newInstr,num_new);
        for(int j = 0; j<3; j++)
            free(newInstr[j]);
        free(newInstr);
        free(newName);
        return 1;
    } else if (strcmp(name, "rem") == 0) { //fixme:how to solve hi and lo?s
        if (num_args != 3)
            return 0;
        int rd = translate_reg(args[0]);
        int rs = translate_reg(args[1]);
        int rt = translate_reg(args[2]);
        if ((rd == -1) || (rs == -1) || (rt == -1)) {
            printf("Argument translation failed\n");
            return 0;
        }
        char** newInstr1 = (char**)malloc(2*sizeof(char*));
        char** newInstr2 = (char**)malloc(sizeof(char*));
        int num_new1 = 2;
        int num_new2 = 1;
        char * newName1 = create_copy_of_str("div");
        char * newName2 = create_copy_of_str("mfhi");
        /*newInstr1[0] = (char*)malloc((strlen(args[1])+1)*sizeof(char));
           newInstr1[1] = (char*)malloc((strlen(args[2])+1)*sizeof(char));
           newInstr2[0] = (char*)malloc(strlen(args[0]+1)*sizeof(char));
           if(newInstr1[1] == NULL || newInstr1[0] == NULL || newName1 == NULL )
                allocation_failed();
           if( newInstr2[0] == NULL || newName2 == NULL )
                allocation_failed();*/

        newInstr1[0] = create_copy_of_str(args[1]);
        newInstr1[1] = create_copy_of_str(args[2]);
        //newName1 = "div";
        //newName2 = "mfhi";
        newInstr2[0] = create_copy_of_str(args[0]);
        write_inst_string(output,newName1,newInstr1,num_new1);
        write_inst_string(output,newName2,newInstr2,num_new2);
        free(newName1);
        free(newName2);
        for(int j = 0; j<2; j++)
            free(newInstr1[j]);
        for(int j = 0; j<1; j++)
            free(newInstr2[j]);
        free(newInstr1);
        free(newInstr2);
        return 2;


    } else if (strcmp(name, "bge") == 0) {
        if (num_args != 3)
            return 0;
        //long int addr;
        int rs = translate_reg(args[0]);
        int rt = translate_reg(args[1]);
        char * label = args[2];
        if ((label == NULL) || (rs == -1) || (rt == -1)) { // fixme:how to keep track of the address?
            printf("Argument translation failed\n");
            return 0;
        }
        char** newInstr1 = (char**)malloc(sizeof(char*)*3);
        char** newInstr2 = (char**)malloc(sizeof(char*)*3);
        if (newInstr1 == NULL || newInstr2 ==NULL)
            allocation_failed();
        int num_new1 = 3;
        int num_new2 = 3;
        char * newName1 = create_copy_of_str("slt");
        char * newName2 = create_copy_of_str("beq");
        /*newInstr1[0] = (char*)malloc(4*sizeof(char));
           newInstr1[1] = (char*)malloc((strlen(args[0])+1)*sizeof(char));
           newInstr1[2] = (char*)malloc((strlen(args[1])+1)*sizeof(char));
           newInstr2[0] = (char*)malloc(4*sizeof(char));
           newInstr2[1] = (char*)malloc(3*sizeof(char));
           newInstr2[2] = (char*)malloc((strlen(args[2])+1)*sizeof(char));
           if(newInstr1[2] == NULL || newInstr1[1] == NULL || newInstr1[0] == NULL || newName1 == NULL )
                allocation_failed();
           if(newInstr2[2] == NULL || newInstr2[1] == NULL || newInstr2[0] == NULL || newName2 == NULL )
                allocation_failed();*/
        newInstr2[2] = create_copy_of_str(args[2]);
        newInstr1[0] = create_copy_of_str("$at");
        newInstr1[1] = create_copy_of_str(args[0]);
        newInstr1[2] = create_copy_of_str(args[1]);
        newInstr2[0] = create_copy_of_str("$at");
        newInstr2[1] = create_copy_of_str("$0");
        //newName1 = "slt";
        //newName2 = "beq";
        write_inst_string(output,newName1,newInstr1,num_new1);
        write_inst_string(output,newName2,newInstr2,num_new2);
        for(int j = 0; j<3; j++)
            free(newInstr1[j]);
        for(int j = 0; j<3; j++)
            free(newInstr2[j]);
        free(newInstr1);
        free(newInstr2);
        free(newName1);
        free(newName2);
        return 2;
    } else if (strcmp(name, "bnez") == 0) {
        if (num_args != 2)
            return 0;
        int rs = translate_reg(args[0]);
        if ((rs == -1) || (args[1] == NULL)) {
            printf("Argument translation failed\n");
            return 0;
        }
        char * newName = create_copy_of_str("bne");

        int num_new = 3;
        char** newInstr = (char**)malloc(sizeof(char*)*3);
        if(newInstr == NULL) {
            allocation_failed();
        }
        /*newInstr[0] = (char*)malloc((strlen(args[0])+1)*sizeof(char));
           newInstr[1] = (char*)malloc(3*sizeof(char));
           newInstr[2] = (char*)malloc((strlen(args[1])+1)*sizeof(char));
            if(newInstr[2] == NULL || newInstr[1] == NULL || newInstr[0] == NULL|| newName == NULL  )
                allocation_failed();*/
        newInstr[0] = create_copy_of_str(args[0]);
        newInstr[1] = create_copy_of_str("$0");
        newInstr[2] = create_copy_of_str(args[1]);
        //newName = "bne";
        write_inst_string(output,newName,newInstr,num_new);
        for(int j = 0; j<3; j++)
            free(newInstr[j]);
        free(newInstr);
        free(newName);
        return 1;
    }


    write_inst_string(output, name, args, num_args);//I disable this function ,for it is complicated when allocating new instructions
    return 1;

}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step. If a symbol should be relocated, it should be added to the
   relocation table (RELTBL), and the fields for that symbol should be set to
   all zeros.

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write
   anything to OUTPUT but simply return -1. MARS may be a useful resource for
   this step.

   Some function declarations for the write_*() functions are provided in translate.h, and you MUST implement these
   and use these as function headers. You may use helper functions, but you still must implement
   the provided write_* functions declared in translate.h.

   Returns 0 on success and -1 on error.
 */
int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr,
                   SymbolTable* symtbl, SymbolTable* reltbl) {
    //R type, including both shifting and non-shifting
    if (strcmp(name, "addu") == 0) return write_rtype (0x21, output, args, num_args);
    //else if (strcmp(name, "add") == 0)    return write_rtype (0x20, output, args, num_args);
    //else if (strcmp(name, "and") == 0)    return write_rtype (0x24, output, args, num_args);
    else if (strcmp(name, "div") == 0) return write_div (0x1A, output, args, num_args);
    //else if (strcmp(name, "divu") == 0)   return write_rtype (0x1B, output, args, num_args);
    else if (strcmp(name, "jr") == 0) return write_jr    (0x08, output, args, num_args);
    else if (strcmp(name, "mfhi") == 0) return write_mf (0x10, output, args, num_args);
    else if (strcmp(name, "mflo") == 0) return write_mf (0x12, output, args, num_args);
    else if (strcmp(name, "mult") == 0) return write_div (0x18, output, args, num_args);
    //else if (strcmp(name, "multu") == 0)  return write_rtype (0x19, output, args, num_args);
    //else if (strcmp(name, "nor") == 0)    return write_rtype (0x27, output, args, num_args);
    else if (strcmp(name, "or") == 0) return write_rtype (0x25, output, args, num_args);
    else if (strcmp(name, "sll") == 0) return write_shift (0x00, output, args, num_args);
    else if (strcmp(name, "slt") == 0) return write_rtype (0x2a, output, args, num_args);
    else if (strcmp(name, "sltu") == 0) return write_rtype (0x2b, output, args, num_args);
    //else if (strcmp(name, "sra") == 0)    return write_shift (0x03, output, args, num_args);
    //else if (strcmp(name, "srl") == 0)    return write_shift (0x02, output, args, num_args);
    //else if (strcmp(name, "sub") == 0)    return write_rtype (0x22, output, args, num_args);
    //else if (strcmp(name, "subu") == 0)   return write_rtype (0x23, output, args, num_args);
    //else if (strcmp(name, "xor") == 0)    return write_rtype (0x26, output, args, num_args);

    //I type
    else if (strcmp(name, "addiu") == 0) return write_addiu (0x09, output, args, num_args);
    else if (strcmp(name, "ori") == 0) return write_ori (0x0d, output, args, num_args);
    else if (strcmp(name, "lui") == 0) return write_lui (0x0f, output, args, num_args);
    //Using write_addiu for all of the byte operations
    else if (strcmp(name, "lb") == 0) return write_mem (0x20, output, args, num_args);
    else if (strcmp(name, "lbu") == 0) return write_mem (0x24, output, args, num_args);

//CE: i switch these from write addiu ot write mem
    else if (strcmp(name, "lw") == 0) return write_mem (0x23, output, args, num_args);
    else if (strcmp(name, "sb") == 0) return write_mem (0x28, output, args, num_args);
//CE: i switch these from write addiu ot write mem

    else if (strcmp(name, "sw") == 0) return write_mem (0x2b, output, args, num_args);
    //Branching instructions
    else if (strcmp(name, "beq") == 0) return write_branch (0x04, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "bne") == 0) return write_branch (0x05, output, args, num_args, addr, symtbl);
    //Jumping instructions
    else if (strcmp(name, "j") == 0) return write_jump (0x02, output, args, num_args, addr, reltbl);
    else if (strcmp(name, "jal") == 0) return write_jump (0x03, output, args, num_args, addr, reltbl);

    else return -1;
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(uint8_t funct, FILE* output, char** args, size_t num_args) {//r type transform


    int rd = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int rt = translate_reg(args[2]);
    if (rd==-1||rs==-1||rt==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    uint32_t instruction = 0;
    instruction += funct;//six bits
    //instruction += shamt<<6;
    instruction += rd<<11;
    instruction += rt<<16;
    instruction += rs<<21;//opcode is just zero
    write_inst_hex(output, instruction);//write the instruction to the output file
    return 0;
}
/* sample shift code for R type
    uint32_t instruction = 0;
    instruction += funct;
    instruction += shamt<<6;
    instruction += rd<<11;
    instruction += rt<<16;
    instruction += rs<<21;
    write_inst_hex(output, instruction);
 */


/* A helper function for writing shift instructions. You should use
   translate_num() to parse numerical arguments. translate_num() is defined
   in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_shift(uint8_t funct, FILE* output, char** args, size_t num_args) {


    long int shamt;
    int rd = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int err = translate_num(&shamt, args[2], 0, 31);
    if (rd==-1||err==-1||rt==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    uint32_t instruction = 0;
    instruction += funct;
    instruction += shamt<<6;
    instruction += rd<<11;
    instruction += rt<<16;
    //instruction += rs<<21;
    write_inst_hex(output, instruction);
    return 0;
}

/* The rest of your write_*() functions below */

int write_jr(uint8_t funct, FILE* output, char** args, size_t num_args) {


    int rs = translate_reg(args[0]);//translate a reg into int
    if (rs==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    uint32_t instruction = 0;
    instruction += funct;
    instruction += rs<<21;
    write_inst_hex(output, instruction);
    return 0;
}

int write_div(uint8_t funct, FILE* output, char** args, size_t num_args) {

    //for div and mult only, could also be applied to other 2 argument r type
    int rs = translate_reg(args[0]);//translate a reg into int
    int rt = translate_reg(args[1]);
    if (rs==-1||rt==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    uint32_t instruction = 0;
    instruction += funct;
    instruction += rs<<21;
    instruction += rt<<16;
    write_inst_hex(output, instruction);
    return 0;
}

int write_mf(uint8_t funct, FILE* output, char** args, size_t num_args) {

    //For mfhi and mflo, only one register r type
    int rd = translate_reg(args[0]);//translate a reg into int
    if (rd==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    uint32_t instruction = 0;
    instruction += funct;
    instruction += rd<<11;
    write_inst_hex(output, instruction);
    return 0;
}

int write_addiu(uint8_t opcode, FILE* output, char** args, size_t num_args) {

    //for generic r type with 3 arguments
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], INT16_MIN, INT16_MAX);
    if (rs==-1||err==-1||rt==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    imm &= 0xFFFF;
    uint32_t instruction = 0;
    instruction += imm;
    instruction += rt<<16;
    instruction += rs<<21;
    instruction += opcode<<26;
    write_inst_hex(output, instruction);
    return 0;
}

int write_ori(uint8_t opcode, FILE* output, char** args, size_t num_args) {


    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[1]);
    int err = translate_num(&imm, args[2], 0, UINT16_MAX);
    if (rs==-1||err==-1||rt==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    imm &= 0xFFFF;
    uint32_t instruction = 0;
    instruction += imm;
    instruction += rt<<16;
    instruction += rs<<21;
    instruction += opcode<<26;
    write_inst_hex(output, instruction);
    return 0;
}

int write_lui(uint8_t opcode, FILE* output, char** args, size_t num_args) {


    long int imm;
    int rt = translate_reg(args[0]);
    int err = translate_num(&imm, args[1], 0, UINT16_MAX);
    if (err==-1||rt==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    imm &= 0xFFFF;
    uint32_t instruction = 0;
    instruction += imm;
    instruction += rt<<16;
    instruction += opcode<<26;
    write_inst_hex(output, instruction);
    return 0;
}

int write_mem(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    long int imm;
    int rt = translate_reg(args[0]);
    int rs = translate_reg(args[2]);
    int err = translate_num(&imm, args[1], INT16_MIN, INT16_MAX);
    if (rs==-1||err==-1||rt==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    imm &= 0xFFFF;
    uint32_t instruction = 0;
    instruction += imm;
    instruction += rt<<16;
    instruction += rs<<21;
    instruction += opcode<<26;
    write_inst_hex(output, instruction);
    return 0;
}

/*  A helper function to determine if a destination address
    can be branched to
 */
static int can_branch_to(uint32_t src_addr, uint32_t dest_addr) {
    int32_t diff = dest_addr - src_addr;
    return (diff >= 0 && diff <= TWO_POW_SEVENTEEN) || (diff < 0 && diff >= -(TWO_POW_SEVENTEEN - 4));
}


int write_branch(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl) {
    int rs = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int label_addr = get_addr_for_symbol(symtbl, args[2]);
    if (rs==-1||label_addr==-1||rt==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    if (can_branch_to(addr+4,label_addr)==0) {
        printf("Cannot branch that far.\n");
        return -1;
    }

    //Please compute the branch offset using the MIPS rules.
    int32_t offset = (label_addr-(addr+4))>>2;
    uint32_t instruction = 0;
    offset &= 0xFFFF;
    instruction += offset;
    instruction += rt<<16;
    instruction += rs<<21;
    instruction += opcode<<26;
    write_inst_hex(output, instruction);
    return 0;
}

int write_jump(uint8_t opcode, FILE* output, char** args, size_t num_args, uint32_t addr, SymbolTable* reltbl) {
    int label_addr = get_addr_for_symbol(reltbl, args[0]);
    if (label_addr==-1) {
        printf("Argument translation failed\n");
        return -1;
    }

    int32_t jaddr = (label_addr<<4)>>6;//eliminating first four and last two bits
    jaddr &= 0x3FFFFFF;
    uint32_t instruction = 0;
    //instruction += jaddr;
    instruction += opcode<<26;
    write_inst_hex(output, instruction);
    return 0;
}
