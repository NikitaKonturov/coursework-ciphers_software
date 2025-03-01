interfaceLanguage = 'ru';
cipherLanguage = 'ru';
encryptFolderPath = 'testFiles';
decryptFolderPath = 'testFiles';

async function showToast(message, type, duration = 3000) {
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

async function createSettingsWindow() {
    Array.from(document.getElementsByClassName("settingWindow-class")).forEach(elem => { elem.remove(); });

    const settingsWindow = document.createElement('div');
    settingsWindow.id = 'settingsWindow';
    settingsWindow.className = 'settingWindow-class';

    const heading = document.createElement('h3');
    heading.textContent = 'Настройки';
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
        if(lang.value == interfaceLanguage) {
            option.selected = true;
        }
        uiLanguageSelect.appendChild(option);
    });

    settingsWindow.appendChild(uiLanguageSelect);

    const cipherLanguageLabel = document.createElement('label');
    cipherLanguageLabel.textContent = 'Язык текста:';
    settingsWindow.appendChild(cipherLanguageLabel);

    const cipherLanguageSelect = document.createElement('select');
    cipherLanguageSelect.name = 'cipherLanguage';
    cipherLanguageSelect.id = 'cipherLanguage';

    const cipherLanguages = [
        { value: 'en', text: 'Английский' },
        { value: 'ru', text: 'Русский' }
    ];

    cipherLanguages.forEach(lang => {
        const option = document.createElement('option');
        option.value = lang.value;
        option.textContent = lang.text;
        if(lang.value == cipherLanguage) {
            option.selected = true;
        }
        cipherLanguageSelect.appendChild(option);
    });
    settingsWindow.appendChild(cipherLanguageSelect);

    const folderPathEncryptLabel = document.createElement('label');
    folderPathEncryptLabel.textContent = 'Путь к папке с результатами зашифрования:';
    settingsWindow.appendChild(folderPathEncryptLabel);

    const folderPathEncryptButton = document.createElement('button');
    folderPathEncryptButton.textContent = encryptFolderPath;
    folderPathEncryptButton.dataset.encriptFolderPath = encryptFolderPath
    folderPathEncryptButton.onclick = async () => {
        const folderHandle = await selectFolder();
        if (folderHandle) {
            folderPathEncryptButton.dataset.encriptFolderPath = folderHandle.name;
            folderPathEncryptButton.textContent = `Selected: ${folderHandle.name}`;
        }
    };
    settingsWindow.appendChild(folderPathEncryptButton);

    const folderPathDecryptLabel = document.createElement('label');
    folderPathDecryptLabel.textContent = 'Путь к папке с результатами расшифрования:';
    settingsWindow.appendChild(folderPathDecryptLabel);

    const folderPathDecryptButton = document.createElement('button');
    folderPathDecryptButton.textContent = decryptFolderPath;
    folderPathDecryptButton.dataset.decriptFolderPath = decryptFolderPath
    folderPathDecryptButton.onclick = async () => {
        const folderHandle = await selectFolder();
        if (folderHandle) {
            folderPathDecryptButton.dataset.decriptFolderPath = folderHandle.name;
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
        console.log('Выбранная папка:', folderHandle.name);
        return folderHandle;
    } catch (error) {
        console.error('Выбор папки прекращен:', error);
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

async function saveSettings() {
    interfaceLanguage = document.getElementById('uiLanguage').value;
    cipherLanguage = document.getElementById('cipherLanguage').value;
    encryptFolderPath = document.querySelector('button[data-encript-folder-path]').dataset.encriptFolderPath;
    decryptFolderPath = document.querySelector('button[data-decript-folder-path]').dataset.decriptFolderPath;

    const data = {
        "interfaceLanguage": interfaceLanguage,
        "cipherLanguage": cipherLanguage,
        "encryptFolderPath": encryptFolderPath,
        "decryptFolderPath": decryptFolderPath,
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
            showToast('Настройки сохранены!', 'success');
        } else {
            showToast('Не получилось сохранить настройки!', 'error');
        }
    } catch (error) {
        console.error('Error:', error);
        alert('Возникла ошибка при сохранении настроек!');
    }

    closeSettings();
}
