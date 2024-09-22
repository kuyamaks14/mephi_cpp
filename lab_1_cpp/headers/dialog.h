#ifndef LAB_1_DIALOG_H
#define LAB_1_DIALOG_H

#include "converter.h"

extern const int NMsgs;
extern const char *msgs[];
extern const char *err_msgs[];
extern int (*dialog_options[])();

/////////////////////////////////////

//        DIALOG FUNCTIONS

/////////////////////////////////////
int dialog(const char *msgs[], int NMsgs);
int dialog_xml2arr();
int dialog_arr2xml();


/////////////////////////////////////

//         AUXILIARY TOOLS

/////////////////////////////////////
int get_int(int *num);
char *get_str();
IntegerArray *str_to_arr();

#endif //LAB_1_DIALOG_H
