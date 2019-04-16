# 1 工具链
工具链是构建Linux内核映像的一组程序。链的概念源于一个工具的输出作为另一个进程的输入这一特点。
工具链包含 编译程序、汇编程序和链接程序。

## 1.1 ELF二进制目标文件
主要的ELF文件类型有：可执行文件、可重定位目标文件、核心文件和共享库。

ELF文件头总是位于ELF文件中偏移为0的位置，ELF文件的所有信息均可通过ELF文件头获得。

### 1.1.1 ELF文件头
数据结构 elfhdr （include/linux/elf.h) 存储ELF文件头。
readelf -h 显示。

	typedef struct elf64_hdr {
	  unsigned char	e_ident[EI_NIDENT];	/* ELF "magic number" */
	  Elf64_Half e_type; 	// 目标文件类型 可执行文件、重定位文件、共享文件、核心文件
	  Elf64_Half e_machine;	// 目标系统架构
	  Elf64_Word e_version;	// 目标文件版本
	  Elf64_Addr e_entry;		/* Entry point virtual address */ // 程序的起始地址
	  Elf64_Off e_phoff;		/* Program header table file offset */ // 程序头在文件中的偏移
	  Elf64_Off e_shoff;		/* Section header table file offset */	 // 节头表在文件中的偏移
	  Elf64_Word e_flags;		// 特定处理器标志
	  Elf64_Half e_ehsize;	// ELF头的大小
	  Elf64_Half e_phentsize;	// 每项程序头的大小
	  Elf64_Half e_phnum;		// 程序头个数
	  Elf64_Half e_shentsize;	// 每项节头的大小
	  Elf64_Half e_shnum;		// 节头个数
	  Elf64_Half e_shstrndx;	 // 节头中字符串的索引
	} Elf64_Ehdr;

### 1.1.3 程序头表
数据结构 elf_phdr （include/linux/elf.h) 存储程序头表。
可执行文件或共享文件其程序头是一个结构体数组。每项描述执行该程序所需要的其他信息。
readelf -l 显示。

	typedef struct elf64_phdr {
	  Elf64_Word p_type;		// 该项的类型
	  Elf64_Word p_flags;		// PF_R PF_W PF_X
	  Elf64_Off p_offset;		/* Segment file offset */	// 该段的开始相在文件中的偏移量
	  Elf64_Addr p_vaddr;		/* Segment virtual address */	// 若使用该字段，则保存该段的虚拟地址
	  Elf64_Addr p_paddr;		/* Segment physical address */ // 若使用该字段，则保存该段的物理地址
	  Elf64_Xword p_filesz;		/* Segment size in file */	// 在文件中该段的大小
	  Elf64_Xword p_memsz;		/* Segment size in memory */		//内存映像中该段的大小
	  Elf64_Xword p_align;		/* Segment alignment, file & memory */		// 该段的对齐方式 该值是2的整数次幂
	} Elf64_Phdr;

### 1.1.3 节头表
数据结构 elf_shdr （include/linux/elf.h)。
readelf -S 显示

	typedef struct elf64_shdr {
	  Elf64_Word sh_name;		/* Section name, index in string tbl */	// 节名
	  Elf64_Word sh_type;		/* Type of section */		// 该节的类型
	  Elf64_Xword sh_flags;		/* Miscellaneous section attributes */	// 该节的属性
	  Elf64_Addr sh_addr;		/* Section virtual addr at execution */	// 该节在内存映像中的位置
	  Elf64_Off sh_offset;		/* Section file offset */	// 该节在ELF文件中的偏移
	  Elf64_Xword sh_size;		/* Size of section in bytes */	// 该节的大小
	  Elf64_Word sh_link;		/* Index of another section */		// 其它节的索引
	  Elf64_Word sh_info;		/* Additional section information */	// 附加信息
	  Elf64_Xword sh_addralign;	/* Section alignment */	// 地址对齐方式
	  Elf64_Xword sh_entsize;	/* Entry size if section holds table */	// 当节中保存一张固定大小的表时，该值表示每项的大小
	} Elf64_Shdr;

### 1.1.4 ELF文件中的节
ELF文件被分成多个节，每节都包含特定类型的信息。
只有当在编译时设置了一定的编译器标志位，某些节才会出现。

- .bss 未初始化的数据
- .comment GCC用于表示编译器的版本信息
- .data 忆初始化的数据
- .debug 以符号表的形式给出的符号调试信息
- .dynamic 动态链接信息
- .dynstr 动态链接时的字符串
- .fini 进程的终止代码 ，GCC的退出代码
- .got 全局偏移量表
- .hash 符号散列表
- .init 初始化代码 
- .interp 程序解释器的路径
- .line 标记调试时的行号
- .note 编译程序控制版本时所需要的信息
- .plt 进程链接表
- .relname 重定位信息
- .rodata 只读数据
- .shstrtab 节名
- .symtab 符号名
- .text 可执行的指令

# 2 内核源代码的构建
源代码分为与体系相关的部分和与体系无关的部分。
arch目录包含所有与体系相关的代码。该目录下列出了支持的体系结构。

## 2.1 体系结构无关的代码
- crypto 加密API的代码
- drivers 设备驱动程序的代码
- fs VFS及Linux支持的文件系统的代码
- include 头文件代码 以asm开头的目录用于存储特定体系结构的头文件。
- ipc 进程间通信的代码 
- init 引导程序以及初始化代码中与体系无关的部分
- kernel 内核空间代码
- lib 辅助函数代码
- mm 内存管理代码 
- net 支持各种网络协议的代码
- security 安全子系统的代码
- sound 支持音频系统的代码

## 2.2 体系相关的代码

## 2.3 文件和目录杂项
- COPYING GPL许可证
- CREDITS 对Linux作用贡献的人名
- MAINTAINERS 提交内核发动时涉及的维护人员及说明
- README 发布说明
- Documentation Linux相关文档
- scripts 构建内核的过程中可以使用的功能及脚本

## 2.4 内核配置
利用内核配置工具自动生成.config的内核配置文件。每个内核配置选项都有相关的名字和值。
其中名字的形式为CONFIG_<NAME>。
变量可以有三个值: y(yes) n(no) m(module)  m表示该选项会以独立于内核源代码的模块的方式编译到内核中。
选项是在调用 make xconfig 时，由qconf配置工具提供。
调用 make defconfig 时按默认生成配置文件

## 2.5 内核构建
调用 make 构建内核，当构建结束时，就会产生一个内核映像文件。
因为内核是通过zlib算法进行压缩的，故可启动的压缩内核映像文件被称作 zImage 或 vmlinuz。