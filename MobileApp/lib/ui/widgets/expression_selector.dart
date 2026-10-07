import 'package:flutter/material.dart';
import '../../models/robot_state.dart';

class CurrentEmotionCard extends StatelessWidget {
  final RobotExpression currentExpression;

  const CurrentEmotionCard({
    super.key,
    required this.currentExpression,
  });

  Map<String, String> _getEmotionDetails(RobotExpression expression) {
    switch (expression) {
      case RobotExpression.happy:
        return {'label': 'Felice', 'emoji': '😊', 'desc': 'Il robot è contento e pronto!'};
      case RobotExpression.dizzy:
        return {'label': 'Capogiro', 'emoji': '😵', 'desc': 'Il robot ha la testa che gira!'};
      case RobotExpression.sleepy:
        return {'label': 'Assonnato', 'emoji': '😴', 'desc': 'Il robot sta per addormentarsi...'};
      case RobotExpression.neutral:
      default:
        return {'label': 'Neutro', 'emoji': '😐', 'desc': 'Il robot è tranquillo e a riposo.'};
    }
  }

  @override
  Widget build(BuildContext context) {
    final details = _getEmotionDetails(currentExpression);
    const primaryColor = Color(0xFF6D597A);
    const accentColor = Color(0xFFB56576);

    return Card(
      elevation: 0,
      color: Colors.white.withOpacity(0.9),
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(20),
        side: BorderSide(
          color: const Color(0xFFFFB5A7).withOpacity(0.5),
          width: 1.5,
        ),
      ),
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Row(
              children: [
                Text("🌸 ", style: TextStyle(fontSize: 18)),
                Text(
                  "Stato Emotivo Robot",
                  style: TextStyle(
                    fontSize: 18,
                    fontWeight: FontWeight.bold,
                    color: primaryColor,
                  ),
                ),
              ],
            ),
            const SizedBox(height: 12),
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: const Color(0xFFFFF0F3),
                borderRadius: BorderRadius.circular(16),
              ),
              child: Row(
                children: [
                  Text(
                    details['emoji']!,
                    style: const TextStyle(fontSize: 36),
                  ),
                  const SizedBox(width: 16),
                  Expanded(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          details['label']!,
                          style: const TextStyle(
                            fontSize: 18,
                            fontWeight: FontWeight.bold,
                            color: accentColor,
                          ),
                        ),
                        const SizedBox(height: 2),
                        Text(
                          details['desc']!,
                          style: TextStyle(
                            fontSize: 13,
                            color: primaryColor.withOpacity(0.8),
                          ),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}