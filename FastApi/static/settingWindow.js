
async function createSettingsWindow() {
    Array.from(document.getElementsByClassName("settingWindow-class")).forEach(elem => { elem.remove(); });

    const settingsWindow = document.createElement('div');
    settingsWindow.id = 'settingsWindow';
    settingsWindow.className = 'settingWindow-class';

    const heading = document.createElement('h3');
    heading.textContent = 'Settings';
    settingsWindow.appendChild(heading);

    const uiLanguageLabel = document.createElement('label');
    uiLanguageLabel.textContent = 'Interface Language:';
    settingsWindow.appendChild(uiLanguageLabel);

    const uiLanguageSelect = document.createElement('select');
    uiLanguageSelect.name = 'uiLanguage';
    uiLanguageSelect.id = 'uiLanguage';

    const uiLanguages = [
        { value: 'en', text: 'English' },
        { value: 'ru', text: 'Russian' }
    ];
    uiLanguages.forEach(lang => {
        const option = document.createElement('option');
        option.value = lang.value;
        option.textContent = lang.text;
        uiLanguageSelect.appendChild(option);
    });
    settingsWindow.appendChild(uiLanguageSelect);

    const cipherLanguageLabel = document.createElement('label');
    cipherLanguageLabel.textContent = 'Cipher Language:';
    settingsWindow.appendChild(cipherLanguageLabel);

    const cipherLanguageSelect = document.createElement('select');
    cipherLanguageSelect.name = 'cipherLanguage';
    cipherLanguageSelect.id = 'cipherLanguage';

    const cipherLanguages = [
        { value: 'en', text: 'English' },
        { value: 'ru', text: 'Russian' }
    ];
    cipherLanguages.forEach(lang => {
        const option = document.createElement('option');
        option.value = lang.value;
        option.textContent = lang.text;
        cipherLanguageSelect.appendChild(option);
    });
    settingsWindow.appendChild(cipherLanguageSelect);

    const folderPathEncryptLabel = document.createElement('label');
    folderPathEncryptLabel.textContent = 'Folder Path for Encrypt:';
    settingsWindow.appendChild(folderPathEncryptLabel);

    const folderPathEncryptButton = document.createElement('button');
    folderPathEncryptButton.textContent = 'Choose Folder';
    folderPathEncryptButton.onclick = async () => {
        const folderHandle = await selectFolder();
        if (folderHandle) {
            folderPathEncryptButton.dataset.folderPath = folderHandle.name;
            folderPathEncryptButton.textContent = `Selected: ${folderHandle.name}`;
        }
    };
    settingsWindow.appendChild(folderPathEncryptButton);

    const folderPathDecryptLabel = document.createElement('label');
    folderPathDecryptLabel.textContent = 'Folder Path for Decrypt:';
    settingsWindow.appendChild(folderPathDecryptLabel);

    const folderPathDecryptButton = document.createElement('button');
    folderPathDecryptButton.textContent = 'Choose Folder';
    folderPathDecryptButton.onclick = async () => {
        const folderHandle = await selectFolder();
        if (folderHandle) {
            folderPathDecryptButton.dataset.folderPath = folderHandle.name;
            folderPathDecryptButton.textContent = `Selected: ${folderHandle.name}`;
        }
    };
    settingsWindow.appendChild(folderPathDecryptButton);

    const saveButton = document.createElement('button');
    saveButton.id = 'saveSettings';
    saveButton.textContent = 'Save';

    const closeButton = document.createElement('button');
    closeButton.id = 'closeSettings';
    closeButton.textContent = 'Close';

    const buttonContainer = document.createElement('div');
    buttonContainer.style.display = 'flex';
    buttonContainer.style.justifyContent = 'space-between';
    buttonContainer.style.marginTop = '20px';

    buttonContainer.appendChild(saveButton);
    buttonContainer.appendChild(closeButton);
    settingsWindow.appendChild(buttonContainer);

    closeButton.onclick = closeSettings;
    saveButton.onclick = saveSettings;
    document.body.appendChild(settingsWindow);
}

async function selectFolder() {
    try {
        const folderHandle = await window.showDirectoryPicker();
        console.log('Selected folder:', folderHandle.name);
        return folderHandle;
    } catch (error) {
        console.error('Folder selection canceled or not supported:', error);
        return null;
    }
}

function openSettings() {
    const settingsWindow = document.getElementById('settingsWindow');
    if (!settingsWindow) {
        createSettingsWindow();
    } else {
        settingsWindow.style.display = 'block';
    }
}

function closeSettings() {
    const settingsWindow = document.getElementById('settingsWindow');
    if (settingsWindow) {
        settingsWindow.style.display = 'none';
    }
}

document.addEventListener("DOMContentLoaded", () => {
    if (!document.getElementById("toast-container")) {
        const container = document.createElement("div");
        container.id = "toast-container";
        document.body.appendChild(container);
    }
});

function showToast(message, type, duration = 3000) {
    console.log(`showToast called with: ${message}, type: ${type}`);

    const container = document.getElementById("toast-container");
    if (!container) {
        console.error("Toast container not found!");
        return;
    }

    const toast = document.createElement("div");
    toast.className = `toast ${type}`;
    toast.innerText = message;

    container.appendChild(toast);

    setTimeout(() => {
        toast.classList.add("hide");
        setTimeout(() => toast.remove(), 500);
    }, duration);
}

 async function preventActionButton() {
    event.preventDefault()
}

async function saveSettings() {
    const uiLanguage = document.getElementById('uiLanguage').value;
    const cipherLanguage = document.getElementById('cipherLanguage').value;

    const encryptButton = document.querySelector('button[data-folder-path]');
    const encryptFolderPath = encryptButton ? encryptButton.dataset.folderPath : '';

    const decryptButton = document.querySelector('button[data-folder-path]');
    const decryptFolderPath = decryptButton ? decryptButton.dataset.folderPath : '';

    const data = {
        interfaceLanguage: uiLanguage,
        cipherLanguage: cipherLanguage,
        encryptFolderPath,
        decryptFolderPath,
    };

    try {
        const response = await fetch('http://127.0.0.1:8000/settings', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
        });

        if (response.ok) {
            showToast('Settings saved successfully!', 'success');
        } else {
            showToast('Failed to save settings!', 'error');
        }
    } catch (error) {
        console.error('Error:', error);
        alert('An error occurred while saving settings!');
    }

    closeSettings();
}
