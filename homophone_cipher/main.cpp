#include <iostream>
#include <ctime>
#include <stdexcept>
#include <Windows.h>
#include <fstream>
#include <string>
#include <cctype>
#include <typeinfo>
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>


void check_file(std::ifstream& file)
{
	if (!file.good()) {
		throw std::runtime_error("File does not open");
	}
	if (!file) {
		throw std::runtime_error("File is not exist");
	}
	if (file.peek() == EOF) {
		throw std::runtime_error("File is empty");
	}
}

std::map<char, double> get_frequencies(std::ifstream& file)
{
	std::map<char, size_t> lettersCounter;
	int64_t totalLetters = 0;
	std::string line;
	while (file >> line) {
		for (char letter : line) {
			++lettersCounter[std::tolower(letter)];
			++totalLetters;
		}
	}

	std::map<char, double> lettersFrequencies;
	for (const auto& letter : lettersCounter) {
		lettersFrequencies[letter.first] = static_cast<double>(letter.second) / totalLetters;
	}
	return lettersFrequencies;
}

int32_t get_random_number(const int32_t& leftBound, const int32_t& rightBound)
{
	return (rand() % (rightBound - leftBound + 1) + leftBound);
}

bool check_stream()
{
	if (std::cin.fail()) {
		std::cin.clear();
		std::cin.ignore(32767, '\n');
		return false;
	}
	return true;
}

std::map<char, std::vector<int32_t>> generate_keys
(
	const std::map<char, double>& lettersFrequencies,
	const int64_t& totalKeysNumber,
	const int64_t& leftBound,
	const int64_t& rightBound
)
{
	std::map<char, std::vector<int32_t>> keysContainer;
	for (const auto& letter : lettersFrequencies) {
		std::vector<int32_t> allKeys;
		double number = (letter.second * totalKeysNumber);
		size_t keysNumber = std::ceil(number);
		std::vector<int32_t> keys;
		for (size_t i = 0; i < keysNumber; ++i) {
			int32_t key = 0;
			do {
				key = get_random_number(leftBound, rightBound);
			} while (std::find(keys.begin(), keys.end(), key) != keys.end() && std::find(allKeys.begin(), allKeys.end(), key) != allKeys.end());
			keys.push_back(key);
			allKeys.push_back(key);
		}
		keysContainer[letter.first] = keys;
	}
	return keysContainer;
}

int64_t get_positive_number()
{
	int64_t number = 0;
	std::cin >> number;
	while (!check_stream()) {
		std::cout << "Неправильный ввод числа; повторите ввод: ";
		std::cin >> number;
	}
	while (number <= 0) {
		std::cout << "Неправильное значение! Повторите ввод: ";
		std::cin >> number;
		if (!check_stream()) {
			continue;
		}
	}
	return number;
}

bool check_langChoice(std::string langChoice)
{
	std::string checkStream = langChoice;
	for (size_t i = 0; i < langChoice.size(); ++i) {
		checkStream[i] = std::tolower(langChoice[i]);
	}
	if (checkStream == "ru" || checkStream == "eng") {
		return true;
	}
	else {
		return false;
	}
}

void print_keys_info(const std::map<char, std::vector<int32_t>>& keysContainer)
{
	for (const auto& element : keysContainer) {
		std::cout << "Буква: " << element.first << "; ";
		std::cout << "Ключи для буквы: ";
		for (size_t number : element.second) {
			std::cout.width(2);
			std::cout.fill('0');
			std::cout << number << " ";
		}
		std::cout << '\n';
	}
}

int main()
{
	try
	{
		srand(time(0));
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
		std::cout << "Для начала работы выберите, на каком языке будет работа\nВыберите язык: Ru/Eng: ";
		std::string langChoice = "ru";
		std::cin >> langChoice;
		while (!check_langChoice(langChoice)) {
			std::cout << "Неправильный ввод языка; повторите ввод: ";
			std::cin >> langChoice;
		}

		std::cout << "Какое количество доступных омофонов Вам необходимо?";
		std::cout << "\n1) Стандартное количество (01 - 99)\n2) Ввести собственные границы\nВыберите один из предствленных вариантов: ";
		size_t amountChoice = 1;
		std::cin >> amountChoice;
		while (!check_stream()) {
			std::cout << "Неправильный ввод выбора; повторите ввод: ";
			std::cin >> amountChoice;
		}

		int64_t homophonesAmount = 0;
		int64_t leftBound = 0, rightBound = 0;
		switch (amountChoice)
		{
		case 1:
			homophonesAmount = 99;
			leftBound = 1;
			rightBound = 99;
			break;
		case 2:
			std::cout << "Введите минимальное значение ключа: ";
			leftBound = get_positive_number();
			std::cout << "Введите максимальное значение ключа: ";
			rightBound = get_positive_number();
			homophonesAmount = rightBound - leftBound - 1;
			break;
		default:
			std::cout << "Для данной цифры невозможна опция выбора";
			break;
		}

		std::ifstream file("test.txt");
		check_file(file);
		std::map<char, double> frequencies = get_frequencies(file);
		std::map<char, std::vector<int32_t>> keysContainer = generate_keys(frequencies, homophonesAmount, leftBound, rightBound);
		print_keys_info(keysContainer);
	}
	catch (std::exception& err) {
		std::cerr << "Exception name: " << err.what() << '\n';
		std::cerr << "Type of exception: " << typeid(err).name() << '\n';
	}

	return 0;
}