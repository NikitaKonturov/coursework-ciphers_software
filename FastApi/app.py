import sys
import re
import threading
import docx
import time
from pathlib import Path
import logging
from typing import Annotated, BinaryIO


from ciphers_api_module.ciphers_api_module import CppCiphers, formCipherSelectOptions, startEncrypt
from ciphers_api_module.requestsClass.requestToEncript import RequToSliceAndEncript
from exception_handlers import (ValidationError, unknown_exception, validatiion_exception, value_exception)
from settings.config import NoCacheMiddleware, start_server, start_webview
from file_converters.saveTxtFile import save_open_text_as_txt_file, save_as_txt_file
from file_converters.docxToTxt import save_open_text_docx_as_txt, save_docx_as_txt
from file_converters.saveToDocx import save_to_docx


from fastapi import FastAPI, File, Form, UploadFile
from fastapi.requests import Request
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates

logging.basicConfig(
    format='\033[32m%(levelname)s\033[0m:\t%(message)s', level=logging.DEBUG)

app = FastAPI()

app.add_exception_handler(ValueError, value_exception)
app.add_exception_handler(ValidationError, validatiion_exception)
# Future exception_handlers
app.add_exception_handler(Exception, unknown_exception)

app.add_middleware(NoCacheMiddleware)

BASE_DIR = Path(__file__).resolve().parent
SAVE_DIR = Path(BASE_DIR,"encript_resualts")

ciphers_obj = CppCiphers(pathToCiphersDir=str(Path(BASE_DIR, 'Ciphers')))

formCipherSelectOptions(ciphers_obj, BASE_DIR)

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
templates = Jinja2Templates(directory=str(Path(BASE_DIR, 'templates')))

app.mount('/static', StaticFiles(directory=str(Path(BASE_DIR, 'static'))), name='static')


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
    startEncrypt(requestToSliceAndEncript, Path(SAVE_DIR, 'resualt.docx'), ciphers_obj)

    return JSONResponse({"Status": 200})

@app.post("/startEncoder/pushUserKeys")
async def catchUsersKeys(keys_file: UploadFile = File(...)):
    extension: str = re.search(".[A-Za-z]+$", keys_file.filename).group()
    pathToUsersKeys: Path = Path(BASE_DIR, "usersKeys.txt")
    if(extension == '.txt'):
        save_as_txt_file(keys_file.file, pathToUsersKeys)
    elif(extension == '.docx'):
        save_docx_as_txt(keys_file.file, pathToUsersKeys)
    
    global requestToSliceAndEncript
    requestToSliceAndEncript = requestToSliceAndEncript.model_copy(update={'selfFileWithUsersKeys': pathToUsersKeys})
    
    startEncrypt(requestToSliceAndEncript, Path(SAVE_DIR, 'resualt.docx'), ciphers_obj)
    
    return JSONResponse({"Status": 200})

@app.post('/selectCipher')
async def select_cipher(reqToKeyProperty: Request):
    return ciphers_obj.get_key_propertys(dict(await reqToKeyProperty.json())["cipher"])


@app.get('/', response_class=HTMLResponse)
async def select(request: Request):
    return templates.TemplateResponse(request=request, name='select.html')


if __name__ == "__main__":
    server_thread = threading.Thread(target=start_server)
    server_thread.daemon = True
    server_thread.start()
    start_webview()
