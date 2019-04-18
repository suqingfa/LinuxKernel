#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/elf.h>

char *buffer;

// ELF头
struct elf64_hdr *elf_hdr;

// 程序头列表
struct elf64_phdr *elf_phdr_table;

// 段头列表
struct elf64_shdr *elf_shdr_table;
// 段名表
char *section_name_table;

/**
 * 显示ELF头
 * @param elf_hdr
 */
void print_elf_hdr() {
    printf("ELF Header:\n");

    printf("\tident:");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf(" %02X", elf_hdr->e_ident[i]);
    }
    printf("\n");

    printf("\tclass: ELF64\n");

    printf("\tdata: ");
    char * data = "";
    switch (elf_hdr->e_ident[EI_DATA]) {
        case ELFDATANONE:
            data = "none";
            break;
        case ELFDATA2LSB:
            data = "lsb";
            break;
        case ELFDATA2MSB:
            data = "msb";
            break;
    }
    printf("%s\n", data);

    printf("\tABI version: ");
    char * ABI_version = "";
    switch (elf_hdr->e_ident[EI_VERSION]) {
        case EV_NONE:
            ABI_version = "none";
            break;
        case EV_CURRENT:
            ABI_version = "current";
            break;
        case EV_NUM:
            ABI_version = "num";
            break;
    }
    printf("%s\n", ABI_version);

    printf("\tOS/ABI: ");
    switch (elf_hdr->e_ident[EI_OSABI]) {
        case ELFOSABI_NONE:
            printf("none\n");
            break;
        case ELFOSABI_LINUX:
            printf("linux\n");
            break;
    }

    printf("\ttype: ");
    char * type = NULL;
    switch (elf_hdr->e_type) {
        case ET_NONE:
            type = "none";
            break;
        case ET_REL:
            type = "relocatable";
            break;
        case ET_EXEC:
            type = "executable";
            break;
        case ET_DYN:
            type = "dynamic";
            break;
        case ET_CORE:
            type = "core";
            break;
        default:
            // 介于 ET_LOPROC - ET_HIPROC 的值包含特定于处理器的语义
            printf("%X\n", elf_hdr->e_type);
            break;
    }
    if(type != NULL){
        printf("%s\n", type);
    }

    printf("\tmachine: ");
    char * machine = "other";
    switch (elf_hdr->e_machine) {
        case EM_386:
            machine = "i386";
            break;
        case EM_X86_64:
            machine = "X86_64";
            break;
            // 其余机器类型未列出 EM_*
    }
    printf("%s\n", machine);

    printf("\tversion: %d\n", elf_hdr->e_version);

    printf("\tEntry point virtual address: 0x%llx\n", elf_hdr->e_entry);

    printf("\tProgram header table file offset: 0x%llx\n", elf_hdr->e_phoff);

    printf("\tSection header table file offset: 0x%llx\n", elf_hdr->e_shoff);

    printf("\tflag: 0x%x\n", elf_hdr->e_flags);

    printf("\tELF header size: %hd\n", elf_hdr->e_ehsize);

    // 该项值为 sizeof(elf64_phdr)
    printf("\tprogram header table entry size: %d\n", elf_hdr->e_phentsize);
    printf("\tnumber of program headers: %d\n", elf_hdr->e_phnum);

    // 该项值为 sizeof(elf64_shdr)
    printf("\tsection header table entry size: %d\n", elf_hdr->e_shentsize);
    printf("\tnumber of section headers: %d\n", elf_hdr->e_shnum);

    // 段表名所在的段下标
    printf("\tsection header string table index: %d\n", elf_hdr->e_shstrndx);
}

/**
 * 显示程序头
 * @param buffer
 * @param elf_hdr
 */
void print_elf_phdr() {
    printf("program headers:\n");

    const int index_width = 7;
    printf("\t%-*s", index_width, "index");

    const int type_width = 20;
    printf("%-*s", type_width, "type");

    const int flags_width = 10;
    printf("%-*s", flags_width, "flags");

    const int offset_width = 10;
    printf("%-*s", offset_width, "offset");

    const int vaddr_width = 10;
    printf("%-*s", vaddr_width, "vaddr");

    const int paddr_width = 10;
    printf("%-*s", paddr_width, "paddr");

    const int filesz_width = 10;
    printf("%-*s", filesz_width, "filesz");

    const int memesz_width = 10;
    printf("%-*s", memesz_width, "memsz");

    const int align_width = 10;
    printf("%-*s", align_width, "align");

    printf("\n");

    for (int i = 0; i < elf_hdr->e_phnum; i++) {
        printf("\t%-*d", index_width, i);

        struct elf64_phdr *elf_phdr = elf_phdr_table + i;

        char * type = NULL;
        switch (elf_phdr->p_type) {
            case PT_NULL:
                type = "null";
                break;
            case PT_LOAD:
                type = "load";
                break;
            case PT_DYNAMIC:
                type = "dynamic";
                break;
            case PT_INTERP:
                type = "interp";
                break;
            case PT_NOTE:
                type = "note";
                break;
            case PT_SHLIB:
                type = "shlib";
                break;
            case PT_PHDR:
                type = "phdr";
                break;
            case PT_TLS:
                type = "tls(Thread local storage segment)";
                break;
            case PT_GNU_EH_FRAME:
                type = "GNU eh frame";
                break;
            case PT_GNU_STACK:
                type = "GNU stack";
                break;

                // 介于 PT_LOOS - PT_HIOS 之间的值为特定操作系统
                // 介于 PT_LOPROC - PT_HIPROC 之间的为特定处理器
            default:
                printf("0x%-*x", type_width - 2, elf_phdr->p_type);
                break;
        }
        printf("%-*s", type_width, type);

        int space = flags_width;
        if (elf_phdr->p_flags & PF_R) {
            printf("R");
            space--;
        }
        if (elf_phdr->p_flags & PF_W) {
            printf("W");
            space--;
        }
        if (elf_phdr->p_flags & PF_X) {
            printf("X");
            space--;
        }
        printf("%*s", space, "");

        printf("0x%-*llx", offset_width - 2, elf_phdr->p_offset);
        printf("0x%-*llx", vaddr_width - 2, elf_phdr->p_vaddr);
        printf("0x%-*llx", paddr_width - 2, elf_phdr->p_paddr);
        printf("0x%-*llx", filesz_width - 2, elf_phdr->p_filesz);
        printf("0x%-*llx", memesz_width - 2, elf_phdr->p_memsz);
        printf("0x%-*llx", align_width - 2, elf_phdr->p_align);

        printf("\n");
    }
}

/**
 * 显示段头
 * @param buffer
 * @param elf_hdr
 */
void print_elf_shdr() {
    printf("section headers:\n");

    const int index_width = 7;
    printf("\t%-*s", index_width, "index");

    const int name_width = 20;
    printf("%-*s", name_width, "name");

    const int type_width = 15;
    printf("%-*s", type_width, "type");

    const int flags_width = 10;
    printf("%-*s", flags_width, "flags");

    const int addr_width = 10;
    printf("%-*s", addr_width, "addr");

    const int offset_width = 10;
    printf("%-*s", offset_width, "offset");

    const int size_width = 10;
    printf("%-*s", size_width, "size");

    const int link_width = 7;
    printf("%-*s", link_width, "link");

    const int info_width = 7;
    printf("%-*s", info_width, "info");

    const int addralign_width = 7;
    printf("%-*s", addralign_width, "align");

    const int entsize_width = 7;
    printf("%-*s", entsize_width, "entity size");

    printf("\n");

    for (int i = 0; i < elf_hdr->e_shnum; i++) {
        printf("\t%-*d", index_width, i);

        struct elf64_shdr *elf_shdr = elf_shdr_table + i;

        // sh_name 为段名在字符串表中的偏移量
        printf("%-*s", name_width, section_name_table + elf_shdr->sh_name);

        char * type = NULL;
        switch (elf_shdr->sh_type) {
            case SHT_NULL:
               type = "null";
                break;

                // 代码/数据
            case SHT_PROGBITS:
               type = "progbits";
                break;

                // 符号表
            case SHT_SYMTAB:
               type = "symtab";
                break;

                // 字符串表
            case SHT_STRTAB:
               type = "strtab";
                break;

                // 重定位表
            case SHT_RELA:
               type = "rela";
                break;
            case SHT_HASH:
               type = "hash";
                break;

                // 动态链接信息
            case SHT_DYNAMIC:
               type = "dynamic";
                break;

                // 提示性信息
            case SHT_NOTE:
               type = "note";
                break;

                // 该段没有内容 如 .bss
            case SHT_NOBITS:
               type = "nobits";
                break;

                // 重定位信息
            case SHT_REL:
               type = "rel";
                break;

                // 保留
            case SHT_SHLIB:
               type = "shlib";
                break;

                // 动态链接符号表
            case SHT_DYNSYM:
               type = "dynsym";
                break;
            case SHT_NUM:
               type = "num";
                break;
            default:
                // SHT_LOPROC - SHT_HIPROC 之间的值为特定于处理器的语义保留
                // SHT_LOUSER - SHT_HIUSER 之间的值为应用程序使用
                printf("0x%-*x ", type_width - 3, elf_shdr->sh_type);
                break;
        }
        if(type != NULL){
            printf("%-*s", type_width, type);
        }

        int space = flags_width;

        // 该段在进程空间中可写
        if (elf_shdr->sh_flags & SHF_WRITE) {
            printf("W");
            space--;
        }
        // 该段要在进程空间中分配空间
        if (elf_shdr->sh_flags & SHF_ALLOC) {
            printf("A");
            space--;
        }
        // 该段在进程空间中可执行
        if (elf_shdr->sh_flags & SHF_EXECINSTR) {
            printf("X");
            space--;
        }
        printf("%-*s", space, "");

        printf("0x%-*llx ", addr_width - 3, elf_shdr->sh_addr);
        printf("0x%-*llx ", offset_width - 3, elf_shdr->sh_offset);
        printf("0x%-*llx ", size_width - 3, elf_shdr->sh_size);

        /**
         * sh_type      sh_link                          sh_info
         * SHT_DYNAMIC  该段使用的字符串表在段表的下标     0
         * SHT_HASH     该段使用的符号表在段表的下标       0
         * SHT_REL      该段所使用的相应符号表在段表的下标  该重定位表所作用的段在段表的的下载
         * SHT_RELA     该段所使用的相应符号表在段表的下标  该重定位表所作用的段在段表的的下载
         * SHT_SYMTAB   操作系统相关                       操作系统相关
         * SHT_DYNSYM   操作系统相关                       操作系统相关
         * other        SHN_UNDEF                         0
         * 特殊的index SHN_*
         */
        printf("%-*d", link_width, elf_shdr->sh_link);
        printf("%-*d", info_width, elf_shdr->sh_info);

        printf("0x%-*llx", addralign_width - 2, elf_shdr->sh_addralign);
        printf("%-*llu", entsize_width, elf_shdr->sh_entsize);

        printf("\n");
    }
}

/**
 * 显示符号段的内容
 * @param index
 */
void print_symbol_section(int index) {
    struct elf64_shdr *elf_shdr = elf_shdr_table + index;
    struct elf64_sym *elf_sym_table = (struct elf64_sym *) (buffer + elf_shdr->sh_offset);

    // sh_entsize 值为 sizeof(elf64_sym)
    int count = elf_shdr->sh_size / elf_shdr->sh_entsize;

    printf("section %d , name: %s\n", index, section_name_table + elf_shdr->sh_name);

    int index_width = 7;
    printf("\t%-*s", index_width, "index");

    int name_width = 40;
    printf("%-*s", name_width, "name");

    int bind_width = 10;
    printf("%-*s", bind_width, "bind");

    int type_width = 10;
    printf("%-*s", type_width, "type");

    int other_width = 10;
    printf("%-*s", other_width, "other");

    int shndx_width = 10;
    printf("%-*s", shndx_width, "shndx");

    int value_width = 10;
    printf("%-*s", value_width, "value");

    int size_width = 10;
    printf("%-*s", size_width, "size");

    printf("\n");

    // 符号表的符号名使用了 sh_link 指向的段的字符串表
    char *symbol_string_table = buffer + elf_shdr_table[elf_shdr->sh_link].sh_offset;

    for (int i = 0; i < count; i++) {
        printf("\t%-*d", index_width, i);

        struct elf64_sym *elf_sym = elf_sym_table + i;

        printf("%-*s", name_width, symbol_string_table + elf_sym->st_name);

        char *bind = "";
        switch (ELF64_ST_BIND(elf_sym->st_info)) {

                // 局部变量，对目标文件外部不可见
            case STB_LOCAL:
                bind = "local";
                break;

                // 全局变量
            case STB_GLOBAL:
                bind = "global";
                break;

                // 弱引用
            case STB_WEAK:
                bind = "weak";
                break;
        }
        printf("%-*s", bind_width, bind);

        char * type = "";
        switch (ELF64_ST_TYPE(elf_sym->st_info)){
            case STT_NOTYPE:
                type = "notype";
                break;

                // 该符号是数据对象，如变量、数组
            case STT_OBJECT:
                type = "object";
                break;

                // 该符号是函数或其它可执行代码
            case STT_FUNC:
                type = "func";
                break;

                // 该符号表示一个段，类型并须为STB_LOCAL
            case STT_SECTION:
                type = "section";
                break;

                // 该符号表示文件名，一般是目标文件所对应的源文件名，类型必须为STB_LOCAL st_shndx必须为SHN_ABS
            case STT_FILE:
                type = "file";
                break;
            case STT_COMMON:
                type = "common";
                break;
            case STT_TLS:
                type = "tls";
                break;
        }
        printf("%-*s", type_width, type);

        printf("%-*d", other_width, elf_sym->st_other);

        char * section_index = NULL;
        switch (elf_sym->st_shndx){

                // 该符号未定义。该符号在本目标文件中引用，但定义在其它目标文件中
            case SHN_UNDEF:
                section_index = "undef";
                break;

                // 该符号包含一个绝对的值。比如表示文件名的符号
            case SHN_ABS:
                section_index = "abs";
                break;

                // common块，一般是未初始化的全局符号
            case SHN_COMMON:
                section_index = "common";
                break;

                // 该符号所在的段索引
            default:
                printf("%-*d", shndx_width, elf_sym->st_shndx);
        }
        if(section_index != NULL){
            printf("%-*s", shndx_width, section_index);
        }

        /**
         * 在目标文件中
         *      如果符号不在common块，即st_shndx不是SHN_COMMON，则value表示该符号在段中的偏移
         *      如果符号在common块中，即st_shndx是SHN_COMMON，则value表示符号的对齐属性
         *
         * 在可执行文件中
         *      value表示符号的虚拟地址
         */
        printf("0x%-*llx", value_width - 2, elf_sym->st_value);

        printf("%-*llu", size_width, elf_sym->st_size);

        printf("\n");
    }
}

/**
 * 显示字符串段的内容
 * @param index
 */
void print_string_section(int index) {
    struct elf64_shdr *elf_shdr = elf_shdr_table + index;
    char *string = buffer + elf_shdr->sh_offset;
    int size = elf_shdr->sh_size;

    printf("section %d , name: %s\n", index, section_name_table + elf_shdr->sh_name);

    int count = 0;
    printf("\t0 : \"");
    for (int i = 0; i < size; i++) {
        if (string[i] == 0) {
            printf("\"\n\t%d : \"", ++count);
            continue;
        }
        printf("%c", string[i]);
    }
    printf("\"\n");
}

/**
 * 显示所有段的内容
 */
void print_section() {

    void (*print_section)(int);

    for (int i = 0; i < elf_hdr->e_shnum; i++) {
        struct elf64_shdr *elf_shdr = elf_shdr_table + i;

        print_section = NULL;
        switch (elf_shdr->sh_type) {
            case SHT_NULL:
                break;
            case SHT_PROGBITS:
                break;
            case SHT_SYMTAB:
                print_section = print_symbol_section;
                break;
            case SHT_STRTAB:
                print_section = print_string_section;
                break;
            case SHT_RELA:
                break;
            case SHT_HASH:
                break;
            case SHT_DYNAMIC:
                break;
            case SHT_NOTE:
                break;
            case SHT_NOBITS:
                break;
            case SHT_REL:
                break;
            case SHT_SHLIB:
                break;
            case SHT_DYNSYM:
                print_section = print_symbol_section;
                break;
            case SHT_NUM:
                break;
            default:
                // SHT_LOPROC - SHT_HIPROC 之间的值为特定于处理器的语义保留
                // SHT_LOUSER - SHT_HIUSER 之间的值为应用程序使用
                break;
        }
        if(print_section != NULL){
            print_section(i);
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        return -1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("cannot open file '%s'\n", argv[1]);
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if (file_size == 0) {
        printf("file '%s' is empty\n", argv[1]);
        return -1;
    }

    buffer = malloc(file_size);
    if (fread(buffer, file_size, 1, file) != 1) {
        printf("read file '%s' fail\n", argv[1]);
        return -1;
    }

    /**
     * ELFMAG 魔法数 ELF文件固定为 ELFMAG
     * EI_CLASS ELF文件类型
     * 本程序只读取ELF64文件
     */
    if (strncmp(buffer, ELFMAG, strlen(ELFMAG)) != 0 || buffer[EI_CLASS] != ELFCLASS64) {
        printf("file '%s' is not a ELF64 file", argv[1]);
        return -1;
    }

    elf_hdr = (struct elf64_hdr *) buffer;
    print_elf_hdr();

    elf_phdr_table = (struct elf64_phdr *) (buffer + elf_hdr->e_phoff);
    print_elf_phdr();

    elf_shdr_table = (struct elf64_shdr *) (buffer + elf_hdr->e_shoff);
    section_name_table = buffer + elf_shdr_table[elf_hdr->e_shstrndx].sh_offset;
    print_elf_shdr();

    print_section();
}