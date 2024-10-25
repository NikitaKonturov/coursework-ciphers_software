import os.path
import sys

from docx import Document # Импорт класса Document в программу

def check_file_path(file_path : str):
    if os.path.exists(file_path):
        print(f'The file {file_path} exists, continuing work...')
    else:
        print(f'The file {file_path} does not exist')
        sys.exit(0)
    
def txt_to_docx(txtFile : str, docxFile): # Определение будущей функции
    
    check_file_path(file_path)
    
    doc = Document()
    
    with open(txtFile, 'r', encoding='utf-8') as file: # With гарантирует, что файл автоматически закроется
        # 'r' открывает файл в режиме для чтения, encoding указывает, что текст должен читаться именно в указанной кодировке
        for line in file:
            
            everyNewLine = line.strip()
            
            if everyNewLine == '':
                doc.add_paragraph()
            
            else:
                doc.add_paragraph(line.rstrip())
                    
    doc.save(docxFile)

file_path = r'D:/VsCode/PythonProjects/testu.txt'

txt_to_docx(file_path, 'outputu.docx')
