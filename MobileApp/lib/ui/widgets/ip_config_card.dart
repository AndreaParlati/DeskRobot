import 'package:flutter/material.dart';
import '../../services/esp32_service.dart';

class IpConfigCard extends StatelessWidget {
  final TextEditingController controller;
  final VoidCallback onSetupWifiPressed;

  const IpConfigCard({
    Key? key,
    required this.controller,
    required this.onSetupWifiPressed,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 2,
      child: Padding(
        padding: const EdgeInsets.all(12.0),
        child: Row(
          children: [
            Expanded(
              child: TextField(
                controller: controller,
                decoration: const InputDecoration(
                  labelText: "Indirizzo Robot / Host",
                  hintText: "deskrobot.local",
                  prefixIcon: Icon(Icons.dns),
                  border: OutlineInputBorder(),
                  isDense: true,
                ),
                onChanged: (value) {
                  // Salva automaticamente l'host appena l'utente lo digita
                  Esp32Service.saveHost(value);
                },
              ),
            ),
            const SizedBox(width: 8),
            IconButton(
              icon: const Icon(Icons.wifi_find),
              tooltip: "Prima Configurazione Wi-Fi",
              onPressed: onSetupWifiPressed,
            ),
          ],
        ),
      ),
    );
  }
}