import 'package:flutter/material.dart';
import '../../services/esp32_service.dart';

class WifiSetupDialog extends StatefulWidget {
  const WifiSetupDialog({Key? key}) : super(key: key);

  @override
  State<WifiSetupDialog> createState() => _WifiSetupDialogState();
}

class _WifiSetupDialogState extends State<WifiSetupDialog> {
  final _ssidController = TextEditingController();
  final _passController = TextEditingController();
  bool _isLoading = false;

  void _submitWifi() async {
    if (_ssidController.text.trim().isEmpty) return;

    setState(() => _isLoading = true);

    bool success = await Esp32Service.sendWifiCredentials(
      ssid: _ssidController.text.trim(),
      password: _passController.text,
    );

    setState(() => _isLoading = false);

    if (mounted) {
      if (success) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(content: Text("Wi-Fi inviato! Il robot si sta riavviando...")),
        );
        Navigator.pop(context);
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text("Errore! Assicurati di essere connesso alla rete Wi-Fi 'DeskRobot-Setup'"),
            backgroundColor: Colors.redAccent,
          ),
        );
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      title: const Text("Setup Wi-Fi Robot"),
      content: SingleChildScrollView(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text(
              "1. Connettiti alla rete Wi-Fi 'DeskRobot-Setup' dallo smartphone.\n"
              "2. Inserisci i dati del tuo Wi-Fi di casa:",
              style: TextStyle(fontSize: 13, color: Colors.grey),
            ),
            const SizedBox(height: 16),
            TextField(
              controller: _ssidController,
              decoration: const InputDecoration(
                labelText: "Nome Rete (SSID)",
                border: OutlineInputBorder(),
                isDense: true,
              ),
            ),
            const SizedBox(height: 12),
            TextField(
              controller: _passController,
              obscureText: true,
              decoration: const InputDecoration(
                labelText: "Password Wi-Fi",
                border: OutlineInputBorder(),
                isDense: true,
              ),
            ),
          ],
        ),
      ),
      actions: [
        TextButton(
          onPressed: () => Navigator.pop(context),
          child: const Text("Annulla"),
        ),
        ElevatedButton(
          onPressed: _isLoading ? null : _submitWifi,
          child: _isLoading
              ? const SizedBox(width: 16, height: 16, child: CircularProgressIndicator(strokeWidth: 2))
              : const Text("Invia e Riavvia"),
        ),
      ],
    );
  }
}