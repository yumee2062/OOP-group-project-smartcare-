
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <direct.h>
#include <cctype>

using namespace std;

// ============================================================
// SMARTCARE - Smart Hospital Emergency Simulator
// ============================================================

// -------------------- COLORS --------------------

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetColor()
{
    setColor(7);
}

void clearScreen()
{
    system("cls");
}

void pauseScreen()
{
    cout << "\n";
    setColor(8);
    cout << "Press ENTER to continue...";
    resetColor();

    cin.ignore();
    cin.get();
}

void line(char ch = '=', int length = 75)
{
    setColor(8);

    for (int i = 0; i < length; i++)
        cout << ch;

    cout << endl;
    resetColor();
}

void success(string message)
{
    setColor(10);
    cout << "\n[SUCCESS] " << message << endl;
    resetColor();
}

void errorMessage(string message)
{
    setColor(12);
    cout << "\n[ERROR] " << message << endl;
    resetColor();
}

void warning(string message)
{
    setColor(14);
    cout << "\n[WARNING] " << message << endl;
    resetColor();
}

void info(string message)
{
    setColor(11);
    cout << "\n[INFO] " << message << endl;
    resetColor();
}

void title(string text)
{
    clearScreen();

    setColor(11);
    line();
    cout << "                    " << text << endl;
    line();
    resetColor();
}

// ============================================================
// HELPER FUNCTIONS
// ============================================================

string toLowerCase(string text)
{
    for (char &c : text)
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));

    return text;
}

string replacePipe(string text)
{
    for (char &c : text)
    {
        if (c == '|')
            c = '/';
    }

    return text;
}

int getInt(string message)
{
    int value;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            cin.ignore(1000, '\n');
            return value;
        }

        cin.clear();
        cin.ignore(1000, '\n');

        errorMessage("Please enter a valid number.");
    }
}

double getDouble(string message)
{
    double value;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            cin.ignore(1000, '\n');
            return value;
        }

        cin.clear();
        cin.ignore(1000, '\n');

        errorMessage("Please enter a valid number.");
    }
}

string getString(string message)
{
    string value;

    cout << message;
    getline(cin, value);

    return value;
}

// ============================================================
// TRIAGE
// ============================================================

enum TriageLevel
{
    LOW = 1,
    MODERATE,
    URGENT,
    CRITICAL
};

string triageToString(TriageLevel level)
{
    switch (level)
    {
    case LOW:
        return "LOW";

    case MODERATE:
        return "MODERATE";

    case URGENT:
        return "URGENT";

    case CRITICAL:
        return "CRITICAL";

    default:
        return "UNKNOWN";
    }
}

// ============================================================
// BASE CLASS - PERSON
// ============================================================

class Person
{
protected:
    int id;
    string name;
    int age;
    string phone;

public:

    Person()
    {
        id = 0;
        age = 0;
    }

    Person(int id, string name, int age, string phone)
    {
        this->id = id;
        this->name = name;
        this->age = age;
        this->phone = phone;
    }

    virtual void display()
    {
        cout << "ID: " << id << endl;
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "Phone: " << phone << endl;
    }

    int getId()
    {
        return id;
    }

    string getName()
    {
        return name;
    }

    int getAge()
    {
        return age;
    }

    string getPhone()
    {
        return phone;
    }
};

// ============================================================
// PATIENT CLASS
// ============================================================

class Patient : public Person
{
private:
    int heartRate;
    int oxygenLevel;
    int systolicBP;

    string symptoms;

    TriageLevel priority;

    bool admitted;
    bool treated;
    bool discharged;

    int assignedDoctorId;
    int assignedBedId;

public:

    Patient()
    {
        heartRate = 0;
        oxygenLevel = 0;
        systolicBP = 0;

        priority = LOW;

        admitted = false;
        treated = false;
        discharged = false;

        assignedDoctorId = 0;
        assignedBedId = 0;
    }

    Patient(
        int id,
        string name,
        int age,
        string phone,
        int heartRate,
        int oxygenLevel,
        int systolicBP,
        string symptoms)
        : Person(id, name, age, phone)
    {
        this->heartRate = heartRate;
        this->oxygenLevel = oxygenLevel;
        this->systolicBP = systolicBP;
        this->symptoms = symptoms;

        admitted = false;
        treated = false;
        discharged = false;

        assignedDoctorId = 0;
        assignedBedId = 0;

        calculateTriage();
    }

    void calculateTriage()
    {
        int score = 0;

        if (oxygenLevel < 85)
            score += 5;
        else if (oxygenLevel < 92)
            score += 4;
        else if (oxygenLevel < 95)
            score += 2;

        if (heartRate > 140 || heartRate < 45)
            score += 4;
        else if (heartRate > 120 || heartRate < 55)
            score += 2;

        if (systolicBP < 80)
            score += 5;
        else if (systolicBP < 90)
            score += 3;

        if (age >= 70)
            score += 2;

        string lowerSymptoms = toLowerCase(symptoms);

        if (lowerSymptoms.find("chest") != string::npos ||
            lowerSymptoms.find("unconscious") != string::npos ||
            lowerSymptoms.find("stroke") != string::npos ||
            lowerSymptoms.find("severe bleeding") != string::npos)
        {
            score += 5;
        }

        if (score >= 9)
            priority = CRITICAL;
        else if (score >= 6)
            priority = URGENT;
        else if (score >= 3)
            priority = MODERATE;
        else
            priority = LOW;
    }

    void display() override
    {
        cout << left
             << setw(8) << id
             << setw(22) << name
             << setw(6) << age
             << setw(12) << heartRate
             << setw(10) << oxygenLevel
             << setw(10) << systolicBP
             << setw(12) << triageToString(priority)
             << endl;
    }

    void detailedDisplay()
    {
        line('-');

        cout << "Patient ID       : " << id << endl;
        cout << "Patient Name     : " << name << endl;
        cout << "Age              : " << age << endl;
        cout << "Phone            : " << phone << endl;
        cout << "Heart Rate       : " << heartRate << " bpm" << endl;
        cout << "Oxygen Level     : " << oxygenLevel << "%" << endl;
        cout << "Blood Pressure   : " << systolicBP << " mmHg" << endl;
        cout << "Symptoms         : " << symptoms << endl;

        setColor(priority == CRITICAL ? 12 :
                 priority == URGENT ? 14 :
                 priority == MODERATE ? 11 : 10);

        cout << "Triage Level     : " << triageToString(priority) << endl;

        resetColor();

        cout << "Admitted         : " << (admitted ? "YES" : "NO") << endl;
        cout << "Treated          : " << (treated ? "YES" : "NO") << endl;
        cout << "Discharged       : " << (discharged ? "YES" : "NO") << endl;

        cout << "Assigned Doctor  : ";

        if (assignedDoctorId == 0)
            cout << "Not Assigned";
        else
            cout << assignedDoctorId;

        cout << endl;

        cout << "Assigned Bed     : ";

        if (assignedBedId == 0)
            cout << "Not Assigned";
        else
            cout << assignedBedId;

        cout << endl;

        line('-');
    }

    TriageLevel getPriority()
    {
        return priority;
    }

    string getSymptoms()
    {
        return symptoms;
    }

    int getHeartRate()
    {
        return heartRate;
    }

    int getOxygen()
    {
        return oxygenLevel;
    }

    int getBP()
    {
        return systolicBP;
    }

    bool isAdmitted()
    {
        return admitted;
    }

    bool isTreated()
    {
        return treated;
    }

    bool isDischarged()
    {
        return discharged;
    }

    int getDoctorId()
    {
        return assignedDoctorId;
    }

    int getBedId()
    {
        return assignedBedId;
    }

    void admit(int doctorId, int bedId)
    {
        admitted = true;
        assignedDoctorId = doctorId;
        assignedBedId = bedId;
    }

    void setTreated(bool value)
    {
        treated = value;
    }

    void setDischarged(bool value)
    {
        discharged = value;
    }

    void setAssignments(int doctorId, int bedId)
    {
        assignedDoctorId = doctorId;
        assignedBedId = bedId;
    }

    void setStatus(bool admitted, bool treated, bool discharged)
    {
        this->admitted = admitted;
        this->treated = treated;
        this->discharged = discharged;
    }

    // ---------------- SAVE ----------------

    void save(ofstream &file)
    {
        file << id << "|"
             << replacePipe(name) << "|"
             << age << "|"
             << replacePipe(phone) << "|"
             << heartRate << "|"
             << oxygenLevel << "|"
             << systolicBP << "|"
             << replacePipe(symptoms) << "|"
             << static_cast<int>(priority) << "|"
             << admitted << "|"
             << treated << "|"
             << discharged << "|"
             << assignedDoctorId << "|"
             << assignedBedId
             << endl;
    }

    // ---------------- LOAD ----------------

    bool load(string data)
    {
        stringstream ss(data);
        string value;

        try
        {
            getline(ss, value, '|');
            id = stoi(value);

            getline(ss, name, '|');

            getline(ss, value, '|');
            age = stoi(value);

            getline(ss, phone, '|');

            getline(ss, value, '|');
            heartRate = stoi(value);

            getline(ss, value, '|');
            oxygenLevel = stoi(value);

            getline(ss, value, '|');
            systolicBP = stoi(value);

            getline(ss, symptoms, '|');

            getline(ss, value, '|');
            priority = static_cast<TriageLevel>(stoi(value));

            getline(ss, value, '|');
            admitted = stoi(value);

            getline(ss, value, '|');
            treated = stoi(value);

            getline(ss, value, '|');
            discharged = stoi(value);

            getline(ss, value, '|');
            assignedDoctorId = stoi(value);

            getline(ss, value);
            assignedBedId = stoi(value);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
};

// ============================================================
// DOCTOR CLASS
// ============================================================

class Doctor : public Person
{
private:
    string specialization;

    bool available;

    int currentPatientId;

    int patientsTreated;

public:

    Doctor()
    {
        specialization = "";
        available = true;
        currentPatientId = 0;
        patientsTreated = 0;
    }

    Doctor(
        int id,
        string name,
        int age,
        string phone,
        string specialization)
        : Person(id, name, age, phone)
    {
        this->specialization = specialization;

        available = true;
        currentPatientId = 0;
        patientsTreated = 0;
    }

    void display() override
    {
        cout << left
             << setw(8) << id
             << setw(22) << name
             << setw(22) << specialization
             << setw(15) << (available ? "AVAILABLE" : "BUSY")
             << setw(10) << patientsTreated
             << endl;
    }

    string getSpecialization()
    {
        return specialization;
    }

    bool isAvailable()
    {
        return available;
    }

    int getCurrentPatientId()
    {
        return currentPatientId;
    }

    int getPatientsTreated()
    {
        return patientsTreated;
    }

    void assignPatient(int patientId)
    {
        available = false;
        currentPatientId = patientId;
    }

    void completeTreatment()
    {
        available = true;
        currentPatientId = 0;
        patientsTreated++;
    }

    void save(ofstream &file)
    {
        file << id << "|"
             << replacePipe(name) << "|"
             << age << "|"
             << replacePipe(phone) << "|"
             << replacePipe(specialization) << "|"
             << available << "|"
             << currentPatientId << "|"
             << patientsTreated
             << endl;
    }

    bool load(string data)
    {
        stringstream ss(data);
        string value;

        try
        {
            getline(ss, value, '|');
            id = stoi(value);

            getline(ss, name, '|');

            getline(ss, value, '|');
            age = stoi(value);

            getline(ss, phone, '|');

            getline(ss, specialization, '|');

            getline(ss, value, '|');
            available = stoi(value);

            getline(ss, value, '|');
            currentPatientId = stoi(value);

            getline(ss, value);
            patientsTreated = stoi(value);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
};

// ============================================================
// BED CLASS
// ============================================================

class Bed
{
private:
    int id;
    string type;
    bool occupied;
    int patientId;

public:

    Bed()
    {
        id = 0;
        occupied = false;
        patientId = 0;
    }

    Bed(int id, string type)
    {
        this->id = id;
        this->type = type;

        occupied = false;
        patientId = 0;
    }

    int getId()
    {
        return id;
    }

    string getType()
    {
        return type;
    }

    bool isOccupied()
    {
        return occupied;
    }

    int getPatientId()
    {
        return patientId;
    }

    void assign(int patientId)
    {
        occupied = true;
        this->patientId = patientId;
    }

    void release()
    {
        occupied = false;
        patientId = 0;
    }

    void display()
    {
        cout << left
             << setw(10) << id
             << setw(18) << type
             << setw(15) << (occupied ? "OCCUPIED" : "AVAILABLE")
             << setw(12) << (occupied ? to_string(patientId) : "-")
             << endl;
    }

    void save(ofstream &file)
    {
        file << id << "|"
             << type << "|"
             << occupied << "|"
             << patientId
             << endl;
    }

    bool load(string data)
    {
        stringstream ss(data);
        string value;

        try
        {
            getline(ss, value, '|');
            id = stoi(value);

            getline(ss, type, '|');

            getline(ss, value, '|');
            occupied = stoi(value);

            getline(ss, value);
            patientId = stoi(value);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
};

// ============================================================
// MEDICINE CLASS
// ============================================================

class Medicine
{
private:
    int id;
    string name;
    int quantity;
    int minimumStock;
    double price;

public:

    Medicine()
    {
        id = 0;
        quantity = 0;
        minimumStock = 0;
        price = 0;
    }

    Medicine(
        int id,
        string name,
        int quantity,
        int minimumStock,
        double price)
    {
        this->id = id;
        this->name = name;
        this->quantity = quantity;
        this->minimumStock = minimumStock;
        this->price = price;
    }

    int getId()
    {
        return id;
    }

    string getName()
    {
        return name;
    }

    int getQuantity()
    {
        return quantity;
    }

    double getPrice()
    {
        return price;
    }

    bool use(int amount)
    {
        if (quantity >= amount)
        {
            quantity -= amount;
            return true;
        }

        return false;
    }

    void addStock(int amount)
    {
        quantity += amount;
    }

    void display()
    {
        cout << left
             << setw(8) << id
             << setw(28) << name
             << setw(12) << quantity
             << setw(15) << minimumStock
             << fixed << setprecision(2)
             << setw(12) << price;

        if (quantity <= minimumStock)
        {
            setColor(12);
            cout << "LOW STOCK";
            resetColor();
        }

        cout << endl;
    }

    void save(ofstream &file)
    {
        file << id << "|"
             << name << "|"
             << quantity << "|"
             << minimumStock << "|"
             << price
             << endl;
    }

    bool load(string data)
    {
        stringstream ss(data);
        string value;

        try
        {
            getline(ss, value, '|');
            id = stoi(value);

            getline(ss, name, '|');

            getline(ss, value, '|');
            quantity = stoi(value);

            getline(ss, value, '|');
            minimumStock = stoi(value);

            getline(ss, value);
            price = stod(value);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
};

// ============================================================
// AMBULANCE CLASS
// ============================================================

class Ambulance
{
private:
    int id;
    string driver;
    bool available;
    string location;
    int missions;

public:

    Ambulance()
    {
        id = 0;
        available = true;
        location = "Hospital";
        missions = 0;
    }

    Ambulance(int id, string driver)
    {
        this->id = id;
        this->driver = driver;

        available = true;
        location = "Hospital";
        missions = 0;
    }

    int getId()
    {
        return id;
    }

    bool isAvailable()
    {
        return available;
    }

    int getMissions()
    {
        return missions;
    }

    void dispatch(string destination)
    {
        available = false;
        location = destination;
        missions++;
    }

    void returnToHospital()
    {
        available = true;
        location = "Hospital";
    }

    void display()
    {
        cout << left
             << setw(10) << id
             << setw(22) << driver
             << setw(18) << (available ? "AVAILABLE" : "ON MISSION")
             << setw(25) << location
             << setw(10) << missions
             << endl;
    }

    void save(ofstream &file)
    {
        file << id << "|"
             << driver << "|"
             << available << "|"
             << location << "|"
             << missions
             << endl;
    }

    bool load(string data)
    {
        stringstream ss(data);
        string value;

        try
        {
            getline(ss, value, '|');
            id = stoi(value);

            getline(ss, driver, '|');

            getline(ss, value, '|');
            available = stoi(value);

            getline(ss, location, '|');

            getline(ss, value);
            missions = stoi(value);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
};

// ============================================================
// ABSTRACT EMERGENCY CLASS
// ============================================================

class Emergency
{
protected:
    int emergencyId;
    string name;
    int severity;

public:

    Emergency(int id, string name, int severity)
    {
        emergencyId = id;
        this->name = name;
        this->severity = severity;
    }

    virtual void displayEmergency() = 0;

    virtual int resourceDemand() = 0;

    virtual ~Emergency() {}
};

// ============================================================
// DERIVED EMERGENCY CLASSES
// ============================================================

class CardiacEmergency : public Emergency
{
public:

    CardiacEmergency()
        : Emergency(1, "Cardiac Emergency", 10)
    {
    }

    void displayEmergency() override
    {
        cout << "CARDIAC EMERGENCY" << endl;
        cout << "Severity Level : " << severity << "/10" << endl;
        cout << "Priority       : CRITICAL" << endl;
    }

    int resourceDemand() override
    {
        return 5;
    }
};

class AccidentEmergency : public Emergency
{
public:

    AccidentEmergency()
        : Emergency(2, "Road Accident", 9)
    {
    }

    void displayEmergency() override
    {
        cout << "ROAD ACCIDENT" << endl;
        cout << "Severity Level : " << severity << "/10" << endl;
        cout << "Priority       : URGENT" << endl;
    }

    int resourceDemand() override
    {
        return 8;
    }
};

class MassCasualtyEmergency : public Emergency
{
public:

    MassCasualtyEmergency()
        : Emergency(3, "Mass Casualty Event", 10)
    {
    }

    void displayEmergency() override
    {
        cout << "MASS CASUALTY EVENT" << endl;
        cout << "Severity Level : " << severity << "/10" << endl;
        cout << "Priority       : CRITICAL" << endl;
    }

    int resourceDemand() override
    {
        return 15;
    }
};

// ============================================================
// BILLING CLASS
// ============================================================

class Bill
{
private:
    int billId;
    int patientId;
    string patientName;
    double treatmentCost;
    double medicineCost;
    double totalCost;

public:

    Bill()
    {
        billId = 0;
        patientId = 0;
        treatmentCost = 0;
        medicineCost = 0;
        totalCost = 0;
    }

    Bill(
        int billId,
        int patientId,
        string patientName,
        double treatmentCost,
        double medicineCost)
    {
        this->billId = billId;
        this->patientId = patientId;
        this->patientName = patientName;
        this->treatmentCost = treatmentCost;
        this->medicineCost = medicineCost;

        totalCost = treatmentCost + medicineCost;
    }

    void display()
    {
        cout << left
             << setw(10) << billId
             << setw(12) << patientId
             << setw(22) << patientName
             << setw(15) << fixed << setprecision(2) << treatmentCost
             << setw(15) << medicineCost
             << setw(15) << totalCost
             << endl;
    }

    void save(ofstream &file)
    {
        file << billId << "|"
             << patientId << "|"
             << replacePipe(patientName) << "|"
             << treatmentCost << "|"
             << medicineCost << "|"
             << totalCost
             << endl;
    }

    bool load(string data)
    {
        stringstream ss(data);
        string value;

        try
        {
            getline(ss, value, '|');
            billId = stoi(value);

            getline(ss, value, '|');
            patientId = stoi(value);

            getline(ss, patientName, '|');

            getline(ss, value, '|');
            treatmentCost = stod(value);

            getline(ss, value, '|');
            medicineCost = stod(value);

            getline(ss, value);
            totalCost = stod(value);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
};

// ============================================================
// SMARTCARE MAIN SYSTEM
// ============================================================

class SmartCare
{
private:

    string hospitalName;
    string administrator;

    double budget;

    int totalPatientsRegistered;
    int patientsTreated;
    int patientsDischarged;

    int emergencyMissions;
    int ambulanceMissions;

    int nextBillId;

    vector<Patient> patients;
    vector<Doctor> doctors;
    vector<Bed> beds;
    vector<Medicine> medicines;
    vector<Ambulance> ambulances;
    vector<Bill> bills;

public:

    SmartCare()
    {
        hospitalName = "SMARTCARE HOSPITAL";
        administrator = "Hospital Administrator";

        budget = 100000;

        totalPatientsRegistered = 0;
        patientsTreated = 0;
        patientsDischarged = 0;

        emergencyMissions = 0;
        ambulanceMissions = 0;

        nextBillId = 1;
    }

    // ========================================================
    // INITIALIZATION
    // ========================================================

    void initialize()
    {
        _mkdir("data");

        loadAllData();

        // If no doctors exist, create default doctors
        if (doctors.empty())
        {
            doctors.push_back(
                Doctor(101, "Dr. Yumee Rai", 27,
                       "9800000001", "Cardiology"));

            doctors.push_back(
                Doctor(102, "Dr. Rohan Thapa", 42,
                       "9800000002", "Emergency Medicine"));

            doctors.push_back(
                Doctor(103, "Dr. Sita Rai", 36,
                       "9800000003", "Neurology"));

            doctors.push_back(
                Doctor(104, "Dr. Kiran Gurung", 45,
                       "9800000004", "General Medicine"));

            doctors.push_back(
                Doctor(105, "Dr. Priya KC", 39,
                       "9800000005", "Trauma Surgery"));
        }

        // If no beds exist, create beds
        if (beds.empty())
        {
            for (int i = 1; i <= 3; i++)
                beds.push_back(Bed(i, "ICU"));

            for (int i = 4; i <= 8; i++)
                beds.push_back(Bed(i, "Emergency"));

            for (int i = 9; i <= 14; i++)
                beds.push_back(Bed(i, "General"));
        }

        // If no medicines exist
        if (medicines.empty())
        {
            medicines.push_back(
                Medicine(1, "Paracetamol", 200, 50, 5));

            medicines.push_back(
                Medicine(2, "Antibiotic", 100, 25, 35));

            medicines.push_back(
                Medicine(3, "Saline", 80, 20, 100));

            medicines.push_back(
                Medicine(4, "Pain Relief Injection", 60, 15, 150));

            medicines.push_back(
                Medicine(5, "Emergency Cardiac Drug", 30, 10, 500));
        }

        // If no ambulances exist
        if (ambulances.empty())
        {
            ambulances.push_back(
                Ambulance(201, "Ram Thapa"));

            ambulances.push_back(
                Ambulance(202, "Suman Rai"));

            ambulances.push_back(
                Ambulance(203, "Bikash Gurung"));
        }
    }

    // ========================================================
    // FILE PATHS
    // ========================================================

    string patientFile()
    {
        return "data/patients.txt";
    }

    string doctorFile()
    {
        return "data/doctors.txt";
    }

    string bedFile()
    {
        return "data/beds.txt";
    }

    string medicineFile()
    {
        return "data/medicines.txt";
    }

    string ambulanceFile()
    {
        return "data/ambulances.txt";
    }

    string billingFile()
    {
        return "data/billing.txt";
    }

    string hospitalFile()
    {
        return "data/hospital_data.txt";
    }

    // ========================================================
    // SAVE PATIENTS
    // ========================================================

    void savePatients()
    {
        ofstream file(patientFile());

        if (!file)
        {
            errorMessage("Unable to save patients.");
            return;
        }

        for (Patient &p : patients)
            p.save(file);

        file.close();
    }

    // ========================================================
    // LOAD PATIENTS
    // ========================================================

    void loadPatients()
    {
        ifstream file(patientFile());

        if (!file)
            return;

        patients.clear();

        string data;

        while (getline(file, data))
        {
            if (!data.empty())
            {
                Patient p;

                if (p.load(data))
                    patients.push_back(p);
            }
        }

        file.close();
    }

    // ========================================================
    // SAVE DOCTORS
    // ========================================================

    void saveDoctors()
    {
        ofstream file(doctorFile());

        if (!file)
            return;

        for (Doctor &d : doctors)
            d.save(file);

        file.close();
    }

    // ========================================================
    // LOAD DOCTORS
    // ========================================================

    void loadDoctors()
    {
        ifstream file(doctorFile());

        if (!file)
            return;

        doctors.clear();

        string data;

        while (getline(file, data))
        {
            if (!data.empty())
            {
                Doctor d;

                if (d.load(data))
                    doctors.push_back(d);
            }
        }

        file.close();
    }

    // ========================================================
    // SAVE BEDS
    // ========================================================

    void saveBeds()
    {
        ofstream file(bedFile());

        if (!file)
            return;

        for (Bed &b : beds)
            b.save(file);

        file.close();
    }

    // ========================================================
    // LOAD BEDS
    // ========================================================

    void loadBeds()
    {
        ifstream file(bedFile());

        if (!file)
            return;

        beds.clear();

        string data;

        while (getline(file, data))
        {
            if (!data.empty())
            {
                Bed b;

                if (b.load(data))
                    beds.push_back(b);
            }
        }

        file.close();
    }

    // ========================================================
    // SAVE MEDICINES
    // ========================================================

    void saveMedicines()
    {
        ofstream file(medicineFile());

        if (!file)
            return;

        for (Medicine &m : medicines)
            m.save(file);

        file.close();
    }

    // ========================================================
    // LOAD MEDICINES
    // ========================================================

    void loadMedicines()
    {
        ifstream file(medicineFile());

        if (!file)
            return;

        medicines.clear();

        string data;

        while (getline(file, data))
        {
            if (!data.empty())
            {
                Medicine m;

                if (m.load(data))
                    medicines.push_back(m);
            }
        }

        file.close();
    }

    // ========================================================
    // SAVE AMBULANCES
    // ========================================================

    void saveAmbulances()
    {
        ofstream file(ambulanceFile());

        if (!file)
            return;

        for (Ambulance &a : ambulances)
            a.save(file);

        file.close();
    }

    // ========================================================
    // LOAD AMBULANCES
    // ========================================================

    void loadAmbulances()
    {
        ifstream file(ambulanceFile());

        if (!file)
            return;

        ambulances.clear();

        string data;

        while (getline(file, data))
        {
            if (!data.empty())
            {
                Ambulance a;

                if (a.load(data))
                    ambulances.push_back(a);
            }
        }

        file.close();
    }

    // ========================================================
    // SAVE BILLING
    // ========================================================

    void saveBills()
    {
        ofstream file(billingFile());

        if (!file)
            return;

        for (Bill &b : bills)
            b.save(file);

        file.close();
    }

    // ========================================================
    // LOAD BILLING
    // ========================================================

    void loadBills()
    {
        ifstream file(billingFile());

        if (!file)
            return;

        bills.clear();

        string data;

        while (getline(file, data))
        {
            if (!data.empty())
            {
                Bill b;

                if (b.load(data))
                    bills.push_back(b);
            }
        }

        file.close();
    }

    // ========================================================
    // SAVE HOSPITAL DATA
    // ========================================================

    void saveHospitalData()
    {
        ofstream file(hospitalFile());

        if (!file)
        {
            errorMessage("Unable to save hospital data.");
            return;
        }

        file << hospitalName << endl;
        file << administrator << endl;
        file << budget << endl;
        file << totalPatientsRegistered << endl;
        file << patientsTreated << endl;
        file << patientsDischarged << endl;
        file << emergencyMissions << endl;
        file << ambulanceMissions << endl;
        file << nextBillId << endl;

        file.close();
    }

    // ========================================================
    // LOAD HOSPITAL DATA
    // ========================================================

    void loadHospitalData()
    {
        ifstream file(hospitalFile());

        if (!file)
            return;

        string value;

        getline(file, hospitalName);
        getline(file, administrator);

        if (file >> budget)
            file >> totalPatientsRegistered;

        if (file >> patientsTreated)
            file >> patientsDischarged;

        if (file >> emergencyMissions)
            file >> ambulanceMissions;

        if (file >> nextBillId)
        {
            // loaded successfully
        }

        file.close();
    }

    // ========================================================
    // SAVE EVERYTHING
    // ========================================================

    void saveAllData()
    {
        savePatients();
        saveDoctors();
        saveBeds();
        saveMedicines();
        saveAmbulances();
        saveBills();
        saveHospitalData();
    }

    // ========================================================
    // LOAD EVERYTHING
    // ========================================================

    void loadAllData()
    {
        loadHospitalData();
        loadPatients();
        loadDoctors();
        loadBeds();
        loadMedicines();
        loadAmbulances();
        loadBills();

        // Correct counter if patient file contains records
        if (!patients.empty())
        {
            int highestId = 1000;

            for (Patient &p : patients)
            {
                if (p.getId() > highestId)
                    highestId = p.getId();
            }

            if (totalPatientsRegistered < highestId - 1000)
                totalPatientsRegistered = highestId - 1000;
        }
    }

    // ========================================================
    // PATIENT MANAGEMENT
    // ========================================================

    void registerPatient()
    {
        title("PATIENT REGISTRATION");

        string name = getString("Enter patient name: ");

        int age = getInt("Enter age: ");

        string phone = getString("Enter phone number: ");

        int heartRate = getInt("Enter heart rate: ");

        int oxygen = getInt("Enter oxygen level (%): ");

        int bp = getInt("Enter systolic blood pressure: ");

        string symptoms =
            getString("Enter symptoms: ");

        int id = 1001 + totalPatientsRegistered;

        Patient patient(
            id,
            name,
            age,
            phone,
            heartRate,
            oxygen,
            bp,
            symptoms);

        patients.push_back(patient);

        totalPatientsRegistered++;

        cout << "\n";
        success("Patient registered successfully.");

        cout << "Patient ID : " << id << endl;

        setColor(
            patient.getPriority() == CRITICAL ? 12 :
            patient.getPriority() == URGENT ? 14 :
            patient.getPriority() == MODERATE ? 11 : 10);

        cout << "Triage     : "
             << triageToString(patient.getPriority())
             << endl;

        resetColor();

        if (patient.getPriority() == CRITICAL)
            warning("CRITICAL patient - immediate attention recommended.");

        else if (patient.getPriority() == URGENT)
            warning("URGENT patient - priority treatment recommended.");

        saveAllData();

        pauseScreen();
    }

    // ========================================================
    // DISPLAY PATIENTS
    // ========================================================

    void displayPatients()
    {
        title("PATIENT RECORDS");

        if (patients.empty())
        {
            warning("No patient records available.");
            pauseScreen();
            return;
        }

        cout << left
             << setw(8) << "ID"
             << setw(22) << "NAME"
             << setw(6) << "AGE"
             << setw(12) << "HEART"
             << setw(10) << "O2"
             << setw(10) << "BP"
             << setw(12) << "TRIAGE"
             << endl;

        line('-');

        for (Patient &p : patients)
            p.display();

        pauseScreen();
    }

    // ========================================================
    // SEARCH PATIENT
    // ========================================================

    void searchPatient()
    {
        title("SEARCH PATIENT");

        int id = getInt("Enter patient ID: ");

        bool found = false;

        for (Patient &p : patients)
        {
            if (p.getId() == id)
            {
                p.detailedDisplay();
                found = true;
                break;
            }
        }

        if (!found)
            errorMessage("Patient not found.");

        pauseScreen();
    }

    // ========================================================
    // DOCTOR SPECIALIZATION MATCHING
    // ========================================================

    string recommendedSpecialization(Patient &p)
    {
        string symptoms = toLowerCase(p.getSymptoms());

        if (symptoms.find("chest") != string::npos ||
            symptoms.find("heart") != string::npos ||
            symptoms.find("cardiac") != string::npos)
        {
            return "Cardiology";
        }

        if (symptoms.find("stroke") != string::npos ||
            symptoms.find("unconscious") != string::npos)
        {
            return "Neurology";
        }

        if (symptoms.find("accident") != string::npos ||
            symptoms.find("fracture") != string::npos ||
            symptoms.find("injury") != string::npos ||
            symptoms.find("bleeding") != string::npos)
        {
            return "Trauma Surgery";
        }

        if (p.getPriority() == CRITICAL ||
            p.getPriority() == URGENT)
        {
            return "Emergency Medicine";
        }

        return "General Medicine";
    }

    // ========================================================
    // FIND DOCTOR
    // ========================================================

    int findAvailableDoctor(Patient &p)
    {
        string recommended = recommendedSpecialization(p);

        // First try exact specialization
        for (int i = 0; i < static_cast<int>(doctors.size()); i++)
        {
            if (doctors[i].isAvailable() &&
                doctors[i].getSpecialization() == recommended)
            {
                return i;
            }
        }

        // Otherwise emergency medicine
        for (int i = 0; i < static_cast<int>(doctors.size()); i++)
        {
            if (doctors[i].isAvailable() &&
                doctors[i].getSpecialization() == "Emergency Medicine")
            {
                return i;
            }
        }

        // Finally any available doctor
        for (int i = 0; i < static_cast<int>(doctors.size()); i++)
        {
            if (doctors[i].isAvailable())
                return i;
        }

        return -1;
    }

    // ========================================================
    // FIND BED
    // ========================================================

    int findBed(Patient &p)
    {
        string requiredType;

        if (p.getPriority() == CRITICAL)
            requiredType = "ICU";
        else if (p.getPriority() == URGENT)
            requiredType = "Emergency";
        else
            requiredType = "General";

        // Preferred bed
        for (int i = 0; i < static_cast<int>(beds.size()); i++)
        {
            if (!beds[i].isOccupied() &&
                beds[i].getType() == requiredType)
            {
                return i;
            }
        }

        // Emergency fallback
        for (int i = 0; i < static_cast<int>(beds.size()); i++)
        {
            if (!beds[i].isOccupied() &&
                beds[i].getType() == "Emergency")
            {
                return i;
            }
        }

        // Any available bed
        for (int i = 0; i < static_cast<int>(beds.size()); i++)
        {
            if (!beds[i].isOccupied())
                return i;
        }

        return -1;
    }

    // ========================================================
    // ADMIT PATIENT
    // ========================================================

    void admitPatient()
    {
        title("PATIENT ADMISSION");

        int id = getInt("Enter patient ID: ");

        int patientIndex = -1;

        for (int i = 0; i < static_cast<int>(patients.size()); i++)
        {
            if (patients[i].getId() == id)
            {
                patientIndex = i;
                break;
            }
        }

        if (patientIndex == -1)
        {
            errorMessage("Patient not found.");
            pauseScreen();
            return;
        }

        Patient &patient = patients[patientIndex];

        if (patient.isAdmitted())
        {
            warning("Patient is already admitted.");
            pauseScreen();
            return;
        }

        int doctorIndex = findAvailableDoctor(patient);

        if (doctorIndex == -1)
        {
            errorMessage("No doctor is currently available.");
            pauseScreen();
            return;
        }

        int bedIndex = findBed(patient);

        if (bedIndex == -1)
        {
            errorMessage("No hospital bed is available.");
            pauseScreen();
            return;
        }

        doctors[doctorIndex].assignPatient(patient.getId());

        beds[bedIndex].assign(patient.getId());

        patient.admit(
            doctors[doctorIndex].getId(),
            beds[bedIndex].getId());

        cout << "\n";
        success("Patient admitted successfully.");

        cout << "Doctor       : "
             << doctors[doctorIndex].getName()
             << endl;

        cout << "Specialization: "
             << doctors[doctorIndex].getSpecialization()
             << endl;

        cout << "Bed           : "
             << beds[bedIndex].getId()
             << " (" << beds[bedIndex].getType() << ")"
             << endl;

        saveAllData();

        pauseScreen();
    }

    // ========================================================
    // TREAT PATIENT
    // ========================================================

    void treatPatient()
    {
        title("EMERGENCY TREATMENT");

        int id = getInt("Enter patient ID: ");

        int patientIndex = -1;

        for (int i = 0; i < static_cast<int>(patients.size()); i++)
        {
            if (patients[i].getId() == id)
            {
                patientIndex = i;
                break;
            }
        }

        if (patientIndex == -1)
        {
            errorMessage("Patient not found.");
            pauseScreen();
            return;
        }

        Patient &patient = patients[patientIndex];

        if (!patient.isAdmitted())
        {
            errorMessage("Patient must be admitted first.");
            pauseScreen();
            return;
        }

        if (patient.isTreated())
        {
            warning("Patient has already been treated.");
            pauseScreen();
            return;
        }

        // Find doctor
        Doctor *doctor = nullptr;

        for (Doctor &d : doctors)
        {
            if (d.getId() == patient.getDoctorId())
            {
                doctor = &d;
                break;
            }
        }

        if (doctor == nullptr)
        {
            errorMessage("Assigned doctor not found.");
            pauseScreen();
            return;
        }

        // Select medicine
        int medicineId;

        if (patient.getPriority() == CRITICAL)
            medicineId = 5;
        else if (patient.getPriority() == URGENT)
            medicineId = 4;
        else
            medicineId = 1;

        Medicine *medicine = nullptr;

        for (Medicine &m : medicines)
        {
            if (m.getId() == medicineId)
            {
                medicine = &m;
                break;
            }
        }

        if (medicine == nullptr)
        {
            errorMessage("Required medicine not found.");
            pauseScreen();
            return;
        }

        if (!medicine->use(1))
        {
            errorMessage("Required medicine is out of stock.");
            pauseScreen();
            return;
        }

        cout << "\n";
        info("Starting treatment");

        for (int i = 0; i < 3; i++)
        {
            cout << ".";
            Sleep(500);
        }

        cout << endl;

        patient.setTreated(true);

        doctor->completeTreatment();

        patientsTreated++;

        double treatmentCost = 1500;

        double medicineCost = medicine->getPrice();

        budget += treatmentCost;

        Bill bill(
            nextBillId++,
            patient.getId(),
            patient.getName(),
            treatmentCost,
            medicineCost);

        bills.push_back(bill);

        success("Treatment completed successfully.");

        cout << "Treatment Cost : Rs. "
             << fixed << setprecision(2)
             << treatmentCost << endl;

        cout << "Medicine Cost  : Rs. "
             << medicineCost << endl;

        cout << "Total Bill     : Rs. "
             << treatmentCost + medicineCost
             << endl;

        saveAllData();

        pauseScreen();
    }

    // ========================================================
    // DISCHARGE PATIENT
    // ========================================================

    void dischargePatient()
    {
        title("PATIENT DISCHARGE");

        int id = getInt("Enter patient ID: ");

        int patientIndex = -1;

        for (int i = 0; i < static_cast<int>(patients.size()); i++)
        {
            if (patients[i].getId() == id)
            {
                patientIndex = i;
                break;
            }
        }

        if (patientIndex == -1)
        {
            errorMessage("Patient not found.");
            pauseScreen();
            return;
        }

        Patient &patient = patients[patientIndex];

        if (!patient.isTreated())
        {
            errorMessage("Patient must be treated before discharge.");
            pauseScreen();
            return;
        }

        if (patient.isDischarged())
        {
            warning("Patient is already discharged.");
            pauseScreen();
            return;
        }

        // Release bed
        for (Bed &b : beds)
        {
            if (b.getId() == patient.getBedId())
            {
                b.release();
                break;
            }
        }

        patient.setDischarged(true);

        patientsDischarged++;

        success("Patient discharged successfully.");

        saveAllData();

        pauseScreen();
    }

    // ========================================================
    // SMART TRIAGE QUEUE
    // ========================================================

    void triageQueue()
    {
        title("SMART TRIAGE QUEUE");

        vector<Patient*> waitingPatients;

        for (Patient &p : patients)
        {
            if (!p.isAdmitted() &&
                !p.isDischarged())
            {
                waitingPatients.push_back(&p);
            }
        }

        if (waitingPatients.empty())
        {
            info("No waiting patients.");
            pauseScreen();
            return;
        }

        sort(
            waitingPatients.begin(),
            waitingPatients.end(),
            [](Patient *a, Patient *b)
            {
                return a->getPriority() >
                       b->getPriority();
            });

        cout << left
             << setw(8) << "ID"
             << setw(25) << "PATIENT"
             << setw(15) << "TRIAGE"
             << setw(25) << "RECOMMENDED DEPARTMENT"
             << endl;

        line('-');

        for (Patient *p : waitingPatients)
        {
            cout << left
                 << setw(8) << p->getId()
                 << setw(25) << p->getName()
                 << setw(15) << triageToString(p->getPriority())
                 << setw(25) << recommendedSpecialization(*p)
                 << endl;
        }

        pauseScreen();
    }

    // ========================================================
    // STAFF MANAGEMENT
    // ========================================================

    void displayDoctors()
    {
        title("DOCTOR MANAGEMENT");

        cout << left
             << setw(8) << "ID"
             << setw(22) << "NAME"
             << setw(22) << "SPECIALIZATION"
             << setw(15) << "STATUS"
             << setw(10) << "TREATED"
             << endl;

        line('-');

        for (Doctor &d : doctors)
            d.display();

        pauseScreen();
    }

    // ========================================================
    // BED MANAGEMENT
    // ========================================================

    void displayBeds()
    {
        title("HOSPITAL BED MANAGEMENT");

        cout << left
             << setw(10) << "BED ID"
             << setw(18) << "TYPE"
             << setw(15) << "STATUS"
             << setw(12) << "PATIENT"
             << endl;

        line('-');

        for (Bed &b : beds)
            b.display();

        pauseScreen();
    }

    // ========================================================
    // PHARMACY
    // ========================================================

    void displayMedicines()
    {
        title("PHARMACY INVENTORY");

        cout << left
             << setw(8) << "ID"
             << setw(28) << "MEDICINE"
             << setw(12) << "QUANTITY"
             << setw(15) << "MIN STOCK"
             << setw(12) << "PRICE"
             << "STATUS"
             << endl;

        line('-');

        for (Medicine &m : medicines)
            m.display();

        pauseScreen();
    }

    void addMedicineStock()
    {
        title("ADD MEDICINE STOCK");

        int id = getInt("Enter medicine ID: ");

        Medicine *medicine = nullptr;

        for (Medicine &m : medicines)
        {
            if (m.getId() == id)
            {
                medicine = &m;
                break;
            }
        }

        if (medicine == nullptr)
        {
            errorMessage("Medicine not found.");
            pauseScreen();
            return;
        }

        int quantity =
            getInt("Enter quantity to add: ");

        if (quantity <= 0)
        {
            errorMessage("Quantity must be positive.");
            pauseScreen();
            return;
        }

        double cost =
            quantity * medicine->getPrice();

        if (budget < cost)
        {
            errorMessage("Insufficient hospital budget.");
            pauseScreen();
            return;
        }

        medicine->addStock(quantity);

        budget -= cost;

        success("Medicine stock updated.");

        cout << "Purchase Cost : Rs. "
             << fixed << setprecision(2)
             << cost << endl;

        cout << "Remaining Budget : Rs. "
             << budget << endl;

        saveAllData();

        pauseScreen();
    }

    // ========================================================
    // AMBULANCE MANAGEMENT
    // ========================================================

    void displayAmbulances()
    {
        title("AMBULANCE MANAGEMENT");

        cout << left
             << setw(10) << "ID"
             << setw(22) << "DRIVER"
             << setw(18) << "STATUS"
             << setw(25) << "LOCATION"
             << setw(10) << "MISSIONS"
             << endl;

        line('-');

        for (Ambulance &a : ambulances)
            a.display();

        pauseScreen();
    }

    void dispatchAmbulance()
    {
        title("AMBULANCE DISPATCH");

        Ambulance *ambulance = nullptr;

        for (Ambulance &a : ambulances)
        {
            if (a.isAvailable())
            {
                ambulance = &a;
                break;
            }
        }

        if (ambulance == nullptr)
        {
            errorMessage("No ambulance is currently available.");
            pauseScreen();
            return;
        }

        string destination =
            getString("Enter emergency location: ");

        ambulance->dispatch(destination);

        ambulanceMissions++;

        budget -= 500;

        cout << "\n";
        success("Ambulance dispatched.");

        cout << "Ambulance ID : "
             << ambulance->getId()
             << endl;

        cout << "Destination  : "
             << destination
             << endl;

        Sleep(1000);

        info("Emergency mission completed.");

        ambulance->returnToHospital();

        success("Ambulance returned to hospital.");

        saveAllData();

        pauseScreen();
    }

    // ========================================================
    // EMERGENCY CONTROL CENTER
    // ========================================================

    void emergencySimulator()
    {
        title("EMERGENCY CONTROL CENTER");

        int choice =
            getInt(
                "1. Cardiac Emergency\n"
                "2. Road Accident\n"
                "3. Mass Casualty Event\n"
                "Enter choice: ");

        Emergency *emergency = nullptr;

        if (choice == 1)
            emergency = new CardiacEmergency();

        else if (choice == 2)
            emergency = new AccidentEmergency();

        else if (choice == 3)
            emergency = new MassCasualtyEmergency();

        else
        {
            errorMessage("Invalid choice.");
            pauseScreen();
            return;
        }

        cout << "\n";
        emergency->displayEmergency();

        cout << "\nResource Demand : "
             << emergency->resourceDemand()
             << " units"
             << endl;

        cout << "\n";
        warning("Emergency response activated.");

        Sleep(1000);

        emergencyMissions++;

        budget -=
            emergency->resourceDemand() * 100;

        success("Emergency response completed.");

        delete emergency;

        saveAllData();

        pauseScreen();
    }

    // ========================================================
    // BILLING
    // ========================================================

    void displayBills()
    {
        title("PATIENT BILLING");

        if (bills.empty())
        {
            info("No billing records available.");
            pauseScreen();
            return;
        }

        cout << left
             << setw(10) << "BILL ID"
             << setw(12) << "PATIENT"
             << setw(22) << "NAME"
             << setw(15) << "TREATMENT"
             << setw(15) << "MEDICINE"
             << setw(15) << "TOTAL"
             << endl;

        line('-');

        for (Bill &b : bills)
            b.display();

        pauseScreen();
    }

    // ========================================================
    // DASHBOARD
    // ========================================================

    void dashboard()
    {
        title("SMARTCARE HOSPITAL DASHBOARD");

        int occupiedBeds = 0;

        for (Bed &b : beds)
        {
            if (b.isOccupied())
                occupiedBeds++;
        }

        int availableBeds =
            static_cast<int>(beds.size()) -
            occupiedBeds;

        int availableDoctors = 0;

        for (Doctor &d : doctors)
        {
            if (d.isAvailable())
                availableDoctors++;
        }

        int availableAmbulances = 0;

        for (Ambulance &a : ambulances)
        {
            if (a.isAvailable())
                availableAmbulances++;
        }

        cout << fixed << setprecision(2);

        cout << "\n";
        setColor(11);
        cout << "PATIENT STATISTICS" << endl;
        resetColor();

        line('-');

        cout << "Total Registered Patients : "
             << totalPatientsRegistered
             << endl;

        cout << "Patients Treated          : "
             << patientsTreated
             << endl;

        cout << "Patients Discharged      : "
             << patientsDischarged
             << endl;

        cout << "\n";

        setColor(11);
        cout << "HOSPITAL RESOURCES" << endl;
        resetColor();

        line('-');

        cout << "Total Beds                : "
             << beds.size()
             << endl;

        cout << "Occupied Beds             : "
             << occupiedBeds
             << endl;

        cout << "Available Beds            : "
             << availableBeds
             << endl;

        cout << "Available Doctors         : "
             << availableDoctors
             << "/" << doctors.size()
             << endl;

        cout << "Available Ambulances      : "
             << availableAmbulances
             << "/" << ambulances.size()
             << endl;

        cout << "\n";

        setColor(11);
        cout << "EMERGENCY STATISTICS" << endl;
        resetColor();

        line('-');

        cout << "Emergency Events          : "
             << emergencyMissions
             << endl;

        cout << "Ambulance Missions        : "
             << ambulanceMissions
             << endl;

        cout << "\n";

        setColor(11);
        cout << "FINANCIAL STATUS" << endl;
        resetColor();

        line('-');

        cout << "Hospital Budget           : Rs. "
             << budget
             << endl;

        cout << "\n";

        double treatmentRate = 0;

        if (totalPatientsRegistered > 0)
        {
            treatmentRate =
                (static_cast<double>(patientsTreated) /
                 totalPatientsRegistered) *
                100;
        }

        cout << "Treatment Rate            : "
             << treatmentRate
             << "%"
             << endl;

        pauseScreen();
    }

    // ========================================================
    // PERFORMANCE REPORT
    // ========================================================

    void performanceReport()
    {
        title("SMARTCARE PERFORMANCE REPORT");

        int occupiedBeds = 0;

        for (Bed &b : beds)
        {
            if (b.isOccupied())
                occupiedBeds++;
        }

        double occupancyRate = 0;

        if (!beds.empty())
        {
            occupancyRate =
                (static_cast<double>(occupiedBeds) /
                 beds.size()) *
                100;
        }

        double treatmentRate = 0;

        if (totalPatientsRegistered > 0)
        {
            treatmentRate =
                (static_cast<double>(patientsTreated) /
                 totalPatientsRegistered) *
                100;
        }

        cout << fixed << setprecision(2);

        cout << "Treatment Rate : "
             << treatmentRate << "%" << endl;

        cout << "Bed Occupancy  : "
             << occupancyRate << "%" << endl;

        cout << "Emergency Events: "
             << emergencyMissions << endl;

        cout << "Ambulance Missions: "
             << ambulanceMissions << endl;

        cout << "\nOverall Performance: ";

        if (treatmentRate >= 80 &&
            occupancyRate <= 90)
        {
            setColor(10);
            cout << "EXCELLENT";
        }
        else if (treatmentRate >= 60)
        {
            setColor(11);
            cout << "GOOD";
        }
        else
        {
            setColor(14);
            cout << "NEEDS IMPROVEMENT";
        }

        resetColor();

        cout << endl;

        pauseScreen();
    }

    // ========================================================
    // ABOUT
    // ========================================================

    void about()
    {
        title("ABOUT SMARTCARE");

        setColor(11);

        cout << "SMARTCARE" << endl;
        cout << "Smart Hospital Emergency Simulator" << endl;

        resetColor();

        line('-');

        cout << "\n";

        cout << "A console-based C++ hospital management and"
             << endl;

        cout << "emergency simulation system developed as an"
             << endl;

        cout << "educational project." << endl;

        cout << "\n";

        cout << "Main Concepts Used:" << endl;

        cout << "1. Encapsulation" << endl;
        cout << "2. Inheritance" << endl;
        cout << "3. Abstraction" << endl;
        cout << "4. Polymorphism" << endl;
        cout << "5. Constructors" << endl;
        cout << "6. Vectors" << endl;
        cout << "7. Sorting" << endl;
        cout << "8. File Handling" << endl;

        cout << "\n";

        cout << "File Handling Features:" << endl;

        cout << "- Patient persistence" << endl;
        cout << "- Doctor persistence" << endl;
        cout << "- Bed persistence" << endl;
        cout << "- Medicine persistence" << endl;
        cout << "- Ambulance persistence" << endl;
        cout << "- Billing persistence" << endl;
        cout << "- Hospital statistics persistence" << endl;

        cout << "\n";

        warning(
            "This project is an educational simulation "
            "and is not a real medical diagnosis system.");

        pauseScreen();
    }

    // ========================================================
    // PATIENT MENU
    // ========================================================

    void patientMenu()
    {
        int choice;

        do
        {
            title("PATIENT MANAGEMENT");

            cout << "1. Register Patient" << endl;
            cout << "2. View Patients" << endl;
            cout << "3. Search Patient" << endl;
            cout << "4. Admit Patient" << endl;
            cout << "5. Treat Patient" << endl;
            cout << "6. Discharge Patient" << endl;
            cout << "7. Smart Triage Queue" << endl;
            cout << "0. Back" << endl;

            line('-');

            choice = getInt("Enter choice: ");

            switch (choice)
            {
            case 1:
                registerPatient();
                break;

            case 2:
                displayPatients();
                break;

            case 3:
                searchPatient();
                break;

            case 4:
                admitPatient();
                break;

            case 5:
                treatPatient();
                break;

            case 6:
                dischargePatient();
                break;

            case 7:
                triageQueue();
                break;

            case 0:
                break;

            default:
                errorMessage("Invalid choice.");
                pauseScreen();
            }

        } while (choice != 0);
    }

    // ========================================================
    // STAFF MENU
    // ========================================================

    void staffMenu()
    {
        int choice;

        do
        {
            title("STAFF MANAGEMENT");

            cout << "1. View Doctors" << endl;
            cout << "0. Back" << endl;

            line('-');

            choice = getInt("Enter choice: ");

            switch (choice)
            {
            case 1:
                displayDoctors();
                break;

            case 0:
                break;

            default:
                errorMessage("Invalid choice.");
                pauseScreen();
            }

        } while (choice != 0);
    }

    // ========================================================
    // PHARMACY MENU
    // ========================================================

    void pharmacyMenu()
    {
        int choice;

        do
        {
            title("PHARMACY MANAGEMENT");

            cout << "1. View Medicine Inventory" << endl;
            cout << "2. Add Medicine Stock" << endl;
            cout << "0. Back" << endl;

            line('-');

            choice = getInt("Enter choice: ");

            switch (choice)
            {
            case 1:
                displayMedicines();
                break;

            case 2:
                addMedicineStock();
                break;

            case 0:
                break;

            default:
                errorMessage("Invalid choice.");
                pauseScreen();
            }

        } while (choice != 0);
    }

    // ========================================================
    // EMERGENCY MENU
    // ========================================================

    void emergencyMenu()
    {
        int choice;

        do
        {
            title("EMERGENCY CONTROL CENTER");

            cout << "1. Emergency Simulator" << endl;
            cout << "2. View Ambulances" << endl;
            cout << "3. Dispatch Ambulance" << endl;
            cout << "0. Back" << endl;

            line('-');

            choice = getInt("Enter choice: ");

            switch (choice)
            {
            case 1:
                emergencySimulator();
                break;

            case 2:
                displayAmbulances();
                break;

            case 3:
                dispatchAmbulance();
                break;

            case 0:
                break;

            default:
                errorMessage("Invalid choice.");
                pauseScreen();
            }

        } while (choice != 0);
    }

    // ========================================================
    // BILLING MENU
    // ========================================================

    void billingMenu()
    {
        displayBills();
    }

    // ========================================================
    // MAIN MENU
    // ========================================================

    void run()
    {
        int choice;

        do
        {
            clearScreen();

            setColor(11);

            cout << "\n";
            line('=');

            cout << "              SMARTCARE HOSPITAL" << endl;
            cout << "        SMART HOSPITAL EMERGENCY SIMULATOR"
                 << endl;

            line('=');

            resetColor();

            cout << "\n";

            cout << "Hospital Administrator : "
                 << administrator
                 << endl;

            cout << "Budget                 : Rs. "
                 << fixed << setprecision(2)
                 << budget
                 << endl;

            cout << "\n";

            setColor(11);
            cout << "MAIN MENU" << endl;
            resetColor();

            line('-');

            cout << "1. Patient Management" << endl;
            cout << "2. Staff Management" << endl;
            cout << "3. Pharmacy" << endl;
            cout << "4. Emergency Control Center" << endl;
            cout << "5. Hospital Dashboard" << endl;
            cout << "6. Bed Management" << endl;
            cout << "7. Billing Records" << endl;
            cout << "8. Performance Report" << endl;
            cout << "9. Save Data" << endl;
            cout << "10. About Project" << endl;
            cout << "0. Exit" << endl;

            line('-');

            choice = getInt("Enter choice: ");

            switch (choice)
            {
            case 1:
                patientMenu();
                break;

            case 2:
                staffMenu();
                break;

            case 3:
                pharmacyMenu();
                break;

            case 4:
                emergencyMenu();
                break;

            case 5:
                dashboard();
                break;

            case 6:
                displayBeds();
                break;

            case 7:
                billingMenu();
                break;

            case 8:
                performanceReport();
                break;

            case 9:
                saveAllData();
                success("All SMARTCARE data has been saved.");
                pauseScreen();
                break;

            case 10:
                about();
                break;

            case 0:
                saveAllData();

                cout << "\n";
                success("All data saved successfully.");
                cout << "Thank you for using SMARTCARE.\n";

                break;

            default:
                errorMessage("Invalid choice.");
                pauseScreen();
            }

        } while (choice != 0);
    }
};

// ============================================================
// MAIN FUNCTION
// ============================================================

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    SetConsoleTitleA(
        "SMARTCARE - Smart Hospital Emergency Simulator");

    SmartCare hospital;

    hospital.initialize();

    hospital.run();

    return 0;
}

