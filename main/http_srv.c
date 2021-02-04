/* WIFI via http

   This POC project is all rights reserved by Bruce.Lu(the author). 
   You can use it under MIT license.

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "app_common.h"

static const char *const TAG = "app_httpsrv";
static char pcMsgWifiInProcess[] = "WIFI config is in processing, command ignored";

esp_err_t http_wifi_handler(httpd_req_t *req)
{
    esp_err_t ret;
    char *ssid = NULL, *password = NULL;
    WIFIManagerConfig *xWifiCfg = (WIFIManagerConfig *)req->user_ctx;

    // wifi config is not finished, don't accept command
    if (xWifiCfg->xApChange)
    {
        ret = httpd_resp_send(req, pcMsgWifiInProcess, sizeof(pcMsgWifiInProcess));
        return ret;
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    char *buf = NULL;
    int buf_len = httpd_req_get_url_query_len(req) + 1;
    int found = 0;
    if (buf_len > 1)
    {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "ssid", param, sizeof(param)) == ESP_OK)
            {
                ESP_LOGI(TAG, "param ssid=%s", param);
                if (*xWifiCfg->sta_ssid != NULL)
                {
                    free(*xWifiCfg->sta_ssid);
                }

                *xWifiCfg->sta_ssid = malloc(strlen(param) + 1);
                strcpy(*xWifiCfg->sta_ssid, param);
                found |= 1;
            }
            if (httpd_query_key_value(buf, "password", param, sizeof(param)) == ESP_OK)
            {
                ESP_LOGI(TAG, "param password=%s", param);
                if (*xWifiCfg->sta_ssid != NULL)
                {
                    free(*xWifiCfg->sta_passwd);
                }

                *xWifiCfg->sta_passwd = malloc(strlen(param) + 1);
                strcpy(*xWifiCfg->sta_passwd, param);
                found |= 2;
            }
        }
        free(buf);
    }

    if (found == 3)
    {
        ESP_LOGI(TAG, "got ssid: %s password: %s from httpsrv", *xWifiCfg->sta_ssid, *xWifiCfg->sta_passwd);
        ESP_LOGI(TAG, "sent STA bootstrapping");
        xEventGroupSetBits(*xWifiCfg->pxEvtGroup, APP_EBIT_WIFI_START_STA);
        ESP_LOGI(TAG, "FINISHED STA bootstrapping");
    }
    else
    {
        ESP_LOGI(TAG, "missing ssid/password");
    }

    ret = httpd_resp_send(req, req->uri, sizeof(req->uri));
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "HTTP send failed");
    }

    return ret;
}

esp_err_t start_http_srv(httpd_handle_t *srv, WIFIManagerConfig *xWifiCfg)
{
    esp_err_t err = ESP_OK;
    /* Configure the HTTP server */
    httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
    server_config.server_port = 80;
    server_config.stack_size = 4096;
    server_config.task_priority = tskIDLE_PRIORITY + 5;
    server_config.lru_purge_enable = true;
    server_config.max_open_sockets = 1;

    httpd_uri_t config_handler = {
        .uri = "/wifi",
        .method = HTTP_GET,
        .handler = http_wifi_handler,
        .user_ctx = xWifiCfg};

    if ((err = httpd_start(srv, &server_config)) != ESP_OK)
    {
        free(srv);
        ESP_LOGE(TAG, "Failed to start http server: %d", err);
    }

    if ((err = httpd_register_uri_handler(*srv, &config_handler)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Uri handler register failed: %d", err);
        free(srv);
        return ESP_FAIL;
    }

    return err;
}