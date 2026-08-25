#include <iostream>
#include "Menu.h"
#include "../include/Storage.h"
#include "../include/StorageManager.h"

int main() {
    Class dsl;
    Subject dsmh;

    // Load all data from disk at startup
    if (!LoadAllData(dsl, dsmh)) {
        std::cerr << "Warning: Some data could not be loaded. Starting with empty data.\n";
    }

    // Show main menu (blocks until user exits)
    ShowMainMenu(dsl, dsmh);

    // Save all data to disk on exit
    if (!SaveAllData(dsl, dsmh)) {
        std::cerr << "Error: Failed to save data on exit.\n";
        return 1;
    }

    return 0;
}
