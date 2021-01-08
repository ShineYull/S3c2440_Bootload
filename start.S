/* 制  作：虞灵乐
 * 工程师：虞灵乐
 * 当前版本：version 1.0
 *
 * 文件内容：初始化寄存器、启动PLL(锁相环)、设置栈、
 *			 制作向量表和重定位代码。
 */


.text
.global _start

_start:
	b reset          /* vector 0x00 : reset 		 */
	ldr pc, und_addr /* vector 0x04 : und   		 */
	ldr pc, swi_addr /* vector 0x08 : swi   		 */
	b halt			 /* vector 0x0c : prefetch abort */
	b halt			 /* vector 0x10 : data abort     */
	b halt			 /* vector 0x14 : reserved	     */
	ldr pc, irq_addr /* vector 0x18 : irq		     */
	b halt		     /* vector 0x1c : fiq		     */


und_addr:
	.word do_und

swi_addr:
	.word do_swi

irq_addr:
	.word do_irq


do_und:
	/* datasheet：
	 * 1. lr_und保存有, 被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_und保存有, 被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为11011, 进入到und模式
	 * 4. 跳到0x4的地方执行程序 
	 */

	/* 设置栈：sp_und */
	ldr sp, =0x34000000
	
	/* 保存现场 */
	/* 在und异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址 */
	stmdb sp!, {r0-r12, lr}  
	
	/* 处理und异常 */
	mrs r0, cpsr  /* 传参 */
	ldr r1, =und_string  /* 传参 */
	bl printException
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把SPSR_und的值恢复到cpsr里 */
	
und_string:
	.string "undefined instruction exception"

.align 4  /* 指令地址对齐 */

do_swi:
	/* datasheet：
	 * 1. lr_svc保存有, 被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_svc保存有, 被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10011, 进入到svc模式
	 * 4. 跳到0x08的地方执行程序 
	 */

	/* 设置栈：sp_svc */
	ldr sp, =0x33e00000

	/* 保存现场 */
	/* 在swi异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr是异常处理完后的返回地址 */
	stmdb sp!, {r0-r12, lr}  
	mov r4, lr  /* 后面会进入函数, 所以保存lr用于读取SWI的value */
	
	/* 处理swi异常 */
	mrs r0, cpsr  /* 传参 */
	ldr r1, =swi_string  /* 传参 */
	bl printException  

	/* 读出SWI指令值 */
	sub r0, r4, #4  /* 使r0指向SWI指令 */
	bl printSWIVal
	
	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把SPSR_svc的值恢复到cpsr里 */
	
swi_string:
	.string "swi exception"

.align 4  /* 指令地址对齐 */

do_irq:
	/* datasheet：
	 * 1. lr_irp保存有, 被中断模式中的下一条即将执行的指令的地址
	 * 2. SPSR_irp保存有, 被中断模式的CPSR
	 * 3. CPSR中的M4-M0被设置为10010, 进入到irp模式
	 * 4. 跳到0x18的地方执行程序 
	 */

	/* 设置栈：sp_irq */
	ldr sp, =0x33d00000
	
	/* 保存现场 */
	/* 在irp异常处理函数中有可能会修改r0-r12, 所以先保存 */
	/* lr-4是异常处理完后的返回地址 */
	sub lr, lr, #4
	stmdb sp!, {r0-r12, lr}  


	/* 处理irp异常 */
	bl handle_irq_c

	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^  /* ^会把SPSR_irp的值恢复到cpsr里 */


reset:
	/* 关闭看门狗 */
	ldr r0, =0x53000000
	ldr r1, =0
	str r1, [r0]

	/* 设置MPLL, FCLK : HCLK : PCLK = 400m : 100m : 50m */
	/* LOCKTIME(0x4C000000) = 0xFFFFFFFF */
	ldr r0, =0x4C000000
	ldr r1, =0xFFFFFFFF
	str r1, [r0]

	/* CLKDIVN(0x4C000014) = 0X5, tFCLK:tHCLK:tPCLK = 1:4:8  */
	ldr r0, =0x4C000014
	ldr r1, =0x5
	str r1, [r0]

	/* 设置CPU工作于异步模式 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000   //R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0

	/* 设置MPLLCON(0x4C000004) = (92<<12)|(1<<4)|(1<<0) 
	 *  m = MDIV+8 = 92+8=100
	 *  p = PDIV+2 = 1+2 = 3
	 *  s = SDIV = 1
	 *  FCLK = 2*m*Fin/(p*2^s) = 2*100*12/(3*2^1)=400M
	 */
	ldr r0, =0x4C000004
	ldr r1, =(92<<12)|(1<<4)|(1<<0)
	str r1, [r0]

	/* 一旦设置PLL, 就会锁定lock time直到PLL输出稳定
	 * 然后CPU工作于新的频率FCLK
	 */
	

	/* 启动ICache */
	bl enable_icache
	

	/* 设置内存: sp 栈 */
	/* 分辨是nor/nand启动
	 * 方法：写0到0地址, 再读出来, 如果得到0, 
	 * 表示0地址上的内容被修改了, 它对应sram, 这就是nand启动
	 * 否则就是nor启动
	 */
	mov r1, #0
	ldr r0, [r1] /* 读出原来的值备份 */
	str r1, [r1] /* 0->[0] */ 
	ldr r2, [r1] /* r2=[0] */
	cmp r1, r2   /* r1==r2? 如果相等表示是NAND启动 */
	ldr sp, =0x40000000+4096 /* 先假设是nor启动 */
	moveq sp, #4096  /* nand启动 */
	streq r0, [r1]   /* 恢复原来的值 */

	/* 初始化sdram, 用于重定位 */
	bl init_sdram

	
	/* 因为设置了链接地址的映射, 
	 * 所以要在重定位前就创建页表, 
	 * 并使能MMU, 启动映射功能
	 */
	/* 创建页表 */			
	bl create_page_table

	/* 创建了页表之后
     * 要去操作协处理器, 使能MMU
	 */
	/* 启动MMU */
	bl mmu_enable


	/* 重定位text, rodata, data段整个程序 */
	bl copysdram

	/* 清除BSS段 */
	bl cleanbss

	/* 复位之后, cpu处于svc模式
	 * 现在, 切换到usr模式
	 */
	mrs r0, cpsr      	 /* 读出cpsr */
	bic r0, r0, #0xf  	 /* 修改M4-M0为0b10000, 进入usr模式 */	
	bic r0, r0, #(1<<7)  /* 清除I位，使能总中断 */
	msr cpsr, r0		 /* 修改后的r0, 存回cpsr */

	/* 设置 sp_usr */
	ldr sp, =0x33f00000

	/* 跳入sdram执行程序 */
	ldr pc, =sdram
sdram:
/* 调试用例 */

//	bl uart0_init

//	bl print1
	/* 故意加入一条未定义指令 */
//und_code:
//	.word 0xdeadc0de  /* 未定义指令, 对照datasheet中的指令设置 */
//	bl print2

//	swi 0x123  /* 执行此命令, 触发SWI异常, 进入0x8执行 */

	//bl main 	   /* 相对跳转, 测试用例 */
	ldr pc, =main  /* 绝对跳转, 跳到SDRAM */

halt:
	b halt


enable_icache:
	/* 设置协处理器, 使能icache */
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #(1<<12)
	mcr p15, 0, r0, c1, c0, 0

	/* 返回 */
	mov pc, lr

mmu_enable:
	/* 把页表基地址告诉cp15's C2 */
	ldr r0, =0x32000000
	mcr p15, 0, r0, c2, c0, 0  /* r0写入协处理器cp15' c2 */

	/* 设置域为0xffffffff,	不进行权限检查 */
	ldr r0, =0xffffffff
	mcr p15, 0, r0, c3, c0, 0

	/* 使能Icache, Dcache, mmu */
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #(1<<12)  /* enable Icache */
	orr r0, r0, #(1<<2)   /* enable Dcache */
	orr r0, r0, #(1<<0)   /* enable MMU    */
	mcr p15, 0, r0, c1, c0, 0

	/* 返回 */
	mov pc, lr

