#ifndef LAB_1_DIALOG_H
#define LAB_1_DIALOG_H

#include <array>
#include <string>
#include <vector>

/////////////////////////////////////

////   EXTERNAL VARIABLES

/////////////////////////////////////

extern std::array<void(*)(), 3> dialog_options;
extern std::array<std::string, 3> msgs;

/////////////////////////////////////

////       DIALOG FUNCTIONS

/////////////////////////////////////

int dialog(const std::array<std::string, msgs.size()> &messages);
void dialog_xml2arr();
void dialog_arr2xml();

/////////////////////////////////////

////         AUXILIARY TOOLS

/////////////////////////////////////

void get_int(int &num);
std::string get_str(bool do_trim = false);

std::string trim(const std::string &str);
std::vector<int> str_to_arr(const std::string &str);

void print_array(const std::vector<int> &arr);

#endif //LAB_1_DIALOG_H
