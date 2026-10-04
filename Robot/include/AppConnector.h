#ifndef APP_CONNECTOR_H
#define APP_CONNECTOR_H

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <Preferences.h>

class AppConnector {
public:
    AppConnector(int port = 80, const char* hostname = "deskrobot");
    
    // Tenta la connessione Wi-Fi o avvia la modalità AP per il setup
    void begin();
    void handle();

    String getCurrentExpression() const { return _currentExpression; }
    String getTasksJson() const { return _tasksJson; }
    bool isProvisioningMode() const { return _isApMode; }

    // Utility per resettare il Wi-Fi salvato (es. pressione prolungata touch)
    void resetWifiCredentials();

private:
    WebServer _server;
    Preferences _preferences;
    const char* _hostname;
    
    String _currentExpression;
    String _tasksJson;
    bool _isApMode;

    bool _connectToSavedWifi();
    void _startAccessPoint();
    void _loadPersistedData();
    void _savePersistedData();
    void _registerRoutes();
    void _handleApiUpdate();
    void _handleWifiSetup();
    void _handleNotFound();
};

#endif // APP_CONNECTOR_H