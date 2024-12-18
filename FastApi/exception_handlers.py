from pydantic import ValidationError

from fastapi.responses import JSONResponse

"""
These are the exception handlers that are used to handle all of the exceptions that
may occur during the execution of the application. 
!!! WIP !!!
"""


async def validatiion_exception(request, exc: ValidationError):
    return JSONResponse(status_code=401, content={"error": "Data validation error", "detail": exc.errors()})


async def value_exception(request, exc: ValueError):
    return JSONResponse(status_code=400, content={"error": "Value error", "detail": str(exc)})

# Все наши ошибки WIP


async def unknown_exception(request, exc: Exception):
    return JSONResponse(status_code=500, content={"error": "An unexpected error occurred", "detail": str(exc)})
