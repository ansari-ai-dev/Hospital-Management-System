#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>   
#include <iomanip>
#include <stdexcept>   
using namespace std;

// helpers
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

bool isNumber(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit(c)) return false;
    return true;
}

void divider() {
    cout << "----------------------------------------" << endl;
}

//  TEMPLATE FUNCTIONS
template <typename T>
T* findById(vector<T>& list, const string& id) {
    auto it = find_if(list.begin(), list.end(),
        [&](const T& item) { return item.id == id; });
    return (it != list.end()) ? &(*it) : nullptr;
}

// print every item in a vector using operator<<
template <typename T>
void printAll(const vector<T>& list, const string& header) {
    if (list.empty()) {
        cout << "No records found." << endl;
        return;
    }
    cout << "\n-- " << header << " --" << endl;
    for (const auto& item : list) {
        divider();
        cout << item;   
    }
    divider();
}

//  BASE CLASS: Person 
class Person {
public:
    string id;
    string name;
    string phone;

    virtual void display() const = 0;
    virtual ~Person() = default;
};

//  CLASS: User  (for login system)
class User {
public:
    string username;
    string password;
    string role;   // admin | receptionist | doctor

    void save(ofstream& f) const {
        f << username << "|" << password << "|" << role << "\n";
    }

    void load(const string& line) {
        stringstream ss(line);
        getline(ss, username, '|');
        getline(ss, password, '|');
        getline(ss, role,     '|');
    }
};

//  CLASS: Patient — inherits from Person
class Patient : public Person {
public:
    string age;
    string gender;
    int    visitCount;

    Patient() : visitCount(0) {}

    // override the pure virtual from Person  (Polymorphism)
    void display() const override {
        cout << "ID      : " << id         << "\n"
             << "Name    : " << name       << "\n"
             << "Age     : " << age        << "\n"
             << "Phone   : " << phone      << "\n"
             << "Gender  : " << gender     << "\n"
             << "Visits  : " << visitCount << "\n";
    }

    friend ostream& operator<<(ostream& os, const Patient& p) {
        os << "ID      : " << p.id         << "\n"
           << "Name    : " << p.name       << "\n"
           << "Age     : " << p.age        << "\n"
           << "Phone   : " << p.phone      << "\n"
           << "Gender  : " << p.gender     << "\n"
           << "Visits  : " << p.visitCount << "\n";
        return os;
    }

    void save(ofstream& f) const {
        f << id << "|" << name << "|" << age << "|"
          << phone << "|" << gender << "|" << visitCount << "\n";
    }

    void load(const string& line) {
        stringstream ss(line);
        string vc;
        getline(ss, id,     '|');
        getline(ss, name,   '|');
        getline(ss, age,    '|');
        getline(ss, phone,  '|');
        getline(ss, gender, '|');
        getline(ss, vc,     '|');

        try {
            visitCount = vc.empty() ? 0 : stoi(vc);
        } catch (const invalid_argument&) {
            visitCount = 0;
            cerr << "Warning: corrupt visit count for patient " << id << ", reset to 0.\n";
        }
    }
};

//  CLASS: Doctor
class Doctor : public Person {
public:
    string specialization;
    string workDays;     // e.g. "Mon,Tue,Wed"
    string workHours;    // e.g. "08:00-16:00"
    double fee;

    Doctor() : fee(0.0) {}

    // override the pure virtual from Person  (Polymorphism)
    void display() const override {
        cout << "ID             : " << id             << "\n"
             << "Name           : " << name           << "\n"
             << "Phone          : " << phone          << "\n"
             << "Specialization : " << specialization << "\n"
             << "Work Days      : " << workDays       << "\n"
             << "Work Hours     : " << workHours      << "\n"
             << "Consultation   : $"
             << fixed << setprecision(2) << fee       << "\n";
    }

    friend ostream& operator<<(ostream& os, const Doctor& d) {
        os << "ID             : " << d.id             << "\n"
           << "Name           : " << d.name           << "\n"
           << "Phone          : " << d.phone          << "\n"
           << "Specialization : " << d.specialization << "\n"
           << "Work Days      : " << d.workDays       << "\n"
           << "Work Hours     : " << d.workHours      << "\n"
           << "Consultation   : $"
           << fixed << setprecision(2) << d.fee       << "\n";
        return os;
    }

    void save(ofstream& f) const {
        f << id << "|" << name << "|" << phone << "|"
          << specialization << "|" << workDays << "|"
          << workHours << "|" << fee << "\n";
    }

    void load(const string& line) {
        stringstream ss(line);
        string feeStr;
        getline(ss, id,             '|');
        getline(ss, name,           '|');
        getline(ss, phone,          '|');
        getline(ss, specialization, '|');
        getline(ss, workDays,       '|');
        getline(ss, workHours,      '|');
        getline(ss, feeStr,         '|');

        try {
            fee = feeStr.empty() ? 0.0 : stod(feeStr);
        } catch (const invalid_argument&) {
            fee = 0.0;
            cerr << "Warning: corrupt fee for doctor " << id << ", reset to 0.\n";
        }
    }
};

//  CLASS: Appointment
class Appointment {
public:
    string id;
    string patientId;
    string doctorId;
    string date;         // DD/MM/YYYY
    string time;         // HH:MM
    string status;       // Scheduled | Completed | Cancelled
    string notes;
    string prescription;
    bool   isEmergency;

    Appointment() : isEmergency(false) {}

    virtual void display() const {
        cout << "Appt ID     : " << id         << "\n"
             << "Patient ID  : " << patientId  << "\n"
             << "Doctor ID   : " << doctorId   << "\n"
             << "Date        : " << date        << "\n"
             << "Time        : " << time        << "\n"
             << "Status      : " << status      << "\n"
             << "Emergency   : " << (isEmergency ? "Yes" : "No") << "\n";
        if (!notes.empty())
            cout << "Notes       : " << notes        << "\n";
        if (!prescription.empty())
            cout << "Prescription: " << prescription << "\n";
    }

    friend ostream& operator<<(ostream& os, const Appointment& a) {
        os << "Appt ID     : " << a.id         << "\n"
           << "Patient ID  : " << a.patientId  << "\n"
           << "Doctor ID   : " << a.doctorId   << "\n"
           << "Date        : " << a.date        << "\n"
           << "Time        : " << a.time        << "\n"
           << "Status      : " << a.status      << "\n"
           << "Emergency   : " << (a.isEmergency ? "Yes" : "No") << "\n";
        if (!a.notes.empty())
            os << "Notes       : " << a.notes        << "\n";
        if (!a.prescription.empty())
            os << "Prescription: " << a.prescription << "\n";
        return os;
    }

    virtual void save(ofstream& f) const {
        f << id << "|" << patientId << "|" << doctorId << "|"
          << date << "|" << time << "|" << status << "|"
          << notes << "|" << prescription << "|"
          << (isEmergency ? "1" : "0") << "\n";
    }

    virtual void load(const string& line) {
        stringstream ss(line);
        string emg;
        getline(ss, id,           '|');
        getline(ss, patientId,    '|');
        getline(ss, doctorId,     '|');
        getline(ss, date,         '|');
        getline(ss, time,         '|');
        getline(ss, status,       '|');
        getline(ss, notes,        '|');
        getline(ss, prescription, '|');
        getline(ss, emg,          '|');
        isEmergency = (emg == "1");
    }

    virtual ~Appointment() {}
};

//  CLASS: EmergencyAppointment
class EmergencyAppointment : public Appointment {
public:
    string reason;       // e.g. "Chest pain", "Accident"
    int    priorityLevel; // 1 = critical, 2 = urgent, 3 = moderate

    EmergencyAppointment() : priorityLevel(1) {
        isEmergency = true;
        status      = "Scheduled";
    }

    // override display() to also show emergency-specific details
    void display() const override {
        Appointment::display();   
        cout << "Priority    : " << priorityLevel << " (1=Critical, 3=Moderate)\n"
             << "Reason      : " << reason        << "\n";
    }

    // operator<< for EmergencyAppointment
    friend ostream& operator<<(ostream& os, const EmergencyAppointment& ea) {
        os << static_cast<const Appointment&>(ea);
        os << "Priority    : " << ea.priorityLevel
           << " (1=Critical, 3=Moderate)\n"
           << "Reason      : " << ea.reason << "\n";
        return os;
    }

    void save(ofstream& f) const override {
        f << id << "|" << patientId << "|" << doctorId << "|"
          << date << "|" << time << "|" << status << "|"
          << notes << "|" << prescription << "|1|"
          << priorityLevel << "|" << reason << "\n";
    }

    void load(const string& line) override {
        stringstream ss(line);
        string emg, priStr;
        getline(ss, id,           '|');
        getline(ss, patientId,    '|');
        getline(ss, doctorId,     '|');
        getline(ss, date,         '|');
        getline(ss, time,         '|');
        getline(ss, status,       '|');
        getline(ss, notes,        '|');
        getline(ss, prescription, '|');
        getline(ss, emg,          '|');
        getline(ss, priStr,       '|');
        getline(ss, reason,       '|');
        isEmergency = true;

        try {
            priorityLevel = priStr.empty() ? 1 : stoi(priStr);
        } catch (const invalid_argument&) {
            priorityLevel = 1;
        }
    }
};

//  CLASS: HospitalSystem
class HospitalSystem {
private:
    vector<User>                  users;
    vector<Patient>               patients;
    vector<Doctor>                doctors;
    vector<Appointment>           appointments;
    vector<EmergencyAppointment>  emergencies;   // stored separately
    User currentUser;

    const string USER_FILE  = "users.txt";
    const string PAT_FILE   = "patients.txt";
    const string DOC_FILE   = "doctors.txt";
    const string APPT_FILE  = "appointments.txt";
    const string EMG_FILE   = "emergencies.txt";
    const string HIST_FILE  = "history.txt";

    // generate the next ID with a prefix
    string nextId(const string& prefix, int count) {
        return prefix + to_string(count + 1);
    }

    // template-based generic finder 
   
    template <typename T>
    T* findByIdLocal(vector<T>& list, const string& id) {
        return findById(list, id);   
    }

    // convenience wrappers kept for readability
    Doctor*  findDoctor (const string& id) { return findByIdLocal(doctors,  id); }
    Patient* findPatient(const string& id) { return findByIdLocal(patients, id); }

    // check if a doctor is already booked at a given date/time
    bool isDoctorBooked(const string& docId, const string& date, const string& time) {
        // STL algorithm: any_of checks if any element satisfies the condition
        return any_of(appointments.begin(), appointments.end(),
            [&](const Appointment& a) {
                return a.doctorId == docId &&
                       a.date     == date  &&
                       a.time     == time  &&
                       a.status   != "Cancelled";
            });
    }

    // ── File I/O with exception handling ────────────
    void saveAll() {
        try {
            ofstream uf(USER_FILE);
            if (!uf) throw runtime_error("Cannot open " + USER_FILE);
            for (const auto& u : users) u.save(uf);

            ofstream pf(PAT_FILE);
            if (!pf) throw runtime_error("Cannot open " + PAT_FILE);
            for (const auto& p : patients) p.save(pf);

            ofstream df(DOC_FILE);
            if (!df) throw runtime_error("Cannot open " + DOC_FILE);
            for (const auto& d : doctors) d.save(df);

            ofstream af(APPT_FILE);
            if (!af) throw runtime_error("Cannot open " + APPT_FILE);
            for (const auto& a : appointments) a.save(af);

            ofstream ef(EMG_FILE);
            if (!ef) throw runtime_error("Cannot open " + EMG_FILE);
            for (const auto& e : emergencies) e.save(ef);

        } catch (const runtime_error& e) {
            cerr << "Save error: " << e.what() << endl;
        }
    }

    void loadAll() {
        string line;

        try {
            ifstream uf(USER_FILE);
            while (getline(uf, line)) {
                if (trim(line).empty()) continue;
                User u; u.load(line); users.push_back(u);
            }

            ifstream pf(PAT_FILE);
            while (getline(pf, line)) {
                if (trim(line).empty()) continue;
                Patient p; p.load(line); patients.push_back(p);
            }

            ifstream df(DOC_FILE);
            while (getline(df, line)) {
                if (trim(line).empty()) continue;
                Doctor d; d.load(line); doctors.push_back(d);
            }

            ifstream af(APPT_FILE);
            while (getline(af, line)) {
                if (trim(line).empty()) continue;
                Appointment a; a.load(line); appointments.push_back(a);
            }

            ifstream ef(EMG_FILE);
            while (getline(ef, line)) {
                if (trim(line).empty()) continue;
                EmergencyAppointment ea; ea.load(line); emergencies.push_back(ea);
            }

        } catch (const exception& e) {
            cerr << "Load warning: " << e.what() << endl;
        }
    }

    // append one entry to the medical history log
    void logHistory(const string& patientId, const string& entry) {
        try {
            ofstream hf(HIST_FILE, ios::app);
            if (!hf) throw runtime_error("Cannot open history file.");
            hf << patientId << "|" << entry << "\n";
        } catch (const runtime_error& e) {
            cerr << "History log error: " << e.what() << endl;
        }
    }

    // create a default admin account if no users exist
    void seedAdmin() {
        if (users.empty()) {
            User admin;
            admin.username = "admin";
            admin.password = "admin123";
            admin.role     = "admin";
            users.push_back(admin);
            saveAll();
            cout << "Default admin created. Username: admin | Password: admin123\n";
        }
    }

public:

    //  STARTUP
    void start() {
        loadAll();
        seedAdmin();
        login();
    }

    //  LOGIN  — with exception-safe attempts
    void login() {
        int attempts = 0;
        while (attempts < 3) {
            cout << "\n===== HOSPITAL SYSTEM LOGIN =====\n";
            string uname, pass;
            cout << "Username: "; cin >> uname;
            cout << "Password: "; cin >> pass;
            cin.ignore();

            // STL find_if to search through users
            auto it = find_if(users.begin(), users.end(),
                [&](const User& u) {
                    return u.username == uname && u.password == pass;
                });

            if (it != users.end()) {
                currentUser = *it;
                cout << "\nWelcome, " << currentUser.username
                     << " [" << currentUser.role << "]\n";
                mainMenu();
                return;
            }

            attempts++;
            cout << "Wrong credentials. " << (3 - attempts) << " attempt(s) left.\n";
        }
        cout << "Too many failed attempts. Exiting.\n";
    }

    //  MAIN MENU
    void mainMenu() {
        int choice;
        do {
            cout << "\n========= MAIN MENU =========\n";
            cout << "Role: " << currentUser.role << "\n";
            divider();
            cout << "1. Patient Management\n"
                 << "2. Doctor Management\n"
                 << "3. Appointment Management\n"
                 << "4. Emergency Appointment\n"
                 << "5. Medical History\n"
                 << "6. Daily Appointment Report\n";
            if (currentUser.role == "admin") {
                cout << "7. User Management\n"
                     << "8. Revenue Report\n"
                     << "9. Sort & Statistics\n";
            }
            cout << "0. Logout\n";
            divider();
            cout << "Choice: "; cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: patientMenu();        break;
                case 2: doctorMenu();         break;
                case 3: appointmentMenu();    break;
                case 4: emergencyMenu();      break;
                case 5: historyMenu();        break;
                case 6: dailyReport();        break;
                case 7: if (currentUser.role == "admin") userMenu();      break;
                case 8: if (currentUser.role == "admin") revenueReport(); break;
                case 9: if (currentUser.role == "admin") statsMenu();     break;
                case 0: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 0);
    }

    //  PATIENT MANAGEMENT
    void patientMenu() {
        int choice;
        do {
            cout << "\n--- Patient Management ---\n"
                 << "1. Add Patient\n"
                 << "2. View All Patients\n"
                 << "3. Search Patient\n"
                 << "4. Edit Patient\n"
                 << "5. Delete Patient\n"
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: addPatient();    break;
                case 2: viewPatients();  break;
                case 3: searchPatient(); break;
                case 4: editPatient();   break;
                case 5: deletePatient(); break;
            }
        } while (choice != 0);
    }

    void addPatient() {
        if (currentUser.role == "doctor") {
            cout << "Access denied.\n"; return;
        }

        Patient p;
        p.id = nextId("P", patients.size());
        cout << "\n-- Add New Patient --\n";

        cout << "Name   : "; getline(cin, p.name);
        if (trim(p.name).empty()) {
            cout << "Name cannot be empty.\n"; return;
        }

        cout << "Age    : "; getline(cin, p.age);
        // exception handling on age input
        try {
            if (!isNumber(p.age)) throw invalid_argument("Age must be a number.");
            int ageVal = stoi(p.age);
            if (ageVal <= 0 || ageVal > 150)
                throw out_of_range("Age must be between 1 and 150.");
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n"; return;
        }

        cout << "Phone  : "; getline(cin, p.phone);
        if (!isNumber(p.phone)) {
            cout << "Phone must be digits only.\n"; return;
        }

        cout << "Gender (M/F): "; getline(cin, p.gender);

        patients.push_back(p);
        saveAll();
        cout << "Patient added! ID: " << p.id << "\n";
    }

    void viewPatients() {
        // uses the template printAll<Patient>() with operator<<
        printAll(patients, "All Patients");
    }

    void searchPatient() {
        cout << "Enter name or ID: ";
        string key; getline(cin, key);

        vector<Patient> results;
        copy_if(patients.begin(), patients.end(), back_inserter(results),
            [&](const Patient& p) {
                return p.id == key ||
                       p.name.find(key) != string::npos;
            });

        if (results.empty()) {
            cout << "No patient found.\n"; return;
        }
        printAll(results, "Search Results");
    }

    void editPatient() {
        if (currentUser.role == "doctor") {
            cout << "Access denied.\n"; return;
        }
        cout << "Enter patient ID to edit: ";
        string id; getline(cin, id);
        Patient* p = findPatient(id);
        if (!p) { cout << "Patient not found.\n"; return; }

        cout << "\nLeave blank and press Enter to keep the current value.\n\n";
        string val;

        cout << "New name   (current: '" << p->name   << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) p->name = val;

        cout << "New age    (current: '" << p->age    << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) {
            try {
                if (!isNumber(val)) throw invalid_argument("Age must be a number.");
                p->age = val;
            } catch (const exception& e) {
                cout << "Error: " << e.what() << " — keeping old value.\n";
            }
        }

        cout << "New phone  (current: '" << p->phone  << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) {
            if (!isNumber(val)) cout << "Invalid phone — keeping old value.\n";
            else p->phone = val;
        }

        cout << "New gender (current: '" << p->gender << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) p->gender = val;

        saveAll();
        cout << "Patient updated successfully.\n";
    }

    void deletePatient() {
        if (currentUser.role != "admin") {
            cout << "Access denied.\n"; return;
        }
        cout << "Enter patient ID to delete: ";
        string id; getline(cin, id);

        // STL remove_if to delete by ID
        auto before = patients.size();
        patients.erase(
            remove_if(patients.begin(), patients.end(),
                [&](const Patient& p) { return p.id == id; }),
            patients.end()
        );

        if (patients.size() < before) {
            saveAll();
            cout << "Patient deleted.\n";
        } else {
            cout << "Patient not found.\n";
        }
    }

    //  DOCTOR MANAGEMENT
    void doctorMenu() {
        if (currentUser.role == "doctor") {
            cout << "Access denied.\n"; return;
        }
        int choice;
        do {
            cout << "\n--- Doctor Management ---\n"
                 << "1. Add Doctor\n"
                 << "2. View All Doctors\n"
                 << "3. Search Doctor\n"
                 << "4. Edit Doctor\n"
                 << "5. Delete Doctor\n"
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: addDoctor();    break;
                case 2: viewDoctors();  break;
                case 3: searchDoctor(); break;
                case 4: editDoctor();   break;
                case 5: deleteDoctor(); break;
            }
        } while (choice != 0);
    }

    void addDoctor() {
        if (currentUser.role != "admin") {
            cout << "Access denied.\n"; return;
        }
        Doctor d;
        d.id = nextId("D", doctors.size());
        cout << "\n-- Add New Doctor --\n";

        cout << "Name           : "; getline(cin, d.name);
        if (trim(d.name).empty()) { cout << "Name cannot be empty.\n"; return; }

        cout << "Phone          : "; getline(cin, d.phone);
        if (!isNumber(d.phone)) { cout << "Phone must be digits only.\n"; return; }

        cout << "Specialization : "; getline(cin, d.specialization);
        cout << "Work Days      : "; getline(cin, d.workDays);
        cout << "Work Hours     : "; getline(cin, d.workHours);

        string feeStr;
        cout << "Consultation Fee ($): "; getline(cin, feeStr);

        // exception handling on fee conversion
        try {
            d.fee = feeStr.empty() ? 0.0 : stod(feeStr);
            if (d.fee < 0) throw out_of_range("Fee cannot be negative.");
        } catch (const exception& e) {
            cout << "Warning: " << e.what() << " — fee set to 0.\n";
            d.fee = 0.0;
        }

        doctors.push_back(d);
        saveAll();
        cout << "Doctor added! ID: " << d.id << "\n";
    }

    void viewDoctors() {
        // uses template printAll<Doctor>() with operator<<
        printAll(doctors, "All Doctors");
    }

    void searchDoctor() {
        cout << "Enter name or specialization: ";
        string key; getline(cin, key);

        vector<Doctor> results;
        copy_if(doctors.begin(), doctors.end(), back_inserter(results),
            [&](const Doctor& d) {
                return d.id == key ||
                       d.name.find(key)           != string::npos ||
                       d.specialization.find(key) != string::npos;
            });

        if (results.empty()) { cout << "No doctor found.\n"; return; }
        printAll(results, "Search Results");
    }

    void editDoctor() {
        if (currentUser.role != "admin") {
            cout << "Access denied.\n"; return;
        }
        cout << "Enter doctor ID to edit: ";
        string id; getline(cin, id);
        Doctor* d = findDoctor(id);
        if (!d) { cout << "Doctor not found.\n"; return; }

        cout << "\nLeave blank and press Enter to keep the current value.\n\n";
        string val;

        cout << "New name           (current: '" << d->name           << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) d->name = val;

        cout << "New phone          (current: '" << d->phone          << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) {
            if (!isNumber(val)) cout << "Invalid phone — keeping old value.\n";
            else d->phone = val;
        }

        cout << "New specialization (current: '" << d->specialization << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) d->specialization = val;

        cout << "New work days      (current: '" << d->workDays       << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) d->workDays = val;

        cout << "New work hours     (current: '" << d->workHours      << "'): ";
        getline(cin, val);
        if (!trim(val).empty()) d->workHours = val;

        cout << "New fee (current: $"
             << fixed << setprecision(2) << d->fee << "): ";
        getline(cin, val);
        if (!trim(val).empty()) {
            try {
                d->fee = stod(val);
                if (d->fee < 0) throw out_of_range("Fee cannot be negative.");
            } catch (const exception& e) {
                cout << "Error: " << e.what() << " — keeping old value.\n";
            }
        }

        saveAll();
        cout << "Doctor updated successfully.\n";
    }

    void deleteDoctor() {
        if (currentUser.role != "admin") {
            cout << "Access denied.\n"; return;
        }
        cout << "Enter doctor ID to delete: ";
        string id; getline(cin, id);

        auto before = doctors.size();
        doctors.erase(
            remove_if(doctors.begin(), doctors.end(),
                [&](const Doctor& d) { return d.id == id; }),
            doctors.end()
        );

        if (doctors.size() < before) {
            saveAll();
            cout << "Doctor deleted.\n";
        } else {
            cout << "Doctor not found.\n";
        }
    }

    //  APPOINTMENT MANAGEMENT
    void appointmentMenu() {
        int choice;
        do {
            cout << "\n--- Appointment Management ---\n"
                 << "1. Book Appointment\n"
                 << "2. View All Appointments\n"
                 << "3. View My Appointments (Doctor)\n"
                 << "4. Cancel Appointment\n"
                 << "5. Complete Appointment\n"
                 << "6. Add Prescription\n"
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: bookAppointment();     break;
                case 2: viewAppointments();    break;
                case 3: myAppointments();      break;
                case 4: cancelAppointment();   break;
                case 5: completeAppointment(); break;
                case 6: addPrescription();     break;
            }
        } while (choice != 0);
    }

    void bookAppointment() {
        if (currentUser.role == "doctor") {
            cout << "Access denied.\n"; return;
        }
        cout << "\n-- Book Appointment --\n";
        viewDoctors();

        Appointment a;
        cout << "Patient ID       : "; getline(cin, a.patientId);
        if (!findPatient(a.patientId)) { cout << "Patient not found.\n"; return; }

        cout << "Doctor ID        : "; getline(cin, a.doctorId);
        Doctor* doc = findDoctor(a.doctorId);
        if (!doc) { cout << "Doctor not found.\n"; return; }

        cout << "Date (DD/MM/YYYY): "; getline(cin, a.date);
        cout << "Time (HH:MM)     : "; getline(cin, a.time);

        if (isDoctorBooked(a.doctorId, a.date, a.time)) {
            cout << "Doctor is already booked at that time.\n"; return;
        }

        a.id       = nextId("A", appointments.size());
        a.status   = "Scheduled";
        a.isEmergency = false;

        appointments.push_back(a);

        Patient* p = findPatient(a.patientId);
        if (p) p->visitCount++;

        logHistory(a.patientId,
            "Appointment booked with Dr." + doc->name +
            " on " + a.date + " at " + a.time);

        saveAll();
        cout << "Appointment booked! ID: " << a.id << "\n";
        cout << "Consultation Fee: $"
             << fixed << setprecision(2) << doc->fee << "\n";
    }

    void viewAppointments() {
        printAll(appointments, "All Appointments");
    }

    void myAppointments() {
        if (currentUser.role != "doctor") {
            cout << "Use 'View All' instead.\n"; return;
        }
        cout << "\n-- My Appointments --\n";
        bool found = false;
        for (const auto& a : appointments) {
            for (const auto& d : doctors) {
                if (d.id == a.doctorId && d.name == currentUser.username) {
                    divider(); cout << a;
                    found = true;
                }
            }
        }
        if (!found) cout << "No appointments found for you.\n";
    }

    void cancelAppointment() {
        cout << "Enter appointment ID to cancel: ";
        string id; getline(cin, id);

        Appointment* a = findByIdLocal(appointments, id);
        if (!a) { cout << "Appointment not found.\n"; return; }

        if (a->status == "Completed") {
            cout << "Cannot cancel a completed appointment.\n"; return;
        }
        a->status = "Cancelled";
        logHistory(a->patientId, "Appointment " + id + " cancelled.");
        saveAll();
        cout << "Appointment cancelled.\n";
    }

    void completeAppointment() {
        cout << "Enter appointment ID to complete: ";
        string id; getline(cin, id);

        Appointment* a = findByIdLocal(appointments, id);
        if (!a) { cout << "Appointment not found.\n"; return; }

        cout << "Add notes (optional): "; getline(cin, a->notes);
        a->status = "Completed";
        logHistory(a->patientId,
            "Appointment " + id + " completed. Notes: " + a->notes);
        saveAll();
        cout << "Appointment marked as completed.\n";
    }

    void addPrescription() {
        cout << "Enter appointment ID: ";
        string id; getline(cin, id);

        Appointment* a = findByIdLocal(appointments, id);
        if (!a) { cout << "Appointment not found.\n"; return; }

        if (a->status != "Completed") {
            cout << "Please complete the appointment first.\n"; return;
        }
        cout << "Prescription (medicine, dosage, duration): ";
        getline(cin, a->prescription);
        logHistory(a->patientId, "Prescription added: " + a->prescription);
        saveAll();
        cout << "Prescription saved.\n";
    }

    //  EMERGENCY APPOINTMENT
    void emergencyMenu() {
        int choice;
        do {
            cout << "\n--- Emergency Appointments ---\n"
                 << "1. Register Emergency Appointment\n"
                 << "2. View All Emergency Appointments\n"
                 << "3. Complete Emergency Appointment\n"
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: registerEmergency(); break;
                case 2: viewEmergencies();   break;
                case 3: completeEmergency(); break;
            }
        } while (choice != 0);
    }

    void registerEmergency() {
        if (currentUser.role == "doctor") {
            cout << "Access denied.\n"; return;
        }

        EmergencyAppointment ea;
        cout << "\n-- Register Emergency Appointment --\n";
        viewDoctors();

        cout << "Patient ID       : "; getline(cin, ea.patientId);
        if (!findPatient(ea.patientId)) { cout << "Patient not found.\n"; return; }

        cout << "Doctor ID        : "; getline(cin, ea.doctorId);
        Doctor* doc = findDoctor(ea.doctorId);
        if (!doc) { cout << "Doctor not found.\n"; return; }

        cout << "Date (DD/MM/YYYY): "; getline(cin, ea.date);
        cout << "Time (HH:MM)     : "; getline(cin, ea.time);
        cout << "Reason           : "; getline(cin, ea.reason);

        string priStr;
        cout << "Priority (1=Critical, 2=Urgent, 3=Moderate): ";
        getline(cin, priStr);

        try {
            ea.priorityLevel = stoi(priStr);
            if (ea.priorityLevel < 1 || ea.priorityLevel > 3)
                throw out_of_range("Priority must be 1, 2, or 3.");
        } catch (const exception& e) {
            cout << "Warning: " << e.what() << " — setting to 1 (Critical).\n";
            ea.priorityLevel = 1;
        }

        ea.id = nextId("E", emergencies.size());

        emergencies.push_back(ea);

        Patient* p = findPatient(ea.patientId);
        if (p) p->visitCount++;

        logHistory(ea.patientId,
            "Emergency appointment (P" + to_string(ea.priorityLevel) +
            ") booked with Dr." + doc->name +
            " on " + ea.date + ". Reason: " + ea.reason);

        saveAll();
        cout << "Emergency appointment registered! ID: " << ea.id << "\n";
    }

    void viewEmergencies() {
        if (emergencies.empty()) { cout << "No emergency appointments.\n"; return; }

        // STL sort — sort emergencies by priority level (1 first = most critical)
        vector<EmergencyAppointment> sorted = emergencies;
        sort(sorted.begin(), sorted.end(),
            [](const EmergencyAppointment& a, const EmergencyAppointment& b) {
                return a.priorityLevel < b.priorityLevel;
            });

        cout << "\n-- Emergency Appointments (sorted by priority) --\n";
        for (const auto& ea : sorted) {
            divider();
            ea.display();   // polymorphic call — uses EmergencyAppointment::display()
        }
        divider();
    }

    void completeEmergency() {
        cout << "Enter emergency appointment ID: ";
        string id; getline(cin, id);

        EmergencyAppointment* ea = findByIdLocal(emergencies, id);
        if (!ea) { cout << "Emergency appointment not found.\n"; return; }

        cout << "Notes: "; getline(cin, ea->notes);
        ea->status = "Completed";
        logHistory(ea->patientId,
            "Emergency appointment " + id + " completed. Notes: " + ea->notes);
        saveAll();
        cout << "Emergency appointment completed.\n";
    }

    //  MEDICAL HISTORY
    void historyMenu() {
        cout << "Enter patient ID to view history: ";
        string id; getline(cin, id);

        Patient* p = findPatient(id);
        if (!p) { cout << "Patient not found.\n"; return; }

        cout << "\n-- Medical History for " << p->name << " --\n";

        try {
            ifstream hf(HIST_FILE);
            if (!hf) throw runtime_error("History file not found.");

            string line;
            bool found = false;
            while (getline(hf, line)) {
                stringstream ss(line);
                string pid, entry;
                getline(ss, pid,   '|');
                getline(ss, entry, '|');
                if (pid == id) {
                    cout << "  > " << entry << "\n";
                    found = true;
                }
            }
            if (!found) cout << "No history found.\n";

        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }

    //  DAILY REPORT
    void dailyReport() {
        cout << "Enter date (DD/MM/YYYY): ";
        string date; getline(cin, date);

        // STL count_if — count how many appointments are on this date
        int total = count_if(appointments.begin(), appointments.end(),
            [&](const Appointment& a) { return a.date == date; });

        if (total == 0) {
            cout << "No appointments on this date.\n"; return;
        }

        cout << "\n===== Daily Report: " << date << " =====\n";
        double revenue = 0;

        for (const auto& a : appointments) {
            if (a.date == date) {
                divider();
                cout << a;   // operator<<
                Doctor* doc = findDoctor(a.doctorId);
                if (doc && a.status == "Completed")
                    revenue += doc->fee;
            }
        }

        divider();
        cout << "Total Appointments : " << total << "\n"
             << "Revenue (completed): $"
             << fixed << setprecision(2) << revenue << "\n";

        // save report to file with exception handling
        try {
            string fname = "daily_report_"
                + date.substr(0,2)
                + date.substr(3,2)
                + date.substr(6,4) + ".txt";

            ofstream rf(fname);
            if (!rf) throw runtime_error("Cannot create report file.");

            rf << "Daily Report: " << date << "\n"
               << "Total Appointments: " << total << "\n"
               << "Revenue: $" << fixed << setprecision(2) << revenue << "\n\n";

            for (const auto& a : appointments) {
                if (a.date == date) {
                    rf << "----\n"
                       << "Appt: "    << a.id        << " | "
                       << "Patient: " << a.patientId << " | "
                       << "Doctor: "  << a.doctorId  << " | "
                       << "Time: "    << a.time      << " | "
                       << "Status: "  << a.status    << "\n";
                }
            }
            cout << "Report saved to " << fname << "\n";

        } catch (const runtime_error& e) {
            cerr << "Report error: " << e.what() << "\n";
        }
    }

    //  REVENUE REPORT  (admin only)
    void revenueReport() {
        cout << "\n===== Revenue Report =====\n";
        double total = 0;

        for (const auto& d : doctors) {
            // STL count_if — count completed appointments per doctor
            int count = count_if(appointments.begin(), appointments.end(),
                [&](const Appointment& a) {
                    return a.doctorId == d.id && a.status == "Completed";
                });

            double docRevenue = count * d.fee;
            cout << "Dr. " << d.name
                 << " (" << d.specialization << ")"
                 << " | Completed: " << count
                 << " | Revenue: $"
                 << fixed << setprecision(2) << docRevenue << "\n";
            total += docRevenue;
        }
        divider();
        cout << "Total Revenue: $" << fixed << setprecision(2) << total << "\n";
    }

    //  SORT & STATISTICS  (admin only)
    void statsMenu() {
        int choice;
        do {
            cout << "\n--- Sort & Statistics ---\n"
                 << "1. Patients sorted by name\n"
                 << "2. Patients sorted by visit count\n"
                 << "3. Doctors sorted by fee\n"
                 << "4. Count scheduled appointments\n"
                 << "5. Most visited patient\n"
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: {
                    // STL sort by name alphabetically
                    vector<Patient> sorted = patients;
                    sort(sorted.begin(), sorted.end(),
                        [](const Patient& a, const Patient& b) {
                            return a.name < b.name;
                        });
                    printAll(sorted, "Patients (A-Z)");
                    break;
                }
                case 2: {
                    // STL sort by visit count descending
                    vector<Patient> sorted = patients;
                    sort(sorted.begin(), sorted.end(),
                        [](const Patient& a, const Patient& b) {
                            return a.visitCount > b.visitCount;
                        });
                    printAll(sorted, "Patients (Most Visits First)");
                    break;
                }
                case 3: {
                    // STL sort doctors by fee descending
                    vector<Doctor> sorted = doctors;
                    sort(sorted.begin(), sorted.end(),
                        [](const Doctor& a, const Doctor& b) {
                            return a.fee > b.fee;
                        });
                    printAll(sorted, "Doctors (Highest Fee First)");
                    break;
                }
                case 4: {
                    // STL count_if for scheduled appointments
                    int n = count_if(appointments.begin(), appointments.end(),
                        [](const Appointment& a) {
                            return a.status == "Scheduled";
                        });
                    cout << "Scheduled appointments: " << n << "\n";
                    break;
                }
                case 5: {
                    if (patients.empty()) { cout << "No patients.\n"; break; }
                    // STL max_element to find highest visit count
                    auto it = max_element(patients.begin(), patients.end(),
                        [](const Patient& a, const Patient& b) {
                            return a.visitCount < b.visitCount;
                        });
                    cout << "\nMost visited patient:\n";
                    divider();
                    cout << *it;   // operator<<
                    break;
                }
            }
        } while (choice != 0);
    }

    //  USER MANAGEMENT  (admin only)
    void userMenu() {
        int choice;
        do {
            cout << "\n--- User Management ---\n"
                 << "1. Add User\n"
                 << "2. View All Users\n"
                 << "3. Delete User\n"
                 << "0. Back\n"
                 << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: addUser();    break;
                case 2: viewUsers();  break;
                case 3: deleteUser(); break;
            }
        } while (choice != 0);
    }

    void addUser() {
        User u;
        cout << "\n-- Add New User --\n";
        cout << "Username : "; getline(cin, u.username);

        // check for duplicate username using STL find_if
        auto exists = find_if(users.begin(), users.end(),
            [&](const User& x) { return x.username == u.username; });
        if (exists != users.end()) {
            cout << "Username already exists.\n"; return;
        }

        cout << "Password : "; getline(cin, u.password);
        cout << "Role (admin/receptionist/doctor): "; getline(cin, u.role);

        if (u.role != "admin" && u.role != "receptionist" && u.role != "doctor") {
            cout << "Invalid role.\n"; return;
        }

        users.push_back(u);
        saveAll();
        cout << "User added.\n";
    }

    void viewUsers() {
        cout << "\n-- All Users --\n";
        for (const auto& u : users)
            cout << "Username: " << u.username << " | Role: " << u.role << "\n";
    }

    void deleteUser() {
        cout << "Enter username to delete: ";
        string uname; getline(cin, uname);

        auto before = users.size();
        users.erase(
            remove_if(users.begin(), users.end(),
                [&](const User& u) { return u.username == uname; }),
            users.end()
        );

        if (users.size() < before) {
            saveAll();
            cout << "User deleted.\n";
        } else {
            cout << "User not found.\n";
        }
    }
};

//  MAIN
int main() {
    try {
        HospitalSystem system;
        system.start();
    } catch (const exception& e) {
        // top-level safety net — catches any unhandled exception
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }
    return 0;
}