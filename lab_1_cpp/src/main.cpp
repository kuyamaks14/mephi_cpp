#include <stdio.h>
#include "../headers/dialog.h"

int main() {
    int operation_code;

    while ((operation_code = dialog(msgs, NMsgs)))
        if (!dialog_options[operation_code]())
            break;

    puts("\nThat's all.");
}