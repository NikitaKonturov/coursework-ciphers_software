import importlib
import importlib.util
import logging
import json
import os
import platform
import re
import sys
from pathlib import Path
from typing import BinaryIO, Optional

from bs4 import BeautifulSoup
from ciphers_api_module.requestsClass.requestToEncript import \
    RequToSliceAndEncript
from ciphers_api_module.telegrams_cutter import cut_telegrams
from docx import Document
from settings.config import save_to_docx, Settings

from fastapi.responses import JSONResponse

from .cpp_exceptions import InvalidKey, InvalidOpenText, KeyPropertyError

# ===================================================================================#
# ============== Класс обеспечивающий взаимодействие с модулями шифров ==============#
# ===================================================================================#


class CppCiphers:
    # путь к папке модулей шифров
    __pathToCiphersDir: str
    # словарь где {название модуля шифра: название шифра на английском для frontend}
    # для запроса на шифрование в методы передаёться именно "название модуля шифра"
    __cipherTitles: dict[str, str]

    # метод импортирования библиотеки находящейся в директории pathToModule
    def __import_module(self, moduleTitle: str, pathToModule: str) -> None:
        try:
            specModule = importlib.util.spec_from_file_location(
                moduleTitle, pathToModule)  # создаём специализацию для модуля
            newModule = importlib.util.module_from_spec(
                specModule)  # создаём из специализции сам модуль
            specModule.loader.exec_module(
                newModule)  # делаем модуль исполяемым

            self.__cipherTitles[newModule.__name__] = newModule.__doc__

            # регистрируем новый модуль в текущей сесси python(возможность использовать это модуль будет только локально у этой программы)
            sys.modules[newModule.__name__] = newModule

        except AttributeError as err:
            raise Exception(err)

    # метод загрузки всех библиотек из дериктории
    def __load_modules(self) -> None:
        filesList: list = os.listdir(self.__pathToCiphersDir)
        extension: str = ""
        # также обеспечение кросплатформенности(может не понадобиться)
        if (platform.system() == "Windows"):
            extension = ".pyd"
        elif (platform.system() == "Darwin"):
            extension = ".dylib"
        else:
            extension = ".so"

        # непосредсвено загруска всех модулей
        for file in filesList:
            baseTitle, fileExtension = os.path.splitext(file)
            if (fileExtension == extension):
                self.__import_module(baseTitle, os.path.join(
                    self.__pathToCiphersDir, file))

    # =========================================================================#
    # ============================== Конструктор ==============================#
    # =========================================================================#

    def __init__(self, pathToCiphersDir: str):
        if (not os.path.exists(pathToCiphersDir)):
            raise FileExistsError(
                f"Путь к шифрам не найден! Путь: {pathToCiphersDir}")
        if (not os.path.isdir(pathToCiphersDir)):
            raise FileExistsError(
                f"Путь к шифрам не является папкой! Путь: {pathToCiphersDir}")
        self.__pathToCiphersDir = os.path.abspath(pathToCiphersDir)
        self.__cipherTitles = {}

        self.__load_modules()

    # Получение словаря шифров где {название модуля шифра в текущей сесси python: название шифра на английском для frontend}
    def get_ciphers_dict(self) -> dict[str, str]:
        return self.__cipherTitles

    # Зашифрование телерам по ключам или с генерацией ключей
    # для включения генерации шифров нужно установить keysGeneration флаг в True
    # в keyPropertys должен быть словарь полученый из .json запроса (в fastapi скорее всего Request) на шифрование
    def encrypt_telegrams(self, cipher: str, openTexts: list[str], keys: list[str] | None, keyProperties: dict | None) -> list[dict[str, str]] | None:
        try:
            
            res: list[dict[str, str]]
            res = []

            if (keys == None):
                keys = sys.modules[cipher].gen_keys(
                    str(keyProperties), len(openTexts))
            if (len(openTexts) <= len(keys)):
                print(keys)
                for i in range(len(openTexts)):
                    # Создаем списки из одного элемента
                    single_text = [openTexts[i]]
                    single_key = [keys[i]]
                    
                    # Шифруем одну пару
                    single_result = sys.modules[cipher].encript(single_text, single_key)
                    res.append(single_result)
                    
            else:
                raise AttributeError("Колличество ключей должно быть больше или равно колличеству открытых текстов...")
                
        except Exception as err:
            # Dynamically check for cipher-specific exceptions
            for key, name in self.get_ciphers_dict().items():
                if hasattr(sys.modules[key], 'InvalidKey') and isinstance(err, sys.modules[key].InvalidKey):
                    raise InvalidKey(str(err))
                elif hasattr(sys.modules[key], 'InvalidOpenText') and isinstance(err, sys.modules[key].InvalidOpenText):
                    raise InvalidOpenText(str(err))
                elif hasattr(sys.modules[key], 'KeyPropertyError') and isinstance(err, sys.modules[key].KeyPropertyError):
                    raise KeyPropertyError(str(err))
            
            
            raise Exception(err)

        return res
       

    # Функция получения шаблона свойств ключа
    def get_key_properties(self, cipher: str) -> JSONResponse | None:
        res: Optional[JSONResponse] = None
        try:
            bodyContent: str = sys.modules[cipher].get_key_propertys()
            bodyContent = bodyContent.strip('\\')
            bodyContentDict: dict = json.loads(bodyContent)
            res = JSONResponse(content=bodyContentDict)
        except KeyError as err:
            raise Exception(err)

        return res

    # Функция расшифрования, в keysAndCipherText {ключ расшифровавние: о.т.}
    # cipher назание модуля шифра в текущей сесси python
    def decrypt_telegrams(self, cipher: str, keusAndCipherText:list[dict[str, str]]) -> dict[str, str] | None:
        res: list[dict[str, str]]
        res = []
        
        try:
            for dictData in keusAndCipherText:
                print(dictData)
                if (cipher in sys.modules):
                    res.append(sys.modules[cipher].decript(dictData))
                else:
                    raise TypeError(f"Шифр {cipher} не найден!")

        except Exception as err:
            
            # Dynamically check for cipher-specific exceptions
            for key, name in self.get_ciphers_dict().items():
                if hasattr(sys.modules[key], 'InvalidKey') and isinstance(err, sys.modules[key].InvalidKey):
                    raise InvalidKey(str(err))
                elif hasattr(sys.modules[key], 'InvalidOpenText') and isinstance(err, sys.modules[key].InvalidOpenText):
                    raise InvalidOpenText(str(err))
                elif hasattr(sys.modules[key], 'KeyPropertyError') and isinstance(err, sys.modules[key].KeyPropertyError):
                    raise KeyPropertyError(str(err))
            
            raise Exception(err)

        return res

      


def form_cipher_select_options(ciphers_obj: CppCiphers, dir: Path):
    all_ciphers = ciphers_obj.get_ciphers_dict()

    with open(str(Path(dir, 'select.html')), "r", encoding="utf-8") as file:
        html_content = file.read()

    file.close()

    # Создаем объект BeautifulSoup
    settings_select_html = BeautifulSoup(html_content, 'html.parser')

    select_tag = settings_select_html.find('select', {'id': "ciphersList"})

    for option in select_tag.find_all('option'):
        option.decompose()

    empty_option = settings_select_html.new_tag('option', value="Empty_tag")
    empty_option.string = "Выберите шифр"
    empty_option["disabled"] = True
    empty_option["selected"] = True
    select_tag.append(empty_option)

    for i in all_ciphers:
        new_option = settings_select_html.new_tag('option', value=i)
        new_option.string = all_ciphers[i]
        select_tag.append(new_option)

    with open(str(Path(dir, 'select.html')), "w", encoding="utf-8") as file:
        file.write(settings_select_html.prettify())

    file.close()


def start_encryption(reqToSileAndEncript: RequToSliceAndEncript, pathToSaveFile: Path, ciphers_object: CppCiphers, fiveGrams: str):
    if(reqToSileAndEncript.selfLengthTelegram >= 2 ** 32 or reqToSileAndEncript.selfNumberOfTelegram >= 2 ** 32):
        raise RuntimeError("Длинна телеграммы и их колличество должно быть меньше чем 2^32...")
    telegrams: list[str] = cut_telegrams(reqToSileAndEncript.selfTextFile.__str__(
    ), reqToSileAndEncript.selfLengthTelegram, reqToSileAndEncript.selfNumberOfTelegram)


    enc_resualt: list[dict] = [{}]

    if (reqToSileAndEncript.selfKeysProperties):
        enc_resualt = ciphers_object.encrypt_telegrams(
            reqToSileAndEncript.selfCipher, telegrams, None, reqToSileAndEncript.selfKeysProperties)
    else:
        AllKeys: str = ""
        tempLine: str = ""
        regToNextKey: str = r'\\nextkey'
        regEndKeys: str = r'\\endkeys'

        with open(reqToSileAndEncript.selfFileWithUsersKeys, "r", encoding="utf-8") as file:
            lines = file.read().splitlines()
        
        
        for line in lines:
            if re.search(regEndKeys, line):
                line = re.sub(regEndKeys, "", line)
                AllKeys += line
                break
            AllKeys += line + '\n'

        splitRes:list = re.split(regToNextKey, AllKeys)
        
        for i in range(len(splitRes)):
            splitRes[i] = re.sub(r'^[\s\n]+', '', splitRes[i])
            splitRes[i] = re.sub(r'[\s\n]+$', '', splitRes[i])

        while ('' in splitRes):
            splitRes.remove('')
        
        print(splitRes)
        enc_resualt = ciphers_object.encrypt_telegrams(
            reqToSileAndEncript.selfCipher, telegrams, splitRes, None)
    

    save_to_docx(ciphers_object.decrypt_telegrams(reqToSileAndEncript.selfCipher, enc_resualt), Path(str(pathToSaveFile)[:-5:]+'PlainText.docx'), fiveGrams=False)

    save_to_docx(enc_resualt, pathToSaveFile, fiveGrams)

    return


def check_encryption_telegram(telegram: str) -> bool:
    if (telegram == ''):
        return False
    if (re.search(r"\\text", telegram) == None):
        return False
    return True


def start_decryption(fileWithCipherTextAndKeys: BinaryIO, fileExtension: str, cipher: str, ciphers_object: CppCiphers, pathToSaveFile: Path):
    keysAndCipherText: list[dict[str, str]] = []
    allDataFromFile: str = ""
    if (fileExtension == '.txt'):
        dataLine: str = ""
        while (dataLine):
            dataLine = str(fileWithCipherTextAndKeys.readline()).encode("utf-8")
            allDataFromFile += dataLine
    elif (fileExtension == '.docx'):
        doc = Document(fileWithCipherTextAndKeys)
        for paragraph in doc.paragraphs:
            allDataFromFile += paragraph.text
    else:
        raise AttributeError("Возможное расширение файла .txt или .docx!")

    regToKeys: str = r"\\key"
    regToText: str = r"\\text"
    listOfTheEncriptTelegrams: list[str] = re.split(regToKeys, allDataFromFile)
    tempKeyAndCipherText: dict[str, str] = {}
    for telegram in listOfTheEncriptTelegrams:
        if (check_encryption_telegram(telegram)):
            tempKeyAndCipherText = re.split(regToText, telegram)
            keysAndCipherText.append({tempKeyAndCipherText[0]: ''.join(re.findall( r'[А-Яа-яA-Za-z0-9]', tempKeyAndCipherText[1]))})
                              

    dec_result: dict[str, str] = ciphers_object.decrypt_telegrams(
        cipher, keysAndCipherText)

    save_to_docx(dec_result, pathToSaveFile, "false")
