li t0, 0 # Loop counter
li t1, 32 # Loop limit

loop_start:
  # Loop Condition Check
  bge t0, t1, loop_end      # counter >= limit

  # logic...

  # Increment loop counter
  addi t0, t0, 1            # counter = counter + 1

  # Jump back to loop_start
  j loop_start

loop_end:


# End program
li a7, 10                   # Set system call nuumber
li a0, 0                    # Set exit status
ecall                       # Execute system call
