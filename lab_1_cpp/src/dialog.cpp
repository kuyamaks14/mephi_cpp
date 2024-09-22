#include "../headers/dialog.h"
#include "../headers/converter.h"
#include <iostream>
#include <sstream>

std::array<std::string, 3> msgs = {"0. Quit",
                                   "1. Integer array to XML string",
                                   "2. XML string to integer array"};

std::array<void(*)(), 3> dialog_options = {nullptr,
                                           dialog_arr2xml,
                                           dialog_xml2arr};

/////////////////////////////////////

////      DIALOG FUNCTIONS

/////////////////////////////////////

int dialog(const std::array<std::string, msgs.size()> &messages) {
    std::string err_msg;
    int option_num;
    do {
        std::cout << err_msg << std::endl;
        err_msg = "You are wrong. Repeate, please!";

        for (const std::string &message : messages)
            std::cout << message << std::endl;

        std::cout << "Make your choice: --> " << std::endl;
        get_int(option_num);
    } while (option_num < 0 || option_num >= messages.size());
    return option_num;
}

void dialog_xml2arr() {
    std::cout << "Enter XML string:" << std::endl;
    std::string xml = get_str();

    try {
        validate_xml_format(xml);
        std::vector<int> arr = xml_to_array(xml);

        std::cout << "The result:" << std::endl;
        print_array(arr);
    } catch (const std::logic_error &e) {
        std::cout << "Error happened: " << e.what() << std::endl;
    }
}

void dialog_arr2xml() {
    puts("Enter an integer array (e.g., \"1 2 3\"):");
    std::string arr_as_str = get_str(true);

    try {
        std::vector<int> arr = str_to_arr(arr_as_str);

        std::cout << "Enter the array's name:" << std::endl;
        std::string arr_name = get_str();

        std::cout << "Enter an element tag name:" << std::endl;
        std::string elem_tag_name = get_str();

        std::string xml = array_to_xml(arr, arr_name, elem_tag_name);

        std::cout << "The result:\n" << xml << std::endl;

    } catch (const std::logic_error &e) {
        std::cout << "Error happened: " << e.what() << std::endl;
    }
}

///////////////////////////////////

////       AUXILIARY TOOLS

///////////////////////////////////

void get_int(int &num) {
    while (!(std::cin >> num)) {
        if (std::cin.eof())
            throw std::runtime_error("EOF");
        if (std::cin.bad())
            throw std::runtime_error("Broken input");
        std::cin.clear();
        std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n');
        std::cout << "Invalid input. Try again: ";
    }
}

std::string get_str(bool do_trim) {
    std::string str;
    while (str.empty()) {
        std::getline(std::cin, str);

        if (do_trim)
            str = trim(str);

        if (std::cin.eof())
            throw std::runtime_error("EOF");
        if (std::cin.bad())
            throw std::runtime_error("Broken input");
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n');
        }
    }

    return str;
}

std::vector<int> str_to_arr(const std::string &str) {
    std::vector<int> arr;
    std::istringstream ss(str);
    int num;

    while (ss >> num) {  // EOF in the end
        arr.push_back(num);
    }

    if (ss.bad())
        throw std::runtime_error("Broken input");
    if (ss.fail() && !ss.eof()) {
        throw std::logic_error("Incorrect array");
    }

    return arr;
}

std::string trim(const std::string &str) {
    auto first = str.find_first_not_of(" \t");
    auto last = str.find_last_not_of(" \t");

    if (first == std::string::npos)
        return "";
    return str.substr(first, last - first + 1);
}

void print_array(const std::vector<int> &arr) {
    if (arr.empty()) {
        std::cout << "Empty array";
    } else {
        for (size_t i = 0; i < arr.size(); i++) {
            std::cout << arr[i];
            if (i < arr.size() - 1)
                std::cout << ", ";
        }
    }
    std::cout << std::endl;
}
