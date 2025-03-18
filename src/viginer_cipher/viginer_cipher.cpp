#include "viginer_cipher.hpp"

/*
// Быстрая проверка русских символов через битовые маски
constexpr bool is_russian_upper(uint32_t cp) {
    return (cp >= 0x0410 && cp <= 0x042F) && (cp != 0x0401);
}

constexpr bool is_russian_lower(uint32_t cp) {
    return (cp >= 0x0430 && cp <= 0x044F) && (cp != 0x0451);
}
void append_utf8(std::string& s, uint32_t cp) {
    if(cp <= 0x7F) {
        s.push_back(static_cast<char>(cp));
    }
    else if(cp <= 0x7FF) {
        s.push_back(0xC0 | ((cp >> 6) & 0x1F));
        s.push_back(0x80 | (cp & 0x3F));
    }
    else if(cp <= 0xFFFF) {
        s.push_back(0xE0 | ((cp >> 12) & 0x0F));
        s.push_back(0x80 | ((cp >> 6) & 0x3F));
        s.push_back(0x80 | (cp & 0x3F));
    }
    else {
        s.push_back(0xF0 | ((cp >> 18) & 0x07));
        s.push_back(0x80 | ((cp >> 12) & 0x3F));
        s.push_back(0x80 | ((cp >> 6) & 0x3F));
        s.push_back(0x80 | (cp & 0x3F));
    }
}

// Оптимизированная обработка русского слова
bool process_russian(const std::string& word, std::string& processed, int& length) {
    processed.clear();
    length = 0;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(word.data());
    const uint8_t* end = p + word.size();
    
    while(p < end) {
        uint32_t cp = 0;
        size_t bytes = 0;
        
        // Декодирование UTF-8
        if((*p & 0x80) == 0) return false; // ASCII
        else if((*p & 0xE0) == 0xC0) bytes = 2;
        else if((*p & 0xF0) == 0xE0) bytes = 3;
        else return false;

        if(p + bytes > end) return false;
        
        // Декодируем кодпоинт
        switch(bytes) {
            case 2: cp = ((p[0] & 0x1F) << 6) | (p[1] & 0x3F); break;
            case 3: cp = ((p[0] & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F); break;
        }

        // Проверка на 'ё'
        if(cp == 0x0401 || cp == 0x0451) return false;
        
        // Преобразование регистра
        if(cp >= 0x0430 && cp <= 0x044F) { // строчные
            cp -= 0x20; // в верхний регистр
        }
        else if(cp < 0x0410 || cp > 0x042F) { // не русские
            return false;
        }

        // Кодируем обратно в UTF-8 и добавляем
        append_utf8(processed, cp);
        length++;
        p += bytes;
    }
    
    return !processed.empty();
}

// Функция обработки английского слова
bool process_english(const std::string& word, std::string& result, int& length) {
    result.resize(word.size());
    length = 0;
    bool valid = true;
    
    for(size_t i = 0; i < word.size(); ++i) {
        unsigned char c = word[i];
        if(!isalpha(c)) {
            valid = false;
            break;
        }
        result[i] = toupper(c);
        length++; // Для английского 1 символ = 1 байт
    }
    
    return valid;
}

// Пакетная вставка с кешированием statement
class BatchInserter {
    sqlite3* db;
    sqlite3_stmt* stmt;
    
    public:
    BatchInserter(sqlite3* db) : db(db) {
        const char* sql = "INSERT INTO words (lang, length, word) VALUES (?, ?, ?)";
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_exec(db, "BEGIN", nullptr, nullptr, nullptr);
    }
    
    ~BatchInserter() {
        sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
        sqlite3_finalize(stmt);
    }
    
    void add(const char* lang, const std::string& word, int length) {
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, lang, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, length); // Используем переданную длину
        sqlite3_bind_text(stmt, 3, word.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
};

void process_file_optimized(const std::string& filename, 
BatchInserter& inserter,
const std::string& lang) {
    std::ifstream file(filename, std::ios::binary);
    if(!file) return;
    
    std::string word;
    std::string processed;
    int length = 0;
    
    while(file >> word) {
        bool valid = false;
        processed.clear();
        length = 0;
        
        if(lang == "ru") {
            valid = process_russian(word, processed, length);
        } else {
            valid = process_english(word, processed, length);
        }
        
        if(valid) {
            inserter.add(lang.c_str(), processed, length);
        }
    }
}

void write_words_fast(const std::string& ru_file, 
const std::string& en_file,
const std::string& db_file) {
    sqlite3* db;
    sqlite3_open_v2(db_file.c_str(), &db, 
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 
                   nullptr);
                   
                   // Экстремальные настройки для скорости
                   sqlite3_exec(db, "PRAGMA journal_mode = OFF", nullptr, nullptr, nullptr);
                   sqlite3_exec(db, "PRAGMA synchronous = OFF", nullptr, nullptr, nullptr);
                   sqlite3_exec(db, "PRAGMA temp_store = MEMORY", nullptr, nullptr, nullptr);
                   sqlite3_exec(db, "PRAGMA locking_mode = EXCLUSIVE", nullptr, nullptr, nullptr);
                   
                   const char* schema = 
                   "CREATE TABLE words(lang TEXT, length INT, word TEXT);"
                   "CREATE INDEX idx_lang ON words(lang);";
                   sqlite3_exec(db, schema, nullptr, nullptr, nullptr);
                   
                   {
                    BatchInserter inserter(db);
                    process_file_optimized(ru_file, inserter, "ru");
                    process_file_optimized(en_file, inserter, "en");
                }
                
                sqlite3_close(db);
            }
            
            // Добавляем заголовок для Windows API
            #ifdef _WIN32
            #include <windows.h>
            #endif
            
            void print_entire_database(const std::string& db_path) {
                sqlite3* db;
                sqlite3_stmt* stmt;
                
                #ifdef _WIN32
                SetConsoleOutputCP(65001);
                CONSOLE_FONT_INFOEX font = { sizeof(font) };
                font.FontFamily = FF_DONTCARE;
                font.dwFontSize.Y = 16;
                wcscpy_s(font.FaceName, L"Consolas");
                SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &font);
                #else
                std::locale::global(std::locale("en_US.utf8"));
                #endif
                
                if(sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
                    std::cerr << "Error opening database\n";
                    return;
                }
                
                const char* sql = "SELECT lang, length, word FROM words;";
                if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
                    std::cerr << "Error preparing statement\n";
                    sqlite3_close(db);
                    return;
                }
                
                #ifdef _WIN32
                std::wcout.imbue(std::locale("en_US.utf8"));
                std::wcout << std::left 
                << std::setw(6) << L"Lang" 
                << std::setw(8) << L"Length" 
                << L"Word" << std::endl;
                #else
                std::cout << std::left 
                << std::setw(6) << "Lang" 
                << std::setw(8) << "Length" 
                << "Word" << std::endl;
                #endif
                
                int rc;
                while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                    const char* lang = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                    int length = sqlite3_column_int(stmt, 1);
                    const char* word = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                    
                    #ifdef _WIN32
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                    std::wstring wlang = converter.from_bytes(lang);
                    std::wstring wword = converter.from_bytes(word);
                    std::wcout << std::left 
                    << std::setw(6) << wlang 
                    << std::setw(8) << length 
                    << wword << std::endl;
                    #else
                    std::cout << std::left 
                    << std::setw(6) << lang 
                    << std::setw(8) << length 
                    << word << std::endl;
                    #endif
                }
                
                sqlite3_finalize(stmt);
                sqlite3_close(db);
            }
*/

std::string get_random_word(const std::string& db_path, 
                           int target_length, 
                           const std::string& lang) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::string result;

    // Открываем базу данных
    if(sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
        return "";
    }

    // Подготавливаем SQL-запрос
    const char* sql = 
        "SELECT word FROM words "
        "WHERE length = ? AND lang = ? "
        "ORDER BY RANDOM() LIMIT 1;";

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return "";
    }

    // Биндим параметры
    sqlite3_bind_int(stmt, 1, target_length);
    sqlite3_bind_text(stmt, 2, lang.c_str(), -1, SQLITE_STATIC);

    // Выполняем запрос
    if(sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* word = sqlite3_column_text(stmt, 0);
        if(word) {
            result = reinterpret_cast<const char*>(word);
        }
    }

    // Освобождаем ресурсы
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return result;
}

/*================================================================*/
/*===================== Шифр вижинера ======================*/
/*================================================================*/
            
std::string define_language(std::wstring text)
{
    bool has_ru = false, has_en = false;
              
    for (wchar_t ch : text) {
        if (static_cast<uint16_t>(ch) >= 1040 && static_cast<uint16_t>(ch) <= 1071) {
            has_ru = true;
        } else if (static_cast<uint16_t>(ch) >= 65 && static_cast<uint16_t>(ch) <= 90) {
            has_en = true;
        }
        if (has_ru && has_en) {
            throw InvalidOpenText("Ошибка: текст содержит несколько языков.");
        }
    }

    if (has_ru) return "ru";
    if (has_en) return "en";
    throw InvalidOpenText("Ошибка: неверный язык в тексте.");
}

wchar_t put_viginer_on_char(const wchar_t& openTextChar, const wchar_t& keyChar, const std::string& lang)
{
    wchar_t cipherTextChar;
    if (lang == "ru") {
        uint16_t buff = static_cast<uint16_t>(openTextChar) - 1040 + static_cast<uint16_t>(keyChar);
        if (buff > 1071) { 
            buff -= 32;
        }
        cipherTextChar = static_cast<wchar_t>(buff);
    }
    else if (lang == "en") {
        int16_t buff = static_cast<uint16_t>(openTextChar) - 65 + static_cast<uint16_t>(keyChar);
        if (buff > 90) { buff -= 26;};
        cipherTextChar = static_cast<wchar_t>(buff);
    }
    return cipherTextChar;
}

std::wstring put_viginer_on_text(const std::wstring& openText, const std::wstring& key)
{
    size_t openTextLen = openText.length();
    size_t keyLen = key.length();
    std::wstring cipherText;
    size_t keyCycle = 0;
    std::string lang = define_language(openText);
    for (size_t i = 0; i < openTextLen; ++i) {
        cipherText += put_viginer_on_char(openText[i], key[keyCycle], lang);
        ++keyCycle;
        if (keyCycle >= keyLen) {
            keyCycle -= keyLen;
        }
    }
    return cipherText;
}

std::map<std::wstring, std::wstring> encript(std::vector<std::wstring> openTexts, std::vector<std::wstring> keys)
{
    std::locale::global(std::locale("ru_RU.UTF-8"));
    std::wcout.imbue(std::locale());   
    if (keys.size() < openTexts.size()) {
        throw InvalidKey("Количество ключей должно быть как минимум равно количеству открытых текстов...");
    }

    std::map<std::wstring, std::wstring> keysAndCiphersTexts;

    for (size_t i = 0; i < keys.size(); ++i) {
        keysAndCiphersTexts[keys[i]] = put_viginer_on_text(openTexts[i], keys[i]);
    }

    return keysAndCiphersTexts;
}

wchar_t put_viginer_off_char(const wchar_t& cipherTextChar, const wchar_t& keyChar, const std::string& lang)
{
    wchar_t openTextChar;
    if (lang == "ru") {
        uint16_t buff = static_cast<uint16_t>(cipherTextChar) + 1040 - static_cast<uint16_t>(keyChar);
        if (buff < 1040) { 
            buff += 32;
        }
        openTextChar = static_cast<wchar_t>(buff);
    }
    else if (lang == "en") {
        int16_t buff = static_cast<uint16_t>(cipherTextChar) + 65 - static_cast<uint16_t>(keyChar);
        if (buff < 65) { buff += 26;};
        openTextChar = static_cast<wchar_t>(buff);
    }
    return openTextChar;
}

std::wstring put_viginer_off_text(const std::wstring& cipherText, const std::wstring& key)
{
    size_t cipherTextLen = cipherText.length();
    size_t keyLen = key.length();
    std::wstring openText;
    size_t keyCycle = 0;
    std::string lang = define_language(cipherText);
    for (size_t i = 0; i < cipherTextLen; ++i) {
        openText += put_viginer_off_char(cipherText[i], key[keyCycle], lang);
        ++keyCycle;
        if (keyCycle >= keyLen) {
            keyCycle -= keyLen;
        }
    }
    return openText;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    for(auto& keyAndCipherText : keysAndCipherTexts) {
        std::wstring key = keyAndCipherText.first;
        std::wstring cipherText = keyAndCipherText.second;  
        
        keysAndOpenTexts[key] = put_viginer_off_text(cipherText, key);

    }
    for (auto i : keysAndOpenTexts) {
        std::wcout << i.first << '\n' << i.second << '\n' << '\n';
    }
    
    return keysAndOpenTexts;
}

std::vector<std::string> gen_keys(std::string keyPropertys, size_t count)
{
    nlohmann::json prop;
    try{
        std::replace(keyPropertys.begin(), keyPropertys.end(), '\'', '\"');
        std::cout << keyPropertys << std::endl;
        prop = nlohmann::json::parse(keyPropertys);
        chekRequest(prop);
        std::vector<std::string> keys;
        
        std::string viginerPath = prop["viginer_path_to_dir"];
        size_t keyLen = prop["key_length"];
        std::string lang = prop["text_language"];

        std::string dbPath = viginerPath;
        dbPath.push_back('\\');
        dbPath.append("words");
        dbPath.append(".db");

        //write_words_fast(viginerPath + "\\ru.txt", viginerPath + "\\en.txt", dbPath);
        
        //print_entire_database(dbPath);

        for (size_t i = 0; i < count; ++i) {
            keys.push_back(get_random_word(dbPath, keyLen, lang));
        }

        return keys;
    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }
}

std::string get_key_propertys()
{
    // сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"params": [{"name": "key_length", "min": 2, "max": null, "value": 0, "type": "number", "default": 0, "label": "Длина ключа"}]})");
    
    return keyProp.dump();
}

void chekRequest(nlohmann::json keyPropertys)
{
    try {
        if(!keyPropertys.at("text_language").is_string()) {
            throw KeyPropertyError("Ключ text_language должен иметь строковое значение...");
        }
        if(keyPropertys["text_language"] != "ru" && keyPropertys["text_language"] != "en") {
            throw InvalidKey("Значение \"Язык текста\" должно быть ru или en...");
        }
        if(!keyPropertys.at("key_length").is_number()) {
            throw KeyPropertyError("Ключ \"Длина ключа\" должен иметь числовое значение...");
        }
        if(keyPropertys.at("key_length") <= 1) {
            throw InvalidKey("Значение \"Длина ключа\" должно быть больше 1...");
        }
        
    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}
