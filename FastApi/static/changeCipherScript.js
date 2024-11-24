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
            buttonConfirm.textContent = "Confirm"
            buttonConfirm.addEventListener("click", function(){event.preventDefault(); sendEncriptRequest("keys-settings-block", "keys_settings")}, true);
            blockConfirmKey.appendChild(buttonConfirm)    

            divParametr.appendChild(nameLabel)
            divParametr.appendChild(inputLabel)
            keysSettingBlock.appendChild(divParametr)
            keysSettingBlock.appendChild(blockConfirmKey)
        });

        
        Array.from(document.getElementsByClassName("keys-settings-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("keys-choose-block-class")).forEach(elem => { elem.remove(); });


        document.getElementById("main-keys-block").appendChild(keysSettingBlock)
        
    } catch (error) {
        console.error("Response error: ", error);
    }
}

async function sendEncriptRequest(formID, keysType) {
    //let dataFromKeyForm = new FormData(document.getElementById(formID))
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

    // let serverResponse = await fetch("http://127.0.0.1:8000/startEncoder", 
    //     {
    //         method: "POST",
    //         headers: {
    //             "Content-Type": "multipart/form-data"
    //         },
    //         body: dataToSliceTelegams
    // });
}



async function addBlockOfGetUsersKeys() {
    try {
        let blockWithChooseKey = document.createElement("div")
        blockWithChooseKey.id = "keys-choose-block"
        blockWithChooseKey.className = "keys-choose-block-class"
        
        let formChooseKeysFile = document.createElement("form")
        formChooseKeysFile.enctype="multipart/form-data"
        formChooseKeysFile.className = "keys-choose-block"

        let labelChooseElement = document.createElement("label")
        labelChooseElement.id = "input-file"

        let spanUsersKeys = document.createElement("span")
        spanUsersKeys.id = "custom-file-label-keys"
        spanUsersKeys.textContent = "Choose file with keys"

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
        blockConfirmKey.id = "keys-choose-block"
        blockConfirmKey.className = "keys-choose-block-class"

        let buttonConfirm = document.createElement("button")
        buttonConfirm.textContent = "Confirm"
        buttonConfirm.addEventListener("click", function(){event.preventDefault(); sendEncriptRequest("keys-choose-block", "users-keys")} ,true)

        labelChooseElement.appendChild(spanUsersKeys)
        labelChooseElement.appendChild(inputUsersKeys)
        formChooseKeysFile.appendChild(labelChooseElement)
        blockConfirmKey.appendChild(buttonConfirm)
        formChooseKeysFile.appendChild(blockConfirmKey)
        blockWithChooseKey.appendChild(formChooseKeysFile)


        
        Array.from(document.getElementsByClassName("keys-settings-block-class")).forEach(elem => { elem.remove(); });
        Array.from(document.getElementsByClassName("keys-choose-block-class")).forEach(elem => { elem.remove(); });
        
        document.getElementById("main-keys-block").appendChild(blockWithChooseKey)
    } catch(error) {
        console.error(error)
    }
}


async function preventActionButton() {
    event.preventDefault()
}


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
            autoGenKeysButton.textContent = "Auto keys generation"
            autoGenKeysButton.addEventListener('click', addBlockOfKeysSettings, true)
            selectKyesTypeBlock.appendChild(autoGenKeysButton)

            let getUserKeysButton = document.createElement("button")
            getUserKeysButton.textContent = "Select keys"
            getUserKeysButton.addEventListener('click', addBlockOfGetUsersKeys, true)
            selectKyesTypeBlock.appendChild(getUserKeysButton)

            let rigthBlock = document.createElement("div")
            rigthBlock.id = "main-keys-block"
            rigthBlock.className = "main-keys-block-class"
            rigthBlock.appendChild(selectKyesTypeBlock)
            
            Array.from(document.getElementsByClassName("main-keys-block-class")).forEach(elem => {elem.remove();})
            document.body.appendChild(rigthBlock)

        }
    }
    catch(err){
        console.error("Error occurred:", err);
    }
}