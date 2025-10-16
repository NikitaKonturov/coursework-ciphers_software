import ciphers_api_module.ciphers_api_module as cipher_api
from fastapi.encoders import jsonable_encoder
from include_ciphers import Homophone_cipher
import os

try:
    # Указываем путь к директории с заголовочными файлами для C++ библиотеки
    pathToLibs = os.path.join(os.path.dirname(__file__), "include_ciphers")

    # Инициализируем API с помощью pybind11, передаем путь к библиотекам C++
    api = cipher_api.CppCiphers(pathToCiphersDir=pathToLibs)


    # Получаем и выводим доступные шифры
    print(api.get_ciphers_dict())

    # # Получаем и выводим свойства ключа для шифра 'Cardan-cipher' в виде JSON
    # key_properties = api.get_key_propertys('Cardan-cipher')
    # print(jsonable_encoder(key_properties.body))

    # # Шифруем несколько сообщений с использованием шифра 'Cardan-cipher' и передаем дополнительные параметры
    # keys_and_ciphertexts = api.encript_telegrams("Cardan-cipher", ["HelloWorld", "WorldHello"], None, {"permutation_size": 5})

    # # Выводим результат шифрования
    # print(keys_and_ciphertexts)

    # # Расшифровываем зашифрованные сообщения с использованием шифра 'Cardan-cipher'
    # decrypted_texts = api.decript_telegrams("Cardan-cipher", keys_and_ciphertexts)

    # # Выводим результат расшифровки
    # print(decrypted_texts)

    # # Пример использования метода decript_telegrams с зашифрованными и вновь зашифрованныpythми данными
    # print(api.decript_telegrams("Cardan-cipher", api.encript_telegrams("Cardan-cipher", ["Hello world!!!"], [" It's me"])))

    # # Пример использования метода шифрования для одного текста
    # print(api.encript_telegram("Hello world!!!", 19, "Cardan-cipher"))

except Exception as e:
    print(f"Произошла ошибка: {e}")

# Задержка, чтобы окно не закрывалось сразу
input("Нажмите Enter для завершения...")  # Программа будет ждать нажатия клавиши
