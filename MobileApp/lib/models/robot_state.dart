import 'task_item.dart';

enum RobotExpression { happy, focus, sleep }

extension RobotExpressionExtension on RobotExpression {
  String get code {
    switch (this) {
      case RobotExpression.happy:
        return 'HAPPY';
      case RobotExpression.focus:
        return 'FOCUS';
      case RobotExpression.sleep:
        return 'SLEEP';
    }
  }

  String get label {
    switch (this) {
      case RobotExpression.happy:
        return '😊 Felice';
      case RobotExpression.focus:
        return '🧐 Studio';
      case RobotExpression.sleep:
        return '😴 Sonno';
    }
  }
}

class RobotState {
  RobotExpression expression;
  List<TaskItem> tasks;

  RobotState({
    this.expression = RobotExpression.happy,
    List<TaskItem>? tasks,
  }) : tasks = tasks ?? [];

  Map<String, dynamic> toJson() => {
        'expression': expression.code,
        'tasks': tasks.map((t) => t.toJson()).toList(),
      };
}