import 'task_item.dart';

enum RobotExpression { neutral, happy, dizzy, sleepy }

extension RobotExpressionExtension on RobotExpression {
  String get code {
    switch (this) {
      case RobotExpression.happy:
        return 'HAPPY';
      case RobotExpression.neutral:
        return 'NEUTRAL';
      case RobotExpression.sleepy:
        return 'SLEEPY';
      case RobotExpression.dizzy:
        return 'DIZZY';
    }
  }

  String get label {
    switch (this) {
      case RobotExpression.happy:
        return '😊 Felice';
      case RobotExpression.neutral:
        return '🙂 Neutrale';
      case RobotExpression.sleepy:
        return '😴 Sonno';
      case RobotExpression.dizzy:
        return '🤢 Nauseato';
    }
  }
}

class RobotState {
  RobotExpression expression;
  List<TaskItem> tasks;

  RobotState({
    this.expression = RobotExpression.neutral,
    List<TaskItem>? tasks,
  }) : tasks = tasks ?? [];

  Map<String, dynamic> toJson() => {
        'expression': expression.code,
        'tasks': tasks.map((t) => t.toJson()).toList(),
      };
}