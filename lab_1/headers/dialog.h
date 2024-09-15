#ifndef LAB_1_DIALOG_H
#define LAB_1_DIALOG_H

#include "converter.h"

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
