#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <queue>
#include <string>

struct FooterState
{
    unsigned long lastSwitchMillis = 0;
    unsigned int messageIndex = 0;
    std::queue<std::string> customMessages;
    unsigned long customMessageStartTime = 0;
};

void showCustomFooterMessage(const char *message);
void showMainScreen();

#endif