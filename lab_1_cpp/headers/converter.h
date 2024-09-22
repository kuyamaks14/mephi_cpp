#ifndef LAB_1_CONVERTER_H
#define LAB_1_CONVERTER_H

typedef struct IntegerArray {
    int *arr;
    unsigned int len;
} IntegerArray;

/////////////////////////////////////

//         AUXILIARY TOOLS

/////////////////////////////////////

unsigned int get_num_len(int number);
void init_integer_array(IntegerArray *arr_ptr);
int is_valid_value(const char* str);
int extract_array_and_value_names_from_xml(const char *xml, char **arr_name, char **val_name);
int validate_xml_format(const char *xml);

/////////////////////////////////////

//   CONVERTERS (arr2xml, xml2arr)

/////////////////////////////////////

char *array_to_xml(IntegerArray *arr_ptr, char *arr_name, char *elem_tag);
IntegerArray *xml_to_array(const char *xml);

/////////////////////////////////////

//     ALLOCATED MEMORY FREEING

/////////////////////////////////////

void free_string_memory(char *str);
void free_array_memory(IntegerArray *arr_ptr);
void free_memory_in_xml2arr(IntegerArray *arr_ptr, char *num_as_str);
void free_memory_in_str2arr(IntegerArray *arr_ptr, char *cur_num_as_str);
void free_memory_in_dialog_xml2arr(char *xml, IntegerArray *arr_ptr_result);
void free_memory_in_dialog_arr2xml(IntegerArray *arr_ptr, char *arr_name, char *elem_tag_name, char *arr2xml_res);
void free_memory_for_arr_value_names(char *arr_name, char *val_name);

#endif //LAB_1_CONVERTER_H
