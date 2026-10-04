import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import '../../models/task_item.dart';

class TaskListWidget extends StatefulWidget {
  final List<TaskItem> tasks;
  final ValueChanged<List<TaskItem>> onTasksChanged;

  const TaskListWidget({
    super.key,
    required this.tasks,
    required this.onTasksChanged,
  });

  @override
  State<TaskListWidget> createState() => _TaskListWidgetState();
}

class _TaskListWidgetState extends State<TaskListWidget> {
  final TextEditingController _taskController = TextEditingController();
  static const String _storageKey = 'saved_tasks_list';

  @override
  void initState() {
    super.initState();
    _loadTasks();
  }

  @override
  void dispose() {
    _taskController.dispose();
    super.dispose();
  }

  // Carica le attività salvate nelle SharedPreferences
  Future<void> _loadTasks() async {
    final prefs = await SharedPreferences.getInstance();
    final String? tasksJson = prefs.getString(_storageKey);

    if (tasksJson != null) {
      final List<dynamic> decodedList = jsonDecode(tasksJson);
      final loadedTasks = decodedList
          .map((item) => TaskItem.fromJson(item as Map<String, dynamic>))
          .toList();
      widget.onTasksChanged(loadedTasks);
    }
  }

  // Salva la lista aggiornata nelle SharedPreferences
  Future<void> _saveTasks(List<TaskItem> tasks) async {
    final prefs = await SharedPreferences.getInstance();
    final String encodedList =
        jsonEncode(tasks.map((t) => t.toJson()).toList());
    await prefs.setString(_storageKey, encodedList);
  }

  void _notifyAndSave(List<TaskItem> updatedTasks) {
    widget.onTasksChanged(updatedTasks);
    _saveTasks(updatedTasks);
  }

  void _addTask() {
    final title = _taskController.text.trim();
    if (title.isNotEmpty) {
      final updatedTasks = List<TaskItem>.from(widget.tasks)
        ..add(TaskItem(
          id: DateTime.now().millisecondsSinceEpoch.toString(),
          title: title,
        ));
      _notifyAndSave(updatedTasks);
      _taskController.clear();
    }
  }

  void _toggleTask(int index) {
    final updatedTasks = List<TaskItem>.from(widget.tasks);
    updatedTasks[index].isCompleted = !updatedTasks[index].isCompleted;
    _notifyAndSave(updatedTasks);
  }

  void _deleteTask(int index) {
    final updatedTasks = List<TaskItem>.from(widget.tasks)..removeAt(index);
    _notifyAndSave(updatedTasks);
  }

  @override
  Widget build(BuildContext context) {
    const primaryColor = Color(0xFF6D597A);
    const accentColor = Color(0xFFB56576);
    const textColor = Color(0xFF355070);

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
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                const Row(
                  children: [
                    Text("🌸 ", style: TextStyle(fontSize: 18)),
                    Text(
                      "Lista Attività",
                      style: TextStyle(
                        fontSize: 18,
                        fontWeight: FontWeight.bold,
                        color: primaryColor,
                      ),
                    ),
                  ],
                ),
                Text(
                  "${widget.tasks.where((t) => t.isCompleted).length}/${widget.tasks.length}",
                  style: const TextStyle(
                    fontSize: 14,
                    fontWeight: FontWeight.w600,
                    color: accentColor,
                  ),
                ),
              ],
            ),
            const SizedBox(height: 12),
            Row(
              children: [
                Expanded(
                  child: TextField(
                    controller: _taskController,
                    decoration: InputDecoration(
                      hintText: "Nuova attività...",
                      hintStyle:
                          TextStyle(color: primaryColor.withOpacity(0.5)),
                      filled: true,
                      fillColor: const Color(0xFFFFF0F3),
                      contentPadding: const EdgeInsets.symmetric(
                        horizontal: 16,
                        vertical: 12,
                      ),
                      border: OutlineInputBorder(
                        borderRadius: BorderRadius.circular(12),
                        borderSide: BorderSide.none,
                      ),
                    ),
                    onSubmitted: (_) => _addTask(),
                  ),
                ),
                const SizedBox(width: 8),
                IconButton.filled(
                  onPressed: _addTask,
                  style: IconButton.styleFrom(
                    backgroundColor: accentColor,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(12),
                    ),
                  ),
                  icon: const Icon(Icons.add, color: Colors.white),
                ),
              ],
            ),
            const SizedBox(height: 12),
            if (widget.tasks.isEmpty)
              const Padding(
                padding: EdgeInsets.symmetric(vertical: 16.0),
                child: Center(
                  child: Text(
                    "Nessuna attività inserita 🌷",
                    style: TextStyle(color: Colors.grey),
                  ),
                ),
              )
            else
              ListView.builder(
                shrinkWrap: true,
                physics: const NeverScrollableScrollPhysics(),
                itemCount: widget.tasks.length,
                itemBuilder: (context, index) {
                  final task = widget.tasks[index];
                  return Container(
                    margin: const EdgeInsets.only(bottom: 8),
                    decoration: BoxDecoration(
                      color: task.isCompleted
                          ? const Color(0xFFFFF5F5)
                          : const Color(0xFFFFF0F3),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: ListTile(
                      dense: true,
                      leading: Checkbox(
                        value: task.isCompleted,
                        activeColor: accentColor,
                        shape: RoundedRectangleBorder(
                          borderRadius: BorderRadius.circular(4),
                        ),
                        onChanged: (_) => _toggleTask(index),
                      ),
                      title: Text(
                        task.title,
                        style: TextStyle(
                          color: task.isCompleted
                              ? textColor.withOpacity(0.5)
                              : textColor,
                          decoration: task.isCompleted
                              ? TextDecoration.lineThrough
                              : null,
                          fontWeight: task.isCompleted
                              ? FontWeight.normal
                              : FontWeight.w500,
                        ),
                      ),
                      trailing: IconButton(
                        icon: const Icon(
                          Icons.delete_outline,
                          color: Color(0xFFE56B6F),
                          size: 20,
                        ),
                        onPressed: () => _deleteTask(index),
                      ),
                    ),
                  );
                },
              ),
          ],
        ),
      ),
    );
  }
}