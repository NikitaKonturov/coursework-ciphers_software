from module.CCiphers_module import CCiphers
import platform
import ctypes
import os

path: str = os.path.dirname(__file__)

ciphers = CCiphers(pathToCiphersDir = path)

print(ciphers.getCiphersList())

