class TaskItem {
  final String id;
  String title;
  bool isCompleted;

  TaskItem({
    required this.id,
    required this.title,
    this.isCompleted = false,
  });

  Map<String, dynamic> toJson() => {
        'id': id,
        'title': title,
        'completed': isCompleted,
      };

  factory TaskItem.fromJson(Map<String, dynamic> json) => TaskItem(
        id: json['id'] as String,
        title: json['title'] as String,
        isCompleted: json['completed'] as bool? ?? false,
      );
}