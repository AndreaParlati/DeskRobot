#ifndef APP_CONNECTOR_H
#define APP_CONNECTOR_H

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <ArduinoJson.h>

class AppConnector {
private:
    WebServer _server;
    Preferences _prefs;
    String _hostname;
    String _currentExpression;
    bool _hasNewExpression;

    void _handleWifiSetup();
    void _handleApiUpdate();
    void _handleNotFound();
    void _loadPersistedData();
    void _savePersistedData();

public:
    AppConnector(int port = 80, const char* hostname = "desk-robot");
    void begin();
    void handle();
    
    bool hasNewExpression() const { return _hasNewExpression; }
    String getNewExpression();
};

#endif // APP_CONNECTOR_H