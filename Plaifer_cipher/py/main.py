import Plaifer_cipher

keys = Plaifer_cipher.gen_keys("{}", 2)

print(keys)


print(Plaifer_cipher.encript(["HELLOW","WORLDS"], keys))

#print(Plaifer_cipher.decript({"8 9 4 16 11 19 3 7 24 18 1 2 25 22 20 14 10 6 12 21 13 5 17 23 15":"INHHMA"}))