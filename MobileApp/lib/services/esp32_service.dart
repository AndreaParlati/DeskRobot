import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:shared_preferences/shared_preferences.dart';
import '../models/task_item.dart';

class Esp32Service {
  static const String _defaultHost = "deskrobot.local";
  static const String _prefHostKey = "esp32_host";

  // FLAG PER IL TEST: Imposta su true quando NON hai l'ESP32 con te
  static const bool isMockMode = true;

  static Future<String> getSavedHost() async {
    final prefs = await SharedPreferences.getInstance();
    return prefs.getString(_prefHostKey) ?? _defaultHost;
  }

  static Future<void> saveHost(String host) async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString(_prefHostKey, host.trim());
  }

  // 1. Invia Espressione e Task
  static Future<bool> updateRobotState({
    required String expression,
    required List<TaskItem> tasks,
  }) async {
    final host = await getSavedHost();
    
    final payload = {
      "expression": expression,
      "tasks": tasks.map((t) => t.toJson()).toList(),
    };

    // --- MOCK SIMULATO ---
    if (isMockMode) {
      print("\n[MOCK ESP32] Request POST -> http://$host/api/update");
      print("[MOCK ESP32] Payload inviato:\n${const JsonEncoder.withIndent('  ').convert(payload)}");
      
      // Simula il tempo di risposta della rete Wi-Fi (500 ms)
      await Future.delayed(const Duration(milliseconds: 500));
      
      print("[MOCK ESP32] Response Status: 200 OK (Successo simulato)\n");
      return true; // Ritorna success
    }

    // --- CHIAMATA REALE SU RETE ---
    final url = Uri.parse("http://$host/api/update");
    try {
      final response = await http
          .post(
            url,
            headers: {"Content-Type": "application/json"},
            body: jsonEncode(payload),
          )
          .timeout(const Duration(seconds: 4));

      return response.statusCode == 200;
    } catch (e) {
      print("[Esp32Service] Errore connessione: $e");
      return false;
    }
  }

  // 2. Invia Credenziali Wi-Fi (Setup AP)
  static Future<bool> sendWifiCredentials({
    required String ssid,
    required String password,
  }) async {
    final payload = {"ssid": ssid, "pass": password};

    if (isMockMode) {
      print("\n[MOCK ESP32] Request POST -> http://192.168.4.1/api/wifi");
      print("[MOCK ESP32] Setup Wi-Fi con SSID: '$ssid'");
      
      await Future.delayed(const Duration(milliseconds: 1000));
      
      print("[MOCK ESP32] Response Status: 200 OK (WiFi Salvato)\n");
      return true;
    }

    final url = Uri.parse("http://192.168.4.1/api/wifi");
    try {
      final response = await http
          .post(
            url,
            headers: {"Content-Type": "application/json"},
            body: jsonEncode(payload),
          )
          .timeout(const Duration(seconds: 5));

      return response.statusCode == 200;
    } catch (e) {
      print("[Esp32Service] Errore invio Wi-Fi AP: $e");
      return false;
    }
  }
}