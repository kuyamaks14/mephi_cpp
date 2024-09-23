#include "../headers/dialog.h"
#include "../headers/converter.h"
#include <iostream>
#include <sstream>

std::array<std::string, 5> msgs = {"0. Quit",
                                   "1. Integer array to XML string (C++)",
                                   "2. Integer array to XML string (C)",
                                   "3. XML string to integer array (C++)",
                                   "4. XML string to integer array (C)"};

std::array<void(*)(), 5> dialog_options = {nullptr,
                                           dialog_arr2xml_cpp,
                                           dialog_arr2xml_c,
                                           dialog_xml2arr_cpp,
                                           dialog_xml2arr_c};

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

void dialog_xml2arr_cpp() {
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

void dialog_xml2arr_c() {
    std::cout << "Enter XML string:" << std::endl;
    std::string xml = get_str();
    char *c_xml = new char[xml.size() + 1]{};
    std::copy(xml.begin(), xml.end(), c_xml);

    try {
        validate_xml_format(c_xml);
        std::vector<int> arr = xml_to_array(xml);

        std::cout << "The result:" << std::endl;
        print_array(arr);
    } catch (const std::logic_error &e) {
        std::cout << "Error happened: " << e.what() << std::endl;
    }

    delete[] c_xml;
}

void dialog_arr2xml_cpp() {
    std::cout << "Enter an integer array (e.g., \"1 2 3\"):" << std::endl;
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

void dialog_arr2xml_c() {
    std::cout << "Enter an integer array (e.g., \"1 2 3\"):" << std::endl;
    std::string arr_as_str = get_str(true);
    char *c_arr_as_str = new char[arr_as_str.size() + 1]{};
    std::copy(arr_as_str.begin(), arr_as_str.end(), c_arr_as_str);

    char *c_arr_name = nullptr;
    char *c_elem_tag_name = nullptr;

    try {
        std::vector<int> arr = str_to_arr(c_arr_as_str);

        std::cout << "Enter the array's name:" << std::endl;
        std::string arr_name = get_str();
        c_arr_name = new char[arr_name.size() + 1]{};
        std::copy(arr_name.begin(), arr_name.end(), c_arr_name);

        try {
            std::cout << "Enter an element tag name:" << std::endl;
            std::string elem_tag_name = get_str();
            c_elem_tag_name = new char[elem_tag_name.size() + 1]{};
            std::copy(elem_tag_name.begin(), elem_tag_name.end(), c_elem_tag_name);
        } catch (const std::bad_alloc &e) {
            delete[] c_arr_name;
            throw e;
        }

        char *xml = array_to_xml(arr, c_arr_name, c_elem_tag_name);
        std::cout << "The result:\n" << xml << std::endl;
    } catch (const std::logic_error &e) {
        std::cout << "Error happened: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        delete[] c_arr_as_str;
        throw e;
    }

    delete[] c_arr_as_str;
    delete[] c_arr_name;
    delete[] c_elem_tag_name;
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

std::vector<int> str_to_arr(const char *str) {
    std::vector<int> arr;

    char *cur_num_as_str = new char[1]{};
    char *tmp_cur_num = nullptr;
    int cur_num_len = 0;
    int digits_amount = 0;

    char cur_char;
    char prev_char = -1;

    size_t i = 0;
    while (prev_char != '\0') {
        cur_char = str[i];
        if (std::isdigit(cur_char) || (cur_char == '-' && prev_char != '-')) {
            try {
                tmp_cur_num = new char[cur_num_len + 2]{};
            } catch (const std::bad_alloc &e) {
                delete[] cur_num_as_str;
                throw e;
            }

            if (cur_num_as_str == nullptr)
                cur_num_as_str = new char[1]{};

            std::copy(cur_num_as_str, cur_num_as_str + cur_num_len, tmp_cur_num);
            tmp_cur_num[cur_num_len] = cur_char;
            cur_num_len++;
            delete[] cur_num_as_str;
            cur_num_as_str = tmp_cur_num;
            tmp_cur_num = nullptr;

            if (cur_char != '-') {
                digits_amount++;
            }
        } else if (cur_char == ' ' || cur_char == '\0') {
            if (digits_amount) {
                int number = std::stoi(cur_num_as_str);
                arr.push_back(number);

                delete[] cur_num_as_str;
                cur_num_as_str = nullptr;
                cur_num_len = 0;
                digits_amount = 0;
            }
        } else {
            delete[] cur_num_as_str;
            throw std::logic_error("You have entered an incorrect array!");
        }
        prev_char = cur_char;
        i++;
    }

    delete[] tmp_cur_num;
    delete[] cur_num_as_str;

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
