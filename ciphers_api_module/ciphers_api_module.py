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
from settings.config import save_to_docx

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
            print(err)

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
    def encrypt_telegrams(self, cipher: str, openTexts: list[str], keys: list[str] | None, keyProperties: dict | None) -> dict[str, str] | None:
        try:
            res: Optional[dict[str, str]]
            res = None
            if (keys == None):
                keys = sys.modules[cipher].gen_keys(
                    str(keyProperties), len(openTexts))
                print(keys)
            if (len(openTexts) <= len(keys)):
                res = sys.modules[cipher].encript(openTexts, keys)
            else:
                # !!!!!!!! Ошибка !!!!!!!!! не обрабатывается
                raise AttributeError("Колличество ключей должно быть больше или равно колличеству открытых текстов...")
                # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        except TypeError as err:
            print(err)

        except RuntimeError as err:
            print(err)

        # except sys.modules[cipher].InvalidKey as err:
        #    raise InvalidKey(err)

        # except sys.modules[cipher].InvalidOpenText as err:
        #    raise InvalidOpenText(err)

        # except sys.modules[cipher].KeyPropertyError as err:
        #    raise KeyPropertyError(err)

        except Exception as err:
            print(err)
            # Dynamically check for cipher-specific exceptions
            for key, name in self.get_ciphers_dict().items():
                if hasattr(sys.modules[key], 'InvalidKey') and isinstance(err, sys.modules[key].InvalidKey):
                    raise InvalidKey(str(err))
                elif hasattr(sys.modules[key], 'InvalidOpenText') and isinstance(err, sys.modules[key].InvalidOpenText):
                    raise InvalidOpenText(str(err))
                elif hasattr(sys.modules[key], 'KeyPropertyError') and isinstance(err, sys.modules[key].KeyPropertyError):
                    raise KeyPropertyError(str(err))
            logging.error(err.__str__())
            
            raise Exception(err)

        return res
        return res

    # Функция получения шаблона свойств ключа

    def get_key_properties(self, cipher: str) -> JSONResponse | None:
        res: Optional[JSONResponse] = None
        try:
            bodyContent: str = sys.modules[cipher].get_key_propertys()
            bodyContent = bodyContent.strip('\\')
            bodyContentDict: dict = json.loads(bodyContent)
            print(bodyContentDict)
            res = JSONResponse(content=bodyContentDict)
            print(res.body)
        except KeyError as err:
            print(err)

        return res

    # Функция расшифрования, в keysAndCipherText {ключ расшифровавние: о.т.}
    # cipher назание модуля шифра в текущей сесси python
    def decrypt_telegrams(self, cipher: str, keusAndCipherText: dict[str, str]) -> dict[str, str] | None:
        res: Optional[dict[str, str]]
        res = None
        try:
            if (cipher in sys.modules):
                res = sys.modules[cipher].decript(keusAndCipherText)
            else:
                raise TypeError(f"Шифр {cipher} не найден!")
        except TypeError as err:
            print(err)
        except Exception as err:
            print(err)
            # Dynamically check for cipher-specific exceptions
            for key, name in self.get_ciphers_dict().items():
                if hasattr(sys.modules[key], 'InvalidKey') and isinstance(err, sys.modules[key].InvalidKey):
                    raise InvalidKey(str(err))
                elif hasattr(sys.modules[key], 'InvalidOpenText') and isinstance(err, sys.modules[key].InvalidOpenText):
                    raise InvalidOpenText(str(err))
                elif hasattr(sys.modules[key], 'KeyPropertyError') and isinstance(err, sys.modules[key].KeyPropertyError):
                    raise KeyPropertyError(str(err))
            
            logging.error(f"Неизвестная ошибка: {err}")
            raise Exception(err)

        return res

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


def start_encryption(reqToSileAndEncript: RequToSliceAndEncript, pathToSaveFile: Path, ciphers_object: CppCiphers):
    telegrams: list[str] = cut_telegrams(reqToSileAndEncript.selfTextFile.__str__(
    ), reqToSileAndEncript.selfLengthTelegram, reqToSileAndEncript.selfNumberOfTelegram)

    print(telegrams)

    enc_resualt: dict = {}

    if (reqToSileAndEncript.selfKeysProperties):
        enc_resualt = ciphers_object.encrypt_telegrams(
            reqToSileAndEncript.selfCipher, telegrams, None, reqToSileAndEncript.selfKeysProperties)
    else:
        AllKeys: str = ""
        tempLine: str = ""
        regToNextKey: str = r'\\nextkey'
        regEndKeys: str = r'\\endkeys'

        with open(reqToSileAndEncript.selfFileWithUsersKeys, "r") as file:
            tempLine = file.readline()
            while (tempLine):
                if (re.search(regEndKeys, tempLine)):
                    tempLine = re.sub(regEndKeys, "", tempLine)
                    AllKeys = AllKeys + tempLine
                    break
                AllKeys = AllKeys + tempLine
                tempLine = file.readline()
                print(tempLine)

        enc_resualt = ciphers_object.encrypt_telegrams(
            reqToSileAndEncript.selfCipher, telegrams, re.split(regToNextKey, AllKeys), None)

    save_to_docx(enc_resualt, pathToSaveFile)

    return


def check_encryption_telegram(telegram: str) -> bool:
    if (telegram == ''):
        return False
    if (re.search(r"\\text", telegram) == None):
        return False
    return True


def start_decryption(fileWithCipherTextAndKeys: BinaryIO, fileExtension: str, cipher: str, ciphers_object: CppCiphers, pathToSaveFile: Path):
    keysAndCipherText: dict[str, str] = {}
    allDataFromFile: str = ""
    if (fileExtension == '.txt'):
        dataLine: str = ""
        while (dataLine):
            dataLine = str(fileWithCipherTextAndKeys.readline()
                           ).encode("utf-8")
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
            keysAndCipherText[tempKeyAndCipherText[0]
                              ] = tempKeyAndCipherText[1]

    dec_result: dict[str, str] = ciphers_object.decrypt_telegrams(
        cipher, keysAndCipherText)

    save_to_docx(dec_result, pathToSaveFile)
