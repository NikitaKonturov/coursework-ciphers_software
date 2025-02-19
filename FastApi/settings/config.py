from pydantic_settings import BaseSettings, SettingsConfigDict
from pydantic import Field, ValidationError
from starlette.middleware.base import BaseHTTPMiddleware
from starlette.responses import Response
from dotenv import load_dotenv, find_dotenv, set_key, dotenv_values
from pathlib import Path
import uvicorn
import re
import webview
import time
import os


class NoCacheMiddleware(BaseHTTPMiddleware):
    async def dispatch(self, request, call_next):
        response: Response = await call_next(request)
        response.headers["Cache-Control"] = "no-store"
        return response


def check_path(path: Path) -> Path | None:
    if not path.exists():
        raise ValidationError(f"Path {str(path)} was not found...")
    return path


def check_host(host: str) -> str | None:
    if not re.match(r'[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+', host):
        raise ValidationError(f"Invalid format for host: {host}")
    return host


# Путь к файлу .env
dotenv_path = find_dotenv("config.env")

# Загружаем переменные окружения
env_values = dotenv_values(dotenv_path)


class Settings(BaseSettings):
    app_name: str = Field(..., env="APP_NAME")
    base_dir_path: Path = Field(..., env="BASE_DIR_PATH")
    path_to_ciphers: Path = Field(..., env="PATH_TO_CIPHERS")
    path_to_templates: Path = Field(..., env="PATH_TO_TEMPLATES")
    path_to_static: Path = Field(..., env="PATH_TO_STATIC")
    filename_to_save_full_open_text: str = Field(
        ..., env="FILENAME_TO_SAVE_FULL_OPEN_TEXT")
    encript_results_path: Path = Field(..., env="ENCRIPT_RESULTS_PATH")
    decript_results_path: Path = Field(..., env="DECRIPT_RESULTS_PATH")
    interface_language: str = Field(..., env="INTERFACE_LANGUAGE")
    ciphers_language: str = Field(..., env="CIPHERS_LANGUAGE")
    host: str = Field(..., env="HOST")
    port: int = Field(..., env="PORT")
    location: str = Field(..., env="LOCATION")

    class Config:
        env_file = dotenv_path  # Указываем файл для поиска переменных окружения
        env_file_encoding = "utf-8"

    def update_settings(self, field: str, value: str) -> None:
        path_to_env = find_dotenv(dotenv_path)
        if not hasattr(self, field):
            raise ValueError(f"{field} was not found...")
        setattr(self, field, value)
        set_key(path_to_env, field.upper(), value)


def load_settings(config_filename: str) -> None:
    path_to_env = find_dotenv(config_filename)
    if not path_to_env:
        raise ValueError(f"{config_filename} was not found...")
    load_dotenv(path_to_env)


def update_js_file(pathToJsFile: Path, parametrs: dict[str, str]) -> None:
    with open(pathToJsFile, "r", encoding="utf-8") as file:
        code = file.read()
    print(parametrs)

    for key, value in parametrs.items():
        value = re.escape(value)
        pattern = rf"({key}\s*=\s*['\"])[^'\"]*(['\"];)"
        code = re.sub(pattern, rf'\1{value}\2', code)

    with open(pathToJsFile, "w", encoding="utf-8") as file:
        file.write(code)


def search_directory(basePath: Path, dirname: str) -> None | Path:

    for root, dirs, files in os.walk("C:\\"):
        if (dirname in dirs):
            return os.path.join(root, dirname)
    return None


def start_server(settings: Settings) -> None:
    uvicorn.run("__main__:app", host=settings.host,
                port=settings.port, reload=False)


def start_webview(settings: Settings) -> None:
    time.sleep(1)
    webview.create_window(settings.app_name, settings.location)
    webview.start()
