#include "AppConnector.h"

AppConnector::AppConnector(int port, const char* hostname) 
    : _server(port), 
      _hostname(hostname), 
      _currentExpression("HAPPY"), 
      _tasksJson("[]"),
      _isApMode(false) {}

void AppConnector::begin() {
    _loadPersistedData();

    // Tenta la connessione al Wi-Fi memorizzato
    if (_connectToSavedWifi()) {
        _isApMode = false;
        if (MDNS.begin(_hostname)) {
            Serial.printf("[AppConnector] mDNS attivo: http://%s.local\n", _hostname);
            MDNS.addService("http", "tcp", 80);
        }
    } else {
        // Se la connessione fallisce o non ci sono credenziali, avvia l'Access Point
        _startAccessPoint();
    }

    _registerRoutes();
    _server.begin();
}

bool AppConnector::_connectToSavedWifi() {
    _preferences.begin("wifi_cred", true);
    String ssid = _preferences.getString("ssid", "");
    String pass = _preferences.getString("pass", "");
    _preferences.end();

    if (ssid.length() == 0) {
        Serial.println("[Wi-Fi] Nessuna credenziale Wi-Fi trovata in memoria.");
        return false;
    }

    Serial.printf("[Wi-Fi] Connessione a %s...\n", ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Timeout ~10 secondi
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[Wi-Fi] Connesso! IP: " + WiFi.localIP().toString());
        return true;
    }

    Serial.println("\n[Wi-Fi] Connessione fallita.");
    return false;
}

void AppConnector::_startAccessPoint() {
    _isApMode = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAP("DeskRobot-Setup"); // Nome della rete generata dall'ESP32
    
    Serial.println("[Wi-Fi] Modalità Access Point avviata.");
    Serial.println("[Wi-Fi] Rete AP: DeskRobot-Setup");
    Serial.println("[Wi-Fi] IP AP: " + WiFi.softAPIP().toString());
}

void AppConnector::_registerRoutes() {
    // Endpoint per aggiornare espressioni/task dall'app
    _server.on("/api/update", HTTP_POST, [this]() {
        _handleApiUpdate();
    });

    // Endpoint per inviare nuove credenziali Wi-Fi dall'app
    _server.on("/api/wifi", HTTP_POST, [this]() {
        _handleWifiSetup();
    });

    _server.onNotFound([this]() {
        _handleNotFound();
    });
}

void AppConnector::_handleWifiSetup() {
    _server.sendHeader("Access-Control-Allow-Origin", "*");

    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Body mancante\"}");
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, _server.arg("plain"));

    if (error || !doc.containsKey("ssid") || !doc.containsKey("pass")) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"SSID o Password mancanti\"}");
        return;
    }

    String newSsid = doc["ssid"].as<String>();
    String newPass = doc["pass"].as<String>();

    // Salvataggio credenziali Wi-Fi in Flash
    _preferences.begin("wifi_cred", false);
    _preferences.putString("ssid", newSsid);
    _preferences.putString("pass", newPass);
    _preferences.end();

    _server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Credenziali salvate. Riavvio in corso...\"}");
    
    delay(1000);
    ESP.restart(); // Riavvia l'ESP32 per connettersi alla nuova rete
}

void AppConnector::_handleApiUpdate() {
    _server.sendHeader("Access-Control-Allow-Origin", "*");

    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Body mancante\"}");
        return;
    }

    JsonDocument doc;
    if (deserializeJson(doc, _server.arg("plain"))) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"JSON non valido\"}");
        return;
    }

    bool hasChanges = false;
    if (doc.containsKey("expression")) {
        _currentExpression = doc["expression"].as<String>();
        hasChanges = true;
    }
    if (doc.containsKey("tasks")) {
        String newTasksJson;
        serializeJson(doc["tasks"], newTasksJson);
        _tasksJson = newTasksJson;
        hasChanges = true;
    }

    if (hasChanges) {
        _savePersistedData();
    }

    _server.send(200, "application/json", "{\"status\":\"success\"}");
}

void AppConnector::resetWifiCredentials() {
    _preferences.begin("wifi_cred", false);
    _preferences.clear();
    _preferences.end();
    Serial.println("[Wi-Fi] Credenziali Wi-Fi cancellate. Riavvio...");
    ESP.restart();
}

void AppConnector::_loadPersistedData() {
    _preferences.begin("robot_data", true);
    _currentExpression = _preferences.getString("expression", "HAPPY");
    _tasksJson = _preferences.getString("tasks", "[]");
    _preferences.end();
}

void AppConnector::_savePersistedData() {
    _preferences.begin("robot_data", false);
    _preferences.putString("expression", _currentExpression);
    _preferences.putString("tasks", _tasksJson);
    _preferences.end();
}

void AppConnector::handle() {
    _server.handleClient();
}

void AppConnector::_handleNotFound() {
    if (_server.method() == HTTP_OPTIONS) {
        _server.sendHeader("Access-Control-Allow-Origin", "*");
        _server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        _server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        _server.send(200);
    } else {
        _server.send(404, "text/plain", "Not Found");
    }
}

#include "AppConnector.h"

AppConnector::AppConnector(int port, const char* hostname) 
    : _server(port), 
      _hostname(hostname), 
      _currentExpression("HAPPY"), 
      _tasksJson("[]") {}

void AppConnector::begin() {
    // 1. Inizializzazione mDNS per usare http://deskrobot.local al posto dell'IP
    if (MDNS.begin(_hostname)) {
        Serial.printf("[AppConnector] mDNS avviato! Raggiungibile su: http://%s.local\n", _hostname);
        // Aggiunge la risposta al servizio HTTP discovery
        MDNS.addService("http", "tcp", 80);
    } else {
        Serial.println("[AppConnector] Errore nell'avvio di mDNS!");
    }

    // 2. Ripristino stato salvato prima dello spegnimento
    _loadPersistedData();

    // 3. Registrazione Endpoint HTTP POST /api/update
    _server.on("/api/update", HTTP_POST, [this]() {
        _handleApiUpdate();
    });

    // Gestione CORS e rotte non trovate
    _server.onNotFound([this]() {
        _handleNotFound();
    });

    _server.begin();
    Serial.println("[AppConnector] Server HTTP pronto sulla porta 80");
}

void AppConnector::handle() {
    _server.handleClient();
}

void AppConnector::_loadPersistedData() {
    _preferences.begin("robot_data", true); // Modalità Read-Only
    _currentExpression = _preferences.getString("expression", "HAPPY");
    _tasksJson = _preferences.getString("tasks", "[]");
    _preferences.end();

    Serial.println("[AppConnector] Dati caricati dalla Flash:");
    Serial.printf("  - Espressione: %s\n", _currentExpression.c_str());
    Serial.printf("  - Task JSON: %s\n", _tasksJson.c_str());
}

void AppConnector::_savePersistedData() {
    _preferences.begin("robot_data", false); // Modalità Read-Write
    _preferences.putString("expression", _currentExpression);
    _preferences.putString("tasks", _tasksJson);
    _preferences.end();
    
    Serial.println("[AppConnector] Nuovi dati salvati in Flash con successo.");
}

void AppConnector::_handleApiUpdate() {
    // Abilita CORS per permettere chiamate dall'app
    _server.sendHeader("Access-Control-Allow-Origin", "*");

    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Body mancante\"}");
        return;
    }

    String body = _server.arg("plain");
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, body);
    if (error) {
        Serial.print(F("[AppConnector] JSON non valido: "));
        Serial.println(error.f_str());
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"JSON non valido\"}");
        return;
    }

    bool hasChanges = false;

    // Aggiorna Espressione
    if (doc.containsKey("expression")) {
        _currentExpression = doc["expression"].as<String>();
        hasChanges = true;
    }

    // Aggiorna Task List
    if (doc.containsKey("tasks")) {
        String newTasksJson;
        serializeJson(doc["tasks"], newTasksJson);
        _tasksJson = newTasksJson;
        hasChanges = true;
    }

    // Se ci sono stati cambiamenti, salva in Flash in modo permanente
    if (hasChanges) {
        _savePersistedData();
    }

    _server.send(200, "application/json", "{\"status\":\"success\"}");
}

void AppConnector::_handleNotFound() {
    // Gestione Pre-flight request CORS (chiamate HTTP OPTIONS)
    if (_server.method() == HTTP_OPTIONS) {
        _server.sendHeader("Access-Control-Allow-Origin", "*");
        _server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        _server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        _server.send(200);
    } else {
        _server.send(404, "text/plain", "Not Found");
    }
}

#include "AppConnector.h"

AppConnector::AppConnector(int port) : _server(port), _currentExpression("HAPPY") {}

void AppConnector::begin() {
    // Configurazione Endpoint POST /api/update
    _server.on("/api/update", HTTP_POST, [this]() {
        _handleApiUpdate();
    });

    // Gestione CORS e rotte non trovate
    _server.onNotFound([this]() {
        _handleNotFound();
    });

    _server.begin();
    Serial.println("[AppConnector] Server HTTP avviato sulla porta 80");
}

void AppConnector::handle() {
    _server.handleClient();
}

void AppConnector::_handleApiUpdate() {
    if (!_server.hasArg("plain")) {
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Body mancante\"}");
        return;
    }

    String body = _server.arg("plain");
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, body);
    if (error) {
        Serial.print(F("[AppConnector] Errore parsing JSON: "));
        Serial.println(error.f_str());
        _server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"JSON non valido\"}");
        return;
    }

    // 1. Lettura Espressione
    if (doc.containsKey("expression")) {
        _currentExpression = doc["expression"].as<String>();
        Serial.print("[AppConnector] Nuova Espressione: ");
        Serial.println(_currentExpression);
        
        // QUI: Se hai una tua funzione per aggiornare lo schermo o lo stato, puoi richiamarla
    }

    // 2. Lettura To-Do List
    if (doc.containsKey("tasks")) {
        JsonArray tasks = doc["tasks"].as<JsonArray>();
        Serial.println("[AppConnector] --- LISTA TASK ---");
        for (JsonObject task : tasks) {
            const char* title = task["title"];
            bool completed = task["completed"];
            Serial.printf("  [%s] %s\n", completed ? "X" : " ", title);
        }
    }

    _server.send(200, "application/json", "{\"status\":\"success\"}");
}

void AppConnector::_handleNotFound() {
    if (_server.method() == HTTP_OPTIONS) {
        _server.sendHeader("Access-Control-Allow-Origin", "*");
        _server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        _server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        _server.send(200);
    } else {
        _server.send(404, "text/plain", "Not Found");
    }
}

Emotion getEmotionFromApp(String expStr) {
  if (expStr == "FOCUS") return NEUTRAL; // O la tua emozione per il focus
  if (expStr == "SLEEP") return SLEEP;   // Se presente nel tuo enum
  if (expStr == "HAPPY") return HAPPY;
  return NEUTRAL;
}