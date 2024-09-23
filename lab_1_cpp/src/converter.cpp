#include "../headers/converter.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <regex>
#include <algorithm>

///////////////////////////////////

////      AUXILIARY TOOLS

///////////////////////////////////

void reset_ostringstream(std::ostringstream &oss) {
    oss.str("");
    oss.clear();
}

void validate_xml_format(const std::string& xml) {
    std::regex xml_regex(R"(^<(\w+)>(<(\w+)>(-?\d+)</\3>)*</\1>$)");

    if (!std::regex_match(xml, xml_regex)) {
        throw std::invalid_argument("The XML string does not match the expected format.");
    }
}

void validate_xml_format(const char *xml) {
    std::cout << "AAA" << std::endl;
    std::string xml_string(xml);
    validate_xml_format(xml_string);
}

int get_cstring_length(const char *str) {
    int i = 0;
    for (; str[i] != '\0'; i++);
    return i;
}

size_t get_num_len(int num) {
    size_t len = 1;

    if (num < 0) {
        len++;
        num = -num;
    }

    while ((num = num / 10) > 0) {
        len++;
    }

    return len;
}

///////////////////////////////////

//// CONVERTERS (arr2xml, xml2arr)

///////////////////////////////////

std::string array_to_xml(const std::vector<int> &arr, const std::string &arr_name, const std::string &elem_tag) {
    std::ostringstream oss;

    if (arr.empty()) {
        oss << "<" << arr_name << "></" << arr_name << ">";
        return oss.str();
    }

    size_t xml_min_len = 2 * (arr_name.length() + arr.size() * elem_tag.length()) + 5 * (arr.size() + 1) + 1;

    // Create the beginning of an XML-string
    oss << "<" << arr_name << ">";
    std::string xml = oss.str();
    xml.reserve(xml_min_len);
    reset_ostringstream(oss);

    // Create the tags for the XML-string
    for (int num : arr) {
        oss << "<" << elem_tag << ">" << num << "</" << elem_tag << ">";
        xml += oss.str();
        reset_ostringstream(oss);
    }

    // Create the tail of the XML-string
    oss << "</" << arr_name << ">";
    xml += oss.str();

    return xml;
}

char *array_to_xml(const std::vector<int> &arr, const char *arr_name, const char *elem_tag) {
    size_t elem_tag_len = std::strlen(elem_tag);
    size_t arr_name_len = std::strlen(arr_name);

    if (arr.empty()) {
        char *xml = new char[arr_name_len + 6]{};  // '<'x2, '>'x2, '/' = 6
        std::snprintf(xml, 6 + 2 * arr_name_len,"<%s></%s>", arr_name, arr_name);
        return xml;
    }

    /*
     * xml_len is minimum length of the XML string (not considering numbers' length)
     */
    size_t xml_len = 2 * (arr_name_len + arr.size() * elem_tag_len) + 5 * (arr.size() + 1) + 1;
    size_t cur_xml_idx = 0;
    char *xml = new char[xml_len]{};
    char *tmp_xml = nullptr;

    std::snprintf(xml, 3 + arr_name_len, "<%s>", arr_name);
    cur_xml_idx += arr_name_len + 2;

    size_t num_len = 0;
    for (int num : arr) {
        num_len = get_num_len(num);
        try {
            tmp_xml = new char[xml_len + num_len]{};
        } catch (const std::bad_alloc &e) {
            delete[] xml;
            throw e;
        }

        std::copy(xml, xml + cur_xml_idx, tmp_xml);
        delete[] xml;
        xml = tmp_xml;
        tmp_xml = nullptr;
        xml_len += num_len;

        std::snprintf(xml + cur_xml_idx, 6 + 2 * elem_tag_len + num_len, "<%s>%d</%s>", elem_tag, num, elem_tag);
        cur_xml_idx += 2 * elem_tag_len + num_len + 5;  // 2x'<' + 2x'>' + '/' = 5 bytes
    }

    std::snprintf(xml + cur_xml_idx, 4 + arr_name_len, "</%s>", arr_name);
    return xml;
}

std::vector<int> xml_to_array(const std::string &xml) {
    std::vector<int> arr;
    std::string num_as_str;
    char prev_char = '\0';

    for (size_t i = 0; i < xml.length(); i++) {
        if (std::isdigit(xml[i]) || xml[i] == '-') {
            if (num_as_str.empty() && prev_char != '>') {
                i++;
                continue;
            }
            num_as_str += xml[i];
        } else if (!num_as_str.empty()) {
            try {
                int num_to_add_in_arr = std::stoi(num_as_str);
                arr.push_back(num_to_add_in_arr);
                num_as_str.clear();
            } catch (const std::exception &e) {
                throw std::invalid_argument("Error happened while converting string to integer!");
            }
        }
        prev_char = xml[i];
    }

    return arr;
}

std::vector<int> xml_to_array(const char *xml) {
    std::vector<int> arr;

    char *num_as_str = new char[1]{};
    size_t num_len = 0;
    char *tmp_num_as_str = nullptr;

    char prev_char = '\0';

    int i = 0;
    while (xml[i] != '\0') {
        if (xml[i] >= 48 && xml[i] <= 57 || xml[i] == '-') {
            if (num_len == 0 && prev_char != '>') {
                i++;
                continue;
            }

            try {
                tmp_num_as_str = new char[num_len + 2]{};
            } catch (const std::bad_alloc &e) {
                delete[] num_as_str;
                throw e;
            }

            std::copy(num_as_str, num_as_str + num_len, tmp_num_as_str);
            tmp_num_as_str[num_len] = xml[i];
            num_len++;
            delete[] num_as_str;
            num_as_str = tmp_num_as_str;
            tmp_num_as_str = nullptr;
        } else if (num_len > 0) {
            int num_to_add = std::stoi(num_as_str);
            arr.push_back(num_to_add);

            delete[] num_as_str;
            num_as_str = new char[1]{};
            num_len = 0;
        }

        prev_char = xml[i];
        i++;
    }

    delete[] tmp_num_as_str;
    delete[] num_as_str;

    return arr;
}