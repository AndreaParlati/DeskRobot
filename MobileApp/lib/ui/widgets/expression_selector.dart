import 'package:flutter/material.dart';
import '../../models/robot_state.dart';

class ExpressionSelector extends StatelessWidget {
  final RobotExpression selectedExpression;
  final ValueChanged<RobotExpression> onExpressionSelected;

  const ExpressionSelector({
    super.key,
    required this.selectedExpression,
    required this.onExpressionSelected,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Text(
          "Espressione Facciale / Stato",
          style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
        ),
        const SizedBox(height: 8),
        Row(
          mainAxisAlignment: MainAxisAlignment.spaceAround,
          children: RobotExpression.values.map((exp) {
            return ChoiceChip(
              label: Text(exp.label),
              selected: selectedExpression == exp,
              onSelected: (selected) {
                if (selected) onExpressionSelected(exp);
              },
            );
          }).toList(),
        ),
      ],
    );
  }
}