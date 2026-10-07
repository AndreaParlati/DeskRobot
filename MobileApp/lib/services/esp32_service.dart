import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:shared_preferences/shared_preferences.dart';
import '../models/task_item.dart';
import '../models/robot_state.dart';

class Esp32Service {
  static const String _hostKey = 'esp32_host_ip';
  static const String _defaultHost = 'deskrobot.local';
  static bool isMockMode = true; // Impostare a false quando ci si connette all'ESP32 reale

  static Future<String> getSavedHost() async {
    final prefs = await SharedPreferences.getInstance();
    return prefs.getString(_hostKey) ?? _defaultHost;
  }

  static Future<void> saveHost(String host) async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString(_hostKey, host);
  }

  // Sincronizza i task con l'ESP32 e riceve l'emozione corrente del robot
  static Future<RobotExpression?> updateRobotState({
    required List<TaskItem> tasks,
  }) async {
    final host = await getSavedHost();

    if (isMockMode) {
      print('[MOCK ESP32] Request POST -> http://$host/api/update');
      print('[MOCK ESP32] Sending tasks: ${tasks.length} items');
      await Future.delayed(const Duration(seconds: 1));
      
      // In modalità MOCK simula una risposta dove il robot restituisce un'emozione
      return RobotExpression.happy;
    }

    try {
      final url = Uri.parse('http://$host/api/update');
      final body = jsonEncode({
        'tasks': tasks.map((t) => t.toJson()).toList(),
      });

      final response = await http
          .post(
            url,
            headers: {'Content-Type': 'application/json'},
            body: body,
          )
          .timeout(const Duration(seconds: 5));

      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        final String? emotionStr = data['current_expression'] ?? data['expression'];
        if (emotionStr != null) {
          return _parseExpression(emotionStr);
        }
        return RobotExpression.neutral;
      }
      return null;
    } catch (e) {
      print('Errore connessione ESP32: $e');
      return null;
    }
  }

  static Future<bool> sendWifiCredentials({
    required String ssid,
    required String password,
  }) async {
    final host = await getSavedHost();

    if (isMockMode) {
      print('[MOCK ESP32] Sending Wifi SSID: $ssid');
      await Future.delayed(const Duration(seconds: 1));
      return true;
    }

    try {
      final url = Uri.parse('http://$host/api/wifi');
      final response = await http
          .post(
            url,
            headers: {'Content-Type': 'application/json'},
            body: jsonEncode({'ssid': ssid, 'password': password}),
          )
          .timeout(const Duration(seconds: 5));

      return response.statusCode == 200;
    } catch (e) {
      print('Errore invio credenziali Wi-Fi: $e');
      return false;
    }
  }

  static RobotExpression _parseExpression(String str) {
    switch (str.toUpperCase()) {
      case 'HAPPY':
        return RobotExpression.happy;
      case 'DIZZY':
        return RobotExpression.dizzy;
      case 'SLEEPY':
        return RobotExpression.sleepy;
      case 'NEUTRAL':
      default:
        return RobotExpression.neutral;
    }
  }
}