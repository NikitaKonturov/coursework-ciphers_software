from starlette.middleware.base import BaseHTTPMiddleware
from starlette.responses import Response
import uvicorn
import webview
import time


class NoCacheMiddleware(BaseHTTPMiddleware):
    async def dispatch(self, request, call_next):
        response: Response = await call_next(request)
        response.headers["Cache-Control"] = "no-store"
        return response


def start_server():
    uvicorn.run("__main__:app", host="127.0.0.1", port=8000, reload=False)


def start_webview():
    time.sleep(1)
    webview.create_window("Encoder", "http://127.0.0.1:8000")
    webview.start()
