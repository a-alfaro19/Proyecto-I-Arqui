# tea_decrypt(v_ptr, key_ptr)
#
# Decrypts an encrypted 64-bit data block using the Tiny Encryption Algorithm (TEA).
#
# Arguments:
# a0: Address of the data block (v)
# a1: Address of the encryption key (key)

.section .text
.globl tea_decrypt
tea_decrypt:
  # --- Reserve stack and save s-regs ---
  addi    sp, sp, -32
  sw      s0,  0(sp)
  sw      s1,  4(sp)
  sw      s2,  8(sp)
  sw      s3, 12(sp)
  sw      s4, 16(sp)
  sw      s5, 20(sp)

  # --- Load v0, v1 ---
  lw t0, 0(a0)          # v0 = v[0]
  lw t1, 4(a0)          # v1 = v[1]

  # sum = DELTA * 32; rounds = 32; DELTA = 0x9E3779B9; preload key values 
  li t2, 0xC6EF3720        # t2 = DELTA * 32
  lw      s0, 0(a1)     # s0 = key[0]
  lw      s1, 4(a1)     # s1 = key[1]
  lw      s2, 8(a1)     # s2 = key[2]
  lw      s3, 12(a1)    # s3 = key[3]
  li s4, 0x9E3779B9     # DELTA (constant)
  li s5, 32 # rounds (loop counter)

  inverse_loop:
    # v1 -= ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3])
    sll t4, t0, 4             # t4 = v0 << 4
    add t4, t4, s2            # t4 = (v0 << 4) + key[2]

    add t5, t0, t2            # t5 = v0 + sum
    xor t4, t4, t5            # t4 = ((v0 << 4) + key[2]) ^ (v0 + sum)

    srl t5, t0, 5             # t5 = v0 >> 5
    add t5, t5, s3            # t5 = (v0 >> 5) + key[3]

    xor t4, t4, t5            # t4 ^= (v0 >> 5) + key[3]
    sub t1, t1, t4            # v1 -= t4

    # v0 -= ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1])
    sll t4, t1, 4             # t4 = v1 << 4
    add t4, t4, s0            # t4 = (v1 << 4) + key[0]

    add t5, t1, t2            # t5 = v1 + sum
    xor t4, t4, t5            # t4 = ((v1 << 4) + key[0]) ^ (v1 + sum)

    srl t5, t1, 5             # t5 = v1 >> 5
    add t5, t5, s1            # t5 = (v1 >> 5) + key[1]

    xor t4, t4, t5            # t4 ^= (v1 >> 5) + key[1]
    sub t0, t0, t4            # v0 -= t4

    # sum -= DELTA
    sub t2, t2, s4  

    # round -= 1; rounds == 0 ? continue : break
    addi s5, s5, -1
    bnez s5, inverse_loop

  # --- Store v0, v1 --- 
  sw t0, 0(a0)                # v0 = v[0]
  sw t1, 4(a0)                # v1 = v[1]

  # --- Restore s-regs and stack ---
  lw s0, 0(sp)
  lw      s1,  4(sp)
  lw      s2,  8(sp)
  lw      s3, 12(sp)
  lw      s4, 16(sp)
  lw      s5, 20(sp)
  addi    sp, sp, 32

  ret