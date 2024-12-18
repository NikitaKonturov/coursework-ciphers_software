import sys
import re
from pathlib import Path
from typing import Annotated, BinaryIO

from pydantic import BaseModel, ConfigDict, create_model, AfterValidator, ValidationError, PlainValidator

def checkCipher(sourceCipher: str):
    if((sourceCipher != "None") and (sourceCipher not in sys.modules.keys())):
        raise ValidationError(f"The cipher ${sourceCipher} was not found")     
    return sourceCipher

def checkLenghtTelegram(tgLength: int):
    if(tgLength < 0 or tgLength == 0):
        raise ValidationError(f"The length of the telegram must be natural. Current length ${tgLength}")
    return tgLength

def checkNumbersOfTelegrams(tgNumbers: int):
    if(tgNumbers < 0 or tgNumbers == 0):
        raise ValidationError(f"The size of the telegram must be natural. Current length ${tgNumbers}")
    return tgNumbers

def checkKeysType(sourceKeysType: str):
    if(sourceKeysType != "keys_settings" and sourceKeysType != "users_keys"):
        raise ValidationError(f"Keys type must be users_keys or keys_settings")
    return sourceKeysType

def pathValidator(pathToCheck: Path):
    if pathToCheck != None and not pathToCheck.exists():
        raise ValidationError(f'The file {pathToCheck} does not exist')        
    return pathToCheck

class RequToSliceAndEncript(BaseModel):
    selfCipher: Annotated[str, AfterValidator(checkCipher)]
    selfTextFile: Annotated[Path, PlainValidator(pathValidator)]
    selfNameTextFile: str
    selfLengthTelegram: Annotated[int, AfterValidator(checkLenghtTelegram)]
    selfNumberOfTelegram: Annotated[int, AfterValidator(checkNumbersOfTelegrams)]
    selfKeysType: Annotated[str, AfterValidator(checkKeysType)]
    selfFileWithUsersKeys: Annotated[Path, PlainValidator(pathValidator)]
    selfKeysProperties: dict
