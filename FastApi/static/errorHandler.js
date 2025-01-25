export function showError(message) {
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
