#include "homophone_generator.hpp"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cmath>

int64_t get_random_number(const int64_t& leftBoarder, const int64_t& rightBoarder, HMAC_DRBG &gen) {
    if (leftBoarder < 0 || rightBoarder < 0) {
        throw std::invalid_argument("Неверное значение: граница должна быть натуральным числом.");
    }
    if (leftBoarder > rightBoarder) {
        throw std::invalid_argument("Неверный диапазон: левая граница больше правой.");
    }
    if (gen.HMAC_DRBG_Ressed_Check()) {
        gen.HMAC_DRBG_Ressed(get_entropy());
    }
    uint64_t random_value = convert_bytes_to_ddword(gen.HMAC_DRBG_Generate_algorithm(8).value());
    return leftBoarder + (random_value % (rightBoarder - leftBoarder + 1));
}


std::map<wchar_t, double> get_frequencies(const std::string& language) {

    std::map<wchar_t, double> lettersFrequencies;
    if (language == "ru")
    {
        lettersFrequencies[L'\u0410'] = 0.08320511;
        lettersFrequencies[L'\u0411'] = 0.01760521;
        lettersFrequencies[L'\u0412'] = 0.04706897;
        lettersFrequencies[L'\u0413'] = 0.01818968;
        lettersFrequencies[L'\u0414'] = 0.02977133;
        lettersFrequencies[L'\u0415'] = 0.08782550;
        lettersFrequencies[L'\u0416'] = 0.01149035;
        lettersFrequencies[L'\u0417'] = 0.01640110;
        lettersFrequencies[L'\u0418'] = 0.06602123;
        lettersFrequencies[L'\u0419'] = 0.01029147;
        lettersFrequencies[L'\u041A'] = 0.03431464;
        lettersFrequencies[L'\u041B'] = 0.05054885;
        lettersFrequencies[L'\u041C'] = 0.02869818;
        lettersFrequencies[L'\u041D'] = 0.06934096;
        lettersFrequencies[L'\u041E'] = 0.11531208;
        lettersFrequencies[L'\u041F'] = 0.02414440;
        lettersFrequencies[L'\u0420'] = 0.03939676;
        lettersFrequencies[L'\u0421'] = 0.05278346;
        lettersFrequencies[L'\u0422'] = 0.05993854;
        lettersFrequencies[L'\u0423'] = 0.02728527;
        lettersFrequencies[L'\u0424'] = 0.00100730;
        lettersFrequencies[L'\u0425'] = 0.00766023;
        lettersFrequencies[L'\u0426'] = 0.00259607;
        lettersFrequencies[L'\u0427'] = 0.01707986;
        lettersFrequencies[L'\u0428'] = 0.00870420;
        lettersFrequencies[L'\u0429'] = 0.00284228;
        lettersFrequencies[L'\u042A'] = 0.00029560;
        lettersFrequencies[L'\u042B'] = 0.01854366;
        lettersFrequencies[L'\u042C'] = 0.02004712;
        lettersFrequencies[L'\u042D'] = 0.00356594;
        lettersFrequencies[L'\u042E'] = 0.00629896;
        lettersFrequencies[L'\u042F'] = 0.02172569;
    }
    else if (language == "en")
    {
        lettersFrequencies[L'\u0041'] = 0.08156168;
        lettersFrequencies[L'\u0042'] = 0.01455422;
        lettersFrequencies[L'\u0043'] = 0.02470439;
        lettersFrequencies[L'\u0044'] = 0.04312160;
        lettersFrequencies[L'\u0045'] = 0.12329048;
        lettersFrequencies[L'\u0046'] = 0.02088630;
        lettersFrequencies[L'\u0047'] = 0.01819378;
        lettersFrequencies[L'\u0048'] = 0.06684792;
        lettersFrequencies[L'\u0049'] = 0.06910234;
        lettersFrequencies[L'\u004A'] = 0.00101750;
        lettersFrequencies[L'\u004B'] = 0.00803485;
        lettersFrequencies[L'\u004C'] = 0.03963282;
        lettersFrequencies[L'\u004D'] = 0.02763166;
        lettersFrequencies[L'\u004E'] = 0.06624114;
        lettersFrequencies[L'\u004F'] = 0.07813534;
        lettersFrequencies[L'\u0050'] = 0.01562356;
        lettersFrequencies[L'\u0051'] = 0.00087073;
        lettersFrequencies[L'\u0052'] = 0.05714864;
        lettersFrequencies[L'\u0053'] = 0.06293236;
        lettersFrequencies[L'\u0054'] = 0.09035117;
        lettersFrequencies[L'\u0055'] = 0.03030702;
        lettersFrequencies[L'\u0056'] = 0.01020895;
        lettersFrequencies[L'\u0057'] = 0.02651194;
        lettersFrequencies[L'\u0058'] = 0.00151730;
        lettersFrequencies[L'\u0059'] = 0.02111156;
        lettersFrequencies[L'\u005A'] = 0.00046074;    
    }
    else throw std::invalid_argument("Неверное значение языка: язык должен быть «ru» или «en».");
    return lettersFrequencies;
}


// Генерирует ключи для шифрования на основе диапазона чисел и языка.
// Каждой букве алфавита сопоставляется список уникальных ключей.
// Если диапазон чисел недостаточен для покрытия всех букв, выбрасывает исключение runtime_error.
// Добавляет ведущие нули к числам для корректного отображения (например, 5 -> "05").
std::map<wchar_t, std::vector<std::wstring>> generate_keys(
    int64_t leftBoarder,
    int64_t rightBoarder,
    std::string& language,
    HMAC_DRBG &gen
) {
     if (leftBoarder < 0 || rightBoarder < 0) {
        throw std::invalid_argument("Неверное значение: граница должна быть натуральным числом.");
    }
    if (language.empty()) {
        throw std::invalid_argument("Значение языка пусто. Невозможно вычислить частоты.");
    }
    if (leftBoarder > rightBoarder) {
        throw std::invalid_argument("Недопустимый диапазон ключей: левая граница больше правой.");
    }

    int64_t maxDigits = std::to_wstring(rightBoarder).length();
    std::map<wchar_t, double> letterFrequencies = get_frequencies(language);
    std::map<wchar_t, std::vector<std::wstring>> keysContainer;
    std::vector<int64_t> availableKeys;
    
    for (int64_t i = leftBoarder; i <= rightBoarder; ++i) {
        availableKeys.push_back(i);
    }

    auto format_number = [&](int64_t num) -> std::wstring {
        std::wstringstream ss;
        ss << std::setw(maxDigits) << std::setfill(L'0') << num;
        return ss.str();
    };

    double totalFrequency = 0;
    for (const auto& [letter, freq] : letterFrequencies) {
        totalFrequency += freq;
    }

    int64_t totalKeys = availableKeys.size();
    if (totalKeys < letterFrequencies.size()) {
        throw std::runtime_error("Недостаточно ключей, чтобы раздать хотя бы один на письмо.");
    }

    std::map<wchar_t, int> requiredKeysPerLetter;
    int totalAssigned = 0;

    // 1. Гарантируем **минимум 1 ключ** каждой букве
    for (const auto& [letter, freq] : letterFrequencies) {
        requiredKeysPerLetter[letter] = 1;
        totalAssigned++;
    }

    // 2. Распределяем **оставшиеся ключи пропорционально частоте**, но с `ceil()`
    for (const auto& [letter, freq] : letterFrequencies) {
        if (totalAssigned >= totalKeys) break;

        int extraKeys = std::ceil(freq * (totalKeys - letterFrequencies.size()) / totalFrequency);
        requiredKeysPerLetter[letter] += extraKeys;
        totalAssigned += extraKeys;
    }

    // 3. Если раздали слишком много ключей, уменьшаем у букв с **максимальным количеством**
    while (totalAssigned > totalKeys) {
        for (auto& [letter, count] : requiredKeysPerLetter) {
            if (count > 1) {
                count--;
                totalAssigned--;
                if (totalAssigned == totalKeys) break;
            }
        }
    }

    // 4. Назначаем ключи буквам
    for (const auto& [letter, keysCount] : requiredKeysPerLetter) {
        for (int j = 0; j < keysCount && !availableKeys.empty(); ++j) {
            size_t index = get_random_number(0, availableKeys.size() - 1, gen);
            int64_t key = availableKeys[index];
            keysContainer[letter].push_back(format_number(key));
            availableKeys.erase(availableKeys.begin() + index);
        }
    }

    // 5. Раздаём оставшиеся ключи случайным буквам
    while (!availableKeys.empty()) {
        for (auto& [letter, keys] : keysContainer) {
            if (availableKeys.empty()) break;
            size_t index = get_random_number(0, availableKeys.size() - 1, gen);
            int64_t key = availableKeys[index];
            keys.push_back(format_number(key));
            availableKeys.erase(availableKeys.begin() + index);
        }
    }
    return keysContainer;
}

