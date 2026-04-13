# Smart Farming Monitoring System

## 1. Abstract
The Smart Farming Monitoring System is a simple college-level project that helps monitor important agricultural parameters such as soil moisture, temperature, humidity, and water level. The system is built using a C++ backend and an HTML/CSS/JavaScript frontend. The backend demonstrates object-oriented programming and run-time polymorphism through a common `Sensor` base class and multiple derived sensor classes. Data is stored in text files, which act as a lightweight database. The system supports complete record management (add, view, search, update, delete) for farms, crops, and sensors, and also generates alerts when sensor values are outside normal ranges.

## 2. Objectives
1. To design a basic smart farming monitoring solution.
2. To implement OOP concepts in C++ for real-world data handling.
3. To demonstrate run-time polymorphism using virtual functions.
4. To implement file handling as a simple database.
5. To provide CRUD operations for farm, crop, and sensor records.
6. To detect abnormal sensor values and display alerts.
7. To build a simple frontend dashboard for visual understanding.

## 3. Main Features
1. Farm management: add, view, search, update, delete.
2. Crop management: add, view, search, update, delete.
3. Sensor management: add, view, search, update, delete.
4. Sensor classes using inheritance:
   - `Sensor` (base class)
   - `MoistureSensor`
   - `TemperatureSensor`
   - `HumiditySensor`
   - `WaterLevelSensor`
5. Run-time polymorphism using virtual methods like `isAbnormal()`.
6. File-based database using `.txt` files.
7. Alert generation for abnormal sensor readings.
8. Frontend dashboard with forms, cards, alerts, and data table.

## 4. System Architecture
1. Presentation Layer:
   - `index.html`, `style.css`, `script.js`
   - Shows dashboard cards, forms, alerts, and table.
2. Application Layer (Backend Logic):
   - `main.cpp`
   - Menus for farm/crop/sensor management.
   - OOP + polymorphism logic for sensor behavior.
3. Data Layer:
   - `farms_db.txt`
   - `crops_db.txt`
   - `sensors_db.txt`
   - Stores records using file handling.

## 5. Explanation of Run-Time Polymorphism
Run-time polymorphism means that a base class pointer can call methods of derived classes, and the correct method is chosen at run time.

In this project:
1. `Sensor` is the base class.
2. `MoistureSensor`, `TemperatureSensor`, `HumiditySensor`, and `WaterLevelSensor` are derived classes.
3. The base class has virtual functions:
   - `getType()`
   - `isAbnormal()`
   - `getNormalRange()`
4. A factory function returns `unique_ptr<Sensor>`.
5. Depending on sensor type, the correct derived object is created.
6. Calling `sensor->isAbnormal()` uses the derived class implementation automatically.

This proves run-time polymorphism and keeps code clean and extensible.

## 6. Sample Workflow
1. Add farm records.
2. Add crop records linked to farms.
3. Add sensor readings linked to farm and crop IDs.
4. View/search/update/delete records.
5. Check alerts for abnormal values.

## 7. Viva Questions and Answers
1. What is the role of the base class `Sensor`?
   - It provides a common interface for all sensor types and enables polymorphism.

2. Why are virtual functions used?
   - To allow derived classes to override behavior and support run-time polymorphism.

3. How is run-time polymorphism shown in this project?
   - By using `unique_ptr<Sensor>` and calling overridden methods like `isAbnormal()`.

4. Why is file handling used here?
   - It provides simple persistent storage without requiring a full DBMS.

5. What CRUD operations are implemented?
   - Add, view, search, update, and delete for farm, crop, and sensor records.

6. How are abnormal values detected?
   - Each derived sensor class defines its normal range in `isAbnormal()`.

7. What happens if a new sensor type is added?
   - Create a new class derived from `Sensor` and update the factory function.

8. Difference between compile-time and run-time polymorphism?
   - Compile-time: function/operator overloading.
   - Run-time: overriding through virtual functions and base pointers.

9. What is the advantage of OOP in this project?
   - Better modularity, code reuse, and easier maintenance.

10. How does frontend help in this system?
   - It gives a user-friendly dashboard to enter data, view records, and monitor alerts.
