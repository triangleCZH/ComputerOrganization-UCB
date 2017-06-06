label1: addiu $s0 $s1 0xFFF
move $s0 $s1
rem $s0 $s1 $s2
bge $s0 $s1 label1
bnez $s0 label1 
