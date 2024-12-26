async function addBlockOfKeysSettings() {
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
            throw WebTransportError("Bad server response...")
        }

        const settingsOfKeys = await serverResponse.json()

        const keysSettingBlock = document.createElement("form");
        keysSettingBlock.id = "keys-settings-block"
        keysSettingBlock.className = "keys-settings-block-class"
        keysSettingBlock.enctype = "multipart/form-data"
        

        let blockName = document.createElement("label");
        blockName.textContent = (document.getElementById("ciphersList").value + ' keys properties:')
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
            let blockConfirmKey = document.createElement("div")
            blockConfirmKey.id = "keys-choose-block"
            blockConfirmKey.className = "keys-choose-block-class"
            let buttonConfirm = document.createElement("button")
<<<<<<< HEAD
            buttonConfirm.textContent = gettext("Confirm")
=======
            buttonConfirm.textContent = "Confirm"
>>>>>>> displaying_key_settings
            buttonConfirm.addEventListener("click", function(){event.preventDefault(); sendEncriptRequest("keys-settings-block", "keys_settings")}, true);
            blockConfirmKey.appendChild(buttonConfirm)    

            divParametr.appendChild(nameLabel)
            divParametr.appendChild(inputLabel)
            keysSettingBlock.appendChild(divParametr)
            keysSettingBlock.appendChild(blockConfirmKey)
        });

        
        Array.from(document.getElementsByClassName("keys-settings-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("keys-choose-block-class")).forEach(elem => { elem.remove(); });
<<<<<<< HEAD


=======
        Array.from(document.getElementsByClassName("decript-block-class")).forEach(elem => { elem.remove(); });
            
>>>>>>> displaying_key_settings
        document.getElementById("main-keys-block").appendChild(keysSettingBlock)
        
    } catch (error) {
        console.error("Response error: ", error);
    }
}

<<<<<<< HEAD
async function sendEncriptRequest(formID, keysType) {
    let dataFromKeyForm = new FormData(document.getElementById(formID))
    let dataToSliceTelegams = new FormData(document.getElementById("slice-telegrmas-form"))
    
=======
function checkNumber(elementValue) {
    if(Number(elementValue) == NaN) {
        return elementValue
    }  else {
        return Number(elementValue)
    }
}


async function sendEncriptRequest(formID, keysType) {
    let dataToSliceTelegams = new FormData(document.getElementById("slice-telegrmas-form"))

>>>>>>> displaying_key_settings
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
        console.error(telegramCuttingResponse.statusText)
<<<<<<< HEAD

        return
    } 


    let dataFromKeyFormInDict = {}

    dataFromKeyForm.forEach((value, field) => {
        dataFromKeyFormInDict[field] = value
        console.log(field, value)
    })

    let keyPropertiesResponse = await fetch("http://127.0.0.1:8000/startEncoder/pushKeysProperties", 
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(dataFromKeyFormInDict)
    });

}


=======
        return
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
            console.error(keyPropertiesResponse.statusText) 
            return
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
    } 
}

async function sendDecriptRequest() 
{
    let dataAboutCipherTextAndKeys = new FormData(document.getElementById("slice-telegrmas-form"))
    
    dataAboutCipherTextAndKeys.delete("length")
    dataAboutCipherTextAndKeys.delete("number")

    if(document.getElementById("ciphersList").value == "Empty_tag") {
        alert("Сhoose a cipher!")
        return
    } 

    const fileInput = document.getElementById("text-file");

    if (!fileInput || !fileInput.files || fileInput.files.length === 0) {
        alert("Choose a file with cipher text");
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
>>>>>>> displaying_key_settings

async function addBlockOfGetUsersKeys() {
    try {
        let blockWithChooseKey = document.createElement("div")
<<<<<<< HEAD
        blockWithChooseKey.id = "keys-choose-block"
=======
>>>>>>> displaying_key_settings
        blockWithChooseKey.className = "keys-choose-block-class"
        
        let formChooseKeysFile = document.createElement("form")
        formChooseKeysFile.enctype="multipart/form-data"
        formChooseKeysFile.className = "keys-choose-block"
<<<<<<< HEAD
=======
        formChooseKeysFile.id = "keys-choose-block"
>>>>>>> displaying_key_settings

        let labelChooseElement = document.createElement("label")
        labelChooseElement.id = "input-file"

        let spanUsersKeys = document.createElement("span")
        spanUsersKeys.id = "custom-file-label-keys"
<<<<<<< HEAD
        spanUsersKeys.textContent = gettext("Choose file with keys")
=======
        spanUsersKeys.textContent = "Choose file with keys"
>>>>>>> displaying_key_settings

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
<<<<<<< HEAD
        blockConfirmKey.id = "keys-choose-block"
        blockConfirmKey.className = "keys-choose-block-class"

        let buttonConfirm = document.createElement("button")
        buttonConfirm.textContent = gettext("Confirm")
=======
        blockConfirmKey.className = "keys-choose-block-class"

        let buttonConfirm = document.createElement("button")
        buttonConfirm.textContent = "Confirm"
>>>>>>> displaying_key_settings
        buttonConfirm.addEventListener("click", function(){event.preventDefault(); sendEncriptRequest("keys-choose-block", "users_keys")} ,true)

        labelChooseElement.appendChild(spanUsersKeys)
        labelChooseElement.appendChild(inputUsersKeys)
        formChooseKeysFile.appendChild(labelChooseElement)
        blockConfirmKey.appendChild(buttonConfirm)
        formChooseKeysFile.appendChild(blockConfirmKey)
        blockWithChooseKey.appendChild(formChooseKeysFile)


        
        Array.from(document.getElementsByClassName("keys-settings-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("keys-choose-block-class")).forEach(elem => { elem.remove(); });
<<<<<<< HEAD
        
=======
        Array.from(document.getElementsByClassName("decript-block-class")).forEach(elem => { elem.remove(); });
            
>>>>>>> displaying_key_settings
        document.getElementById("main-keys-block").appendChild(blockWithChooseKey)
    } catch(error) {
        console.error(error)
    }
}

<<<<<<< HEAD

=======
>>>>>>> displaying_key_settings
async function preventActionButton() {
    event.preventDefault()
}

<<<<<<< HEAD
=======
// async function showDecriptSettings() {
//     try {
//         if(document.getElementById("ciphersList").value == "Empty_tag") {
//             alert("Сhoose a cipher!")
//         }else {
//             let decriptBlock = document.createElement("div")
//             decriptBlock.id = "decript-block"
//             decriptBlock.className = "decript-block-class"

//             let decriptBlockLabel = document.createElement("label")
//             decriptBlockLabel.textContent = "Choose cipher text"
//             decriptBlock.appendChild(decriptBlockLabel)

//             let formChooseCipherTextFile = document.createElement("form")
//             formChooseCipherTextFile.enctype="multipart/form-data"
//             formChooseCipherTextFile.className = "cipher-text-choose-block"
//             formChooseCipherTextFile.id = "cipher-text-choose-block"
    
//             let labelChooseElement = document.createElement("label")
//             labelChooseElement.id = "input-file"
    
//             let spanCipherText = document.createElement("span")
//             spanCipherText.id = "custom-file-label-cipher-text"
//             spanCipherText.textContent = "Choose file with cipher text"
    
//             let inputCipherTexts = document.createElement("input")
//             inputCipherTexts.id = "cipher-text-file"
//             inputCipherTexts.addEventListener('change', function(){document.getElementById('choose-cipher-text-file').textContent = this.files[0].name}, true)
//             inputCipherTexts.accept = ".doc,.docx,application/msword, text/plain"
//             inputCipherTexts.name = "cipher_text_file"
//             inputCipherTexts.addEventListener('change', function() {
//                 document.getElementById('custom-file-label-cipher-text').textContent = this.files[0].name;
//             })
//             inputCipherTexts.required = ""
//             inputCipherTexts.style = "display: none;"
//             inputCipherTexts.type = "file"
            
//             let blockConfirmCipherText = document.createElement("div")
//             blockConfirmCipherText.className = "keys-choose-block-class"
    
//             let buttonConfirm = document.createElement("button")
//             buttonConfirm.textContent = "Confirm"
//             buttonConfirm.addEventListener("click", function(){event.preventDefault()} ,true)
    
        
//             labelChooseElement.appendChild(spanCipherText)
//             labelChooseElement.appendChild(inputCipherTexts)
//             formChooseCipherTextFile.appendChild(labelChooseElement)
//             blockConfirmCipherText.appendChild(buttonConfirm)
//             formChooseCipherTextFile.appendChild(blockConfirmCipherText)
//             decriptBlock.appendChild(formChooseCipherTextFile)
    
    
            
//             Array.from(document.getElementsByClassName("keys-settings-block-class")).forEach(elem => { elem.remove(); });
//             Array.from(document.getElementsByClassName("keys-choose-block-class")).forEach(elem => { elem.remove(); });
//             Array.from(document.getElementsByClassName("decript-block-class")).forEach(elem => { elem.remove(); });
//             Array.from(document.getElementsByClassName("main-keys-block-class")).forEach(elem => { elem.remove(); });
            
//             document.body.appendChild(decriptBlock)

//         }
//     }
//     catch(err){
//         console.error("Error occurred:", err);
//     }
// }

>>>>>>> displaying_key_settings

async function encriptSettings() {
    try {
        if(document.getElementById("ciphersList").value == "Empty_tag") {
            alert("Сhoose a cipher!")
        } else if(document.getElementById("count-of-tg").value <= 0 || document.getElementById("lenght-of-tg").value <= 0) {
                alert("Count of telegrmas and size of telegrams must be nutural")
        } else if(Array.from(document.getElementById("text-file").files).length == 0) {
                alert("Choose file with text")
        }else {
            let selectKyesTypeBlock = document.createElement("div")
            selectKyesTypeBlock.id = "button-container"
            selectKyesTypeBlock.className = "button-container"
            let autoGenKeysButton = document.createElement("button")
<<<<<<< HEAD
            autoGenKeysButton.textContent = gettext("Auto keys generation")
=======
            autoGenKeysButton.textContent = "Auto keys generation"
>>>>>>> displaying_key_settings
            autoGenKeysButton.addEventListener('click', addBlockOfKeysSettings, true)
            selectKyesTypeBlock.appendChild(autoGenKeysButton)

            let getUserKeysButton = document.createElement("button")
<<<<<<< HEAD
            getUserKeysButton.textContent = gettext("Select keys")
=======
            getUserKeysButton.textContent = "Select keys"
>>>>>>> displaying_key_settings
            getUserKeysButton.addEventListener('click', addBlockOfGetUsersKeys, true)
            selectKyesTypeBlock.appendChild(getUserKeysButton)

            let rigthBlock = document.createElement("div")
            rigthBlock.id = "main-keys-block"
            rigthBlock.className = "main-keys-block-class"
            rigthBlock.appendChild(selectKyesTypeBlock)
            
            Array.from(document.getElementsByClassName("main-keys-block-class")).forEach(elem => {elem.remove();})
<<<<<<< HEAD
=======
            Array.from(document.getElementsByClassName("decript-block-class")).forEach(elem => { elem.remove(); });
>>>>>>> displaying_key_settings
            document.body.appendChild(rigthBlock)

        }
    }
    catch(err){
        console.error("Error occurred:", err);
    }
<<<<<<< HEAD
}
=======
}
>>>>>>> displaying_key_settings
