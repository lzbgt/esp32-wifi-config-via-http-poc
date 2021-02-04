#ifndef __APP_WIFI_H__
#define __APP_WIFI_H__
#include "app_common.h"

esp_err_t  start_wifi_sta(char *ssid, char *password);
esp_err_t start_wifi_ap(const char *ssid, const char *pass);

#endif