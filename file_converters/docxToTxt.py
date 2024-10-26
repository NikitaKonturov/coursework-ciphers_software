import os
import re

from docx import Document

def check_file_path(file_path : str):
    if os.path.exists(file_path):
        print(f'The file {file_path} exists, continuing work...')
    else:
        raise Exception(f'The file {file_path} does not exist')
        
def docx_to_txt(docxFile : str, txtFile):
    
    check_file_path(filePath)
    
    # Открываем документ .docx
    doc = Document(docxFile)
    
    # Записываем текст в файл .txt поблочно
    with open(txtFile, 'w', encoding='utf-8') as txt_file:
        for para in doc.paragraphs:
            cleanedText = ''.join(re.findall(r'[А-Яа-я]', para.text)).upper()
            
            if cleanedText:
                txt_file.write(cleanedText)

filePath = r'D:/download/Tolstoy_Anna_Karenina.docx'

docx_to_txt(filePath, 'outputu.txt')
