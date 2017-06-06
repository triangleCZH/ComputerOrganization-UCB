addiu $t0 $a0 0
L1: lw $t1 0($t0)
# This is a comment
L2:
ori $t1 $t1 0xABCD
addiu $t1 $t1 3
bne $t1 $a2 L2
