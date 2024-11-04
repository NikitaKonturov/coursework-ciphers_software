from fastapi import FastAPI, Form, File, UploadFile
from fastapi.requests import Request
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from pathlib import Path
import uvicorn
import webview
import time
import threading
from pydantic import BaseModel
from bs4 import BeautifulSoup
from ciphers_api_module.ciphers_api_module import CppCiphers


app = FastAPI()

BASE_DIR = Path(__file__).resolve().parent

ciphers_obj = CppCiphers(pathToCiphersDir=str(Path(BASE_DIR, 'Ciphers')))
all_ciphers = ciphers_obj.get_ciphers_dict()

with open(str(Path(BASE_DIR, 'templates')) + '\\select.html', "r", encoding="utf-8") as file:
    html_content = file.read()

file.close()

# Создаем объект BeautifulSoup
settings_select_html = BeautifulSoup(html_content, 'html.parser')

select_tag = settings_select_html.find('select', {'id': "option"})

for option in select_tag.find_all('option'):
    option.decompose()

for i in all_ciphers:
    new_option = settings_select_html.new_tag('option', value=i)
    new_option.string = all_ciphers[i]
    select_tag.append(new_option)
    
with open(str(Path(BASE_DIR, 'templates')) + '\\select.html', "w", encoding="utf-8") as file:
    file.write(settings_select_html.prettify())

file.close()


templates = Jinja2Templates(directory=str(Path(BASE_DIR, 'templates')))

app.mount('/static', StaticFiles(directory=str(Path(BASE_DIR, 'static'))), name='static')

class SelectOptions(BaseModel):
    option: str
    file: UploadFile
    length: int
    number: int
    
@app.get('/', response_class=HTMLResponse)
async def main(request: Request):
    return templates.TemplateResponse(request=request, name='main.html')

@app.post('/selectCipher')
async def select_cipher(option: str = Form(...), file: UploadFile = File(...), length: int = Form(...), number: int = Form(...)):
    return JSONResponse(
        {
            "option": option,
            "file": file.filename,
            "length": length,
            "number": number
        }
    )

@app.get('/select', response_class=HTMLResponse)
async def select(request: Request):
    return templates.TemplateResponse(request=request, name='select.html')

def start_server():
    uvicorn.run("__main__:app", host="127.0.0.1", port=8000, reload=False)

def start_webview():
    time.sleep(1)
    webview.create_window("FastAPI Desktop App", "http://127.0.0.1:8000")
    webview.start()

if __name__ == "__main__":
    server_thread = threading.Thread(target=start_server)
    server_thread.daemon = True
    server_thread.start()
    start_webview()