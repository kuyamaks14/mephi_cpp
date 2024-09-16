#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../headers/converter.h"

extern const char *err_msgs[];

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
    if (arr_ptr != NULL) {
        arr_ptr->arr = NULL;
        arr_ptr->len = 0;
    }
}

int is_valid_value(const char* str) {
    int is_negative = 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i]) && (str[i] != '-' || is_negative)) {
            return 0;
        } else if (str[i] == '-') {
            is_negative = 1;
        }
    }
    return 1;
}

int extract_array_and_value_names_from_xml(const char *xml, char **arr_name, char **val_name) {
    unsigned int xml_len = strlen(xml);
    const char right_arrow = '>';

    unsigned int arr_name_len = strchr(xml, right_arrow) - xml - 1;
    if (arr_name_len == 0) {
        return 1;  // Array name length is zero, not ok!
    }
    *arr_name = (char *) calloc(arr_name_len + 1, sizeof(char));
    strncpy(*arr_name, xml + 1, arr_name_len);
    (*arr_name)[arr_name_len] = '\0';

    // Check if the array is empty (i.e., XML is "<arr_name></arr_name>")
    if (xml_len == 2*arr_name_len + 5) {  // 2x'<' + 2x'>' + '/' == 5 bytes
        return 0;  // Ok
    }

    unsigned int val_name_len = strchr(xml + arr_name_len + 2, right_arrow) - (xml + arr_name_len + 2) - 1;
    if (val_name_len == 0) {
        return 2;  // Value name length is zero, not ok!
    }
    *val_name = (char *) calloc(val_name_len + 1, sizeof(char));
    strncpy(*val_name, xml + arr_name_len + 3, val_name_len);
    (*val_name)[val_name_len] = '\0';

    return 0;  // Ok
}

int validate_xml_format(const char *xml) {
    char *arr_name = NULL, *val_name = NULL;
    int name_extraction_result = extract_array_and_value_names_from_xml(xml, &arr_name, &val_name);
    if (name_extraction_result != 0) {
        printf("Error: %s\n", err_msgs[name_extraction_result]);
        free_memory_for_arr_value_names(arr_name, val_name);
        return 3;  // The string is not XML-like string
    }

    char arr_name_start[strlen(arr_name) + 3];  // '<' + '>' + '\0' == 3 bytes
    char arr_name_end[strlen(arr_name) + 4];  // '<' + '>' + '/' + '\0' == 4 bytes
    sprintf(arr_name_start, "<%s>", arr_name);
    sprintf(arr_name_end, "</%s>", arr_name);

    if (strncmp(xml, arr_name_start, strlen(arr_name_start)) != 0 ||
        strncmp(xml + strlen(xml) - strlen(arr_name_end), arr_name_end, strlen(arr_name_end)) != 0) {
        free_memory_for_arr_value_names(arr_name, val_name);
        return 4;  // Not ok (the string does not start with <arr_name> or it does not end with </arr_name>)
    }

    if (name_extraction_result == 0 && val_name == NULL) {
        return 0;  // XML-string is like "<arr_name></arr_name>" so it is valid
    }

    char val_name_start[strlen(val_name) + 3];  // '<' + '>' + '\0' == 3 bytes
    char val_name_end[strlen(val_name) + 4];  // '<' + '>' + '/' + '\0' == 4 bytes
    sprintf(val_name_start, "<%s>", val_name);
    sprintf(val_name_end, "</%s>", val_name);

    xml += strlen(arr_name_start);
    while (*xml) {
        if (strncmp(xml, val_name_start, strlen(val_name_start)) == 0) {
            xml += strlen(val_name_start);

            const char *value_end_pos = strstr(xml, val_name_end);
            if (!value_end_pos) {
                free_memory_for_arr_value_names(arr_name, val_name);
                return 5; // Not ok (not found closing value name tag)
            }

            // Extracting a number between <val_name> and </val_name>
            char value[100];
            strncpy(value, xml, value_end_pos - xml);
            value[value_end_pos - xml] = '\0';

            if (!is_valid_value(value)) {
                free_memory_for_arr_value_names(arr_name, val_name);
                return 6; // Not ok (value is not a correct number)
            }

            // Going to next <val_name> tag
            xml = value_end_pos + strlen(val_name_end);
        } else {
            break; // No next <val_name> tag in XML-string so skip the further checking it
        }
    }

    // Check if the next XML-string's tag is </arr_name>
    if (strncmp(xml, arr_name_end, strlen(arr_name_end)) != 0) {
        free_memory_for_arr_value_names(arr_name, val_name);
        return 7; // Not ok (the next XML-string's tag us not </arr_name>)
    }

    free_memory_for_arr_value_names(arr_name, val_name);
    return 0;  // Ok
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

void free_memory_for_arr_value_names(char *arr_name, char *val_name) {
    free_string_memory(arr_name);
    free_string_memory(val_name);
}