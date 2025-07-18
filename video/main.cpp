#include "ASCIIVideoPlayer.h"
#include <iostream>

int main() {
    try {
        ASCIIVideoPlayer player;
        player.showMenu();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}