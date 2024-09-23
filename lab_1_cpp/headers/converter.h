#ifndef LAB_1_CONVERTER_H
#define LAB_1_CONVERTER_H

#include <string>
#include <vector>

///////////////////////////////////////

////         AUXILIARY TOOLS

///////////////////////////////////////

void reset_ostringstream(std::ostringstream &oss);

void validate_xml_format(const std::string& xml);
void validate_xml_format(const char *xml);

int get_cstring_length(const char *str);

size_t get_num_len(int num);

///////////////////////////////////////

////   CONVERTERS (arr2xml, xml2arr)

///////////////////////////////////////

std::string array_to_xml(const std::vector<int> &arr, const std::string &arr_name, const std::string &elem_tag);
char *array_to_xml(const std::vector<int> &arr, const char *arr_name, const char *elem_tag);

std::vector<int> xml_to_array(const std::string &xml);
std::vector<int> xml_to_array(const char *xml);

#endif //LAB_1_CONVERTER_H
