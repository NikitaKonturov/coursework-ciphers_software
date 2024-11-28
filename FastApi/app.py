import logging
import threading
from pathlib import Path

from ciphers_api_module.ciphers_api_module import (CppCiphers,
                                                   RequestToSliceAndEncript, formCipherSelectOptions)
from exception_handlers import (ValidationError, unknown_exception,
                                validatiion_exception, value_exception)
from settings.config import NoCacheMiddleware, start_server, start_webview

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

ciphers_obj = CppCiphers(pathToCiphersDir=str(Path(BASE_DIR, 'Ciphers')))

formCipherSelectOptions(ciphers_obj, BASE_DIR)

requestToSliceAndEncript: RequestToSliceAndEncript = RequestToSliceAndEncript()

templates = Jinja2Templates(directory=str(Path(BASE_DIR, 'templates')))

app.mount(
    '/static', StaticFiles(directory=str(Path(BASE_DIR, 'static'))), name='static')


@app.post("/startEncoder/pushTelegramsCuttingData")
async def catchTelegramsCuttinngData(
    cipher: str = Form(...),
    textFile: UploadFile = File(...),
    length: int = Form(...),
    number: int = Form(...),
    keysType: str = Form(...)
):
    requestToSliceAndEncript.configurationDataAboutSlice(
        cipher, textFile, length, number, keysType)

    return JSONResponse({"Status": 200})


@app.post("/startEncoder/pushKeysProperties")
async def catchKeysProperties(keyPropReq: Request):
    print((await keyPropReq.json()))
    return JSONResponse({"Status": 200})


@app.post('/startEncoder')
async def startEncoder(requestConfig):
    return JSONResponse(
        {
            "cipher": "df"
        }
    )


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
