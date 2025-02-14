import ciphers_api_module.ciphers_api_module as cipher_api
from fastapi.encoders import jsonable_encoder
import os
import sys


pathToLibs = os.path.join(os.path.dirname(__file__), "include_ciphers")

api = cipher_api.CppCiphers(pathToCiphersDir = pathToLibs)

print(api.get_ciphers_dict())
# # test: JSONResponse = JSONResponse(content= "agvafyuh")
# # template: str = b'{ "tm_hour": 23, "tm_mday": 31, "tm_min": 59, "tm_mon": 12, "tm_sec": 59, "tm_wday": 0, "tm_yday": 364, "tm_year": 2023}'
# # test.body = template
print(jsonable_encoder(api.get_key_propertys('HPC_cipher').body))

# # print(test.body.decode())

keysAndCipherTexts = api.encript_telegrams("HPC_cipher", ["HelloWorld", "WorldHello"], None, {"permutation_size": 5})

print(keysAndCipherTexts)

print(api.decript_telegrams("HPC_cipher", keysAndCipherTexts))

#print(api.decript_telegrams("HPC_cipher", api.encript_telegrams("HPC_cipher", ["Hello world!!!"], [" It's me"])))
# print(api.encript_telegram("Hello world!!!", 19, "SHPZ-cipher")), 


