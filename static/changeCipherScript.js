import { showError } from './errorHandler.js'

function showErrorAndLog(error) {
    const message = "Ошибка: " + error.message;
    showError(message); 
    console.error("Response error: ", error);  
}


export async function addBlockOfKeysSettings() {
    try {
        let serverResponse = await fetch("http://127.0.0.1:8000/selectCipher", 
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({cipher: document.getElementById('ciphersList').value})
        });

        if(!serverResponse.ok) {
            const errorData = await serverResponse.json();
            const errorMessage = errorData.error || "Неизвестная ошибка на сервере";
            const errorDetail = errorData.detail || "Нет дополнительных данных";
            
            throw new Error(`${errorMessage}: ${errorDetail}`);
        }

        const settingsOfKeys = await serverResponse.json()

        const keysSettingBlock = document.createElement("form");
        keysSettingBlock.id = "keys-settings-block"
        keysSettingBlock.className = "keys-settings-block-class"
        keysSettingBlock.enctype = "multipart/form-data"
        
        let blockName = document.createElement("label");
        blockName.textContent = (document.getElementById("ciphersList").value + ' параметры ключа:')
        keysSettingBlock.appendChild(blockName)

        settingsOfKeys.params.forEach(param => {
            let divParametr = document.createElement("div")
            divParametr.id = param.name
            let nameLabel = document.createElement("label")
            nameLabel.id = param.name + "Label"
            nameLabel.htmlFor = param.name
            nameLabel.textContent = param.label + ':'
            let inputLabel = document.createElement("input")
            inputLabel.type = param.type
            inputLabel.name = param.name
            inputLabel.placeholder = param.default
            if (param.type == 'number') {
                inputLabel.min = param.min
                inputLabel.max = param.max
            }
            
            divParametr.appendChild(nameLabel)
            divParametr.appendChild(inputLabel)
            keysSettingBlock.appendChild(divParametr)
        });
        let blockConfirmKey = document.createElement("div")
        blockConfirmKey.id = "keys-choose-block"
        blockConfirmKey.className = "keys-choose-block-class"
        let buttonConfirm = document.createElement("button")
        buttonConfirm.textContent = "Подтвердить"
        buttonConfirm.addEventListener("click", function(){event.preventDefault(); sendEncriptRequest("keys-settings-block", "keys_settings")}, true);
        blockConfirmKey.appendChild(buttonConfirm)    
        keysSettingBlock.appendChild(blockConfirmKey)

        Array.from(document.getElementsByClassName("keys-settings-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("keys-choose-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("decript-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("settingWindow-class")).forEach(elem => { elem.remove(); });
            
        document.getElementById("main-keys-block").appendChild(keysSettingBlock)
        
    } catch (error) {  
        showErrorAndLog(error);
    }
}

export function checkNumber(elementValue) {
    if (isNaN(Number(elementValue))) {
        return elementValue;  
    } else {
        return Number(elementValue); 
    }
}


export async function sendEncriptRequest(formID, keysType) {
    let dataToSliceTelegams = new FormData(document.getElementById("slice-telegrmas-form"))

    dataToSliceTelegams.append("keysType", keysType)
    dataToSliceTelegams.forEach((fieldValue, key) => {
        console.log(key, fieldValue)
    })

    let telegramCuttingResponse = await fetch("http://127.0.0.1:8000/startEncoder/pushTelegramsCuttingData",
        {
            method: "POST",
            body: dataToSliceTelegams
        }
    )

    if (!telegramCuttingResponse.ok) {
        try {
            const errorData = await telegramCuttingResponse.json();
            const errorMessage = errorData.error || "Неизвестная ошибка на сервере";
            const errorDetail = errorData.detail || "Нет дополнительных данных";
            console.error(`Error: ${errorMessage} - ${errorDetail}`);
            showError(`Ошибка: ${errorMessage} - ${errorDetail}`);
        } catch (e) {
            console.error(`Error: ${telegramCuttingResponse.statusText}`);
            showError(`Ошибка: ${telegramCuttingResponse.statusText}`);
        }
        return;
    } 

    if(keysType == 'keys_settings') {
        let dataFromKeySettingForm = Array.from(document.querySelectorAll(('#' + formID + ' input'))).reduce((anyFields, thisField) => ({...anyFields, [thisField.name]: checkNumber(thisField.value)}), {})
        console.log(dataFromKeySettingForm)

        let keyPropertiesResponse = await fetch("http://127.0.0.1:8000/startEncoder/pushKeysProperties", 
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify(dataFromKeySettingForm)
            });

        if (!keyPropertiesResponse.ok) {
            try {
                const errorData = await keyPropertiesResponse.json();
                const errorMessage = errorData.error || "Неизвестная ошибка на сервере";
                const errorDetail = errorData.detail || "Нет дополнительных данных";
                console.error(`Error: ${errorMessage} - ${errorDetail}`);
                showError(`Ошибка: ${errorMessage} - ${errorDetail}`);
            } catch (e) {
                console.error(`Error: ${keyPropertiesResponse.statusText}`);
                showError(`Ошибка: ${keyPropertiesResponse.statusText}`);
            }
            return;
        }
        else{
            showToast("Зашифрование прошло успешно!","success");
        }
    } else if (keysType == 'users_keys') {
        let dataFromUserKeysForm = new FormData(document.getElementById(formID))
        Array.from(dataFromUserKeysForm).forEach(element => {console.log(element)})
        let userKeysResponse = await fetch("http://127.0.0.1:8000/startEncoder/pushUserKeys",
            {
                method: "POST",
                body: dataFromUserKeysForm
            })

        if(!userKeysResponse.ok) {
            console.error(userKeysResponse.statusText)
            return
        }
        else{
            showToast("Зашифрование прошло успешно","success");
        }
    } 
}

export async function sendDecriptRequest() 
{
    let dataAboutCipherTextAndKeys = new FormData(document.getElementById("slice-telegrmas-form"))
    
    dataAboutCipherTextAndKeys.delete("length")
    dataAboutCipherTextAndKeys.delete("number")

    if(document.getElementById("ciphersList").value == "Empty_tag") {
        alert("Выберите шифр")
        return
    } 

    const fileInput = document.getElementById("text-file");

    if (!fileInput || !fileInput.files || fileInput.files.length === 0) {
        alert("Выберите файл с шифр текстом");
        return;
    }

    let responseFromDecript = await fetch("http://127.0.0.1:8000/startDecoder", 
        {
            method: "POST",
            body: dataAboutCipherTextAndKeys
        }
    )

    if(!responseFromDecript.ok) {
        console.error(responseFromDecript.statusText)
        return
    }
    return
}

export async function addBlockOfGetUsersKeys() {
    try {
        let blockWithChooseKey = document.createElement("div")
        blockWithChooseKey.className = "keys-choose-block-class"
        
        let formChooseKeysFile = document.createElement("form")
        formChooseKeysFile.enctype="multipart/form-data"
        formChooseKeysFile.className = "keys-choose-block"
        formChooseKeysFile.id = "keys-choose-block"

        let labelChooseElement = document.createElement("label")
        labelChooseElement.id = "input-file"

        let spanUsersKeys = document.createElement("span")
        spanUsersKeys.id = "custom-file-label-keys"
        spanUsersKeys.textContent = "Выберите файл с ключами"

        let inputUsersKeys = document.createElement("input")
        inputUsersKeys.id = "keys-file"
        inputUsersKeys.addEventListener('change', function(){document.getElementById('choose-keys-label').textContent = this.files[0].name}, true)
        inputUsersKeys.accept = ".doc,.docx,application/msword, text/plain"
        inputUsersKeys.name = "keys_file"
        inputUsersKeys.addEventListener('change', function() {
            document.getElementById('custom-file-label-keys').textContent = this.files[0].name;
        })
        inputUsersKeys.required = ""
        inputUsersKeys.style = "display: none;"
        inputUsersKeys.type = "file"
        
        let blockConfirmKey = document.createElement("div")
        blockConfirmKey.className = "keys-choose-block-class"

        let buttonConfirm = document.createElement("button")
        buttonConfirm.textContent = "Подтвердить"
        buttonConfirm.addEventListener("click", function(){event.preventDefault(); sendEncriptRequest("keys-choose-block", "users_keys")} ,true)

        labelChooseElement.appendChild(spanUsersKeys)
        labelChooseElement.appendChild(inputUsersKeys)
        formChooseKeysFile.appendChild(labelChooseElement)
        blockConfirmKey.appendChild(buttonConfirm)
        formChooseKeysFile.appendChild(blockConfirmKey)
        blockWithChooseKey.appendChild(formChooseKeysFile)

        Array.from(document.getElementsByClassName("keys-settings-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("keys-choose-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("decript-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("settingWindow-class")).forEach(elem => { elem.remove(); });

        document.getElementById("main-keys-block").appendChild(blockWithChooseKey)
    } catch(error) {
        showErrorAndLog(error);
    }
}

export async function preventActionButton() {
    event.preventDefault()
}

export async function encriptSettings() {
    try {
        if(document.getElementById("ciphersList").value == "Empty_tag") {
            alert("Выберите шифр!")
        } else if(document.getElementById("count-of-tg").value <= 0 || document.getElementById("lenght-of-tg").value <= 0) {
                alert("Колличество телеграм и их размер должны быть натуральными")
        } else if(Array.from(document.getElementById("text-file").files).length == 0) {
                alert("Выберите файл с текстом")
        }else {
            let selectKyesTypeBlock = document.createElement("div")
            selectKyesTypeBlock.id = "button-container"
            selectKyesTypeBlock.className = "button-container"
            let autoGenKeysButton = document.createElement("button")
            autoGenKeysButton.textContent = "Автоматическая генерация ключей"
            autoGenKeysButton.addEventListener('click', addBlockOfKeysSettings, true)
            selectKyesTypeBlock.appendChild(autoGenKeysButton)

            let getUserKeysButton = document.createElement("button")
            getUserKeysButton.textContent = "Выберите ключ"
            getUserKeysButton.addEventListener('click', addBlockOfGetUsersKeys, true)
            selectKyesTypeBlock.appendChild(getUserKeysButton)

            let rigthBlock = document.createElement("div")
            rigthBlock.id = "main-keys-block"
            rigthBlock.className = "main-keys-block-class"
            rigthBlock.appendChild(selectKyesTypeBlock)
            
            Array.from(document.getElementsByClassName("main-keys-block-class")).forEach(elem => {elem.remove();})
            Array.from(document.getElementsByClassName("decript-block-class")).forEach(elem => { elem.remove(); });
            Array.from(document.getElementsByClassName("settingWindow-class")).forEach(elem => { elem.remove(); });

            document.getElementById("ciphersList").addEventListener("change", () => {
                document.querySelectorAll(".keys-settings-block-class, .keys-choose-block-class, .main-keys-block-class")
                    .forEach(elem => elem.remove());
            });

            document.body.appendChild(rigthBlock)

        }
    }
    catch(err){
        showErrorAndLog(err);
    }
}



window.encriptSettings = encriptSettings
window.preventActionButton = preventActionButton
