/**
 * Initialises the LFSR of crypto1
 * @param key A 48 bit key
 */
crypto1_init(key)

/**
 * Shifts the LFSR of crypto1 to the left and inserts 
 * shift_bit ^ feedback_bit in the rightmost position.
 * @param key shift_bit The bit to shift into the state
 * @return The next bit of the keystream
 */
crypto1_init(shift_bit)

/**
 * Psuedo code for brute-force attack against Mifare Classic.
 * @param auths A list with valid authentication attempts
 * @param uid The unique identifier of the card
 * @return A valid key
 */
int mifare_bf(auths, uid):
    assert(auth.size > 5)
    for key = 1 to 2^48
        foreach a in auths
            crypto1_init(key)
            array keystream[32]
            // shift in the card uid and tag nonce producing
            // 32 bits of keystream
            for i = 0 to 31
                keystream[i] = crypto1_shift(a.Nt[i] ^ uid[i])
            // decrypt the reader nonce
            Nr = a.{Nr} ^ keystream
            if !check_parity_bits(Nr)
                try next key
            // shift in the reader nonce, producing another 32
            // bits of keystream
            for i = 0 to 31
                keystream[i] = crypto1_shift(Nr)
            // decrypt the reader response
            Ar = {Ar} ^ keystream
            if !check_parity_bits(Ar)
                try next key
            // check if the this key produces the correct response
            if suc(Nt, 64) != Ar
                try next key
        return key
    return error