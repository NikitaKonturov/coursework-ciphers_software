from typing import BinaryIO
from pathlib import Path
import re
import os

class FileLanguageError(Exception):
    def __init__(self, errorLanguage: str, message: str = "Invalid language in file..."):
        self.errorLanguage = errorLanguage
        self.message = message
        super().__init__(message)

    def __str__(self):
        return f"{self.message} (Language: {self.errorLanguage})"


def save_open_text_as_txt_file(language: str, file: BinaryIO, pathToSaveTxt: Path, bufferSize: int = 20):
    with open(pathToSaveTxt, "w", encoding="utf-8") as resTxtFile:
        dataBuffer: str = " "
        while dataBuffer != '':
            dataBuffer = file.read(bufferSize).decode('utf-8')
            if(language.lower() == "ru"):
                checkPart = ''.join(re.findall(r'[A-Za-z]', dataBuffer))
                cleanedText = ''.join(re.findall(r'[А-Яа-я]', dataBuffer)).upper()

                if checkPart:
                    raise FileLanguageError(f'The file contains symbols from the other language', errorLanguage="en")
            
                if cleanedText:
                    resTxtFile.write(cleanedText)
            elif(language.lower() == "en"):
                checkPart = ''.join(re.findall(r'[А-Яа-я]', dataBuffer))
                cleanedText = ''.join(re.findall(r'[A-Za-z]', dataBuffer)).upper()

                if checkPart:
                    raise FileLanguageError(f'The file contains symbols from the other language', errorLanguage="ru")
            
                if cleanedText:
                    resTxtFile.write(cleanedText)        
            else:
                raise FileLanguageError(f'The language is not defined. Supported languages: ru, en', errorLanguage="any")
    return

def save_as_txt_file(file: BinaryIO, pathToSaveTxtFile: Path, bufferSize: int = 20):
    with open(pathToSaveTxtFile, "wb", encoding = "utf-8") as resTxtFile:
        dataBuffer = b' '
        while dataBuffer.decode('utf-8') != '':
            dataBuffer = file.read(bufferSize)
            resTxtFile.write(dataBuffer)
    return
