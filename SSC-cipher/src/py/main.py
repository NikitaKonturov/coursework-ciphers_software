import Simple_substitution_cipher


keys = Simple_substitution_cipher.gen_keys("{\"permutation_size\": 32}", 2)

print(keys)

res = Simple_substitution_cipher.encript(["МАМА","ПАПА"], keys)

for key in res:
    print(res[key])

