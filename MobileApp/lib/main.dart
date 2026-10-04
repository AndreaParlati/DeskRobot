import 'package:flutter/material.dart';
import 'ui/screens/home_screen.dart';

void main() {
  runApp(const DeskRobotApp());
}

class DeskRobotApp extends StatelessWidget {
  const DeskRobotApp({super.key});

  @override
  Widget build(BuildContext context) {
    final Color primarySeed = const Color(0xFFE5989B);
    final Color backgroundColor = const Color(0xFFFFF5F5);

    return MaterialApp(
      title: 'DeskRobot Controller',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        useMaterial3: true,
        colorScheme: ColorScheme.fromSeed(
          seedColor: primarySeed,
          brightness: Brightness.light,
          surface: Colors.white,
          primary: const Color(0xFFB56576),
          secondary: const Color(0xFFE5989B),
        ),
        scaffoldBackgroundColor: backgroundColor,
        
        // Sostituito CardTheme(...) con CardThemeData(...)
        cardTheme: CardThemeData(
          color: Colors.white.withOpacity(0.9),
          elevation: 2,
          shadowColor: const Color(0xFFE5989B).withOpacity(0.2),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(20),
          ),
        ),

        appBarTheme: const AppBarTheme(
          backgroundColor: Colors.transparent,
          elevation: 0,
          centerTitle: true,
          titleTextStyle: TextStyle(
            color: Color(0xFF6D597A),
            fontSize: 22,
            fontWeight: FontWeight.bold,
            letterSpacing: 0.5,
          ),
          iconTheme: IconThemeData(color: Color(0xFF6D597A)),
        ),

        elevatedButtonTheme: ElevatedButtonThemeData(
          style: ElevatedButton.styleFrom(
            backgroundColor: const Color(0xFFB56576),
            foregroundColor: Colors.white,
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(16),
            ),
            padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 12),
          ),
        ),
      ),
      home: const HomeScreen(),
    );
  }
}