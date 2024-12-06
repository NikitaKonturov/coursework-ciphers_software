import sys
import re
import threading
import docx
import time
from pathlib import Path
import logging
from typing import Annotated, BinaryIO

import uvicorn
import webview
from bs4 import BeautifulSoup
from ciphers_api_module.ciphers_api_module import CppCiphers
from file_converters.saveTxtFile import save_open_text_as_txt_file, save_as_txt_file
from file_converters.docxToTxt import save_open_text_docx_as_txt, save_docx_as_txt
from file_converters.saveToDocx import save_to_docx


from pydantic import BaseModel, ConfigDict, create_model, AfterValidator, ValidationError, PlainValidator
from pydantic_settings import BaseSettings
from starlette.middleware.base import BaseHTTPMiddleware
from starlette.responses import Response
from telegrams_cutter import cut_telegrams

from fastapi import FastAPI, File, Form, UploadFile, status
from fastapi.requests import Request
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates




app = FastAPI()

logging.basicConfig(format='\033[32m%(levelname)s\033[0m:\t%(message)s', level=logging.DEBUG)

BASE_DIR = Path(__file__).resolve().parent
SAVE_DIR = Path(BASE_DIR,"encript_resualts")


class NoCacheMiddleware(BaseHTTPMiddleware):
    async def dispatch(self, request, call_next):
        response: Response = await call_next(request)
        response.headers["Cache-Control"] = "no-store"
        return response

def checkCipher(sourceCipher: str):
    if((sourceCipher != "None") and (sourceCipher not in sys.modules.keys())):
        raise ValidationError(f"The cipher ${sourceCipher} was not found")     
    return sourceCipher

def checkLenghtTelegram(tgLength: int):
    if(tgLength < 0 or tgLength == 0):
        raise ValidationError(f"The length of the telegram must be natural. Current length ${tgLength}")
    return tgLength

def checkNumbersOfTelegrams(tgNumbers: int):
    if(tgNumbers < 0 or tgNumbers == 0):
        raise ValidationError(f"The size of the telegram must be natural. Current length ${tgNumbers}")
    return tgNumbers

def checkKeysType(sourceKeysType: str):
    if(sourceKeysType != "keys_settings" and sourceKeysType != "users_keys"):
        raise ValidationError(f"Keys type must be users_keys or keys_settings")
    return sourceKeysType

def pathValidator(pathToCheck: Path):
    if pathToCheck != None and not pathToCheck.exists():
        raise ValidationError(f'The file {pathToCheck} does not exist')        
    return pathToCheck

class RequToSliceAndEncript(BaseModel):
    selfCipher: Annotated[str, AfterValidator(checkCipher)]
    selfTextFile: Annotated[Path, PlainValidator(pathValidator)]
    selfNameTextFile: str
    selfLengthTelegram: Annotated[int, AfterValidator(checkLenghtTelegram)]
    selfNumberOfTelegram: Annotated[int, AfterValidator(checkNumbersOfTelegrams)]
    selfKeysType: Annotated[str, AfterValidator(checkKeysType)]
    selfFileWithUsersKeys: Annotated[Path, PlainValidator(pathValidator)]
    selfKeysProperties: dict

ciphers_obj = CppCiphers(pathToCiphersDir=str(Path(BASE_DIR, 'Ciphers')))
all_ciphers = ciphers_obj.get_ciphers_dict()


with open(str(Path(BASE_DIR, 'templates')) + '\\select.html', "r", encoding="utf-8") as file:
    html_content = file.read()

file.close()

# Создаем объект BeautifulSoup
settings_select_html = BeautifulSoup(html_content, 'html.parser')


select_tag = settings_select_html.find('select', {'id': "ciphersList"})


for option in select_tag.find_all('option'):
    option.decompose()


empty_option = settings_select_html.new_tag('option', value="Empty_tag")
empty_option.string = "Change cipher"
empty_option["disabled"] = True
empty_option["selected"] = True
select_tag.append(empty_option)


for i in all_ciphers:
    new_option = settings_select_html.new_tag('option', value=i)
    new_option.string = all_ciphers[i]
    select_tag.append(new_option)


with open(str(Path(BASE_DIR, 'templates')) + '\\select.html', "w", encoding="utf-8") as file:
    file.write(settings_select_html.prettify())

file.close()


templates = Jinja2Templates(directory=str(Path(BASE_DIR, 'templates')))


app.mount('/static', StaticFiles(directory=str(Path(BASE_DIR, 'static'))), name='static')

app.add_middleware(NoCacheMiddleware)


def startEncrypt(reqToSileAndEncript: RequToSliceAndEncript):
    telegrams: list[str] = cut_telegrams(reqToSileAndEncript.selfTextFile.__str__(), reqToSileAndEncript.selfLengthTelegram, reqToSileAndEncript.selfNumberOfTelegram)
    
    
    enc_resualt: dict = ciphers_obj.encript_telegrams(reqToSileAndEncript.selfCipher, telegrams, None, reqToSileAndEncript.selfKeysProperties)
    
    print(enc_resualt)
    print(SAVE_DIR)
    print(BASE_DIR)
    
    save_file: Path = Path(SAVE_DIR, "resualt.docx")
    
    save_to_docx(enc_resualt, save_file)
    
    return

requestToSliceAndEncript: RequToSliceAndEncript = RequToSliceAndEncript(
    selfCipher = 'None',
    selfFileWithUsersKeys = None,
    selfKeysType = 'users_keys',
    selfTextFile = None,
    selfLengthTelegram = 1,
    selfNameTextFile = "",
    selfNumberOfTelegram = 1,
    selfKeysProperties = {}
)

@app.post("/startEncoder/pushTelegramsCuttingData")
async def catchTelegramsCuttinngData(
    cipher: str = Form(...), 
    textFile: UploadFile = File(...), 
    length: int = Form(...), 
    number: int = Form(...), 
    keysType: str = Form(...)
):
    extension: str = re.search(".[A-Za-z]+$", textFile.filename).group()
    pathToOpenText: Path = Path(BASE_DIR, "fullOpenText.txt")
    if(extension == '.txt'):
        save_open_text_as_txt_file("en", textFile.file, pathToOpenText)
    elif(extension == '.docx'):
        save_open_text_docx_as_txt("en", textFile.file, pathToOpenText)
    
    global requestToSliceAndEncript
    requestToSliceAndEncript = RequToSliceAndEncript(
        selfCipher = cipher, 
        selfKeysProperties = {},
        selfKeysType = keysType, 
        selfTextFile = pathToOpenText, 
        selfLengthTelegram = length, 
        selfNumberOfTelegram = number,
        selfNameTextFile = textFile.filename, 
        selfFileWithUsersKeys = None
        )

    return JSONResponse({"Status": 200})

@app.post("/startEncoder/pushKeysProperties")
async def catchKeysProperties(keyPropReq: Request):
    keyPropDict = (await keyPropReq.json())
    global requestToSliceAndEncript
    requestToSliceAndEncript = requestToSliceAndEncript.model_copy(update={'selfKeysProperties': keyPropDict})
    startEncrypt(requestToSliceAndEncript)
    return JSONResponse({"Status": 200})

@app.post("/startEncoder/pushUserKeys")
async def catchUsersKeys(keys_file: UploadFile = File(...)):
    pathToUsersKeysFile: Path = Path(BASE_DIR, "/usersKeys.txt")
    extension: str = re.search(".[A-Za-z]+$", keys_file.filename).group()
    if(extension == '.txt'):
        save_as_txt_file(keys_file.file, pathToUsersKeysFile)
    elif(extension == '.docx'):
        save_docx_as_txt(keys_file.file, pathToUsersKeysFile)
    
    global requestToSliceAndEncript
    requestToSliceAndEncript = requestToSliceAndEncript.model_copy(update={'selfFileWithUsersKeys': pathToUsersKeysFile})
    return JSONResponse({"Status": 200})

@app.post('/selectCipher')
async def select_cipher(reqToKeyProperty: Request):
    return ciphers_obj.get_key_propertys(dict(await reqToKeyProperty.json())['cipher'])


@app.get('/', response_class=HTMLResponse)
async def select(request: Request):
    return templates.TemplateResponse(request=request, name='select.html')


def start_server():
    uvicorn.run('__main__:app', host='127.0.0.1', port=8000, reload=False)


def start_webview():
    time.sleep(1)
    webview.create_window('FastAPI Desktop App', 'http://127.0.0.1:8000')
    webview.start()


if __name__ == '__main__':
    server_thread = threading.Thread(target = start_server)
    server_thread.daemon = True
    server_thread.start()
    start_webview()
