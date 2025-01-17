from pydantic_settings import BaseSettings, SettingsConfigDict
from pydantic import AfterValidator, ValidationError
from starlette.middleware.base import BaseHTTPMiddleware
from starlette.responses import Response
from dotenv import load_dotenv, find_dotenv, set_key, dotenv_values
from typing import Annotated
from pathlib import Path
import uvicorn
import re
import webview
import time


class NoCacheMiddleware(BaseHTTPMiddleware):
    async def dispatch(self, request, call_next):
        response: Response = await call_next(request)
        response.headers["Cache-Control"] = "no-store"
        return response

def check_path(path: Path) -> Path | None:
    if(not path.exists()):
        raise ValidationError(f"Path {str(path)} was not found...")
    return path

def check_host(host: str) -> str | None:
    if (not re.match('[0-9]+.[0-9]+.[0-9]+.[0-9]', host)):
       raise ValidationError(f"Invalid format for host: {host}") 
    return host
    

class Settings(BaseSettings):
    model_config = SettingsConfigDict(env_file='config.env', env_file_encoding='utf-8')

    app_name: str
    base_dir_path: Path = Annotated[Path, AfterValidator(check_path)]
    path_to_ciphers: Path = Annotated[Path, AfterValidator(check_path)]
    path_to_templates: Path = Annotated[Path, AfterValidator(check_path)]
    path_to_static: Path = Annotated[Path, AfterValidator(check_path)]
    filename_to_save_full_open_text: str
    encript_results_path: Path = Annotated[Path, AfterValidator(check_path)]
    decript_results_path: Path = Annotated[Path, AfterValidator(check_path)]
    host: str = Annotated[str, AfterValidator(check_host)]
    port: int
    location: str    
    
def update_settings(self, field: str, value: str):
    pathToEnv = find_dotenv('config.env')
    if(not hasattr(self, field)):
        raise ValueError(f'{field} was not found...')
    setattr(self, field, value)
    set_key(pathToEnv, field, value)
    
    
def load_settings(configFilename: str):
    pathToEnv = find_dotenv(configFilename)
    if(pathToEnv == ''):
        raise ValueError(f"{configFilename} was not found...")
    load_dotenv(pathToEnv)
    
    

def start_server(settings: Settings):
    uvicorn.run("__main__:app", host=settings.host, port=settings.port, reload=False)


def start_webview(settings: Settings):
    time.sleep(1)
    webview.create_window(settings.app_name, settings.location)
    webview.start()
