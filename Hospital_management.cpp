#include <iostream>
#include <sqlite3.h>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>

using namespace std;

// Function to initialize the SQLite database and create tables
void initDatabase() {
    sqlite3* DB;
    
    string sql = "CREATE TABLE IF NOT EXISTS PATIENT("
                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "NAME TEXT NOT NULL, "
                 "ADDRESS TEXT NOT NULL, "
                 "CONTACT TEXT NOT NULL, "
                 "AGE TEXT NOT NULL, "
                 "SEX TEXT NOT NULL, "
                 "BLOOD_GROUP TEXT NOT NULL, "
                 "DISEASE_PAST TEXT NOT NULL, "
                 "PATIENT_ID TEXT NOT NULL UNIQUE);";

    // SQL command to create the DIAGNOSIS table
    string diagnosisTable = "CREATE TABLE IF NOT EXISTS DIAGNOSIS("
                            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "PATIENT_ID TEXT NOT NULL, "
                            "DIAGNOSIS TEXT NOT NULL, "
                            "MEDICATION TEXT NOT NULL, "
                            "DOCTOR TEXT NOT NULL, "
                            "FOREIGN KEY (PATIENT_ID) REFERENCES PATIENT (PATIENT_ID));";

    // SQL command to create the APPOINTMENT table
    string appointmentTable = "CREATE TABLE IF NOT EXISTS APPOINTMENT("
                              "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "PATIENT_ID TEXT NOT NULL, "
                              "DOCTOR_ID TEXT NOT NULL, "
                              "DATE TEXT NOT NULL, "
                              "TIME TEXT NOT NULL, "
                              "FOREIGN KEY (PATIENT_ID) REFERENCES PATIENT (PATIENT_ID));";

    // SQL command to create the SCHEDULE table
    string scheduleTable = "CREATE TABLE IF NOT EXISTS SCHEDULE("
                           "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "DOCTOR_ID TEXT NOT NULL, "
                           "DATE TEXT NOT NULL, "
                           "TIME TEXT NOT NULL);";

    int exit = sqlite3_open("hospital.db", &DB);
    char* messageError;

    // Execute all the SQL commands
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    exit = sqlite3_exec(DB, diagnosisTable.c_str(), NULL, 0, &messageError);
    exit = sqlite3_exec(DB, appointmentTable.c_str(), NULL, 0, &messageError);
    exit = sqlite3_exec(DB, scheduleTable.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error creating tables: " << messageError << endl;
        sqlite3_free(messageError);
    } else {
        cout << "Database and tables created successfully!" << endl;
    }

    sqlite3_close(DB);
}

// Function to add a new patient to the database
void addNewPatient() {
    sqlite3* DB;
    sqlite3_open("hospital.db", &DB);

    char name[50], address[100], contact[15], age[5], sex[8], blood_gp[5], disease_past[50], id[15];
    cout << "\nEnter Patient Details\n";
    cout << "Name: "; cin.ignore(); cin.getline(name, 50);
    cout << "Address: "; cin.getline(address, 100);
    cout << "Contact Number: "; cin.getline(contact, 15);
    cout << "Age: "; cin.getline(age, 5);
    cout << "Sex: "; cin.getline(sex, 8);
    cout << "Blood Group: "; cin.getline(blood_gp, 5);
    cout << "Any Major disease suffered earlier: "; cin.getline(disease_past, 50);
    cout << "Patient ID: "; cin.getline(id, 15);

    string sql = "INSERT INTO PATIENT (NAME, ADDRESS, CONTACT, AGE, SEX, BLOOD_GROUP, DISEASE_PAST, PATIENT_ID) VALUES('" +
                 string(name) + "', '" + string(address) + "', '" + string(contact) + "', '" +
                 string(age) + "', '" + string(sex) + "', '" + string(blood_gp) + "', '" +
                 string(disease_past) + "', '" + string(id) + "');";

    char* messageError;
    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error inserting data: " << messageError << endl;
        sqlite3_free(messageError);
    } else {
        cout << "\nInformation Saved Successfully\n";
    }

    sqlite3_close(DB);
}

// Function to fetch a patient's history from the database
void fetchPatientHistory() {
    sqlite3* DB;
    sqlite3_open("hospital.db", &DB);

    char id[15];
    cout << "\nEnter the Patient ID to retrieve history: ";
    cin.ignore(); cin.getline(id, 15);

    string sql = "SELECT * FROM PATIENT WHERE PATIENT_ID = '" + string(id) + "';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "\nName: " << sqlite3_column_text(stmt, 1);
            cout << "\nAddress: " << sqlite3_column_text(stmt, 2);
            cout << "\nContact: " << sqlite3_column_text(stmt, 3);
            cout << "\nAge: " << sqlite3_column_text(stmt, 4);
            cout << "\nSex: " << sqlite3_column_text(stmt, 5);
            cout << "\nBlood Group: " << sqlite3_column_text(stmt, 6);
            cout << "\nPast Diseases: " << sqlite3_column_text(stmt, 7);
            cout << "\nPatient ID: " << sqlite3_column_text(stmt, 8) << endl;
        }
    } else {
        cout << "Error fetching patient history." << endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
}

// Function to append diagnosis information
void addDiagnosisInfo() {
    sqlite3* DB;
    sqlite3_open("hospital.db", &DB);

    char id[15], diagnosis[100], medication[100], doctor[50];
    cout << "\nEnter Patient ID: "; cin.ignore(); cin.getline(id, 15);
    cout << "Diagnosis: "; cin.getline(diagnosis, 100);
    cout << "Medication Prescribed: "; cin.getline(medication, 100);
    cout << "Consulting Doctor: "; cin.getline(doctor, 50);

    string sql = "INSERT INTO DIAGNOSIS (PATIENT_ID, DIAGNOSIS, MEDICATION, DOCTOR) VALUES('" +
                 string(id) + "', '" + string(diagnosis) + "', '" + string(medication) + "', '" + string(doctor) + "');";

    char* messageError;
    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error inserting diagnosis data: " << messageError << endl;
        sqlite3_free(messageError);
    } else {
        cout << "\nDiagnosis Information Added Successfully\n";
    }

    sqlite3_close(DB);
}

// Function to book an appointment
void bookAppointment() {
    sqlite3* DB;
    sqlite3_open("hospital.db", &DB);

    char patientID[15], doctorID[15], date[11], time[6];

    cout << "\nEnter Patient ID: "; cin.ignore(); cin.getline(patientID, 15);
    
    // Display available doctors
    cout << "\nAvailable Doctors:\n";
    cout << setw(10) << left << "ID" << setw(20) << "Doctor Name" << setw(20) << "Specialization" << endl;
    cout << "------------------------------------------------------------------------------------------\n";
    cout << setw(10) << left << "1" << setw(20) << "Dr. Smith" << setw(20) << "Cardiology" << endl;
    cout << setw(10) << left << "2" << setw(20) << "Dr. Jones" << setw(20) << "Neurology" << endl;
    cout << setw(10) << left << "3" << setw(20) << "Dr. Brown" << setw(20) << "Orthopedics" << endl;
    cout << setw(10) << left << "4" << setw(20) << "Dr. Taylor" << setw(20) << "Pediatrics" << endl;
    cout << setw(10) << left << "5" << setw(20) << "Dr. Wilson" << setw(20) << "Dermatology" << endl;
    cout << "------------------------------------------------------------------------------------------\n";

    cout << "\nEnter Doctor ID to schedule an appointment: "; cin.getline(doctorID, 15);
    cout << "Enter Appointment Date (YYYY-MM-DD): "; cin.getline(date, 11);
    cout << "Enter Appointment Time (HH:MM): "; cin.getline(time, 6);

    string sql = "INSERT INTO APPOINTMENT (PATIENT_ID, DOCTOR_ID, DATE, TIME) VALUES('" +
                 string(patientID) + "', '" + string(doctorID) + "', '" + string(date) + "', '" + string(time) + "');";

    char* messageError;
    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error booking appointment: " << messageError << endl;
        sqlite3_free(messageError);
    } else {
        cout << "\nAppointment Booked Successfully\n";
    }

    sqlite3_close(DB);
}

// Function to check the security key before accessing system features
bool checkSecurityKey() {
    string security_key;
    cout << "\nEnter the Security Key to access the system: ";
    cin >> security_key;

    // Correct security key
    if (security_key == "admin123") {
        return true;
    } else {
        cout << "Incorrect Security Key! Access Denied.\n";
        return false;
    }
}

// Main menu to display options
void displayMenu() {
    cout << "\n----------------------------------------------------\n";
    cout << "           E-Healthcare Management System           \n";
    cout << "----------------------------------------------------\n";
    cout << "1. Add New Patient\n";
    cout << "2. Add Diagnosis Information\n";
    cout << "3. Fetch Patient's History\n";
    cout << "4. Book an Appointment\n";
    cout << "5. Exit\n";
    cout << "----------------------------------------------------\n";
}

int main() {
    initDatabase();

    if (!checkSecurityKey()) {
        return 0; // Exit the program if the security key is incorrect
    }

    int choice;
    while (true) {
        displayMenu();
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addNewPatient();
                break;
            case 2:
                addDiagnosisInfo();
                break;
            case 3:
                fetchPatientHistory();
                break;
            case 4:
                bookAppointment();
                break;
            case 5:
                cout << "Exiting the system...\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
        cout << "----------------------------------------------------\n";
    }
    return 0;
}
