import { showError } from './errorHandler.js'

function showErrorAndLog(error) {
    const message = "Ошибка: " + error.message;
    showError(message); 
    console.error("Response error: ", error);  
}


export async function addBlockOfKeysSettings() {
    try {
        showLoadingIndicator();
        let serverResponse = await fetch("http://127.0.0.1:8000/selectCipher", 
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({cipher: document.getElementById('ciphersList').value})
        });
        hideLoadingIndicator();
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
    if(document.getElementById("ciphersList").value == "Empty_tag") {
            alert("Выберите шифр!")
            showError("Выберите шифр!")
            return 
        } else if(!isValidNaturalNumber(document.getElementById("count-of-tg").value)  || !isValidNaturalNumber(document.getElementById("lenght-of-tg").value)) {
                alert("Колличество телеграм и их размер должны быть натуральными и не содержать 'e'!")
                showError("Колличество телеграм и их размер должны быть натуральными и не содержать 'e'!");
                return
        } 

    if(Array.from(document.getElementById("text-file").files).length == 0) {
        alert("Выберите файл с текстом!");
        showError("Ошибка файл с тектсом не выбран!");
        return 
    }

    dataToSliceTelegams.append("keysType", keysType)
    dataToSliceTelegams.forEach((fieldValue, key) => {
        console.log(key, fieldValue)
    })
    showLoadingIndicator();
    let telegramCuttingResponse = await fetch("http://127.0.0.1:8000/startEncoder/pushTelegramsCuttingData",
        {
            method: "POST",
            body: dataToSliceTelegams
        }
    )
    hideLoadingIndicator();
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

        showLoadingIndicator();
        let keyPropertiesResponse = await fetch("http://127.0.0.1:8000/startEncoder/pushKeysProperties", 
            {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify(dataFromKeySettingForm),
                credentials: 'omit'
            });
        hideLoadingIndicator();
        document.getElementById("text-file").value = "";
        document.getElementById("custom-file-label").textContent ="Выберите файл";
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

            const blob = await keyPropertiesResponse.blob();
            const url = window.URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.style.display = 'none';
            a.href = url;
            
            // Get filename from response headers or use a default
            //const contentDisposition = keyPropertiesResponse.headers.get('content-disposition');
            let filename = 'encryption-result.zip';
            /*
            if (contentDisposition) {
                const filenameMatch = contentDisposition.match(/filename="?(.+)"?/);
                if (filenameMatch) filename = filenameMatch[1];
            }
            filename = filename.substring(0,filename.length - 1)
            */
            a.download = filename;
            document.body.appendChild(a);
            a.click();
            window.URL.revokeObjectURL(url);
            document.body.removeChild(a);

            showToast("Зашифрование прошло успешно!","success");
        }
    } else if (keysType == 'users_keys') {
        let dataFromUserKeysForm = new FormData(document.getElementById(formID))
        Array.from(dataFromUserKeysForm).forEach(element => {console.log(element)}) 
        showLoadingIndicator();
        let userKeysResponse = await fetch("http://127.0.0.1:8000/startEncoder/pushUserKeys",
            {
                method: "POST",
                body: dataFromUserKeysForm
            })
        hideLoadingIndicator();
        document.getElementById("text-file").value = "";
        document.getElementById("custom-file-label").textContent ="Выберите файл";
        document.getElementById("keys-file").value = "";
        document.getElementById("custom-file-label-keys").textContent ="Выберите файл c ключами";
        if(!userKeysResponse.ok) {
            try {
                const errorData = await userKeysResponse.json();
                const errorMessage = errorData.error || "Неизвестная ошибка на сервере";
                const errorDetail = errorData.detail || "Нет дополнительных данных";
                console.error(`Error: ${errorMessage} - ${errorDetail}`);
                showError(`Ошибка: ${errorMessage} - ${errorDetail}`);
            } catch (e) {
                console.error(`Error: ${userKeysResponse.statusText}`);
                showError(`Ошибка: ${userKeysResponse.statusText}`);
            }
            return;
        }
        else{

            const blob = await userKeysResponse.blob();
            const url = window.URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.style.display = 'none';
            a.href = url;
            
            // Get filename from response headers or use a default
            //const contentDisposition = keyPropertiesResponse.headers.get('content-disposition');
            let filename = 'encryption-result.zip';
            /*
            if (contentDisposition) {
                const filenameMatch = contentDisposition.match(/filename="?(.+)"?/);
                if (filenameMatch) filename = filenameMatch[1];
            }
            filename = filename.substring(0,filename.length - 1)
            */
            a.download = filename;
            document.body.appendChild(a);
            a.click();
            window.URL.revokeObjectURL(url);
            document.body.removeChild(a);
            
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

    showLoadingIndicator();
    let responseFromDecript = await fetch("http://127.0.0.1:8000/startDecoder", 
        {
            method: "POST",
            body: dataAboutCipherTextAndKeys
        }
    )
    hideLoadingIndicator();
    document.getElementById("text-file").value = "";
    document.getElementById("custom-file-label").textContent ="Выберите файл";
    if(!responseFromDecript.ok) {
        try {
            const errorData = await responseFromDecript.json();
            const errorMessage = errorData.error || "Неизвестная ошибка на сервере";
            const errorDetail = errorData.detail || "Нет дополнительных данных";
            console.error(`Error: ${errorMessage} - ${errorDetail}`);
            showError(`Ошибка: ${errorMessage} - ${errorDetail}`);
        } catch (e) {
            console.error(`Error: ${responseFromDecript.statusText}`);
            showError(`Ошибка: ${responseFromDecript.statusText}`);
        }
        return;
    } else {

        const blob = await responseFromDecript.blob();
        const url = window.URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.style.display = 'none';
        a.href = url;
            
        // Get filename from response headers or use a default
        const contentDisposition = responseFromDecript.headers.get('content-disposition');
        let filename = 'decryption-result.docx';
        
        if (contentDisposition) {
            const filenameMatch = contentDisposition.match(/filename="?(.+)"?/);
            if (filenameMatch) filename = filenameMatch[1];
        }
        filename = filename.substring(0,filename.length - 1)

        a.download = filename;
        document.body.appendChild(a);
        a.click();
        window.URL.revokeObjectURL(url);
        document.body.removeChild(a);

        showToast("Расшифрование прошло успешно","success");
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

function isValidNaturalNumber(value) {
    // Проверяем, что строка состоит только из цифр и не начинается с 0 (кроме самого 0)
    return /^([1-9]\d*)$/.test(value) && parseInt(value) > 0;
}

export async function encriptSettings() {
    try {
        if(document.getElementById("ciphersList").value == "Empty_tag") {
            alert("Выберите шифр!")
            showError("Выберите шифр!")
        } else if(!isValidNaturalNumber(document.getElementById("count-of-tg").value)  || !isValidNaturalNumber(document.getElementById("lenght-of-tg").value)) {
                alert("Колличество телеграм и их размер должны быть натуральными не содержать 'e' в своей записи")
                showError("Колличество телеграм и их размер должны быть натуральными не содержать 'e' в своей записи")
        } else if(Array.from(document.getElementById("text-file").files).length == 0) {
                alert("Выберите файл с текстом")
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
window.sendDecriptRequest = sendDecriptRequest



function showLoadingIndicator() {
    // Создаем элемент для индикатора загрузки, если его еще нет
    let loader = document.getElementById('loadingIndicator');
    if (!loader) {
        loader = document.createElement('div');
        loader.id = 'loadingIndicator';
        // loader.innerHTML = '⏳'; // или можно использовать CSS-анимацию
        loader.style.cssText = `
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: rgba(0, 0, 0, 0.1);
            display: flex;
            justify-content: center;
            align-items: center;
            font-size: 2rem;
            z-index: 9999;
            cursor: wait;
        `;
        document.body.appendChild(loader);
    }
    
    // Устанавливаем курсор "wait" для всей страницы
    document.body.style.cursor = 'wait';
    loader.style.display = 'flex';
}

function hideLoadingIndicator() {
    // Скрываем индикатор загрузки
    const loader = document.getElementById('loadingIndicator');
    if (loader) {
        loader.style.display = 'none';
    }
    
    // Восстанавливаем обычный курсор
    document.body.style.cursor = 'default';
}





