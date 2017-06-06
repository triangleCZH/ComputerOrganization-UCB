# CS 61C Summer 2016 Project 2-2
# string.s

#==============================================================================
#                              Project 2-2 Part 1
#                               String README
#==============================================================================
# In this file you will be implementing some utilities for manipulating strings.
# The functions you need to implement are:
#  - strlen()
#  - strncpy()
#  - copy_of_str()
# Test cases are in linker-tests/test_string.s
#==============================================================================

.data
newline:	.asciiz "\n"
tab:	.asciiz "\t"

.text
#------------------------------------------------------------------------------
# function strlen()
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = string input
#
# Returns: the length of the string
#------------------------------------------------------------------------------
strlen:
	addiu $sp, $sp, -4 # This is the Prologue
	sw $ra, 0($sp)

	addiu $v0, $0, 0 #initializing the return value to 0

strlen_loop:
	lbu $t0, 0($a0) #loading the first element of the string into $t0
	beq $t0, $0, strlen_end #if the string has nothing at all in the first place, then jump to the Epilogue
	addiu $v0, $v0, 1 #if there is indeed one elemnet, increment $v0 by one
	addiu $a0, $a0, 1 #incrementing the address, for reading the next character
	j strlen_loop


strlen_end:
	lw $ra, 0($sp)
	addiu $sp, $sp, 4
	jr $ra # return
#fixme: should we ignore certain characters during the string copy?
#------------------------------------------------------------------------------
# function strncpy()
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = pointer to destination array
#  $a1 = source string
#  $a2 = number of characters to copy
#
# Returns: the destination array
#------------------------------------------------------------------------------
strncpy:
	addiu $sp, $sp, -4 # This is the Prologue
	sw $ra, 0($sp)

	addiu $v0, $a0, 0 # setting the return value to the destination array
	addiu $t1, $0, -1 # initializing the counter to be 0, in order to copy the null terimator at the end of the string

strncpy_loop:
	lbu $t0, 0($a1) #t0 stores the character to be copied, loading the elements into the t0
	beq $t1, $a2, strncpy_end
	#fixme: do we need to stop when encountering the null terminator?
	sb $t0, 0($a0)
	addi $t1, $t1, 1 #incrementing t1 by one
	addi $a0, $a0, 1
	addi $a1, $a1, 1
	j strncpy_loop

strncpy_end:
	lw $ra, 0($sp)
	addiu $sp, $sp, 4
	jr $ra # return

#------------------------------------------------------------------------------
# function copy_of_str()
#------------------------------------------------------------------------------
# Creates a copy of a string. You will need to use sbrk (syscall 9) to allocate
# space for the string. strlen() and strncpy() will be helpful for this function.
# In MARS, to malloc memory use the sbrk syscall (syscall 9). See help for details.
#
# Arguments:
#   $a0 = string to copy
#
# Returns: pointer to the copy of the string
#------------------------------------------------------------------------------
copy_of_str:
	addiu $sp,$sp,-12 # This is the Prologue
	sw $ra,8($sp) # Save saved registers
	sw $s0,4($sp)
	sw $s1,0($sp)

	addiu $s0, $a0, 0 # Temporarily storing the pointer to the argument string to $s0, as running strlen will change it

	jal strlen #obtaining the length of the source string, for memory allocation and copying
	addiu $s1, $v0, 0 # Storing the length of the source string to $s1

	#Memory allocation
	addiu $a0, $s1, 1 # storing the memory required for the new string, adding one to include the space for the null terminator
	li  $v0, 9 #for syscall
	syscall

	addiu $a0, $v0, 0 #storing the pointer (from syscall) to the target string to $a0
	addiu $a1, $s0, 0 #preparing for calling strncpy
	addiu $a2, $s1, 0
	jal strncpy

	lw $s1,0($sp) # This is the Epilogue
	lw $s0,4($sp) # Restore saved registers
	lw $ra,8($sp)
	addiu $sp,$sp,12
	jr $ra # return

###############################################################################
#                 DO NOT MODIFY ANYTHING BELOW THIS POINT
###############################################################################

#------------------------------------------------------------------------------
# function streq() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = string 1
#  $a1 = string 2
#
# Returns: 0 if string 1 and string 2 are equal, -1 if they are not equal
#------------------------------------------------------------------------------
streq:
	beq $a0, $0, streq_false	# Begin streq()
	beq $a1, $0, streq_false
streq_loop:
	lb $t0, 0($a0)
	lb $t1, 0($a1)
	addiu $a0, $a0, 1
	addiu $a1, $a1, 1
	bne $t0, $t1, streq_false
	beq $t0, $0, streq_true
	j streq_loop
streq_true:
	li $v0, 0
	jr $ra
streq_false:
	li $v0, -1
	jr $ra			# End streq()

#------------------------------------------------------------------------------
# function dec_to_str() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Convert a number to its unsigned decimal integer string representation, eg.
# 35 => "35", 1024 => "1024".
#
# Arguments:
#  $a0 = int to write
#  $a1 = character buffer to write into
#
# Returns: the number of digits written
#------------------------------------------------------------------------------
dec_to_str:
	li $t0, 10			# Begin dec_to_str()
	li $v0, 0
dec_to_str_largest_divisor:
	div $a0, $t0
	mflo $t1		# Quotient
	beq $t1, $0, dec_to_str_next
	mul $t0, $t0, 10
	j dec_to_str_largest_divisor
dec_to_str_next:
	mfhi $t2		# Remainder
dec_to_str_write:
	div $t0, $t0, 10	# Largest divisible amount
	div $t2, $t0
	mflo $t3		# extract digit to write
	addiu $t3, $t3, 48	# convert num -> ASCII
	sb $t3, 0($a1)
	addiu $a1, $a1, 1
	addiu $v0, $v0, 1
	mfhi $t2		# setup for next round
	bne $t2, $0, dec_to_str_write
	jr $ra			# End dec_to_str()
