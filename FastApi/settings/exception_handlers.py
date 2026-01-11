from pydantic import ValidationError

from fastapi.responses import JSONResponse

from .cpp_exceptions import InvalidKey, InvalidOpenText, KeyPropertyError

"""
These are the exception handlers that are used to handle all of the exceptions that
may occur during the execution of the application. 
!!! WIP !!!
KeyPropertyError
InvalidKey
InvalidOpenText
"""


async def validatiion_exception(request, exc: ValidationError):
    return JSONResponse(status_code=401, content={"error": "Data validation error", "detail": exc.errors()})


async def value_exception(request, exc: ValueError):
    return JSONResponse(status_code=400, content={"error": "Value error", "detail": str(exc)})


async def key_property_exception(request, exc: KeyPropertyError):
    return JSONResponse(status_code=402, content={"error": "Key property error", "detail": str(exc)})


async def invalid_key_exception(request, exc: InvalidKey):
    return JSONResponse(status_code=403, content={"error": "Invalid key", "detail": str(exc)})


async def invalid_open_text_exception(request, exc: InvalidOpenText):
    return JSONResponse(status_code=405, content={"error": "Invalid open text", "detail": str(exc)})


async def unknown_exception(request, exc: Exception):
    return JSONResponse(status_code=500, content={"error": "An unexpected error occurred", "detail": str(exc)})
