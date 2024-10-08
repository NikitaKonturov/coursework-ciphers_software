import platform
import ctypes
import os


class CCiphers:
    
    __libExtension: str
    __pathToCiphersDir: str
    __dynamicLibraries: dict  # Словарь где ключ название самого шифра как он будет представлен пользователю а значение сам объект подключаемой библиотеки
    
    
    def __init__(self, pathToCiphersDir: str):
        pathToCiphersDir = os.path.abspath(pathToCiphersDir)
        if((not os.path.exists(pathToCiphersDir)) or (not os.path.isdir(pathToCiphersDir))):
            raise Exception("Invalid path to ciphers folder. Path: " + os.path.abspath(pathToCiphersDir)) 
        else:
            self.__pathToCiphersDir = pathToCiphersDir
        
        if(platform.system() == "Windows"):
            self.__libExtension = ".dll"
        elif(platform.system() == "Darwin"):
            self.__libExtension = ".dylib"
        else:
            self.__libExtension = ".so"
        
        self.__dynamicLibraries = {}
        
        self.__generatCiphersDict()
    
        
    
    
    def __generatCiphersDict(self):
        filesList: list = os.listdir(self.__pathToCiphersDir)

        for file in filesList:
            cipherBaseTitle, extension = os.path.splitext(file)
            if (extension == self.__libExtension):
                dynamicLib = ctypes.CDLL(os.path.join(self.__pathToCiphersDir, file))
                
                try:
                    dynamicLib.title.restype = ctypes.c_char_p
                    title_value: str = dynamicLib.title().decode("utf-8")
                    self.__dynamicLibraries[title_value] = dynamicLib
                    title_value = ""
                except(AttributeError):
                    print("Library {cipherBaseTitle} is invalid....", cipherBaseTitle)            
                    
            
        return 0    

    def getCiphersList(self):
        return self.__dynamicLibraries.keys()
        
        
    
        
        
        
        
    
    
    
    
    
    
    
    
    