#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/converter.h"

/////////////////////////////////////

//         AUXILIARY TOOLS

/////////////////////////////////////

unsigned int get_num_len(int num) {
    int len = 1;

    if (num < 0) {
        len++;
        num = -num;
    }

    while ((num = num / 10) > 0) {
        len++;
    }

    return len;
}

void init_integer_array(IntegerArray *arr_ptr) {
    arr_ptr->arr = NULL;
    arr_ptr->len = 0;
}

/////////////////////////////////////

//   CONVERTERS (arr2xml, xml2arr)

/////////////////////////////////////

// TODO: free memory
char *array_to_xml(IntegerArray *arr_ptr, char *arr_name, char *elem_tag) {
    unsigned int elem_tag_len = strlen(elem_tag);
    unsigned int arr_name_len = strlen(arr_name);

    // TODO: free
    if (!arr_ptr->len) {
        char *xml = (char *) calloc(arr_name_len + 6, sizeof(char)); // '<'x2, '>'x2, '/' = 6
        sprintf(xml, "<%s></%s>", arr_name, arr_name);
        return xml;
    }

    // TODO: explain 11
    unsigned int xml_len = 2*(arr_name_len + arr_ptr->len*elem_tag_len) + 11;  // Minimum length of the XML string (not considering numbers' length)
    char *xml = (char *) calloc(xml_len, sizeof(char));
    if (xml == NULL) {
        return NULL;
    }

    char *tmp_xml = NULL;
    unsigned int cur_xml_idx = 0;

    sprintf(xml, "<%s>", arr_name);
    cur_xml_idx += arr_name_len + 2;

    unsigned int num_len = 0;
    for (int i = 0; i < arr_ptr->len; i++) {
        num_len = get_num_len(arr_ptr->arr[i]);
        tmp_xml = (char *) realloc(xml, xml_len + num_len);
        if (tmp_xml == NULL) {
            free(xml);
            return NULL;
        } else {
            xml = tmp_xml;
            tmp_xml = NULL;
        }

        sprintf(xml + cur_xml_idx, "<%s>%d</%s>", elem_tag, arr_ptr->arr[i], elem_tag);
        cur_xml_idx += 2*elem_tag_len + num_len + 5;  // TODO: explain 5
    }

    sprintf(xml + cur_xml_idx, "</%s>", arr_name);
    return xml;
}

// TODO: free memory
IntegerArray *xml_to_array(const char *xml) {
    IntegerArray *arr_ptr = (IntegerArray *) calloc(1, sizeof(IntegerArray));
    init_integer_array(arr_ptr);
    int *tmp_arr = NULL;

    char *num_as_str = (char *) calloc(1, sizeof(char));
    unsigned int num_len = 0;
    char *tmp_num_as_str = NULL;

    char prev_char = '\0';

    int i = 0;
    while (xml[i] != '\0') {
        if (xml[i] >= 48 && xml[i] <= 57 || xml[i] == '-') {
            if (num_len == 0 && prev_char != '>') {
                i++;
                continue;
            }

            tmp_num_as_str = (char *) realloc(num_as_str, (num_len + 1)*sizeof(char));
            if (tmp_num_as_str == NULL) {
                free_memory_in_xml2arr(arr_ptr, num_as_str);
                return NULL;
            }
            
            num_as_str = tmp_num_as_str;
            num_as_str[num_len] = xml[i];
            num_len++;
            tmp_num_as_str = NULL;
        } else if (num_len > 0) {
            tmp_arr = (int *) realloc(arr_ptr->arr, (arr_ptr->len + 1)*sizeof(int));
            if (tmp_arr == NULL) {
                free_memory_in_xml2arr(arr_ptr, num_as_str);
                return NULL;
            }

            arr_ptr->arr = tmp_arr;
            arr_ptr->arr[arr_ptr->len] = atoi(num_as_str);
            arr_ptr->len++;
            tmp_arr = NULL;

            free(num_as_str);
            num_as_str = (char *) calloc(1, sizeof(char));
            num_len = 0;
        }

        prev_char = xml[i];
        i++;
    }

    free(num_as_str);
    return arr_ptr;
}

/////////////////////////////////////

//     ALLOCATED MEMORY FREEING

/////////////////////////////////////

void erase_xml(char *xml) {
    if (xml != NULL) {
        free(xml);
    }
}

void free_memory_in_xml2arr(IntegerArray *arr_ptr, char *num_as_str) {
    if (arr_ptr != NULL) {
        if (arr_ptr->arr != NULL) {
            free(arr_ptr->arr);
        }
        free(arr_ptr);
    }

    if (num_as_str != NULL) {
        free(num_as_str);
    }
}

void free_memory_in_str2arr(IntegerArray *arr_ptr, char *cur_num_as_str) {
    if (arr_ptr != NULL) {
        if (arr_ptr->arr != NULL) {
            free(arr_ptr->arr);
        }
        free(arr_ptr);
    }

    if (cur_num_as_str != NULL) {
        free(cur_num_as_str);
    }
}

void free_memory_in_dialog_xml2arr(char *xml, IntegerArray *arr_ptr_result) {
    if (xml != NULL) {
        free(xml);
    }

    if (arr_ptr_result != NULL) {
        if (arr_ptr_result->arr != NULL) {
            free(arr_ptr_result->arr);
        }
        free(arr_ptr_result);
    }
}

void free_memory_in_dialog_arr2xml(IntegerArray *arr_ptr, char *arr_name, char *elem_tag_name, char *arr2xml_res) {
    if (arr_ptr != NULL) {
        if (arr_ptr->arr != NULL) {
            free(arr_ptr->arr);
        }
        free(arr_ptr);
    }

    if (arr_name != NULL) {
        free(arr_name);
    }

    if (elem_tag_name != NULL) {
        free(elem_tag_name);
    }

    if (arr2xml_res != NULL) {
        free(arr2xml_res);
    }
}