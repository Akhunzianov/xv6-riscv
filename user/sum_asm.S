.extern printf
.extern fprintf
.extern gets
.extern atoi
.extern exit

.set BUFFER_SIZE, 100

.global main

.section .text
main:
    addi sp, sp, -128
    la a0, buf 
    li a1, BUFFER_SIZE
    call gets

    la a0, buf  
    call strlen    
    mv t0, a0      
    beqz t0, check_digits_and_spaces  
    addi t0, t0, -1
    la a1, buf     
    add a1, a1, t0  
    lb t1, 0(a1) 
    li t2, '\n'   
    bne t1, t2, check_digits_and_spaces
    sb zero, 0(a1)   

check_digits_and_spaces:
    la a0, buf
check_loop:
    lb t1, 0(a0) 
    beqz t1, check_passed
    li t2, ' '
    beq t1, t2, next_char
    li t2, '0'
    blt t1, t2, invalid_arg
    li t2, '9'
    ble t1, t2, next_char
    j invalid_arg

next_char:
    addi a0, a0, 1
    j check_loop

check_passed:
    la a0, fmt_print
    la a1, buf
    call printf

    la a0, buf
    li a1, ' '
    call strchr 
    mv s0, a0
    beqz s0, space_not_found

    li t0, 0
    sb t0, 0(s0) 
    addi s1, s0, 1
    lb t1, 0(s1) 
    beqz t1, second_empty

    la a0, buf
    lb t1, 0(a0) 
    beqz t1, first_empty

    la a0, buf
    li a1, ' '
    call strchr 
    mv t0, a0  
    bnez t0, too_many_spaces

    mv a0, s1
    li a1, ' '
    call strchr 
    mv t0, a0  
    bnez t0, too_many_spaces

    la a0, buf
    call atoi
    mv s2, a0
    mv a0, s1
    call atoi
    mv s3, a0
    add s4, s2, s3

    la a0, fmt_sum
    mv a1, s4
    call printf
    j finish

invalid_arg:
    li a0, 2
    la a1, err_invalid_arg
    call fprintf
    li a0, 1
    call exit

space_not_found:
    li a0, 2
    la a1, err_no_space
    call fprintf
    li a0, 1
    call exit

first_empty:
    li a0, 2
    la a1, err_no_fst_number
    call fprintf
    li a0, 1
    call exit

second_empty:
    li a0, 2
    la a1, err_no_snd_number
    call fprintf
    li a0, 1
    call exit

too_many_spaces:
    li a0, 2
    la a1, err_too_many_spaces
    call fprintf
    li a0, 1
    call exit

finish:
    li a0, 0
    call exit

.section .rodata
fmt_print:
    .asciz "|%s|\n"
fmt_sum:
    .asciz "%d\n"
err_invalid_arg:
    .asciz "sum_asm: invalid argument\n"
err_no_space:
    .asciz "sum_asm: no space in input\n"
err_no_fst_number:
    .asciz "sum_asm: no fst number\n"
err_no_snd_number:
    .asciz "sum_asm: no snd number\n"
err_too_many_spaces:
    .asciz "sum_asm: too many spaces\n"

.data
buf:
    .space BUFFER_SIZE