interfaceLanguage = 'ru';
cipherLanguage = 'ru';
encryptFolderPath = 'dest';
decryptFolderPath = 'dest';
fiveGramsEnabled = 'false'

function showError(message) {
    console.log("showError вызван с сообщением:", message);

    let existingError = document.querySelector('.error-message');
    if (existingError) {
        existingError.remove();
    }

    let errorDiv = document.createElement('div');
    errorDiv.className = 'error-message';
    errorDiv.textContent = message;

    document.body.appendChild(errorDiv);

    setTimeout(() => {
        errorDiv.remove();
    }, 5000);
}

document.addEventListener("DOMContentLoaded", () => {
    if (!document.getElementById("toast-container")) {
        const container = document.createElement("div");
        container.id = "toast-container";
        document.body.appendChild(container);
    }
});

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
    console.log('Creat setting window...');
    Array.from(document.getElementsByClassName("settingWindow-class")).forEach(elem => { elem.remove(); });
    Array.from(document.getElementsByClassName("keys-settings-block-class")).forEach(elem => { elem.remove(); });
    Array.from(document.getElementsByClassName("keys-choose-block-class")).forEach(elem => { elem.remove(); });
    Array.from(document.getElementsByClassName("decript-block-class")).forEach(elem => { elem.remove(); });
    Array.from(document.getElementsByClassName("main-keys-block-class")).forEach(elem => { elem.remove(); });



    const settingsWindow = document.createElement('div');
    settingsWindow.id = 'settingsWindow';
    settingsWindow.className = 'settingWindow-class';



    const heading = document.createElement('h3');
    heading.textContent = 'Настройки';
    settingsWindow.appendChild(heading);

    

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
    
    console.log('Adding checkbox...');
    const fiveGramsContainer = document.createElement('div');
    fiveGramsContainer.style.margin = '10px 0';
    
    const fiveGramsCheckbox = document.createElement('input');
    fiveGramsCheckbox.type = 'checkbox';
    fiveGramsCheckbox.id = 'fiveGramsCheckboxId';
    fiveGramsCheckbox.name = 'fiveGramsCheckbox';
    fiveGramsCheckbox.checked = Boolean(fiveGramsEnabled); // или true, в зависимости от вашей логики
    // Здесь нужно установить начальное состояние чекбокса
    // fiveGramsCheckbox.checked = fiveGramsEnabled; // если у вас есть такая переменная
    
    const fiveGramsLabel = document.createElement('label');
    fiveGramsLabel.htmlFor = 'fiveGramsCheckbox';
    fiveGramsLabel.textContent = 'Использовать 5-граммы';
    fiveGramsLabel.style.marginLeft = '5px';
    
    fiveGramsContainer.appendChild(fiveGramsCheckbox);
    fiveGramsContainer.appendChild(fiveGramsLabel);
    settingsWindow.appendChild(fiveGramsContainer);

    const saveButton = document.createElement('button');
    saveButton.id = 'saveSettings';
    saveButton.textContent = 'Сохранить';

    const closeButton = document.createElement('button');
    closeButton.id = 'closeSettings';
    closeButton.textContent = 'Закрыть';

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
        console.log("Create setting window...")
        createSettingsWindow();
    } else {
        console.log('Showing existing settings window');
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
    interfaceLanguage = "ru";
    cipherLanguage = document.getElementById('cipherLanguage').value;
    fiveGramsEnabled = document.getElementById('fiveGramsCheckboxId').checked;

    const data = {
        "interfaceLanguage": interfaceLanguage,
        "cipherLanguage": cipherLanguage,
        "fiveGramsEnabled": fiveGramsEnabled 
    };

    showLoadingIndicator();
    try {
        let response = await fetch('http://127.0.0.1:8000/settings', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
        });

        if (response.ok) {
            showToast('Настройки сохранены!', 'success');
        } else {
            const errorData = await response.json();
                            const errorMessage = errorData.error || "Неизвестная ошибка на сервере";
                            const errorDetail = errorData.detail || "Нет дополнительных данных";
                            console.error(`Error: ${errorMessage} - ${errorDetail}`);
                            showError(`Ошибка: ${errorMessage} - ${errorDetail}`);
        }
    } catch (error) {
        console.error('Error:', error);
        alert('Возникла ошибка при сохранении настроек!');
    }  finally {
        hideLoadingIndicator();
    }

    closeSettings();
}

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

