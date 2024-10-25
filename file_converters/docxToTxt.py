import sys
import os

from docx import Document

def check_file_path(file_path : str):
    if os.path.exists(file_path):
        print(f'The file {file_path} exists, continuing work...')
    else:
        print(f'The file {file_path} does not exist')
        sys.exit(0)

def docx_to_txt(docxFile : str, txtFile):
    
    check_file_path(file_path)
    
    # Открываем документ .docx
    doc = Document(docxFile)
    
    # Записываем текст в файл .txt поблочно
    with open(txtFile, 'w', encoding='utf-8') as txt_file:
        for para in doc.paragraphs:
            txt_file.write(para.text + '\n')

file_path = r'D:/VsCode/PythonProjects/test.docx'

docx_to_txt(file_path, 'outputu.txt')
