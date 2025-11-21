#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <cstdio>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;

// --- Constants ---
const int STR_SIZE = 50;
const int MAX_TAS = 5;
const int MAX_SECTIONS = 5;

// =======================================================================
// ======================= INPUT VALIDATION UTILITIES =====================
// =======================================================================

// ---------------- TIME VALIDATION ----------------
bool isValidTime(const char* t) {
    if (strlen(t) != 5) return false;
    if (t[2] != ':') return false;
    if (!isdigit(t[0]) || !isdigit(t[1]) || !isdigit(t[3]) || !isdigit(t[4]))
        return false;

    int h = (t[0] - '0') * 10 + (t[1] - '0');
    int m = (t[3] - '0') * 10 + (t[4] - '0');

    return (h >= 0 && h < 24 && m >= 0 && m < 60);
}

bool isStartBeforeEnd(const char* start, const char* end) {
    int h1, m1, h2, m2;
    sscanf(start, "%d:%d", &h1, &m1);
    sscanf(end, "%d:%d", &h2, &m2);

    return (h1 * 60 + m1) < (h2 * 60 + m2);
}

// ---------------- NAME VALIDATION ----------------
bool isValidName(const char* n) {
    if (strlen(n) == 0) return false;

    for (int i = 0; n[i]; i++) {
        if (!isalpha(n[i]) && n[i] != '-' && n[i] != '_')
            return false;
    }
    return true;
}

// ---------------- COURSE CODE VALIDATION (e.g., CS101) ----------------
bool isValidCourseCode(const char* c) {
    int n = strlen(c);
    if (n < 4 || n > 10) return false;

    int i = 0;

    // Letters prefix
    while (i < n && isalpha(c[i])) i++;
    if (i < 2) return false; // at least 2 letters

    // Numbers suffix
    while (i < n && isdigit(c[i])) i++;

    return i == n;
}

// ---------------- DATE VALIDATION (DD-MM or DD-MMM) ----------------
bool isValidDate(const char* d) {
    int len = strlen(d);

    // Format DD-MM (numeric)
    if (len == 5 && isdigit(d[0]) && isdigit(d[1]) && d[2] == '-' &&
        isdigit(d[3]) && isdigit(d[4])) {
        return true;
    }

    // Format DD-MMM
    if (len == 6 || len == 7) {
        if (!isdigit(d[0]) || !isdigit(d[1]) || d[2] != '-') return false;

        for (int i = 3; i < len; i++) {
            if (!isalpha(d[i])) return false;
        }
        return true;
    }

    return false;
}

// =======================================================================
// =========================== CORE CLASSES ===============================
// =======================================================================

// ---------------- Timing ----------------
class Timing {
public:
    char date[20];
    char startTime[10];
    char endTime[10];

    Timing() {
        strcpy(date, "");
        strcpy(startTime, "");
        strcpy(endTime, "");
    }

    void set(const char* d, const char* s, const char* e) {
        strcpy(date, d);
        strcpy(startTime, s);
        strcpy(endTime, e);
    }
};

// ---------------- Room ----------------
class Room {
public:
    int roomId;
    int roomNumber;

    Room() : roomId(0), roomNumber(0) {}
    Room(int id, int num) : roomId(id), roomNumber(num) {}
};

// ---------------- Building ----------------
class Building {
public:
    int buildingId;
    char name[STR_SIZE];

    Building() : buildingId(0) { strcpy(name, ""); }
    Building(int id, const char* n) : buildingId(id) { strcpy(name, n); }
};
// =======================================================================
// =========================== SECTION & LAB =============================
// =======================================================================

class Section {
public:
    char sectionName[10];
    int instructorId;
    char instructorName[STR_SIZE];

    int taCount;
    int taIds[MAX_TAS];
    char taNames[MAX_TAS][STR_SIZE];

    int buildingId;
    int roomId;
    Timing scheduleTime;

    Section() : instructorId(0), taCount(0), buildingId(0), roomId(0) {
        strcpy(sectionName, "");
        strcpy(instructorName, "");
    }

    void setDetails(const char* name, int instId, const char* instName, int bId, int rId) {
        strcpy(sectionName, name);
        instructorId = instId;
        strcpy(instructorName, instName);
        buildingId = bId;
        roomId = rId;
    }

    void addTA(int id, const char* name) {
        if (taCount < MAX_TAS) {
            // Check for duplicate TA ID
            for (int i = 0; i < taCount; i++) {
                if (taIds[i] == id) return;
            }
            taIds[taCount] = id;
            strcpy(taNames[taCount], name);
            taCount++;
        }
    }
};

class Lab {
public:
    int labId;
    char courseCode[STR_SIZE];

    int sectionCount;
    Section sections[MAX_SECTIONS];

    Lab() : labId(0), sectionCount(0) { strcpy(courseCode, ""); }

    void addSection(Section s) {
        if (sectionCount < MAX_SECTIONS) {
            sections[sectionCount] = s;
            sectionCount++;
        } else {
            cout << "Error: Max sections reached for this lab.\n";
        }
    }

    Section* findSection(const char* secName) {
        for (int i = 0; i < sectionCount; i++) {
            if (strcmp(sections[i].sectionName, secName) == 0) {
                return &sections[i];
            }
        }
        return nullptr;
    }
};

// =======================================================================
// =========================== TIMESHEET ENTRY ===========================
// =======================================================================

class TimeSheetEntry {
public:
    int labId;
    char sectionName[10];
    Timing actualTiming;
    bool isLeave;

    TimeSheetEntry() : labId(0), isLeave(false) { strcpy(sectionName, ""); }
};

// =======================================================================
// =========================== REPOSITORIES ==============================
// =======================================================================

class ILabRepository {
public:
    virtual void addLab(Lab l) = 0;
    virtual void updateLab(Lab l) = 0;
    virtual Lab* findLab(int id) = 0;
    virtual vector<Lab>& getAllLabs() = 0;
    virtual ~ILabRepository() {}
};

class ITimeSheetRepository {
public:
    virtual void addEntry(TimeSheetEntry entry) = 0;
    virtual vector<TimeSheetEntry>& getAllEntries() = 0;
    virtual ~ITimeSheetRepository() {}
};

class IVenueRepository {
public:
    virtual void addBuilding(Building b) = 0;
    virtual void addRoom(Room r) = 0;
    virtual Building* findBuilding(int id) = 0;
    virtual Room* findRoom(int id) = 0;
    virtual ~IVenueRepository() {}
};

// =======================================================================
// ======================== CONCRETE REPOSITORIES ========================
// =======================================================================

class LabRepository : public ILabRepository {
    vector<Lab> labs;
public:
    void addLab(Lab l) override { labs.push_back(l); }

    void updateLab(Lab l) override {
        for (auto& existing : labs) {
            if (existing.labId == l.labId) {
                existing = l;
                return;
            }
        }
    }

    Lab* findLab(int id) override {
        for (auto& l : labs) {
            if (l.labId == id) return &l;
        }
        return nullptr;
    }

    vector<Lab>& getAllLabs() override { return labs; }
};

class TimeSheetRepository : public ITimeSheetRepository {
    vector<TimeSheetEntry> timesheets;
public:
    void addEntry(TimeSheetEntry entry) override { timesheets.push_back(entry); }
    vector<TimeSheetEntry>& getAllEntries() override { return timesheets; }
};

class VenueRepository : public IVenueRepository {
    vector<Building> buildings;
    vector<Room> rooms;
public:
    void addBuilding(Building b) override { buildings.push_back(b); }
    void addRoom(Room r) override { rooms.push_back(r); }

    Building* findBuilding(int id) override {
        for (auto& b : buildings) if (b.buildingId == id) return &b;
        return nullptr;
    }

    Room* findRoom(int id) override {
        for (auto& r : rooms) if (r.roomId == id) return &r;
        return nullptr;
    }

    void seedData() {
        if (buildings.empty()) {
            buildings.push_back(Building(1, "Computer Science Block"));
            buildings.push_back(Building(2, "Electrical Eng Block"));
        }
        if (rooms.empty()) {
            rooms.push_back(Room(101, 101));
            rooms.push_back(Room(102, 102));
            rooms.push_back(Room(201, 201));
        }
    }
};
// =======================================================================
// =========================== PERSON & ROLES ============================
// =======================================================================

class Person {
protected:
    char name[STR_SIZE];
public:
    Person(const char* n) { strcpy(name, n); }
    virtual ~Person() {}
};

// ---------------- HOD ----------------
class HOD : public Person {
private:
    float calculateHours(const char* start, const char* end) {
        int h1, m1, h2, m2;
        sscanf(start, "%d:%d", &h1, &m1);
        sscanf(end, "%d:%d", &h2, &m2);
        int startMins = h1 * 60 + m1;
        int endMins = h2 * 60 + m2;
        return (float)(endMins - startMins) / 60.0f;
    }

public:
    HOD() : Person("Department Head") {}

    void generateScheduleReport(ILabRepository* lRepo, IVenueRepository* vRepo) {
        cout << "\n[HOD ACTION] Generating Weekly Schedule Report...\n";
        cout << "--------------------------------------------------------------------------------\n";
        vector<Lab>& labs = lRepo->getAllLabs();
        if (labs.empty()) { cout << "No labs scheduled.\n"; return; }

        for (auto& l : labs) {
            cout << "COURSE: " << l.courseCode << " (ID: " << l.labId << ")\n";

            if (l.sectionCount == 0) { cout << "  No sections defined.\n"; }

            for (int i = 0; i < l.sectionCount; i++) {
                Section& sec = l.sections[i];
                Building* b = vRepo->findBuilding(sec.buildingId);
                Room* r = vRepo->findRoom(sec.roomId);

                cout << "  >> SECTION " << sec.sectionName << " | "
                     << sec.scheduleTime.date << " "
                     << sec.scheduleTime.startTime << "-" << sec.scheduleTime.endTime << "\n";

                cout << "     Venue: " << (b ? b->name : "Unknown") << " - Room " 
                     << (r ? to_string(r->roomNumber) : "N/A") << "\n";
                cout << "     Instructor: " << sec.instructorName << " (ID: " << sec.instructorId << ")\n";
                cout << "     TAs: ";
                if (sec.taCount == 0) cout << "None";
                for (int k = 0; k < sec.taCount; k++) {
                    cout << sec.taNames[k] << " (" << sec.taIds[k] << ")" 
                         << (k < sec.taCount - 1 ? ", " : "");
                }
                cout << "\n\n";
            }
            cout << "--------------------------------------------------------------------------------\n";
        }
    }

    void generateWeeklyTimeSheetReport(ITimeSheetRepository* repo) {
        cout << "\n[HOD REPORT] Weekly Filled Time Sheets\n";
        cout << "----------------------------------------------------------------------\n";
        vector<TimeSheetEntry>& ts = repo->getAllEntries();
        if (ts.empty()) { cout << "No timesheets filled.\n"; return; }

        cout << left << setw(8) << "Lab ID" << setw(10) << "Section" 
             << setw(15) << "Date" << setw(15) << "Time" << setw(10) << "Status" << endl;
        for (auto& t : ts) {
            cout << left << setw(8) << t.labId
                 << setw(10) << t.sectionName
                 << setw(15) << t.actualTiming.date
                 << setw(6) << t.actualTiming.startTime << "-" << t.actualTiming.endTime << "   "
                 << setw(10) << (t.isLeave ? "LEAVE" : "PRESENT") << endl;
        }
        cout << "----------------------------------------------------------------------\n";
    }

    void generateLabSemesterReport(ITimeSheetRepository* repo, int targetLabId) {
        cout << "\n[HOD REPORT] Semester Summary for Lab ID: " << targetLabId << "\n";
        cout << "--------------------------------------------------\n";
        vector<TimeSheetEntry>& ts = repo->getAllEntries();

        float totalContactHours = 0;
        int totalLeaves = 0;
        int sessionsCount = 0;
        bool found = false;

        for (auto& t : ts) {
            if (t.labId == targetLabId) {
                found = true;
                sessionsCount++;
                if (t.isLeave) totalLeaves++;
                else totalContactHours += calculateHours(t.actualTiming.startTime, t.actualTiming.endTime);
            }
        }

        if (!found) {
            cout << "No records found for Lab ID " << targetLabId << ".\n";
        } else {
            cout << "Total Sessions Recorded: " << sessionsCount << endl;
            cout << "Total Contact Hours:     " << fixed << setprecision(2) 
                 << totalContactHours << " hours" << endl;
            cout << "Total Leaves:            " << totalLeaves << endl;
        }
        cout << "--------------------------------------------------\n";
    }
};

// ---------------- Academic Officer ----------------
class AcademicOfficer : public Person {
public:
    AcademicOfficer() : Person("Officer") {}

    void scheduleSection(ILabRepository* repo, int id, const char* code, 
                         const char* secName,
                         int instId, const char* instName,
                         int bId, int rId,
                         const char* day, const char* start, const char* end,
                         const vector<pair<int, string>>& tas) {

        // Validation: Course Code
        if (!isValidCourseCode(code)) {
            cout << "Invalid Course Code. Must be letters+digits (e.g., CS101).\n";
            return;
        }

        // Validation: Section Name
        if (!isValidName(secName)) {
            cout << "Invalid Section Name. Only letters allowed.\n";
            return;
        }

        // Validation: Instructor Name
        if (!isValidName(instName)) {
            cout << "Invalid Instructor Name.\n";
            return;
        }

        // Validation: Times
        if (!isValidTime(start) || !isValidTime(end) || !isStartBeforeEnd(start, end)) {
            cout << "Invalid timing. Ensure HH:MM format and Start < End.\n";
            return;
        }

        Lab* existingLab = repo->findLab(id);
        Lab workLab;

        if (existingLab != nullptr) workLab = *existingLab;
        else {
            workLab.labId = id;
            strcpy(workLab.courseCode, code);
        }

        if (workLab.findSection(secName) != nullptr) {
            cout << ">> Error: Section " << secName << " already exists for Lab " << id << ".\n";
            return;
        }

        Section newSec;
        newSec.setDetails(secName, instId, instName, bId, rId);
        newSec.scheduleTime.set(day, start, end);

        // Add TAs with name validation & uniqueness
        for (auto& ta : tas) {
            if (!isValidName(ta.second.c_str())) {
                cout << "Invalid TA Name: " << ta.second << ". Skipping.\n";
                continue;
            }
            newSec.addTA(ta.first, ta.second.c_str());
        }

        workLab.addSection(newSec);

        if (existingLab != nullptr) repo->updateLab(workLab);
        else repo->addLab(workLab);

        cout << ">> Success: Section " << secName << " added to Lab " << id << " (" << code << ").\n";
    }
};

// ---------------- Instructor ----------------
class Instructor : public Person {
public:
    Instructor() : Person("Instructor") {}

    void requestMakeupLab(ILabRepository* repo, int labId, const char* secName, const char* newStart, const char* newEnd) {
        if (!isValidTime(newStart) || !isValidTime(newEnd) || !isStartBeforeEnd(newStart, newEnd)) {
            cout << "Invalid new timings. Ensure HH:MM format and Start < End.\n";
            return;
        }

        Lab* l = repo->findLab(labId);
        if (!l) { cout << ">> Error: Lab ID " << labId << " not found.\n"; return; }

        Section* s = l->findSection(secName);
        if (!s) { cout << ">> Error: Section " << secName << " not found.\n"; return; }

        s->scheduleTime.set("Makeup", newStart, newEnd);
        cout << ">> Success: Makeup requested/approved for Lab " << labId << " Section " << secName << ".\n";
    }
};

// =======================================================================
// =========================== ATTENDANT ================================
// =======================================================================

class Attendant : public Person {
public:
    Attendant() : Person("Lab Attendant") {}

    void fillTimeSheet(ITimeSheetRepository* repo, int labId, const char* secName, 
                       const char* date, const char* start, const char* end, bool isLeave) {
        // Validate times if not on leave
        if (!isLeave) {
            if (!isValidTime(start) || !isValidTime(end) || !isStartBeforeEnd(start, end)) {
                cout << "Invalid timing. Cannot log time sheet entry.\n";
                return;
            }
        }

        TimeSheetEntry entry;
        entry.labId = labId;
        strcpy(entry.sectionName, secName);
        entry.actualTiming.set(date, start, end);
        entry.isLeave = isLeave;
        repo->addEntry(entry);

        cout << ">> Success: Time sheet entry logged for Section " << secName << ".\n";
    }
};

// =======================================================================
// ======================== PERSISTENCE SERVICE =========================
// =======================================================================

class PersistenceService {
    ILabRepository* labRepo;
    ITimeSheetRepository* tsRepo;
public:
    PersistenceService(ILabRepository* l, ITimeSheetRepository* t) : labRepo(l), tsRepo(t) {}

    void save() {
        ofstream lOut("labs_v2.dat", ios::binary);
        vector<Lab>& labs = labRepo->getAllLabs();
        int lSize = labs.size();
        lOut.write((char*)&lSize, sizeof(int));
        for (auto &l : labs) lOut.write((char*)&l, sizeof(Lab));
        lOut.close();

        ofstream tOut("ts_v2.dat", ios::binary);
        vector<TimeSheetEntry>& ts = tsRepo->getAllEntries();
        int tSize = ts.size();
        tOut.write((char*)&tSize, sizeof(int));
        for (auto &t : ts) tOut.write((char*)&t, sizeof(TimeSheetEntry));
        tOut.close();

        cout << "[System] Data saved.\n";
    }

    void load() {
        ifstream lIn("labs_v2.dat", ios::binary);
        if (lIn) {
            int lSize = 0;
            lIn.read((char*)&lSize, sizeof(int));
            for (int i = 0; i < lSize; i++) {
                Lab l;
                lIn.read((char*)&l, sizeof(Lab));
                labRepo->addLab(l);
            }
            lIn.close();
        }

        ifstream tIn("ts_v2.dat", ios::binary);
        if (tIn) {
            int tSize = 0;
            tIn.read((char*)&tSize, sizeof(int));
            for (int i = 0; i < tSize; i++) {
                TimeSheetEntry t;
                tIn.read((char*)&t, sizeof(TimeSheetEntry));
                tsRepo->addEntry(t);
            }
            tIn.close();
        }
    }
};
// =======================================================================
// ====================== ROLE-SPECIFIC MENUS ============================
// =======================================================================

void hodMenu(HOD& hod, ILabRepository* lRepo, ITimeSheetRepository* tRepo, IVenueRepository* vRepo) {
    while (true) {
        cout << "\n--- HOD DASHBOARD ---\n";
        cout << "1. View Weekly Schedule (All Sections)\n";
        cout << "2. Report: All Filled Time Sheets\n";
        cout << "3. Report: Lab Semester Summary\n";
        cout << "4. Logout\nSelect: ";
        int choice; 
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        switch (choice) {
            case 1: hod.generateScheduleReport(lRepo, vRepo); break;
            case 2: hod.generateWeeklyTimeSheetReport(tRepo); break;
            case 3: {
                int id; 
                cout << "Enter Lab ID: "; 
                if (!(cin >> id)) { cin.clear(); cin.ignore(1000, '\n'); break; }
                hod.generateLabSemesterReport(tRepo, id); 
                break;
            }
            case 4: return;
            default: cout << "Invalid choice.\n";
        }
    }
}

// ---------------- Academic Officer Menu ----------------
void officerMenu(AcademicOfficer& ao, ILabRepository* lRepo) {
    while (true) {
        cout << "\n--- ACADEMIC OFFICER DASHBOARD ---\n";
        cout << "1. Schedule New Section for Lab\n";
        cout << "2. Logout\nSelect: ";
        int choice; 
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (choice == 1) {
            int id, instId, bId, rId, taCount;
            char code[20], secName[10], instName[50], day[15], s[10], e[10];
            vector<pair<int, string>> tas;

            cout << "Enter Lab ID: "; cin >> id;
            cout << "Enter Course Code (e.g., CS101): "; cin >> code;
            cout << "Enter Section Name (e.g., A): "; cin >> secName;

            cout << "Enter Instructor ID: "; cin >> instId;
            cout << "Enter Instructor Name: "; cin >> instName;

            cout << "Enter Building ID (1=CS, 2=EE): "; cin >> bId;
            cout << "Enter Room ID (e.g., 101): "; cin >> rId;

            cout << "How many TAs? "; cin >> taCount;
            for (int i = 0; i < taCount; i++) {
                int tId; char tName[50];
                cout << "  TA " << i+1 << " ID: "; cin >> tId;
                cout << "  TA " << i+1 << " Name: "; cin >> tName;
                tas.push_back({tId, string(tName)});
            }

            cout << "Enter Day (e.g., Monday): "; cin >> day;
            cout << "Enter Start Time (HH:MM): "; cin >> s;
            cout << "Enter End Time (HH:MM): "; cin >> e;

            ao.scheduleSection(lRepo, id, code, secName, instId, instName, bId, rId, day, s, e, tas);
        } 
        else if (choice == 2) return;
        else cout << "Invalid choice.\n";
    }
}

// ---------------- Instructor Menu ----------------
void instructorMenu(Instructor& inst, ILabRepository* lRepo) {
    while (true) {
        cout << "\n--- INSTRUCTOR DASHBOARD ---\n";
        cout << "1. Request Makeup Class\n";
        cout << "2. Logout\nSelect: ";
        int choice; 
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (choice == 1) {
            int id; char sec[10], s[10], e[10];
            cout << "Enter Lab ID: "; cin >> id;
            cout << "Enter Section Name: "; cin >> sec;
            cout << "Enter New Start Time: "; cin >> s;
            cout << "Enter New End Time: "; cin >> e;
            inst.requestMakeupLab(lRepo, id, sec, s, e);
        } 
        else if (choice == 2) return;
        else cout << "Invalid choice.\n";
    }
}

// ---------------- Attendant Menu ----------------
void attendantMenu(Attendant& att, ITimeSheetRepository* tRepo) {
    while (true) {
        cout << "\n--- ATTENDANT DASHBOARD ---\n";
        cout << "1. Fill Time Sheet\n";
        cout << "2. Logout\nSelect: ";
        int choice; 
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (choice == 1) {
            int id, isLeave; char sec[10], s[10], e[10], d[20];
            cout << "Enter Lab ID: "; cin >> id;
            cout << "Enter Section Name: "; cin >> sec;
            cout << "Enter Date (e.g., 12-Nov): "; cin >> d;
            cout << "Is Instructor on Leave? (1=Yes, 0=No): "; cin >> isLeave;

            if (isLeave) {
                att.fillTimeSheet(tRepo, id, sec, d, "00:00", "00:00", true);
            } else {
                cout << "Enter Actual Start (HH:MM): "; cin >> s;
                cout << "Enter Actual End (HH:MM): "; cin >> e;
                att.fillTimeSheet(tRepo, id, sec, d, s, e, false);
            }
        } 
        else if (choice == 2) return;
        else cout << "Invalid choice.\n";
    }
}
// =======================================================================
// ============================ MAIN FUNCTION ============================
// =======================================================================

int main() {
    // --- Repositories ---
    LabRepository labRepo;
    TimeSheetRepository tsRepo;
    VenueRepository venueRepo; 
    venueRepo.seedData();      

    // --- Persistence Service ---
    PersistenceService persistence(&labRepo, &tsRepo);
    persistence.load();  // Load previously saved data if any

    // --- Role Instances ---
    HOD hodUser;
    AcademicOfficer aoUser;
    Instructor instUser;
    Attendant attUser;

    while (true) {
        cout << "\n=========================================\n";
        cout << "   UNIVERSITY LAB MANAGEMENT SYSTEM      \n";
        cout << "=========================================\n";
        cout << "Who are you?\n";
        cout << "1. HOD\n";
        cout << "2. Academic Officer\n";
        cout << "3. Instructor\n";
        cout << "4. Lab Attendant\n";
        cout << "5. Save & Exit System\n";
        cout << "Select your Role: ";

        int role;
        if (!(cin >> role)) { 
            cin.clear(); 
            cin.ignore(1000, '\n'); 
            continue; 
        }

        switch (role) {
            case 1: hodMenu(hodUser, &labRepo, &tsRepo, &venueRepo); break;
            case 2: officerMenu(aoUser, &labRepo); break;
            case 3: instructorMenu(instUser, &labRepo); break;
            case 4: attendantMenu(attUser, &tsRepo); break;
            case 5:
                persistence.save();
                cout << "Exiting system. Goodbye!\n";
                return 0;
            default: cout << "Invalid role selected.\n";
        }
    }

    return 0;
}
