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

// To correctly use this function, you must free memory allocated for returned result in the end.
char *array_to_xml(IntegerArray *arr_ptr, char *arr_name, char *elem_tag) {
    unsigned int elem_tag_len = strlen(elem_tag);
    unsigned int arr_name_len = strlen(arr_name);

    if (!arr_ptr->len) {
        char *xml = (char *) calloc(arr_name_len + 6, sizeof(char)); // '<'x2, '>'x2, '/' = 6
        sprintf(xml, "<%s></%s>", arr_name, arr_name);
        return xml;
    }

    /*
     * 4x'<' + 4x'>' + 2x'/' + '\0' = 11 bytes
     * xml_len is minimum length of the XML string (not considering numbers' length)
     */
    unsigned int xml_len = 2*(arr_name_len + arr_ptr->len*elem_tag_len) + 11;
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
        cur_xml_idx += 2*elem_tag_len + num_len + 5;  // 2x'<' + 2x'>' + '/' = 5 bytes
    }

    sprintf(xml + cur_xml_idx, "</%s>", arr_name);
    return xml;
}

// To correctly use this function, you must free memory allocated for returned result in the end.
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

void free_string_memory(char *str) {
    if (str != NULL) {
        free(str);
    }
}

void free_array_memory(IntegerArray *arr_ptr) {
    if (arr_ptr != NULL) {
        if (arr_ptr->arr != NULL) {
            free(arr_ptr->arr);
        }
        free(arr_ptr);
    }
}

void free_memory_in_xml2arr(IntegerArray *arr_ptr, char *num_as_str) {
    free_array_memory(arr_ptr);
    free_string_memory(num_as_str);
}

void free_memory_in_str2arr(IntegerArray *arr_ptr, char *cur_num_as_str) {
    free_array_memory(arr_ptr);
    free_string_memory(cur_num_as_str);
}

void free_memory_in_dialog_xml2arr(char *xml, IntegerArray *arr_ptr_result) {
    free_array_memory(arr_ptr_result);
    free_string_memory(xml);
}

void free_memory_in_dialog_arr2xml(IntegerArray *arr_ptr, char *arr_name, char *elem_tag_name, char *arr2xml_res) {
    free_array_memory(arr_ptr);
    free_string_memory(arr_name);
    free_string_memory(elem_tag_name);
    free_string_memory(arr2xml_res);
}