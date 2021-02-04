#ifndef __APP_HTTP_SRV_H__
#define __APP_HTTP_SRV_H__

#include "app_common.h"
esp_err_t start_http_srv(httpd_handle_t *httpsrv, WIFIManagerConfig *xWifiCfg);
#endif