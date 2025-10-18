# ============================== Imports =============================#

import os
import re
import sys
import threading
from pathlib import Path
import time
import zipfile

# ШГП, ШПЗ проверки ключей


from ciphers_api_module.ciphers_api_module import (CppCiphers,
                                                   form_cipher_select_options,
                                                   start_decryption,
                                                   start_encryption)


from ciphers_api_module.requestsClass.requestToEncript import \
    RequToSliceAndEncript


from exception_handlers import (InvalidKey, InvalidOpenText, KeyPropertyError,
                                ValidationError, invalid_key_exception,
                                invalid_open_text_exception,
                                key_property_error_exception,
                                runtime_exception, type_exception,
                                unknown_exception, validatiion_exception,
                                value_exception, path_exception)

from settings.config import (NoCacheMiddleware, Settings, match, search_directory,
                             start_server, start_webview, update_js_file, save_docx_as_txt,
                             save_open_text_docx_as_bin_file, save_as_txt_file,
                             save_open_text_as_bin_file)

                            
from fastapi import FastAPI, File, Form, UploadFile
from fastapi.requests import Request
from fastapi.responses import HTMLResponse, JSONResponse, FileResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates

sys.path.append(os.path.abspath("Ciphers"))
# ======================== FastAPI and Config Initialization =========================#


app = FastAPI()
settings = Settings()


# ======================== Exception handlers Initialization =========================#


app.add_exception_handler(InvalidKey, invalid_key_exception)
app.add_exception_handler(InvalidOpenText, invalid_open_text_exception)
app.add_exception_handler(KeyPropertyError, key_property_error_exception)
app.add_exception_handler(ValueError, value_exception)
app.add_exception_handler(ValidationError, validatiion_exception)
app.add_exception_handler(TypeError, type_exception)
app.add_exception_handler(RuntimeError, runtime_exception)
app.add_exception_handler(Exception, unknown_exception)
app.add_exception_handler(FileExistsError, path_exception)


# ========================== Middleware Intialization =========================



BASE_DIR = settings.base_dir_path

ciphers_obj = CppCiphers(pathToCiphersDir=str(settings.path_to_ciphers))

form_cipher_select_options(ciphers_obj, settings.path_to_templates)

requestToSliceAndEncript: RequToSliceAndEncript = RequToSliceAndEncript(
    selfCipher='None',
    selfFileWithUsersKeys=None,
    selfKeysType='users_keys',
    selfTextFile=None,
    selfLengthTelegram=1,
    selfNameTextFile="",
    selfNumberOfTelegram=1,
    selfKeysProperties={}
)

class NoCacheStaticFiles(StaticFiles):
    async def get_response(self, path: str, scope):
        response = await super().get_response(path, scope)
        
        # Добавляем заголовки против кэширования
        response.headers["Cache-Control"] = "no-store, no-cache, must-revalidate, max-age=0"
        response.headers["Pragma"] = "no-cache"
        response.headers["Expires"] = "0"
        response.headers["ETag"] = f'"{int(time.time())}"'
        
        return response


app.mount('/static', NoCacheStaticFiles(directory=str(settings.path_to_static)), name='static')

app.add_middleware(NoCacheMiddleware)


templates = Jinja2Templates(
    directory=str(settings.path_to_templates),
    auto_reload=True,  # Автоматически перезагружать шаблоны
    cache_size=0       # Не кэшировать шаблоны
)

# ================================= EndPoints ===================================


@app.get("/favicon.ico")
async def favicon():
    return FileResponse("logo.ico")

@app.post("/startEncoder/pushTelegramsCuttingData")
async def catchTelegramsCuttinngData(
    cipher: str = Form(...),
    textFile: UploadFile = File(...),
    length: int = Form(...),
    number: int = Form(...),
    keysType: str = Form(...)
):
    extension: str = re.search(".[A-Za-z]+$", textFile.filename).group()
    pathToOpenText: Path = Path(
        BASE_DIR, settings.filename_to_save_full_open_text + ".bin")
    if (extension == '.txt'):
        save_open_text_as_bin_file(
            settings.ciphers_language, textFile.file, pathToOpenText)
    elif (extension == '.docx'):
        save_open_text_docx_as_bin_file(
            settings.ciphers_language, textFile.file, pathToOpenText)
    else:
        raise RuntimeError("Расширение файла должно быть либо .txt либо .docx")

    global requestToSliceAndEncript
    requestToSliceAndEncript = RequToSliceAndEncript(
        selfCipher=cipher,
        selfKeysProperties={},
        selfKeysType=keysType,
        selfTextFile=pathToOpenText,
        selfLengthTelegram=length,
        selfNumberOfTelegram=number,
        selfNameTextFile=textFile.filename,
        selfFileWithUsersKeys=None
    )

    return JSONResponse({"Status": 200})


@app.post("/startEncoder/pushKeysProperties")
async def catchKeysProperties(keyPropReq: Request):
    keyPropDict = (await keyPropReq.json())
    keyPropDict["text_language"] = settings.ciphers_language
    keyPropDict["viginer_path_to_dir"] = settings.path_to_dir_viginer_dict.__str__()
    global requestToSliceAndEncript
    requestToSliceAndEncript = requestToSliceAndEncript.model_copy(
        update={'selfKeysProperties': keyPropDict})
    start_encryption(requestToSliceAndEncript, Path(settings.encript_results_path,
                     'encription-result-' + requestToSliceAndEncript.selfCipher + '.docx'), ciphers_obj, settings.fiveGramsEnabled)

    os.remove(requestToSliceAndEncript.selfTextFile)
    
    #   Архивируем результаты перед отправкой на загрузку
    with zipfile.ZipFile('encription_results.zip', 'w') as zip_file:
        zip_file.write(Path(settings.encript_results_path,'encription-result-' + requestToSliceAndEncript.selfCipher + '.docx'), 
                       arcname='encription-result-' + requestToSliceAndEncript.selfCipher + '.docx')
        zip_file.write(Path(settings.encript_results_path,'encription-result-' + requestToSliceAndEncript.selfCipher + 'PlainText.docx'),
                       arcname='encription-result-' + requestToSliceAndEncript.selfCipher + 'PlainText.docx')
    #
     
    return FileResponse(
        path=Path('encription_results.zip'), 
        status_code=200, 
        media_type='application/zip',
        filename='encription-result.zip')
    

@app.post("/startEncoder/pushUserKeys")
async def catchUsersKeys(keys_file: UploadFile = File(...)):
    extension: str = re.search(".[A-Za-z]+$", keys_file.filename).group()
    pathToUsersKeys: Path = Path(BASE_DIR, "usersKeys.txt")
    if (extension == '.txt'):
        save_as_txt_file(keys_file.file, pathToUsersKeys)
    elif (extension == '.docx'):
        save_docx_as_txt(keys_file.file, pathToUsersKeys)
    else:
        raise RuntimeError("Расширение файла должно быть либо .txt либо .docx")

    global requestToSliceAndEncript
    requestToSliceAndEncript = requestToSliceAndEncript.model_copy(
        update={'selfFileWithUsersKeys': pathToUsersKeys})

    start_encryption(requestToSliceAndEncript, Path(settings.encript_results_path,'encription-result-' + requestToSliceAndEncript.selfCipher + '.docx'), ciphers_obj, settings.fiveGramsEnabled)

    os.remove(requestToSliceAndEncript.selfTextFile)
    os.remove(pathToUsersKeys)

    #   Архивируем результаты перед отправкой на загрузку
    with zipfile.ZipFile('encription_results.zip', 'w') as zip_file:
        zip_file.write(Path(settings.encript_results_path,'encription-result-' + requestToSliceAndEncript.selfCipher + '.docx'), 
                       arcname='encription-result-' + requestToSliceAndEncript.selfCipher + '.docx')
        zip_file.write(Path(settings.encript_results_path,'encription-result-' + requestToSliceAndEncript.selfCipher + 'PlainText.docx'),
                       arcname='encription-result-' + requestToSliceAndEncript.selfCipher + 'PlainText.docx')
    #

    return FileResponse(
        path=Path('encription_results.zip'), 
        status_code=200, 
        media_type='application/zip',
        filename='encription-result.zip')


@app.post('/startDecoder')
async def catchDecriptRequest(
    cipher: str = Form(...),
    textFile: UploadFile = File(...)
):
    extension: str = re.search(".[A-Za-z]+$", textFile.filename).group()
    start_decryption(textFile.file, extension, cipher, ciphers_obj, Path(
        settings.decript_results_path, 'decription-result-' + cipher + '.docx'))

    return FileResponse(
        path=Path(settings.decript_results_path, 'decription-result-' + cipher + '.docx'), 
        status_code=200, 
        media_type='application/vnd.openxmlformats-officedocument.wordprocessingml.document',
        filename='decription-result-' + cipher + '.docx')


@app.post('/selectCipher')
async def select_cipher(reqToKeyProperty: Request):
    return ciphers_obj.get_key_properties(dict(await reqToKeyProperty.json())["cipher"])


@app.post('/settings', response_class=HTMLResponse)
async def save_settings(reqToSetting: Request):
    settingJson: dict = dict(await reqToSetting.json())
    print(settingJson)
    settings.update_settings("interface_language",
                             settingJson['interfaceLanguage'])
    settings.update_settings("ciphers_language", settingJson['cipherLanguage'])
    settings.update_settings("fiveGramsEnabled", str(settingJson['fiveGramsEnabled']).lower())
    print("start update")
    update_js_file(Path(BASE_DIR, "static", "settingWindow.js"), settingJson)

    return templates.TemplateResponse(request=reqToSetting, name='select.html')


@app.get('/', response_class=HTMLResponse)
async def select(request: Request):
    return templates.TemplateResponse(request=request, name='select.html')


# ========================== Server Initialization =========================


if __name__ == "__main__":
    server_thread = threading.Thread(target=start_server, args=[settings, BASE_DIR])
    server_thread.daemon = True
    server_thread.start()
    start_webview(settings)
 