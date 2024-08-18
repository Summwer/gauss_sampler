.global main
 
.extern add  // 声明外部函数 add
 
main:
    MOV R0, #5       // 第一个参数 a = 5
    MOV R1, #10      // 第二个参数 b = 10
    BL add           // 调用 C 函数 add
    MOV R7, #1       // syscall: exit
    SWI 0            // 软件中断，退出程序




    