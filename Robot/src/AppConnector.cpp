#include "AppConnector.h"

AppConnector::AppConnector(int port, const char* hostname) 
    : _server(port), _hostname(hostname), _currentExpression("NEUTRAL"), _hasNewExpression(false) {}

void AppConnector::begin() {
    _loadPersistedData();

    // Avvia AP temporaneo o connessione Wi-Fi salvata
    WiFi.mode(WIFI_STA);
    WiFi.begin();

    if (MDNS.begin(_hostname.c_str())) {
        Serial.printf("mDNS responder avviato: http://%s.local\n", _hostname.c_str());
    }

    // Endpoint API per l'app Flutter
    _server.on("/api/wifi", HTTP_POST, std::bind(&AppConnector::_handleWifiSetup, this));
    _server.on("/api/expression", HTTP_POST, std::bind(&AppConnector::_handleApiUpdate, this));
    _server.onNotFound(std::bind(&AppConnector::_handleNotFound, this));

    _server.begin();
    Serial.println("Server HTTP per App Flutter pronto.");
}

void AppConnector::handle() {
    _server.handleClient();
}

String AppConnector::getNewExpression() {
    _hasNewExpression = false;
    return _currentExpression;
}

void AppConnector::_loadPersistedData() {
    _prefs.begin("robot_cfg", true);
    _currentExpression = _prefs.getString("expr", "NEUTRAL");
    _prefs.end();
}

void AppConnector::_savePersistedData() {
    _prefs.begin("robot_cfg", false);
    _prefs.putString("expr", _currentExpression);
    _prefs.end();
}

void AppConnector::_handleWifiSetup() {
    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Body vuoto\"}");
        return;
    }

    // Allocazione dinamica compatibile con v6 e v7
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, _server.arg("plain"));

    if (error) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"JSON non valido\"}");
        return;
    }

    const char* ssid = doc["ssid"];
    const char* pass = doc["password"];

    if (ssid && pass) {
        WiFi.begin(ssid, pass);
        _server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Connessione Wi-Fi in corso...\"}");
    } else {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"SSID o Password mancanti\"}");
    }
}

void AppConnector::_handleApiUpdate() {
    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Body vuoto\"}");
        return;
    }

    // Allocazione dinamica compatibile con v6 e v7
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, _server.arg("plain"));

    if (error) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"JSON non valido\"}");
        return;
    }

    if (doc.containsKey("expression")) {
        _currentExpression = doc["expression"].as<String>();
        _hasNewExpression = true;
        _savePersistedData();
        _server.send(200, "application/json", "{\"status\":\"success\",\"expression\":\"" + _currentExpression + "\"}");
    } else {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Parametro expression mancante\"}");
    }
}

void AppConnector::_handleNotFound() {
    _server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"Endpoint non trovato\"}");
}