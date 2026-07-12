#include "../include/Validation.h"

bool ValidateNonEmpty(const char* text) {
    return text != nullptr && text[0] != '\0';
}
