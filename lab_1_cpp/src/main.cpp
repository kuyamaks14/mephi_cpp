#include <iostream>
#include "../headers/dialog.h"

int main() {
    try {
        int operation_code;
        while ((operation_code = dialog(msgs)))
            dialog_options[operation_code]();
    } catch (const std::exception &e) {
        std::cout << "Error happened: " << e.what() << std::endl;
    }

    std::cout << "\nThat's all" << std::endl;
}