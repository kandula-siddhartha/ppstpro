#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ------------------------------
// Data files (file-based database)
// ------------------------------
const string FARM_DB = "farms_db.txt";
const string CROP_DB = "crops_db.txt";
const string SENSOR_DB = "sensors_db.txt";

// ------------------------------
// Utility helpers
// ------------------------------
vector<string> split(const string &line, char delimiter) {
    vector<string> parts;
    string token;
    stringstream ss(line);
    while (getline(ss, token, delimiter)) {
        parts.push_back(token);
    }
    return parts;
}

string nowDateTime() {
    time_t t = time(nullptr);
    tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &t);
#else
    localtime_r(&t, &localTime);
#endif
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return string(buffer);
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ------------------------------
// Base class for run-time polymorphism
// ------------------------------
class Sensor {
  protected:
    string sensorId;
    string location;
    double value;

  public:
    Sensor(string id, string loc, double val) : sensorId(id), location(loc), value(val) {}
    virtual ~Sensor() = default;

    string getSensorId() const { return sensorId; }
    string getLocation() const { return location; }
    double getValue() const { return value; }

    // Virtual methods for run-time polymorphism
    virtual string getType() const = 0;
    virtual bool isAbnormal() const = 0;
    virtual string getNormalRange() const = 0;
};

class MoistureSensor : public Sensor {
  public:
    MoistureSensor(string id, string loc, double val) : Sensor(id, loc, val) {}

    string getType() const override { return "Moisture"; }
    bool isAbnormal() const override { return value < 20.0 || value > 80.0; }
    string getNormalRange() const override { return "20 - 80 %"; }
};

class TemperatureSensor : public Sensor {
  public:
    TemperatureSensor(string id, string loc, double val) : Sensor(id, loc, val) {}

    string getType() const override { return "Temperature"; }
    bool isAbnormal() const override { return value < 15.0 || value > 35.0; }
    string getNormalRange() const override { return "15 - 35 C"; }
};

class HumiditySensor : public Sensor {
  public:
    HumiditySensor(string id, string loc, double val) : Sensor(id, loc, val) {}

    string getType() const override { return "Humidity"; }
    bool isAbnormal() const override { return value < 30.0 || value > 85.0; }
    string getNormalRange() const override { return "30 - 85 %"; }
};

class WaterLevelSensor : public Sensor {
  public:
    WaterLevelSensor(string id, string loc, double val) : Sensor(id, loc, val) {}

    string getType() const override { return "WaterLevel"; }
    bool isAbnormal() const override { return value < 25.0 || value > 90.0; }
    string getNormalRange() const override { return "25 - 90 %"; }
};

// Factory function creates derived object but returns base class pointer
unique_ptr<Sensor> createSensor(const string &sensorType, const string &id, const string &location, double value) {
    if (sensorType == "Moisture")
        return make_unique<MoistureSensor>(id, location, value);
    if (sensorType == "Temperature")
        return make_unique<TemperatureSensor>(id, location, value);
    if (sensorType == "Humidity")
        return make_unique<HumiditySensor>(id, location, value);
    if (sensorType == "WaterLevel")
        return make_unique<WaterLevelSensor>(id, location, value);

    return nullptr;
}

// ------------------------------
// Records for file storage
// ------------------------------
struct Farm {
    string id;
    string name;
    string location;
};

struct Crop {
    string id;
    string farmId;
    string cropName;
    string season;
};

struct SensorRecord {
    string sensorId;
    string farmId;
    string cropId;
    string sensorType;
    double value;
    string location;
    string timestamp;
};

// ------------------------------
// Farm management (CRUD)
// ------------------------------
vector<Farm> loadFarms() {
    vector<Farm> farms;
    ifstream file(FARM_DB);
    string line;

    while (getline(file, line)) {
        if (line.empty())
            continue;
        vector<string> p = split(line, '|');
        if (p.size() == 3) {
            farms.push_back({p[0], p[1], p[2]});
        }
    }
    return farms;
}

void saveFarms(const vector<Farm> &farms) {
    ofstream file(FARM_DB, ios::trunc);
    for (const auto &f : farms) {
        file << f.id << '|' << f.name << '|' << f.location << '\n';
    }
}

void addFarm() {
    Farm f;
    cout << "\nEnter Farm ID: ";
    getline(cin, f.id);
    cout << "Enter Farm Name: ";
    getline(cin, f.name);
    cout << "Enter Farm Location: ";
    getline(cin, f.location);

    ofstream file(FARM_DB, ios::app);
    file << f.id << '|' << f.name << '|' << f.location << '\n';

    cout << "Farm added successfully.\n";
}

void viewFarms() {
    vector<Farm> farms = loadFarms();
    if (farms.empty()) {
        cout << "\nNo farm records found.\n";
        return;
    }

    cout << "\n---------------- FARM RECORDS ----------------\n";
    cout << left << setw(12) << "Farm ID" << setw(25) << "Farm Name" << setw(25) << "Location" << '\n';
    cout << string(62, '-') << '\n';
    for (const auto &f : farms) {
        cout << left << setw(12) << f.id << setw(25) << f.name << setw(25) << f.location << '\n';
    }
}

void searchFarm() {
    string id;
    cout << "\nEnter Farm ID to search: ";
    getline(cin, id);

    vector<Farm> farms = loadFarms();
    for (const auto &f : farms) {
        if (f.id == id) {
            cout << "Found: " << f.id << " | " << f.name << " | " << f.location << '\n';
            return;
        }
    }
    cout << "Farm not found.\n";
}

void updateFarm() {
    string id;
    cout << "\nEnter Farm ID to update: ";
    getline(cin, id);

    vector<Farm> farms = loadFarms();
    bool found = false;

    for (auto &f : farms) {
        if (f.id == id) {
            cout << "Enter new Farm Name: ";
            getline(cin, f.name);
            cout << "Enter new Farm Location: ";
            getline(cin, f.location);
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Farm not found.\n";
        return;
    }

    saveFarms(farms);
    cout << "Farm updated successfully.\n";
}

void deleteFarm() {
    string id;
    cout << "\nEnter Farm ID to delete: ";
    getline(cin, id);

    vector<Farm> farms = loadFarms();
    size_t oldSize = farms.size();

    farms.erase(remove_if(farms.begin(), farms.end(), [&](const Farm &f) { return f.id == id; }), farms.end());

    if (farms.size() == oldSize) {
        cout << "Farm not found.\n";
        return;
    }

    saveFarms(farms);
    cout << "Farm deleted successfully.\n";
}

// ------------------------------
// Crop management (CRUD)
// ------------------------------
vector<Crop> loadCrops() {
    vector<Crop> crops;
    ifstream file(CROP_DB);
    string line;

    while (getline(file, line)) {
        if (line.empty())
            continue;
        vector<string> p = split(line, '|');
        if (p.size() == 4) {
            crops.push_back({p[0], p[1], p[2], p[3]});
        }
    }
    return crops;
}

void saveCrops(const vector<Crop> &crops) {
    ofstream file(CROP_DB, ios::trunc);
    for (const auto &c : crops) {
        file << c.id << '|' << c.farmId << '|' << c.cropName << '|' << c.season << '\n';
    }
}

void addCrop() {
    Crop c;
    cout << "\nEnter Crop ID: ";
    getline(cin, c.id);
    cout << "Enter Farm ID: ";
    getline(cin, c.farmId);
    cout << "Enter Crop Name: ";
    getline(cin, c.cropName);
    cout << "Enter Season: ";
    getline(cin, c.season);

    ofstream file(CROP_DB, ios::app);
    file << c.id << '|' << c.farmId << '|' << c.cropName << '|' << c.season << '\n';

    cout << "Crop added successfully.\n";
}

void viewCrops() {
    vector<Crop> crops = loadCrops();
    if (crops.empty()) {
        cout << "\nNo crop records found.\n";
        return;
    }

    cout << "\n---------------- CROP RECORDS ----------------\n";
    cout << left << setw(12) << "Crop ID" << setw(12) << "Farm ID" << setw(25) << "Crop Name" << setw(15) << "Season" << '\n';
    cout << string(64, '-') << '\n';
    for (const auto &c : crops) {
        cout << left << setw(12) << c.id << setw(12) << c.farmId << setw(25) << c.cropName << setw(15) << c.season << '\n';
    }
}

void searchCrop() {
    string id;
    cout << "\nEnter Crop ID to search: ";
    getline(cin, id);

    vector<Crop> crops = loadCrops();
    for (const auto &c : crops) {
        if (c.id == id) {
            cout << "Found: " << c.id << " | Farm: " << c.farmId << " | " << c.cropName << " | " << c.season << '\n';
            return;
        }
    }
    cout << "Crop not found.\n";
}

void updateCrop() {
    string id;
    cout << "\nEnter Crop ID to update: ";
    getline(cin, id);

    vector<Crop> crops = loadCrops();
    bool found = false;

    for (auto &c : crops) {
        if (c.id == id) {
            cout << "Enter new Farm ID: ";
            getline(cin, c.farmId);
            cout << "Enter new Crop Name: ";
            getline(cin, c.cropName);
            cout << "Enter new Season: ";
            getline(cin, c.season);
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Crop not found.\n";
        return;
    }

    saveCrops(crops);
    cout << "Crop updated successfully.\n";
}

void deleteCrop() {
    string id;
    cout << "\nEnter Crop ID to delete: ";
    getline(cin, id);

    vector<Crop> crops = loadCrops();
    size_t oldSize = crops.size();

    crops.erase(remove_if(crops.begin(), crops.end(), [&](const Crop &c) { return c.id == id; }), crops.end());

    if (crops.size() == oldSize) {
        cout << "Crop not found.\n";
        return;
    }

    saveCrops(crops);
    cout << "Crop deleted successfully.\n";
}

// ------------------------------
// Sensor management (CRUD + alerts)
// ------------------------------
vector<SensorRecord> loadSensors() {
    vector<SensorRecord> records;
    ifstream file(SENSOR_DB);
    string line;

    while (getline(file, line)) {
        if (line.empty())
            continue;
        vector<string> p = split(line, '|');
        if (p.size() == 7) {
            SensorRecord r;
            r.sensorId = p[0];
            r.farmId = p[1];
            r.cropId = p[2];
            r.sensorType = p[3];
            r.value = stod(p[4]);
            r.location = p[5];
            r.timestamp = p[6];
            records.push_back(r);
        }
    }
    return records;
}

void saveSensors(const vector<SensorRecord> &records) {
    ofstream file(SENSOR_DB, ios::trunc);
    for (const auto &r : records) {
        file << r.sensorId << '|' << r.farmId << '|' << r.cropId << '|' << r.sensorType << '|' << r.value << '|' << r.location
             << '|' << r.timestamp << '\n';
    }
}

void addSensorRecord() {
    SensorRecord r;
    cout << "\nEnter Sensor ID: ";
    getline(cin, r.sensorId);
    cout << "Enter Farm ID: ";
    getline(cin, r.farmId);
    cout << "Enter Crop ID: ";
    getline(cin, r.cropId);

    cout << "Sensor Types: Moisture, Temperature, Humidity, WaterLevel\n";
    cout << "Enter Sensor Type: ";
    getline(cin, r.sensorType);

    cout << "Enter Sensor Value: ";
    cin >> r.value;
    clearInput();

    cout << "Enter Sensor Location: ";
    getline(cin, r.location);

    r.timestamp = nowDateTime();

    unique_ptr<Sensor> sensor = createSensor(r.sensorType, r.sensorId, r.location, r.value);
    if (!sensor) {
        cout << "Invalid sensor type. Record not added.\n";
        return;
    }

    ofstream file(SENSOR_DB, ios::app);
    file << r.sensorId << '|' << r.farmId << '|' << r.cropId << '|' << r.sensorType << '|' << r.value << '|' << r.location << '|'
         << r.timestamp << '\n';

    cout << "Sensor record added successfully.\n";

    // Polymorphic alert check
    if (sensor->isAbnormal()) {
        cout << "ALERT: " << sensor->getType() << " value abnormal! Normal range: " << sensor->getNormalRange() << '\n';
    }
}

void viewSensorRecords() {
    vector<SensorRecord> records = loadSensors();
    if (records.empty()) {
        cout << "\nNo sensor records found.\n";
        return;
    }

    cout << "\n------------------------------------- SENSOR RECORDS --------------------------------------\n";
    cout << left << setw(10) << "SID" << setw(10) << "FarmID" << setw(10) << "CropID" << setw(14) << "Type" << setw(10)
         << "Value" << setw(15) << "Status" << setw(15) << "Location" << "Timestamp" << '\n';
    cout << string(98, '-') << '\n';

    for (const auto &r : records) {
        unique_ptr<Sensor> sensor = createSensor(r.sensorType, r.sensorId, r.location, r.value);
        string status = "UNKNOWN";
        if (sensor) {
            status = sensor->isAbnormal() ? "ABNORMAL" : "NORMAL";
        }

        cout << left << setw(10) << r.sensorId << setw(10) << r.farmId << setw(10) << r.cropId << setw(14) << r.sensorType << setw(10)
             << fixed << setprecision(2) << r.value << setw(15) << status << setw(15) << r.location << r.timestamp << '\n';
    }
}

void searchSensorRecord() {
    string id;
    cout << "\nEnter Sensor ID to search: ";
    getline(cin, id);

    vector<SensorRecord> records = loadSensors();
    for (const auto &r : records) {
        if (r.sensorId == id) {
            unique_ptr<Sensor> sensor = createSensor(r.sensorType, r.sensorId, r.location, r.value);
            string status = (sensor && sensor->isAbnormal()) ? "ABNORMAL" : "NORMAL";
            cout << "Found: " << r.sensorId << " | " << r.sensorType << " | " << r.value << " | " << status << " | " << r.timestamp
                 << '\n';
            return;
        }
    }
    cout << "Sensor record not found.\n";
}

void updateSensorRecord() {
    string id;
    cout << "\nEnter Sensor ID to update: ";
    getline(cin, id);

    vector<SensorRecord> records = loadSensors();
    bool found = false;

    for (auto &r : records) {
        if (r.sensorId == id) {
            cout << "Enter new Sensor Type (Moisture/Temperature/Humidity/WaterLevel): ";
            getline(cin, r.sensorType);
            cout << "Enter new Sensor Value: ";
            cin >> r.value;
            clearInput();
            cout << "Enter new Sensor Location: ";
            getline(cin, r.location);
            r.timestamp = nowDateTime();

            unique_ptr<Sensor> sensor = createSensor(r.sensorType, r.sensorId, r.location, r.value);
            if (!sensor) {
                cout << "Invalid sensor type. Update cancelled.\n";
                return;
            }

            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Sensor record not found.\n";
        return;
    }

    saveSensors(records);
    cout << "Sensor record updated successfully.\n";
}

void deleteSensorRecord() {
    string id;
    cout << "\nEnter Sensor ID to delete: ";
    getline(cin, id);

    vector<SensorRecord> records = loadSensors();
    size_t oldSize = records.size();

    records.erase(remove_if(records.begin(), records.end(), [&](const SensorRecord &r) { return r.sensorId == id; }), records.end());

    if (records.size() == oldSize) {
        cout << "Sensor record not found.\n";
        return;
    }

    saveSensors(records);
    cout << "Sensor record deleted successfully.\n";
}

void showAlerts() {
    vector<SensorRecord> records = loadSensors();
    bool hasAlert = false;

    cout << "\n---------------- ALERTS ----------------\n";
    for (const auto &r : records) {
        unique_ptr<Sensor> sensor = createSensor(r.sensorType, r.sensorId, r.location, r.value);
        if (sensor && sensor->isAbnormal()) {
            hasAlert = true;
            cout << "ALERT -> SensorID: " << r.sensorId << ", Type: " << sensor->getType() << ", Value: " << r.value
                 << ", Normal Range: " << sensor->getNormalRange() << '\n';
        }
    }

    if (!hasAlert) {
        cout << "No abnormal sensor values right now.\n";
    }
}

// ------------------------------
// Menus
// ------------------------------
void farmMenu() {
    int choice;
    do {
        cout << "\n===== FARM MANAGEMENT =====\n";
        cout << "1. Add Farm\n";
        cout << "2. View Farms\n";
        cout << "3. Search Farm\n";
        cout << "4. Update Farm\n";
        cout << "5. Delete Farm\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;
        clearInput();

        switch (choice) {
        case 1:
            addFarm();
            break;
        case 2:
            viewFarms();
            break;
        case 3:
            searchFarm();
            break;
        case 4:
            updateFarm();
            break;
        case 5:
            deleteFarm();
            break;
        case 0:
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void cropMenu() {
    int choice;
    do {
        cout << "\n===== CROP MANAGEMENT =====\n";
        cout << "1. Add Crop\n";
        cout << "2. View Crops\n";
        cout << "3. Search Crop\n";
        cout << "4. Update Crop\n";
        cout << "5. Delete Crop\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;
        clearInput();

        switch (choice) {
        case 1:
            addCrop();
            break;
        case 2:
            viewCrops();
            break;
        case 3:
            searchCrop();
            break;
        case 4:
            updateCrop();
            break;
        case 5:
            deleteCrop();
            break;
        case 0:
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

void sensorMenu() {
    int choice;
    do {
        cout << "\n===== SENSOR MANAGEMENT =====\n";
        cout << "1. Add Sensor Record\n";
        cout << "2. View Sensor Records\n";
        cout << "3. Search Sensor Record\n";
        cout << "4. Update Sensor Record\n";
        cout << "5. Delete Sensor Record\n";
        cout << "6. Show Alerts\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;
        clearInput();

        switch (choice) {
        case 1:
            addSensorRecord();
            break;
        case 2:
            viewSensorRecords();
            break;
        case 3:
            searchSensorRecord();
            break;
        case 4:
            updateSensorRecord();
            break;
        case 5:
            deleteSensorRecord();
            break;
        case 6:
            showAlerts();
            break;
        case 0:
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    int choice;

    cout << "==============================================\n";
    cout << " SMART FARMING MONITORING SYSTEM (C++ OOP)\n";
    cout << " File-based DB + Run-time Polymorphism Demo\n";
    cout << "==============================================\n";

    do {
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. Farm Record Management\n";
        cout << "2. Crop Record Management\n";
        cout << "3. Sensor Record Management\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        clearInput();

        switch (choice) {
        case 1:
            farmMenu();
            break;
        case 2:
            cropMenu();
            break;
        case 3:
            sensorMenu();
            break;
        case 0:
            cout << "Exiting program. Thank you!\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
