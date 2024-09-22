#include <stdio.h>
#include <stdlib.h>
#include "../headers/dialog.h"

const char *msgs[] = {"0. Quit",
                      "1. Integer array to XML string",
                      "2. XML string to integer array"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

const char *err_msgs[] = {"Ok",
                          "Array name length is zero",
                          "Value name length is zero",
                          "The string is not XML-like string",
                          "The string does not start with <arr_name> or it does not end with </arr_name>",
                          "Not found closing value name tag",
                          "Value is not a correct number",
                          "The next XML-string's tag us not </arr_name>"};

int (*dialog_options[])() = {NULL,
                             dialog_arr2xml,
                             dialog_xml2arr};

/////////////////////////////////////

//        DIALOG FUNCTIONS

/////////////////////////////////////

int dialog(const char *msgs[], int NMsgs) {
    const char *err_msg = "";

    int option_num;
    int i, n;
    do {
        puts(err_msg);
        err_msg = "You are wrong. Repeate, please!";

        for (i = 0; i < NMsgs; i++)
            puts(msgs[i]);
        puts("Make your choice: --> ");

        n = get_int(&option_num);

        if (n == 0) // EOF -> quit from program
            option_num = 0;
    } while (option_num < 0 || option_num >= NMsgs);
    return option_num;
}

int dialog_xml2arr() {
    puts("Enter XML string:");
    char *xml = get_str();
    if (xml == NULL) {
        return 0;  // Error happened while reading an XML-string
    }

    int validate_xml_result = validate_xml_format(xml);
    if (validate_xml_result != 0) {
        printf("Error while validating your XML-string: %s\n", err_msgs[validate_xml_result]);
        puts("Try again!");
        free_memory_in_dialog_xml2arr(xml, NULL);
        return 1;  // Ok
    }

    IntegerArray *xml2arr_result = xml_to_array(xml);
    if (xml2arr_result == NULL) {
        free_memory_in_dialog_xml2arr(xml, xml2arr_result);
        return 0;  // Error happened while converting XML-string to array
    }

    puts("The result:");
    if (!xml2arr_result->len) {
        puts("empty array");
    } else {
        for (int i = 0; i < xml2arr_result->len; i++) {
            if (i < xml2arr_result->len - 1) {
                printf("%d, ", xml2arr_result->arr[i]);
            } else {
                printf("%d\n", xml2arr_result->arr[i]);
            }
        }
    }

    free_memory_in_dialog_xml2arr(xml, xml2arr_result);

    return 1; // Ok
}

int dialog_arr2xml() {
    puts("Enter an integer array (e.g., \"1 2 3\"):");
    IntegerArray *arr_ptr = str_to_arr();
    if (arr_ptr == NULL) {
        return 0;  // Error happened while executing str_to_arr()
    }

    puts("Enter the array's name:");
    char *arr_name = get_str();
    if (arr_name == NULL) {
        free_memory_in_dialog_arr2xml(arr_ptr, arr_name, NULL, NULL);
        return 0;  // Error happened while reading a string
    }

    puts("Enter an element tag name:");
    char *elem_tag_name = get_str();
    if (elem_tag_name == NULL) {
        free_memory_in_dialog_arr2xml(arr_ptr, arr_name, elem_tag_name, NULL);
        return 0;  // Error happened while reading a string
    }

    char *arr2xml_result = array_to_xml(arr_ptr, arr_name, elem_tag_name);
    if (arr2xml_result == NULL) {
        free_memory_in_dialog_arr2xml(arr_ptr, arr_name, elem_tag_name, arr2xml_result);
        return 0;  // Error while memory allocating in array_to_xml()
    }

    puts("The result:");
    printf("%s\n", arr2xml_result);

    free_memory_in_dialog_arr2xml(arr_ptr, arr_name, elem_tag_name, arr2xml_result);

    return 1;  // Ok
}

/////////////////////////////////////

//         AUXILIARY TOOLS

/////////////////////////////////////

int get_int(int *num) {
    int scanf_result;
    do {
        scanf_result = scanf("%d", num);

        if (scanf_result == -1) {
            return 0;
        }

        if (scanf_result == 0) {
            scanf("%*[^\n]");
            printf("%s", "Try once again!: ");
            scanf_result = 0;
        }
    } while (scanf_result == 0);

    scanf("%*[^\n]");
    getchar();
    return 1;
}

char *get_str() {
    char *str = (char *) calloc(1, sizeof(char));
    *str = '\0';
    int str_size = sizeof(char);

    char character;
    for (int i = 0; ; i++) {
        character = (char) getchar();

        if (character == EOF) {
            free(str);
            return NULL;
        } else if (character == '\n') {
            if (i > 0) {
                break;
            } else {
                i--;
            }
        } else {
            char *str2 = (char *) realloc(str, str_size + sizeof(char));
            str_size += sizeof(char);

            if (str2 == NULL) {
                free(str);
                return NULL;
            }
            str = str2;

            str[i] = (char) character;
            str[i + 1] = '\0';
        }
    }

    return str;
}

IntegerArray *str_to_arr() {
    IntegerArray *arr_ptr = (IntegerArray *) calloc(1, sizeof(IntegerArray));
    init_integer_array(arr_ptr);

    char *cur_num_as_str = (char *) calloc(1, sizeof(char));
    int cur_num_len = 0;
    int digits_amount = 0;

    char cur_char;
    char prev_char = '\0';

    while (prev_char != '\n') {
        cur_char = getchar();
        if (cur_char >= 48 && cur_char <= 57 || (cur_char == '-' && prev_char != '-')) {
            char *tmp_cur_num = (char *) realloc(cur_num_as_str, (cur_num_len + 1) * sizeof(char));
            if (tmp_cur_num == NULL) {
                free_memory_in_str2arr(arr_ptr, cur_num_as_str);
                return NULL;
            }

            cur_num_as_str = tmp_cur_num;
            cur_num_as_str[cur_num_len] = cur_char;
            cur_num_len++;
            if (cur_char != '-') {
                digits_amount++;
            }
        } else if (cur_char == ' ' || cur_char == '\n') {
            if (digits_amount) {
                int number = atoi(cur_num_as_str);
                int *tmp_arr_ptr = (int *) realloc(arr_ptr->arr, (arr_ptr->len + 1) * sizeof(int));
                if (tmp_arr_ptr == NULL) {
                    free_memory_in_str2arr(arr_ptr, cur_num_as_str);
                    return NULL;
                }

                arr_ptr->arr = tmp_arr_ptr;
                arr_ptr->arr[arr_ptr->len] = number;
                arr_ptr->len++;
                tmp_arr_ptr = NULL;

                free(cur_num_as_str);
                cur_num_as_str = NULL;
                cur_num_len = 0;
                digits_amount = 0;
            }
        } else {
            puts("You have entered an incorrect array!");
            free_memory_in_str2arr(arr_ptr, cur_num_as_str);
            return NULL;
        }

        prev_char = cur_char;
    }

    return arr_ptr;
}