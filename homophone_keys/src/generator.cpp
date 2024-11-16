#include "generator.hpp"

bool check_language(std::string& language)
{
    for(size_t i = 0; i < language.length(); ++i)
    {
        language[i] = std::tolower(language[i]);
    }
    return language == "ru" || language == "eng";
}

bool check_stream()
{
    if (std::cin.fail()) 
    {
	    std::cin.clear();
	    std::cin.ignore(32767, '\n');
	    return false;
    }
    return true;
}

int64_t get_positive_number()
{
    int64_t number = 0;
    std::cin >> number;
    if (!check_stream()) 
    {
        throw std::logic_error("Entered symbol instead of a number");
    }
    if (number <= 0) 
    {
        throw std::logic_error("Entered value is not positive");
    }
    return number;
}

int64_t get_random_number(const int64_t& leftBoarder, const int64_t& rightBoarder)
{
    return (rand() % (rightBoarder - leftBoarder + 1) + leftBoarder);
}

bool get_homophones_amount()
{
    std::cout << "How many homophones do you need available?\n";
    std::cout << "1) Standard quantity; (01 - 99)\n";
    std::cout << "2) Enter your own borders\n";
    std::cout << "Choose one of the presented options: ";
    int64_t choice = get_positive_number();
    switch (choice)
    {
    case 1:
        return true;
        break;
    case 2:
        return false;
        break;
    default:
        throw std::out_of_range("Wrong number input. Must be 1 or 2");
        break;
    }
}

void get_boarders(int64_t& leftBoarder, int64_t& rightBoarder, const std::string& language)
{
    if(get_homophones_amount())
    {
        leftBoarder = 1;
        rightBoarder = 99;
    }
    else
    {
        std::cout << "Enter the minimum key value: ";
        leftBoarder = get_positive_number();
        std::cout << "Enter the maximum value of the key: ";
        rightBoarder = get_positive_number();
        if (rightBoarder < leftBoarder)
        {
            std::swap(leftBoarder, rightBoarder);
        }
        if ((language == "ru" && (rightBoarder - (leftBoarder - 1)) < 33) || (language == "eng" && (rightBoarder - (leftBoarder - 1)) < 26))
        {
            throw std::runtime_error("There cannot be so many homophones for the selected language, there is an error with borders");
        }
    }
}

std::map<char, double> get_frequencies(const std::string& text)
{
    std::map<char, size_t> lettersCounter;
    int64_t totalLetters = 0;
	for (char letter : text) {
		++lettersCounter[letter];
		++totalLetters;
	}

    std::map<char, double> lettersFrequencies;
    for (const auto& element : lettersCounter) {
	    lettersFrequencies[element.first] = static_cast<double>(element.second) / totalLetters;
    }
    return lettersFrequencies;
}

std::map<char, std::vector<int64_t>> generate_keys
(
    const int64_t& leftBoarder, 
    const int64_t& rightBoarder, 
    const std::string& text
)
{
    int64_t totalKeysNumber = rightBoarder - (leftBoarder - 1);
    std::map<char, double> letterFrequencies = get_frequencies(text);
    std::map<char, std::vector<int64_t>> keysContainer;
    for (const auto& letter : letterFrequencies) 
    {
	    std::vector<int64_t> allKeys;
        double number = (letter.second * totalKeysNumber);
	    size_t keysNumber = static_cast<size_t>(std::ceil(number));
	    std::vector<int64_t> keys;
	    for (size_t i = 0; i < keysNumber; ++i) 
        {
		    int64_t key = 0;
		    do 
            {
			    key = get_random_number(leftBoarder, rightBoarder);
		    } while (std::find(keys.begin(), keys.end(), key) != keys.end() && std::find(allKeys.begin(), allKeys.end(), key) != allKeys.end());
		keys.push_back(key);
		allKeys.push_back(key);
	    }
	    keysContainer[letter.first] = keys;
    }
    return keysContainer;
}

void print_keys_info(const std::map<char, std::vector<int64_t>>& container)
{
    for (const auto& element : container) 
    {
	std::cout << "Letter: " << element.first << '\t';
	std::cout << "Keys for the letter: ";
	for (size_t number : element.second) {
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << number << " ";
	}
	std::cout << '\n';
    }
}