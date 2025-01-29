#ifndef DATA_ENDPOINT_H
#define DATA_ENDPOINT_H

#include <Arduino.h>

extern String dataEndpoint;

void sendDataToServer();
void loadEndpoint();
void saveEndpoint(String endpoint);

#endif
