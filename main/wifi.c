/* WIFI via http

   This POC project is all rights reserved by Bruce.Lu(the author). 
   You can use it under MIT license.

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "app_wifi.h"

static const char *const TAG = "app_wifi";
esp_err_t start_wifi_sta(char *ssid, char *password)
{
    esp_err_t err = ESP_OK;
    wifi_mode_t mode = 0;
    wifi_config_t wifi_config;
    err = esp_wifi_get_mode(&mode);
    ESP_LOGI(TAG, "starting sta. current mode: %d", mode);
    // IMPORTANT!!!! toooo many GROUND truth
    esp_wifi_disconnect();
    esp_wifi_stop();

    memset(&wifi_config, 0, sizeof(wifi_config));
    strcpy((char *)wifi_config.sta.ssid, ssid);
    strcpy((char *)wifi_config.sta.password, password);
    err = esp_wifi_set_mode(WIFI_MODE_STA);
    err = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "failed to set ap STA config");
    }
    err = esp_wifi_start();
    ESP_LOGI(TAG, "start_wifi_sta COMPLETED");

    return err;
}

esp_err_t start_wifi_ap(const char *ssid, const char *pass)
{
    esp_err_t err = ESP_OK;
    wifi_mode_t mode = 0;
    wifi_config_t wifi_config;
    ESP_LOGI(TAG, "starting ap. current mode: %d", mode);
    err = esp_wifi_get_mode(&mode);
    esp_wifi_disconnect();
    esp_wifi_stop();

    // config
    memset(&wifi_config, 0, sizeof(wifi_config));
    wifi_config.ap.max_connection = 3;

    // ssid
    wifi_config.ap.ssid_len = strlen(ssid);
    strcpy((char *)wifi_config.ap.ssid, ssid);

    // password
    if (strlen(pass) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    else
    {
        strcpy((char *)wifi_config.ap.password, pass);
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    }

    ESP_LOGI(TAG, "starting ap with ssid: %s, password: %s", wifi_config.ap.ssid, wifi_config.ap.password);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    err = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "failed to set ap config: %d", err);
    }
    err = esp_wifi_start();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "failed to start ap wifi");
    }
    //tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
    //err |= tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);

    ESP_LOGI(TAG, "start ap FINISHED");

    return ESP_OK;
}
