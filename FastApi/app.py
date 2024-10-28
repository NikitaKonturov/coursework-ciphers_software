from fastapi import FastAPI
from fastapi.requests import Request
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from pathlib import Path
import multiprocessing
import uvicorn
import webview
import time
import threading

app = FastAPI()

BASE_DIR = Path(__file__).resolve().parent

app.mount('/static', StaticFiles(directory='static'), name='static')

templates = Jinja2Templates(directory=str(Path(BASE_DIR, 'templates')))

#@app.post('/cipher')
#async def select_cipher()

@app.get('/', response_class=HTMLResponse)
async def main(request: Request):
    return templates.TemplateResponse(request=request, name='main.html')

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