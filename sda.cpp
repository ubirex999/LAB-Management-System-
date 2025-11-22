#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <cstdio>
#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>

using namespace std;

// System Configuration
class AppConfiguration
{
public:
    static const int STR_SIZE = 50;
};

// Domain Classes for Constants

class AssistantRules
{
public:
    static const int MAX_TAS = 5;
};

// UTILITY CLASSES (Static Helpers)

class InputOutput
{
public:
    static void SafeReadInt(int &val)
    {
        if (!(cin >> val))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            val = -1;
        }
    }

    static void SafeReadString(string &val)
    {
        cin >> val;
    }
};

class DataValidator
{
public:
    static bool IsValidTime(const string &t)
    {
        if (t.length() != 5 || t[2] != ':')
            return false;
        if (!isdigit(t[0]) || !isdigit(t[1]) || !isdigit(t[3]) || !isdigit(t[4]))
            return false;

        try
        {
            int h = stoi(t.substr(0, 2));
            int m = stoi(t.substr(3, 2));
            return (h >= 0 && h < 24 && m >= 0 && m < 60);
        }
        catch (...)
        {
            return false;
        }
    }

    static bool IsStartBeforeEnd(const string &start, const string &end)
    {
        int h1, m1, h2, m2;
        sscanf(start.c_str(), "%d:%d", &h1, &m1);
        sscanf(end.c_str(), "%d:%d", &h2, &m2);
        return (h1 * 60 + m1) < (h2 * 60 + m2);
    }

    static bool IsValidName(const string &n)
    {
        if (n.empty())
            return false;
        for (char c : n)
        {
            if (!isalpha(c) && c != '-' && c != '_' && c != ' ')
                return false;
        }
        return true;
    }

    static bool IsValidCourseCode(const string &c)
    {
        int n = c.length();
        if (n < 4 || n > 10)
            return false;

        int i = 0;
        while (i < n && isalpha(c[i]))
            i++;
        if (i < 2)
            return false;

        while (i < n && isdigit(c[i]))
            i++;
        return i == n;
    }

    static bool IsValidDate(const string &d)
    {
        int len = d.length();
        if (len < 5 || len > 7)
            return false;

        if (!isdigit(d[0]) || !isdigit(d[1]) || d[2] != '-')
            return false;

        for (int i = 3; i < len; i++)
        {
            if (!isalnum(d[i]))
                return false;
        }
        return true;
    }
};

// CORE DATA STRUCTURES

class DateAndTime
{
public:
    char date[20];
    char startTime[10];
    char endTime[10];

    DateAndTime()
    {
        strcpy(date, "");
        strcpy(startTime, "");
        strcpy(endTime, "");
    }

    void Set(const string &d, const string &s, const string &e)
    {
        strncpy(date, d.c_str(), sizeof(date) - 1);
        date[sizeof(date) - 1] = '\0';
        strncpy(startTime, s.c_str(), sizeof(startTime) - 1);
        startTime[sizeof(startTime) - 1] = '\0';
        strncpy(endTime, e.c_str(), sizeof(endTime) - 1);
        endTime[sizeof(endTime) - 1] = '\0';
    }
};

class CampusBlock
{
public:
    int buildingId;
    char name[AppConfiguration::STR_SIZE];

    CampusBlock() : buildingId(0) { strcpy(name, ""); }
    CampusBlock(int id, const string &n) : buildingId(id)
    {
        strncpy(name, n.c_str(), sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }
};

class LectureHall
{
public:
    int roomId;
    int roomNumber;

    LectureHall() : roomId(0), roomNumber(0) {}
    LectureHall(int id, int num) : roomId(id), roomNumber(num) {}
};

class ClassSection
{
public:
    static const int MAX_SECTIONS = 5;

    char sectionName[10];
    int instructorId;
    char instructorName[AppConfiguration::STR_SIZE];

    int taCount;
    int taIds[AssistantRules::MAX_TAS];
    char taNames[AssistantRules::MAX_TAS][AppConfiguration::STR_SIZE];

    int buildingId;
    int roomId;

    DateAndTime scheduleTime;

    ClassSection() : instructorId(0), taCount(0), buildingId(0), roomId(0)
    {
        strcpy(sectionName, "");
        strcpy(instructorName, "");
    }

    void SetDetails(const string &name, int instId, const string &instName, int bId, int rId)
    {
        strncpy(sectionName, name.c_str(), sizeof(sectionName) - 1);
        sectionName[sizeof(sectionName) - 1] = '\0';
        instructorId = instId;
        strncpy(instructorName, instName.c_str(), sizeof(instructorName) - 1);
        instructorName[sizeof(instructorName) - 1] = '\0';
        buildingId = bId;
        roomId = rId;
    }

    void AddTA(int id, const string &name)
    {
        if (taCount < AssistantRules::MAX_TAS)
        {
            for (int i = 0; i < taCount; i++)
            {
                if (taIds[i] == id)
                    return;
            }
            taIds[taCount] = id;
            strncpy(taNames[taCount], name.c_str(), sizeof(taNames[taCount]) - 1);
            taNames[taCount][sizeof(taNames[taCount]) - 1] = '\0';
            taCount++;
        }
    }
};

class CourseLaboratory
{
public:
    int labId;
    char courseCode[AppConfiguration::STR_SIZE];

    int sectionCount;
    ClassSection sections[ClassSection::MAX_SECTIONS];

    CourseLaboratory() : labId(0), sectionCount(0) { strcpy(courseCode, ""); }

    void AddSection(const ClassSection &s)
    {
        if (sectionCount < ClassSection::MAX_SECTIONS)
        {
            sections[sectionCount] = s;
            sectionCount++;
        }
        else
        {
            cout << "Error: Max sections reached for this lab.\n";
        }
    }

    ClassSection *FindSection(const string &secName)
    {
        for (int i = 0; i < sectionCount; i++)
        {
            if (strcmp(sections[i].sectionName, secName.c_str()) == 0)
            {
                return &sections[i];
            }
        }
        return nullptr;
    }
};

class WorkLog
{
public:
    int labId;
    char sectionName[10];
    DateAndTime actualTiming;
    bool isLeave;

    WorkLog() : labId(0), isLeave(false) { strcpy(sectionName, ""); }
};

// DETAILS INTERFACES (Formerly Repositories)

class ILabDetails
{
public:
    virtual void AddLab(const CourseLaboratory &l) = 0;
    virtual void UpdateLab(const CourseLaboratory &l) = 0;
    virtual CourseLaboratory *FindLab(int id) = 0;
    virtual vector<CourseLaboratory> &GetAllLabs() = 0;
    virtual ~ILabDetails() {}
};

class IWorkLogDetails
{
public:
    virtual void AddEntry(const WorkLog &entry) = 0;
    virtual vector<WorkLog> &GetAllEntries() = 0;
    virtual ~IWorkLogDetails() {}
};

class IVenueDetails
{
public:
    virtual void AddBuilding(const CampusBlock &b) = 0;
    virtual void AddRoom(const LectureHall &r) = 0;
    virtual CampusBlock *FindBuilding(int id) = 0;
    virtual LectureHall *FindRoom(int id) = 0;
    virtual ~IVenueDetails() {}
};

// CONCRETE DETAILS CLASSES

class LabDetails : public ILabDetails
{
    vector<CourseLaboratory> labs;

public:
    void AddLab(const CourseLaboratory &l) override { labs.push_back(l); }

    void UpdateLab(const CourseLaboratory &l) override
    {
        for (auto &existing : labs)
        {
            if (existing.labId == l.labId)
            {
                existing = l;
                return;
            }
        }
    }

    CourseLaboratory *FindLab(int id) override
    {
        for (auto &l : labs)
        {
            if (l.labId == id)
                return &l;
        }
        return nullptr;
    }

    vector<CourseLaboratory> &GetAllLabs() override { return labs; }
};

class WorkLogDetails : public IWorkLogDetails
{
    vector<WorkLog> logs;

public:
    void AddEntry(const WorkLog &entry) override { logs.push_back(entry); }
    vector<WorkLog> &GetAllEntries() override { return logs; }
};

class VenueDetails : public IVenueDetails
{
    vector<CampusBlock> buildings;
    vector<LectureHall> rooms;

public:
    void AddBuilding(const CampusBlock &b) override { buildings.push_back(b); }
    void AddRoom(const LectureHall &r) override { rooms.push_back(r); }

    CampusBlock *FindBuilding(int id) override
    {
        for (auto &b : buildings)
            if (b.buildingId == id)
                return &b;
        return nullptr;
    }

    LectureHall *FindRoom(int id) override
    {
        for (auto &r : rooms)
            if (r.roomId == id)
                return &r;
        return nullptr;
    }

    void SeedData()
    {
        if (buildings.empty())
        {
            buildings.push_back(CampusBlock(1, "Computer Science Block"));
            buildings.push_back(CampusBlock(2, "Electrical Eng Block"));
        }
        if (rooms.empty())
        {
            rooms.push_back(LectureHall(101, 101));
            rooms.push_back(LectureHall(102, 102));
            rooms.push_back(LectureHall(201, 201));
        }
    }
};

// =========================================================
// ACTOR ROLES
// =========================================================

class UserRole
{
protected:
    string roleName;

public:
    UserRole(const string &n) : roleName(n) {}
    virtual ~UserRole() {}
};

class Director : public UserRole
{
private:
    float CalculateHours(const string &start, const string &end)
    {
        int h1, m1, h2, m2;
        sscanf(start.c_str(), "%d:%d", &h1, &m1);
        sscanf(end.c_str(), "%d:%d", &h2, &m2);
        int startMins = h1 * 60 + m1;
        int endMins = h2 * 60 + m2;
        return (float)(endMins - startMins) / 60.0f;
    }

    void GenerateScheduleReport(ILabDetails *lDetails, IVenueDetails *vDetails)
    {
        cout << "\n[DIRECTOR ACTION] Generating Weekly Schedule Report...\n";
        cout << "--------------------------------------------------------------------------------\n";
        vector<CourseLaboratory> &labs = lDetails->GetAllLabs();
        if (labs.empty())
        {
            cout << "No labs scheduled.\n";
            return;
        }

        for (auto &l : labs)
        {
            cout << "COURSE: " << l.courseCode << " (ID: " << l.labId << ")\n";

            for (int i = 0; i < l.sectionCount; i++)
            {
                ClassSection &sec = l.sections[i];
                CampusBlock *b = vDetails->FindBuilding(sec.buildingId);
                LectureHall *r = vDetails->FindRoom(sec.roomId);

                cout << "  >> SECTION " << sec.sectionName << " | "
                     << sec.scheduleTime.date << " "
                     << sec.scheduleTime.startTime << "-" << sec.scheduleTime.endTime << "\n";

                cout << "     Venue: " << (b ? b->name : "Unknown") << " - Room "
                     << (r ? to_string(r->roomNumber) : "N/A") << "\n";
                cout << "     Instructor: " << sec.instructorName << " (ID: " << sec.instructorId << ")\n";
                cout << "     TAs: ";
                if (sec.taCount == 0)
                    cout << "None";
                for (int k = 0; k < sec.taCount; k++)
                {
                    cout << sec.taNames[k] << " (" << sec.taIds[k] << ")"
                         << (k < sec.taCount - 1 ? ", " : "");
                }
                cout << "\n\n";
            }
        }
        cout << "--------------------------------------------------------------------------------\n";
    }

    void GenerateWeeklyWorkLogReport(IWorkLogDetails *logDetails)
    {
        cout << "\n[DIRECTOR REPORT] Weekly Filled Time Sheets\n";
        cout << "----------------------------------------------------------------------\n";
        vector<WorkLog> &logs = logDetails->GetAllEntries();
        if (logs.empty())
        {
            cout << "No work logs filled.\n";
            return;
        }

        cout << left << setw(8) << "Lab ID" << setw(10) << "Section"
             << setw(15) << "Date" << setw(15) << "Time" << setw(10) << "Status" << endl;
        for (auto &t : logs)
        {
            cout << left << setw(8) << t.labId
                 << setw(10) << t.sectionName
                 << setw(15) << t.actualTiming.date
                 << setw(6) << t.actualTiming.startTime << "-" << t.actualTiming.endTime << "   "
                 << setw(10) << (t.isLeave ? "LEAVE" : "PRESENT") << endl;
        }
        cout << "----------------------------------------------------------------------\n";
    }

    void GenerateLabSemesterReport(IWorkLogDetails *logDetails, int targetLabId)
    {
        cout << "\n[DIRECTOR REPORT] Semester Summary for Lab ID: " << targetLabId << "\n";
        cout << "--------------------------------------------------\n";
        vector<WorkLog> &logs = logDetails->GetAllEntries();

        float totalContactHours = 0;
        int totalLeaves = 0;
        int sessionsCount = 0;
        bool found = false;

        for (auto &t : logs)
        {
            if (t.labId == targetLabId)
            {
                found = true;
                sessionsCount++;
                if (t.isLeave)
                    totalLeaves++;
                else
                    totalContactHours += CalculateHours(t.actualTiming.startTime, t.actualTiming.endTime);
            }
        }

        if (!found)
        {
            cout << "No records found for Lab ID " << targetLabId << ".\n";
        }
        else
        {
            cout << "Total Sessions Recorded: " << sessionsCount << endl;
            cout << "Total Contact Hours:     " << fixed << setprecision(2)
                 << totalContactHours << " hours" << endl;
            cout << "Total Leaves:            " << totalLeaves << endl;
        }
        cout << "--------------------------------------------------\n";
    }

public:
    Director() : UserRole("Department Head") {}

    void ShowMenu(ILabDetails *lDetails, IWorkLogDetails *logDetails, IVenueDetails *vDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- DIRECTOR DASHBOARD ---\n";
            cout << "1. View Weekly Schedule (All Sections)\n";
            cout << "2. Report: All Filled Time Sheets\n";
            cout << "3. Report: Lab Semester Summary\n";
            cout << "4. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == -1)
                continue;

            switch (choice)
            {
            case 1:
                GenerateScheduleReport(lDetails, vDetails);
                break;
            case 2:
                GenerateWeeklyWorkLogReport(logDetails);
                break;
            case 3:
            {
                int id;
                cout << "Enter Lab ID: ";
                InputOutput::SafeReadInt(id);
                if (id != -1)
                    GenerateLabSemesterReport(logDetails, id);
                break;
            }
            case 4:
                return;
            default:
                cout << "Invalid choice.\n";
            }
        }
    }
};

class Coordinator : public UserRole
{
private:
    void ScheduleSection(ILabDetails *lDetails, int id, const string &code,
                         const string &secName,
                         int instId, const string &instName,
                         int bId, int rId,
                         const string &day, const string &start, const string &end,
                         const vector<pair<int, string>> &tas)
    {

        if (!DataValidator::IsValidCourseCode(code))
        {
            cout << "Invalid Course Code. Must be letters+digits (e.g., CS101).\n";
            return;
        }
        if (!DataValidator::IsValidName(secName))
        {
            cout << "Invalid Section Name. Only letters allowed.\n";
            return;
        }
        if (!DataValidator::IsValidName(instName))
        {
            cout << "Invalid Instructor Name.\n";
            return;
        }
        if (!DataValidator::IsValidTime(start) || !DataValidator::IsValidTime(end) || !DataValidator::IsStartBeforeEnd(start, end))
        {
            cout << "Invalid timing. Ensure HH:MM format and Start < End.\n";
            return;
        }

        CourseLaboratory *existingLab = lDetails->FindLab(id);
        CourseLaboratory workLab;

        if (existingLab != nullptr)
            workLab = *existingLab;
        else
        {
            workLab.labId = id;
            strncpy(workLab.courseCode, code.c_str(), sizeof(workLab.courseCode) - 1);
            workLab.courseCode[sizeof(workLab.courseCode) - 1] = '\0';
        }

        if (workLab.FindSection(secName) != nullptr)
        {
            cout << ">> Error: Section " << secName << " already exists for Lab " << id << ".\n";
            return;
        }

        ClassSection newSec;
        newSec.SetDetails(secName, instId, instName, bId, rId);
        newSec.scheduleTime.Set(day, start, end);

        for (const auto &ta : tas)
        {
            if (!DataValidator::IsValidName(ta.second))
            {
                cout << "Invalid TA Name: " << ta.second << ". Skipping.\n";
                continue;
            }
            newSec.AddTA(ta.first, ta.second);
        }

        workLab.AddSection(newSec);

        if (existingLab != nullptr)
            lDetails->UpdateLab(workLab);
        else
            lDetails->AddLab(workLab);

        cout << ">> Success: Section " << secName << " added to Lab " << id << " (" << code << ").\n";
    }

public:
    Coordinator() : UserRole("Coordinator") {}

    void ShowMenu(ILabDetails *lDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- COORDINATOR DASHBOARD ---\n";
            cout << "1. Schedule New Section for Lab\n";
            cout << "2. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == -1)
                continue;

            if (choice == 1)
            {
                int id, instId, bId, rId, taCount;
                string code, secName, instName, day, s, e;
                vector<pair<int, string>> tas;

                cout << "Enter Lab ID: ";
                InputOutput::SafeReadInt(id);
                cout << "Enter Course Code (e.g., CS101): ";
                InputOutput::SafeReadString(code);
                cout << "Enter Section Name (e.g., A): ";
                InputOutput::SafeReadString(secName);

                cout << "Enter Instructor ID: ";
                InputOutput::SafeReadInt(instId);
                cout << "Enter Instructor Name: ";
                InputOutput::SafeReadString(instName);

                cout << "Enter Building ID (1=CS, 2=EE): ";
                InputOutput::SafeReadInt(bId);
                cout << "Enter Room ID (e.g., 101): ";
                InputOutput::SafeReadInt(rId);

                cout << "How many TAs? ";
                InputOutput::SafeReadInt(taCount);
                if (taCount > AssistantRules::MAX_TAS)
                    taCount = AssistantRules::MAX_TAS;
                for (int i = 0; i < taCount; i++)
                {
                    int tId;
                    string tName;
                    cout << "  TA " << i + 1 << " ID: ";
                    InputOutput::SafeReadInt(tId);
                    cout << "  TA " << i + 1 << " Name: ";
                    InputOutput::SafeReadString(tName);
                    tas.push_back({tId, tName});
                }

                cout << "Enter Day (e.g., Monday): ";
                InputOutput::SafeReadString(day);
                cout << "Enter Start Time (HH:MM): ";
                InputOutput::SafeReadString(s);
                cout << "Enter End Time (HH:MM): ";
                InputOutput::SafeReadString(e);

                ScheduleSection(lDetails, id, code, secName, instId, instName, bId, rId, day, s, e, tas);
            }
            else if (choice == 2)
                return;
            else
                cout << "Invalid choice.\n";
        }
    }
};

class Lecturer : public UserRole
{
private:
    void RequestMakeupLab(ILabDetails *lDetails, int labId, const string &secName, const string &newStart, const string &newEnd)
    {
        if (!DataValidator::IsValidTime(newStart) || !DataValidator::IsValidTime(newEnd) || !DataValidator::IsStartBeforeEnd(newStart, newEnd))
        {
            cout << "Invalid new timings. Ensure HH:MM format and Start < End.\n";
            return;
        }

        CourseLaboratory *l = lDetails->FindLab(labId);
        if (!l)
        {
            cout << ">> Error: Lab ID " << labId << " not found.\n";
            return;
        }

        ClassSection *s = l->FindSection(secName);
        if (!s)
        {
            cout << ">> Error: Section " << secName << " not found.\n";
            return;
        }

        s->scheduleTime.Set("Makeup", newStart, newEnd);
        cout << ">> Success: Makeup requested/approved for Lab " << labId << " Section " << secName << ".\n";
    }

public:
    Lecturer() : UserRole("Lecturer") {}

    void ShowMenu(ILabDetails *lDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- LECTURER DASHBOARD ---\n";
            cout << "1. Request Makeup Class\n";
            cout << "2. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == -1)
                continue;

            if (choice == 1)
            {
                int id;
                string sec, s, e;
                cout << "Enter Lab ID: ";
                InputOutput::SafeReadInt(id);
                cout << "Enter Section Name: ";
                InputOutput::SafeReadString(sec);
                cout << "Enter New Start Time (HH:MM): ";
                InputOutput::SafeReadString(s);
                cout << "Enter New End Time (HH:MM): ";
                InputOutput::SafeReadString(e);
                RequestMakeupLab(lDetails, id, sec, s, e);
            }
            else if (choice == 2)
                return;
            else
                cout << "Invalid choice.\n";
        }
    }
};

class LabStaff : public UserRole
{
private:
    void FillTimeSheet(IWorkLogDetails *logDetails, int labId, const string &secName,
                       const string &date, const string &start, const string &end, bool isLeave)
    {
        if (!isLeave)
        {
            if (!DataValidator::IsValidTime(start) || !DataValidator::IsValidTime(end) || !DataValidator::IsStartBeforeEnd(start, end))
            {
                cout << "Invalid timing. Cannot log time sheet entry.\n";
                return;
            }
        }
        if (!DataValidator::IsValidDate(date))
        {
            cout << "Invalid date format (e.g., 12-Nov).\n";
            return;
        }

        WorkLog entry;
        entry.labId = labId;
        strncpy(entry.sectionName, secName.c_str(), sizeof(entry.sectionName) - 1);
        entry.sectionName[sizeof(entry.sectionName) - 1] = '\0';
        entry.actualTiming.Set(date, start, end);
        entry.isLeave = isLeave;
        logDetails->AddEntry(entry);

        cout << ">> Success: Work log entry logged for Section " << secName << ".\n";
    }

public:
    LabStaff() : UserRole("Lab Staff") {}

    void ShowMenu(IWorkLogDetails *logDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- STAFF DASHBOARD ---\n";
            cout << "1. Fill Work Log\n";
            cout << "2. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == -1)
                continue;

            if (choice == 1)
            {
                int id, isLeave;
                string sec, s, e, d;
                cout << "Enter Lab ID: ";
                InputOutput::SafeReadInt(id);
                cout << "Enter Section Name: ";
                InputOutput::SafeReadString(sec);
                cout << "Enter Date (DD-MMM or DD-MM): ";
                InputOutput::SafeReadString(d);
                cout << "Is Instructor on Leave? (1=Yes, 0=No): ";
                InputOutput::SafeReadInt(isLeave);

                if (isLeave == 1)
                {
                    FillTimeSheet(logDetails, id, sec, d, "00:00", "00:00", true);
                }
                else
                {
                    cout << "Enter Actual Start (HH:MM): ";
                    InputOutput::SafeReadString(s);
                    cout << "Enter Actual End (HH:MM): ";
                    InputOutput::SafeReadString(e);
                    FillTimeSheet(logDetails, id, sec, d, s, e, false);
                }
            }
            else if (choice == 2)
                return;
            else
                cout << "Invalid choice.\n";
        }
    }
};

class StorageManager
{
    ILabDetails *labDetails;
    IWorkLogDetails *logDetails;

public:
    StorageManager(ILabDetails *l, IWorkLogDetails *t) : labDetails(l), logDetails(t) {}

    void Save()
    {
        ofstream lOut("labs_v2.dat", ios::binary);
        vector<CourseLaboratory> &labs = labDetails->GetAllLabs();
        int lSize = labs.size();
        lOut.write(reinterpret_cast<char *>(&lSize), sizeof(int));
        for (const auto &l : labs)
            lOut.write(reinterpret_cast<const char *>(&l), sizeof(CourseLaboratory));
        lOut.close();

        ofstream tOut("ts_v2.dat", ios::binary);
        vector<WorkLog> &logs = logDetails->GetAllEntries();
        int tSize = logs.size();
        tOut.write(reinterpret_cast<char *>(&tSize), sizeof(int));
        for (const auto &t : logs)
            tOut.write(reinterpret_cast<const char *>(&t), sizeof(WorkLog));
        tOut.close();

        cout << "[System] Data saved.\n";
    }

    void Load()
    {
        ifstream lIn("labs_v2.dat", ios::binary);
        if (lIn)
        {
            int lSize = 0;
            lIn.read(reinterpret_cast<char *>(&lSize), sizeof(int));
            for (int i = 0; i < lSize; i++)
            {
                CourseLaboratory l;
                lIn.read(reinterpret_cast<char *>(&l), sizeof(CourseLaboratory));
                labDetails->AddLab(l);
            }
            lIn.close();
        }

        ifstream tIn("ts_v2.dat", ios::binary);
        if (tIn)
        {
            int tSize = 0;
            tIn.read(reinterpret_cast<char *>(&tSize), sizeof(int));
            for (int i = 0; i < tSize; i++)
            {
                WorkLog t;
                tIn.read(reinterpret_cast<char *>(&t), sizeof(WorkLog));
                logDetails->AddEntry(t);
            }
            tIn.close();
        }
    }
};
int main()
{
    // Details (formerly Repositories)
    LabDetails labDetails;
    WorkLogDetails logDetails;
    VenueDetails venueDetails;
    venueDetails.SeedData();

    // Storage Manager
    StorageManager storage(&labDetails, &logDetails);
    storage.Load();

    // Role Instances
    Director director;
    Coordinator coordinator;
    Lecturer lecturer;
    LabStaff staff;

    while (true)
    {
        cout << "\n=========================================\n";
        cout << "   UNIVERSITY LAB MANAGEMENT SYSTEM      \n";
        cout << "=========================================\n";
        cout << "Who are you?\n";
        cout << "1. Director (HOD)\n";
        cout << "2. Coordinator (Academic Officer)\n";
        cout << "3. Lecturer (Instructor)\n";
        cout << "4. Lab Staff (Attendant)\n";
        cout << "5. Save & Exit System\n";
        cout << "Select your Role: ";

        int role;
        if (!(cin >> role))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        switch (role)
        {
        case 1:
            director.ShowMenu(&labDetails, &logDetails, &venueDetails);
            break;
        case 2:
            coordinator.ShowMenu(&labDetails);
            break;
        case 3:
            lecturer.ShowMenu(&labDetails);
            break;
        case 4:
            staff.ShowMenu(&logDetails);
            break;
        case 5:
            storage.Save();
            cout << "Exiting system. Goodbye!\n";
            return 0;
        default:
            cout << "Invalid role selected.\n";
        }
    }
    return 0;
}