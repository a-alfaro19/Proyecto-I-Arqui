tea_decrypt:

  li t0, 32 # rounds (loop counter)

  loop:

    # logic...

    # round -= 1; rounds == 0 ? continue : break
    addi t0, t0, -1
    bnez t0, loop

  ret