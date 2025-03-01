import os
from typing import BinaryIO
from pathlib import Path
import re
from docx import Document


class FileLanguageError(Exception):
    def __init__(self, errorLanguage: str, message: str = "Неизвестный язык файла."):
        self.errorLanguage = errorLanguage
        self.message = message
        super().__init__(message)

    def __str__(self):
        return f"{self.message} (Language: {self.errorLanguage})"


def check_file_path(filePath: str):
    if not os.path.exists(filePath):
        raise FileExistsError(f'The file {filePath} does not exist')


def save_open_text_docx_as_bin_file(language: str, openTextFile: BinaryIO, saveOpenTextTxtFile: Path):

    doc = Document(openTextFile)

    if (language.lower() == 'ru'):
        with open(saveOpenTextTxtFile, 'bw') as binFile:
            for para in doc.paragraphs:
                checkPart = ''.join(re.findall(r'[A-Za-z]', para.text))
                cleanedText = ''.join(re.findall(
                    r'[А-Яа-я]', para.text)).upper()

                if checkPart:
                    raise FileLanguageError(
                        f'Файл содержит символы другого языка!', errorLanguage="en")

                if cleanedText:
                    binFile.write(cleanedText.encode("utf-16-le"))

    elif (language.lower() == 'en'):
        with open(saveOpenTextTxtFile, 'bw') as binFile:
            for para in doc.paragraphs:
                checkPart = ''.join(re.findall(r'[А-Яа-я]', para.text))
                cleanedText = ''.join(re.findall(
                    r'[A-Za-z]', para.text)).upper()

                if checkPart:
                    raise FileLanguageError(
                        f'Файл содержит символы другого языка!', errorLanguage="ru")

                if cleanedText:
                    binFile.write(cleanedText.encode("utf-16-le"))

    else:
        raise FileLanguageError(errorLanguage="Anny", message='Неверный язык!')


def save_docx_as_txt(textFile: BinaryIO, saveTxtFile: Path):
    doc = Document(textFile)
    with open(saveTxtFile, "w", encoding='utf-8') as txtFile:
        for paragraph in doc.paragraphs:
            txtFile.write(paragraph.text)

    return
