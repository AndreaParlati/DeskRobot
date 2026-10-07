import 'dart:async';
import 'package:flutter/material.dart';
import '../../models/task_item.dart';
import '../../models/robot_state.dart';
import '../../services/esp32_service.dart';
import '../widgets/expression_selector.dart';
import '../widgets/ip_config_card.dart';
import '../widgets/task_list_widget.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  final TextEditingController _hostController = TextEditingController();
  final TextEditingController _ssidController = TextEditingController();
  final TextEditingController _passwordController = TextEditingController();

  RobotExpression _currentExpression = RobotExpression.neutral;
  bool _isSyncing = false;
  Timer? _autoSyncTimer; // Timer per il polling automatico

  List<TaskItem> _tasks = [];

  @override
  void initState() {
    super.initState();
    _loadSavedHost();
    
    // 2. Avvia la sincronizzazione automatica ogni 5 secondi
    _startAutoSync();
  }

  @override
  void dispose() {
    // 3. Cancella sempre il timer prima di distruggere il widget
    _autoSyncTimer?.cancel();
    _hostController.dispose();
    _ssidController.dispose();
    _passwordController.dispose();
    super.dispose();
  }

  void _startAutoSync() {
    _autoSyncTimer = Timer.periodic(const Duration(seconds: 5), (timer) {
      // Sincronizza in background senza mostrare la SnackBar di notifica
      _syncWithRobot(showSnackBar: false);
    });
  }

  Future<void> _loadSavedHost() async {
    String host = await Esp32Service.getSavedHost();
    setState(() {
      _hostController.text = host;
    });
  }

  // Modificato per accettare un parametro opzionale showSnackBar (default: true per il tasto manuale)
  Future<void> _syncWithRobot({bool showSnackBar = true}) async {
    // Evita di accavallare più richieste se una è già in corso
    if (_isSyncing) return;

    setState(() {
      _isSyncing = true;
    });

    final currentHost = _hostController.text.trim();
    if (currentHost.isNotEmpty) {
      await Esp32Service.saveHost(currentHost);
    }

    RobotExpression? updatedExpression = await Esp32Service.updateRobotState(
      tasks: _tasks,
    );

    if (!mounted) return;

    setState(() {
      _isSyncing = false;
      if (updatedExpression != null) {
        _currentExpression = updatedExpression;
      }
    });

    // Mostra la SnackBar solo se richiesto (es. quando si preme il pulsante manuale)
    if (showSnackBar) {
      final bool success = updatedExpression != null;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(
            success
                ? 'Sincronizzazione completata! Stato robot aggiornato 🌸'
                : 'Errore di connessione con il DeskRobot 😞',
          ),
          backgroundColor: success ? const Color(0xFFB56576) : Colors.redAccent,
          duration: const Duration(seconds: 2),
        ),
      );
    }
  }

  void _openWifiDialog() {
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(20),
          ),
          title: const Row(
            children: [
              Text("🌸 "),
              Text(
                "Configura Wi-Fi ESP32",
                style: TextStyle(color: Color(0xFF6D597A), fontSize: 18),
              ),
            ],
          ),
          content: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              TextField(
                controller: _ssidController,
                decoration: InputDecoration(
                  labelText: "Nome Rete (SSID)",
                  prefixIcon: const Icon(Icons.wifi, color: Color(0xFFB56576)),
                  filled: true,
                  fillColor: const Color(0xFFFFF0F3),
                  border: OutlineInputBorder(
                    borderRadius: BorderRadius.circular(12),
                    borderSide: BorderSide.none,
                  ),
                ),
              ),
              const SizedBox(height: 12),
              TextField(
                controller: _passwordController,
                obscureText: true,
                decoration: InputDecoration(
                  labelText: "Password",
                  prefixIcon: const Icon(Icons.lock_outline, color: Color(0xFFB56576)),
                  filled: true,
                  fillColor: const Color(0xFFFFF0F3),
                  border: OutlineInputBorder(
                    borderRadius: BorderRadius.circular(12),
                    borderSide: BorderSide.none,
                  ),
                ),
              ),
            ],
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text("Annulla", style: TextStyle(color: Colors.grey)),
            ),
            ElevatedButton(
              onPressed: () async {
                Navigator.pop(context);
                final ssid = _ssidController.text.trim();
                final pass = _passwordController.text.trim();

                if (ssid.isNotEmpty) {
                  bool success = await Esp32Service.sendWifiCredentials(
                    ssid: ssid,
                    password: pass,
                  );
                  if (!mounted) return;
                  ScaffoldMessenger.of(context).showSnackBar(
                    SnackBar(
                      content: Text(
                        success
                            ? 'Credenziali inviate! Il robot si sta connettendo...'
                            : 'Errore nell\'invio delle credenziali.',
                      ),
                      backgroundColor: success ? const Color(0xFFB56576) : Colors.redAccent,
                    ),
                  );
                }
              },
              child: const Text("Invia"),
            ),
          ],
        );
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: const BoxDecoration(
        gradient: LinearGradient(
          colors: [
            Color(0xFFFFF0F3),
            Color(0xFFFFCCD5),
          ],
          begin: Alignment.topCenter,
          end: Alignment.bottomCenter,
        ),
      ),
      child: Scaffold(
        backgroundColor: Colors.transparent,
        appBar: AppBar(
          title: const Row(
            mainAxisSize: MainAxisSize.min,
            children: [
              Text("🌸 "),
              Text("DeskRobot"),
              Text(" 🌸"),
            ],
          ),
          actions: [
            IconButton(
              icon: _isSyncing
                  ? const SizedBox(
                      width: 20,
                      height: 20,
                      child: CircularProgressIndicator(
                        color: Color(0xFFB56576),
                        strokeWidth: 2,
                      ),
                    )
                  : const Icon(Icons.sync_rounded),
              tooltip: "Sincronizza Robot",
              onPressed: () => _syncWithRobot(showSnackBar: true),
            )
          ],
        ),
        body: SingleChildScrollView(
          padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 8.0),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Container(
                width: double.infinity,
                padding: const EdgeInsets.all(16),
                decoration: BoxDecoration(
                  color: Colors.white.withOpacity(0.8),
                  borderRadius: BorderRadius.circular(20),
                  border: Border.all(
                    color: const Color(0xFFFFB5A7).withOpacity(0.5),
                    width: 1.5,
                  ),
                ),
                child: const Row(
                  children: [
                    Text("🌺", style: TextStyle(fontSize: 28)),
                    SizedBox(width: 12),
                    Expanded(
                      child: Text(
                        "Gestisci il tuo DeskRobot in un ambiente fiorito!",
                        style: TextStyle(
                          color: Color(0xFF6D597A),
                          fontWeight: FontWeight.w600,
                          fontSize: 14,
                        ),
                      ),
                    ),
                  ],
                ),
              ),
              const SizedBox(height: 16),

              IpConfigCard(
                controller: _hostController,
                onSetupWifiPressed: _openWifiDialog,
              ),
              const SizedBox(height: 20),

              CurrentEmotionCard(
                currentExpression: _currentExpression,
              ),
              const SizedBox(height: 20),

              TaskListWidget(
                tasks: _tasks,
                onTasksChanged: (newTasks) {
                  setState(() {
                    _tasks = newTasks;
                  });
                  // Sincronizza subito con l'ESP32 quando l'utente modifica la lista dei task
                  _syncWithRobot(showSnackBar: false);
                },
              ),
              const SizedBox(height: 24),
            ],
          ),
        ),
      ),
    );
  }
}