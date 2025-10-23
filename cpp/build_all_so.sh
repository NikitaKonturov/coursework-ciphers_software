#!/bin/bash
set -e

# ====== CONFIGURATION ======
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYLIB_DIR="${ROOT_DIR}/pylib"
BUILD_TYPE="Release"
BUILD_DIR_NAME="build"

echo "🔹 Сборка всех шифров в ${ROOT_DIR}"
echo "🔹 Режим: ${BUILD_TYPE}"
echo "🔹 Выходная директория: ${PYLIB_DIR}"
echo

# Очистим старые артефакты (если нужно)
mkdir -p "${PYLIB_DIR}"

# ====== ПОИСК ВСЕХ ПРОЕКТОВ ======
# Ищем все поддиректории, содержащие CMakeLists.txt
CIPHER_DIRS=$(find "${ROOT_DIR}" -mindepth 1 -maxdepth 1 -type d | sort)

# ====== ЦИКЛ ПО ВСЕМ ШИФРАМ ======
for cipher_dir in ${CIPHER_DIRS}; do
    if [ -f "${cipher_dir}/CMakeLists.txt" ]; then
        CIPHER_NAME=$(basename "${cipher_dir}")
        BUILD_DIR="${cipher_dir}/${BUILD_DIR_NAME}"

        echo "🚀 Сборка шифра: ${CIPHER_NAME}"
	
        # Создаём директорию для сборк
        rm -rf "${BUILD_DIR}"
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"

        # Генерация и сборка
        cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
        cmake --build . --config ${BUILD_TYPE} -j$(nproc)

        # Копируем .so (или .pyd / .dylib) в pylib
        find . -type f \( -name "*.so" -o -name "*.pyd" -o -name "*.dylib" \) -exec cp {} "${PYLIB_DIR}/" \;

        echo "✅ ${CIPHER_NAME} — собран успешно"
        echo
        cd "${ROOT_DIR}"
    fi
done

echo "🎯 Все шифры собраны. Файлы находятся в: ${PYLIB_DIR}"
