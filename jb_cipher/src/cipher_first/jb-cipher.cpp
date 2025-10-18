#include "jb-cipher.hpp"
#include <random>

/*================================================================*/
/*===================== Шифр Джеферсона-Базери ======================*/
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

std::map<wchar_t, wchar_t> get_alfabet_substitution(Permutation& permut, Permutation languagePermut)
{
    std::wstring ru_alfabet;
    std::wstring en_alfabet;

    for (size_t i = 1040; i < 1072; ++i){
        ru_alfabet.push_back(static_cast<wchar_t>(i));
    }
    
    for (size_t i = 65; i < 91; ++i){
        en_alfabet.push_back(static_cast<wchar_t>(i));
    }

    std::map<wchar_t, wchar_t> alfabetSubstitution; 

    if(languagePermut.size() == 26 && permut.size() == en_alfabet.size()) {
        std::wstring temp = en_alfabet;
        languagePermut.apply(temp);
        languagePermut.apply(en_alfabet);
        permut.apply(temp);
        for (size_t i = 0; i < en_alfabet.size(); ++i) {
            alfabetSubstitution[en_alfabet[i]] = temp[i];
        }

        for(auto para: alfabetSubstitution) {
            std::wcout << para.first << L"=" << para.second << L' ';
        }

        std::wcout << std::endl;

        return alfabetSubstitution;
    }
    
    if(languagePermut.size() == 32 && permut.size() == ru_alfabet.size()) {
        std::wstring temp = ru_alfabet;
        languagePermut.apply(temp);
        languagePermut.apply(ru_alfabet);
        permut.apply(temp);
        for (size_t i = 0; i < permut.size(); ++i) {
            alfabetSubstitution[ru_alfabet[i]] = temp[i];
        }

        for(auto para: alfabetSubstitution) {
            std::wcout << para.first << L"=" << para.second << L' ';
        }

        std::wcout << std::endl;

        return alfabetSubstitution;
    }

    throw std::invalid_argument("Размер перестановки не был равен размеру алфавита...");
}

std::map<wchar_t, wchar_t> revert_substitution(std::map<wchar_t, wchar_t> sub) {
    std::map<wchar_t, wchar_t> res;
    for(const auto& para: sub) {
        res[para.second] = para.first;
    }
    return res;
}


std::vector<int32_t> get_right_cyclic_substitution(const std::vector<int32_t>& trivialPermut, uint32_t shift) {
    if (trivialPermut.empty()) return {};  

    std::vector<int32_t> res(trivialPermut.size());
    for (size_t i = 0; i < res.size(); ++i) {
        res[(i + shift) % res.size()] = trivialPermut[i];
    }
    return res;
}

std::vector<int32_t> get_left_cyclic_substitution(const std::vector<int32_t>& trivialPermut, uint32_t shift) {
    if (trivialPermut.empty()) return {};  

    std::vector<int32_t> res(trivialPermut.size());
    size_t size = res.size();
    shift %= size;  

    for (size_t i = 0; i < size; ++i) {
        res[(i + size - shift) % size] = trivialPermut[i];  
    }
    return res;
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
        std::wstringstream wss(keys[i]);
        std::uint32_t barShift = 0;
 
        std::wstring permutStr;

        if (!(wss >> barShift)) {
            throw InvalidKey("Неверный формат ключа: отсутствует значение сдвига.");
        }
        std::getline(wss, permutStr);
        Permutation key(permutStr);
        
        if(openTexts[i].size() % key.size() != 0) {
            throw InvalidOpenText("Размер открытого текста должен быть кратен количеству дисков...");
        }
        std::vector<int32_t> trivialPermut(((define_language(openTexts[i]) == "en") ? 26 : 32));

        for (size_t j = 0; j < trivialPermut.size(); ++j) {
            trivialPermut[j] = j + 1;
        }
        
        HMAC_DRBG gen(get_entropy(), get_entropy());
        Permutation cyclic_permut(get_right_cyclic_substitution(trivialPermut, barShift)); 
        std::wcout << cyclic_permut << std::endl;
        std::vector<std::map<wchar_t, wchar_t>> substitutionsOnDisks(key.size());
        std::vector<Permutation> trivialDisksPermutation(key.size()); 
        for (size_t j = 0; j < substitutionsOnDisks.size(); ++j) {
            trivialDisksPermutation[j] = Permutation(generat_permutation(trivialPermut, gen));
            substitutionsOnDisks[j] = get_alfabet_substitution(trivialDisksPermutation[j], cyclic_permut);
        }
        
        std::vector<std::map<wchar_t, wchar_t>> disksPermutation(substitutionsOnDisks.size());
        std::wcout << key << std::endl;
        for (int32_t j = 0; j < disksPermutation.size(); ++j) {
            std::cout << key(j) << " ";
            disksPermutation[key(j)] = substitutionsOnDisks[j]; 
        }
        std::cout << std::endl;

        std::wstring cipherText = openTexts[i];
        std::wcout << cipherText << L' ' << disksPermutation.size() << std::endl;
        for (size_t j = 0; j < cipherText.size(); ++j) {
            std::wcout << j % disksPermutation.size() << L' ';
            cipherText[j] = disksPermutation[j % disksPermutation.size()][cipherText[j]];
        }
        std::wcout << std::endl;

        wss.clear();

        wss << keys[i] << L"\n";
        for (size_t j = 0; j < trivialDisksPermutation.size(); ++j) {
            wss << trivialDisksPermutation[j] << L'\n';
        }
        
        std::wcout << cipherText << std::endl;

        keysAndCiphersTexts[wss.str()] = cipherText;
    }
    
    return keysAndCiphersTexts;
}

std::map<std::wstring, std::wstring> decript(std::map<std::wstring, std::wstring> keysAndCipherTexts)
{
    std::map<std::wstring, std::wstring> keysAndOpenTexts;
    for(auto& keyAndCipherText : keysAndCipherTexts) {
        std::wstring key = keyAndCipherText.first;
        std::wstring cipherText = keyAndCipherText.second;  

        std::wstringstream wss(key);

        std::wcout << key << std::endl;

        int32_t barShift = 0;
        wss >> barShift;

        std::cout << "barShift: " << barShift << std::endl;

        key.clear();
        std::wstring line;
        while (std::getline(wss >> std::ws, line)){
            key.append(line);
        }
    
        std::wcout << L"Key after get barShift: " << key << std::endl;
        std::wregex permutReg(LR"(\[\s*\d+(?:\s+\d+)*\])");


        std::wregex re(LR"(\[(.*?)\])");  // Регулярка для поиска массивов
        std::wsmatch match;               // Объект для хранения совпадений
        std::wstring::const_iterator searchStart(key.cbegin());

        std::vector<Permutation> trivialPermutationOfpermutationsOnDisks;
        while (std::regex_search(searchStart, key.cend(), match, re)) {
            trivialPermutationOfpermutationsOnDisks.push_back(Permutation(match[1]));
            searchStart = match.suffix().first;  // Продолжить поиск после найденного совпадения
        }

        std::wcout << "All permutation: " << trivialPermutationOfpermutationsOnDisks.size() << std::endl;

        Permutation keyPermutation = trivialPermutationOfpermutationsOnDisks[0];
        std::wcout << L"Key permutation: " << keyPermutation << std::endl;
        trivialPermutationOfpermutationsOnDisks.erase(trivialPermutationOfpermutationsOnDisks.begin());

        std::wcout << "Count of disks permutation: " << trivialPermutationOfpermutationsOnDisks.size() << std::endl;

        std::vector<Permutation> permutationOfpermutationsOnDisks(trivialPermutationOfpermutationsOnDisks.size());

        for (int32_t j = 0; j < permutationOfpermutationsOnDisks.size(); ++j) {
            permutationOfpermutationsOnDisks[keyPermutation(j)] = trivialPermutationOfpermutationsOnDisks[j];
        }
        
        std::vector<std::map<wchar_t, wchar_t>> permutationOfsubstitutionsOnDisks(permutationOfpermutationsOnDisks.size());

        std::vector<int32_t> trivialPermut(permutationOfpermutationsOnDisks[0].size());

        for (size_t j = 0; j < trivialPermut.size(); ++j) {
            trivialPermut[j] = j + 1;
        }

        Permutation cyclic_permutation(get_right_cyclic_substitution(trivialPermut, barShift));
        std::wcout << cyclic_permutation << std::endl;
        for (size_t j = 0; j < permutationOfsubstitutionsOnDisks.size(); ++j) {
            permutationOfsubstitutionsOnDisks[j] = revert_substitution(get_alfabet_substitution(permutationOfpermutationsOnDisks[j], cyclic_permutation));
        }
        
        for (size_t j = 0; j < cipherText.size(); ++j) {
            cipherText[j] = permutationOfsubstitutionsOnDisks[j % permutationOfsubstitutionsOnDisks.size()][cipherText[j]];
        }
        

        keysAndOpenTexts[keyAndCipherText.first] = cipherText;
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
        
        int32_t diskCount = prop["disk_count"];

        std::vector<int32_t> trivialPermut(diskCount);
        for (size_t i = 0; i < diskCount; ++i) {
            trivialPermut[i] = i + 1;
        }

        std::vector<uint8_t> entropy = get_entropy();
        std::vector<uint8_t> nonce = get_entropy();
        
        HMAC_DRBG gen(entropy, nonce, {'J', 'B', 'C', '-', 'c', 'i', 'p', 'h', 'e', 'r'});
        
        // Все ключи
        std::vector<std::vector<int32_t>> allKeys(count);
        for (size_t i = 0; i < count; ++i) {
            allKeys[i] = generat_permutation(trivialPermut, gen);
        }

        std::vector<std::string> res;
        for(auto key: allKeys) {
            if(gen.HMAC_DRBG_Ressed_Check()) {
                gen.HMAC_DRBG_Ressed(get_entropy());
            }
            std::stringstream ss;
            ss << convert_bytes_to_ddword(gen.HMAC_DRBG_Generate_algorithm(256).value()) %  (prop["text_language"] == "en" ? 27 : 33)<< " ";
            ss << "[";
            for (auto number: key) {
                ss << number << " ";
            }
            ss << "]";
            res.push_back(ss.str());
        }            
    
        return res;
    } catch(nlohmann::json::parse_error &err) {
        throw KeyPropertyError(err.what());
    }

}

std::string get_key_propertys()
{
    // сам шаблон как должен выглядеть .json запрос с параметрами
    nlohmann::json keyProp = nlohmann::json::parse(R"({"params": [{"name": "disk_count", "min": 1, "max": null, "value": 0, "type": "number", "default": 0, "label": "Количество дисков"}]})");
    
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
        if(!keyPropertys.at("disk_count").is_number()) {
            throw KeyPropertyError("Ключ \"Количество дисков\" должен иметь числовое значение...");
        }
        if(keyPropertys.at("disk_count") <= 0) {
            throw InvalidKey("Значение \"Количество дисков\" должно быть больше 0...");
        }   

    } catch (nlohmann::json::type_error &err) {
        throw KeyPropertyError(err.what());
    }
}

