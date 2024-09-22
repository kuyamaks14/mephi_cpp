#include "../headers/converter.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <regex>

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

///////////////////////////////////

//// CONVERTERS (arr2xml, xml2arr)

///////////////////////////////////

std::string array_to_xml(const std::vector<int> &arr, const std::string &arr_name, const std::string &elem_tag) {
    std::ostringstream oss;

    if (arr.empty()) {
        oss << "<" << arr_name << "></" << arr_name << ">";
        return oss.str();
    }

    size_t xml_min_len = 2*(arr_name.length() + arr.size() * elem_tag.length()) + 10;

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