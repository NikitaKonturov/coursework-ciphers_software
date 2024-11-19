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

        const keysSettingBlock = document.createElement("div");
        keysSettingBlock.id = "keys-settings-block"
        
        
        let blockName = document.createElement("label");
        blockName.textContent = (document.getElementById("ciphersList").value + ' keys properties')
        keysSettingBlock.appendChild(blockName)

        settingsOfKeys.params.forEach(param => {
            let divParametr = document.createElement("div")
            divParametr.id = param.name
            let nameLabel = document.createElement("label")
            nameLabel.id = param.name + "Label"
            nameLabel.htmlFor = param.name
            nameLabel.textContent = param.label
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

        
        if(document.getElementById("keys-choose-block") != null) {
            document.getElementById("keys-choose-block").remove()
        }
        document.getElementById("main-keys-block").appendChild(keysSettingBlock)
        
    } catch (error) {
        console.error("Response error: ", error);
    }
}

async function addBlockOfGetUsersKeys() {
    try {
        let blockWithChooseKey = document.createElement("div")
        blockWithChooseKey.id = "keys-choose-block"
        
        let labelChooseElement = document.createElement("label")
        labelChooseElement.id = "choose-keys-label"
        labelChooseElement.textContent = "Choose file with keys"
        labelChooseElement.htmlFor = "input-keys"

        let inputUsersKeys = document.createElement("input")
        inputUsersKeys.id = "keys-file"
        inputUsersKeys.name = "keys_file"
        inputUsersKeys.type = "file"
        inputUsersKeys.addEventListener('change', function(){document.getElementById('choose-keys-label').textContent = this.files[0].name}, true)
        inputUsersKeys.accept = ".doc,.docx,application/msword, text/plain"
        
        blockWithChooseKey.appendChild(labelChooseElement)
        blockWithChooseKey.appendChild(inputUsersKeys)
        
        if(document.getElementById("keys-settings-block") != null) {
            document.getElementById("keys-settings-block").remove()
        }
        document.getElementById("main-keys-block").appendChild(blockWithChooseKey)
    } catch(error) {
        console.error(error)
    }
}



async function encriptSettings() {
    try {
        if(document.getElementById("ciphersList").value == "Empty_tag") {
            alert("Сhoose a cipher!")
        } else {
            if(document.getElementById("count-of-tg").value <= 0 || document.getElementById("lenght-of-tg").value <= 0) {
                alert("Count of telegrmas and size of telegrams must be nutural")
            }
            let selectKyesTypeBlock = document.createElement("div")
            selectKyesTypeBlock.id = "select-type-keys-block"
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
            rigthBlock.appendChild(selectKyesTypeBlock)
                

            document.body.appendChild(rigthBlock)

        }
    }
    catch(err){
        console.error("Error occurred:", err);
    }
}