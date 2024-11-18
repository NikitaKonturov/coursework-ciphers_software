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

        document.getElementById("main-keys-block").appendChild(keysSettingBlock)
        
    } catch (error) {
        console.error("Response error:", error);
    }
}






async function changeCipher() {
    try {
        let selectKyesTypeBlock = document.createElement("div")
        selectKyesTypeBlock.id = "select-type-keys-block"
        let autoGenKeysButton = document.createElement("button")
        autoGenKeysButton.textContent = "Auto keys generation"
        autoGenKeysButton.addEventListener('click', addBlockOfKeysSettings, true)
        selectKyesTypeBlock.appendChild(autoGenKeysButton)
        
        let rigthBlock = document.createElement("div")
        rigthBlock.id = "main-keys-block"
        rigthBlock.appendChild(selectKyesTypeBlock)

        document.body.appendChild(rigthBlock)
    }
    catch(err){
        console.error("Error occurred:", err);
    }
}