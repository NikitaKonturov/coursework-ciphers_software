from pydantic import ValidationError
from fastapi.responses import JSONResponse

from ciphers_api_module.ciphers_api_module import InvalidKey, InvalidOpenText, KeyPropertyError
"""
These are the exception handlers that are used to handle all of the exceptions that
may occur during the execution of the application. 
!!! WIP !!!
"""


async def validatiion_exception(request, exc: ValidationError):
    # logger.error(msg=str(exc))
    return JSONResponse(status_code=401, content={"error": "Data validation error", "detail": exc.errors()})


async def value_exception(request, exc: ValueError):
    # logger.error(msg=str(exc))
    return JSONResponse(status_code=403, content={"error": "Value error", "detail": str(exc)})


async def type_exception(request, exc: TypeError):
    print(str(exc))
    return JSONResponse(status_code=406, content={"error": "Type error", "detail": str(exc)})


async def runtime_exception(request, exc: RuntimeError):
    # logger.error(msg=str(exc))
    print(str(exc))
    return JSONResponse(status_code=417, content={"error": "Runtime error", "detail": str(exc)})

async def path_exception(request, exc: FileExistsError):
    print(str(exc))
    return JSONResponse(status_code=418, content={"error": "Path error", "detail": str(exc)})

async def invalid_key_exception(request, exc: InvalidKey):
    # logger.error(msg=str(exc))
    return JSONResponse(status_code=402, content={"error": "Invalid key", "detail": str(exc)})


async def invalid_open_text_exception(request, exc: InvalidOpenText):
    # logger.error(msg=str(exc))
    return JSONResponse(status_code=403, content={"error": "Invalid open text", "detail": str(exc)})


async def key_property_error_exception(request, exc: KeyPropertyError):
    # logger.error(msg=str(exc))
    return JSONResponse(status_code=405, content={"error": "Key property error", "detail": str(exc)})


async def unknown_exception(request, exc: Exception):
    # logger.error(msg=str(exc))
    return JSONResponse(status_code=500, content={"error": "An unexpected error occurred", "detail": str(exc)})
