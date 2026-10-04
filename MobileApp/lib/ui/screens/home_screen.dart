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

  RobotExpression _selectedExpression = RobotExpression.values.first;
  bool _isSyncing = false;

  List<TaskItem> _tasks = [
    TaskItem(id: '1', title: 'Saluta il tuo nuovo amico :)')
  ];

  @override
  void initState() {
    super.initState();
    _loadSavedHost();
  }

  @override
  void dispose() {
    _hostController.dispose();
    _ssidController.dispose();
    _passwordController.dispose();
    super.dispose();
  }

  Future<void> _loadSavedHost() async {
    String host = await Esp32Service.getSavedHost();
    setState(() {
      _hostController.text = host;
    });
  }

  Future<void> _syncWithRobot() async {
    setState(() {
      _isSyncing = true;
    });

    final currentHost = _hostController.text.trim();
    if (currentHost.isNotEmpty) {
      await Esp32Service.saveHost(currentHost);
    }

    // Convertiamo l'enum RobotExpression in String (.name) per Esp32Service
    bool success = await Esp32Service.updateRobotState(
      expression: _selectedExpression.name,
      tasks: _tasks,
    );

    setState(() {
      _isSyncing = false;
    });

    if (!mounted) return;

    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text(
          success
              ? 'Sincronizzazione completata con successo! 🌸'
              : 'Errore di connessione con il DeskRobot 😞',
        ),
        backgroundColor: success ? const Color(0xFFB56576) : Colors.redAccent,
        duration: const Duration(seconds: 2),
      ),
    );
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
              onPressed: _syncWithRobot,
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
                )
              ),
              const SizedBox(height: 16),

              IpConfigCard(
                controller: _hostController,
                onSetupWifiPressed: _openWifiDialog,
              ),
              const SizedBox(height: 20),

              ExpressionSelector(
                selectedExpression: _selectedExpression,
                onExpressionSelected: (newExp) {
                  setState(() {
                    _selectedExpression = newExp;
                  });
                },
              ),
              const SizedBox(height: 20),

              TaskListWidget(
                tasks: _tasks,
                onTasksChanged: (newTasks) {
                  setState(() {
                    _tasks = newTasks;
                  });
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