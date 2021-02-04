This is a POC of configure esp32 wifi STA info from http request in AP mode.

1. build:
mkdir build && cd build && cmake ../
make flash

2. wifi setting
the board starts as AP with ssid=blu-esp1, password=test123456
connect this AP and request http://192.168.4.1/wifi/?ssid=<YOUR_WIFI_AP>&password=<AP_PASSWD>
and the board will connect to this AP.


License: MIT





