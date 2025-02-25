export function showError(message) {
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