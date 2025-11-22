#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace std;

// UTILITY CLASSES

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

    static void SafeReadLine(string &val)
    {
        cin.ignore();
        getline(cin, val);
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
        int h1 = stoi(start.substr(0, 2));
        int m1 = stoi(start.substr(3, 2));
        int h2 = stoi(end.substr(0, 2));
        int m2 = stoi(end.substr(3, 2));
        return (h1 * 60 + m1) < (h2 * 60 + m2);
    }

    static bool IsValidCourseCode(const string &c)
    {
        return c.length() >= 4;
    }

    static bool IsValidDate(const string &d)
    {
        return d.length() >= 5;
    }
};

// CORE DOMAIN ENTITIES

class DateAndTime
{
public:
    string date;
    string startTime;
    string endTime;

    void Set(const string &d, const string &s, const string &e)
    {
        date = d;
        startTime = s;
        endTime = e;
    }
};

class CampusBlock
{
public:
    int buildingId;
    string name;

    CampusBlock() : buildingId(0) {}
    CampusBlock(int id, string n) : buildingId(id), name(n) {}
};

class LectureHall
{
public:
    int roomId;
    int roomNumber;

    LectureHall() : roomId(0), roomNumber(0) {}
    LectureHall(int id, int num) : roomId(id), roomNumber(num) {}
};

// Renamed to UniversityTeacher to avoid collision with the 'Instructor' Person class
class UniversityTeacher
{
public:
    int teacherId;
    string name;

    UniversityTeacher() : teacherId(0) {}
    UniversityTeacher(int id, string n) : teacherId(id), name(n) {}
};

class TeachingAssistant
{
public:
    int taId;
    string name;

    TeachingAssistant() : taId(0) {}
    TeachingAssistant(int id, string n) : taId(id), name(n) {}
};

class ClassSection
{
public:
    string sectionName;

    // Using Pointers to domain objects
    UniversityTeacher *teacher;
    vector<TeachingAssistant *> assistants;

    CampusBlock *building;
    LectureHall *room;
    DateAndTime scheduleTime;

    ClassSection() : teacher(nullptr), building(nullptr), room(nullptr) {}

    void SetDetails(string name, UniversityTeacher *t, CampusBlock *b, LectureHall *r)
    {
        sectionName = name;
        teacher = t;
        building = b;
        room = r;
    }

    void AddTA(TeachingAssistant *ta)
    {
        assistants.push_back(ta);
    }
};

class CourseLaboratory
{
public:
    int labId;
    string courseCode;
    vector<ClassSection> sections;

    CourseLaboratory() : labId(0) {}

    void AddSection(const ClassSection &s)
    {
        sections.push_back(s);
    }

    ClassSection *FindSection(const string &secName)
    {
        for (auto &s : sections)
        {
            if (s.sectionName == secName)
                return &s;
        }
        return nullptr;
    }
};

class WorkLog
{
public:
    int labId;
    string sectionName;
    DateAndTime actualTiming;
    bool isLeave;

    WorkLog() : labId(0), isLeave(false) {}
};

// DETAILS INTERFACES

class LabDetails
{
public:
    virtual void AddLab(const CourseLaboratory &l) = 0;
    virtual void UpdateLab(const CourseLaboratory &l) = 0;
    virtual CourseLaboratory *FindLab(int id) = 0;
    virtual vector<CourseLaboratory> &GetAllLabs() = 0;
};

class WorkLogDetails
{
public:
    virtual void AddEntry(const WorkLog &entry) = 0;
    virtual vector<WorkLog> &GetAllEntries() = 0;
};

class VenueDetails
{
public:
    virtual CampusBlock *FindBuilding(int id) = 0;
    virtual LectureHall *FindRoom(int id) = 0;
    virtual void SeedData() = 0;
};

class FacultyDetails
{
public:
    virtual void AddTeacher(const UniversityTeacher &t) = 0;
    virtual void AddTA(const TeachingAssistant &t) = 0;
    virtual UniversityTeacher *FindTeacher(int id) = 0;
    virtual TeachingAssistant *FindTA(int id) = 0;
    virtual void SeedData() = 0;
};

// CONCRETE IMPLEMENTATIONS

class InMemoryLabDetails : public LabDetails
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
            if (l.labId == id)
                return &l;
        return nullptr;
    }
    vector<CourseLaboratory> &GetAllLabs() override { return labs; }
};

class InMemoryWorkLogDetails : public WorkLogDetails
{
    vector<WorkLog> logs;

public:
    void AddEntry(const WorkLog &entry) override { logs.push_back(entry); }
    vector<WorkLog> &GetAllEntries() override { return logs; }
};

class InMemoryVenueDetails : public VenueDetails
{
    vector<CampusBlock> buildings;
    vector<LectureHall> rooms;

public:
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
    void SeedData() override
    {
        if (buildings.empty())
        {
            buildings.push_back(CampusBlock(1, "CS Block"));
            buildings.push_back(CampusBlock(2, "EE Block"));
        }
        if (rooms.empty())
        {
            rooms.push_back(LectureHall(101, 101));
            rooms.push_back(LectureHall(102, 102));
        }
    }
};

class InMemoryFacultyDetails : public FacultyDetails
{
    vector<UniversityTeacher> teachers;
    vector<TeachingAssistant> tas;

public:
    void AddTeacher(const UniversityTeacher &t) override { teachers.push_back(t); }
    void AddTA(const TeachingAssistant &t) override { tas.push_back(t); }

    UniversityTeacher *FindTeacher(int id) override
    {
        for (auto &t : teachers)
            if (t.teacherId == id)
                return &t;
        return nullptr;
    }
    TeachingAssistant *FindTA(int id) override
    {
        for (auto &t : tas)
            if (t.taId == id)
                return &t;
        return nullptr;
    }
    void SeedData() override
    {
        if (teachers.empty())
        {
            teachers.push_back(UniversityTeacher(1, "Dr. Smith"));
            teachers.push_back(UniversityTeacher(2, "Dr. Jones"));
        }
        if (tas.empty())
        {
            tas.push_back(TeachingAssistant(101, "Alice"));
            tas.push_back(TeachingAssistant(102, "Bob"));
            tas.push_back(TeachingAssistant(103, "Charlie"));
        }
    }
};

// ACTOR ROLES (Restored to original names)

class Person
{
protected:
    string roleName;

public:
    Person(const string &n) : roleName(n) {}
    virtual ~Person() {}
};

class HOD : public Person
{
private:
    float CalculateHours(const string &start, const string &end)
    {
        int h1 = stoi(start.substr(0, 2));
        int m1 = stoi(start.substr(3, 2));
        int h2 = stoi(end.substr(0, 2));
        int m2 = stoi(end.substr(3, 2));
        return (float)((h2 * 60 + m2) - (h1 * 60 + m1)) / 60.0f;
    }

    void GenerateScheduleReport(LabDetails *lDetails)
    {
        cout << "\n[HOD ACTION] Generating Weekly Schedule Report...\n";
        vector<CourseLaboratory> &labs = lDetails->GetAllLabs();
        for (auto &l : labs)
        {
            cout << "COURSE: " << l.courseCode << " (ID: " << l.labId << ")\n";
            for (auto &sec : l.sections)
            {
                cout << "  >> SECTION " << sec.sectionName << " | " << sec.scheduleTime.date
                     << " " << sec.scheduleTime.startTime << "-" << sec.scheduleTime.endTime << "\n";

                cout << "     Venue: " << (sec.building ? sec.building->name : "N/A")
                     << " - Room " << (sec.room ? to_string(sec.room->roomNumber) : "N/A") << "\n";
                cout << "     Instructor: " << (sec.teacher ? sec.teacher->name : "Unassigned") << "\n";
                cout << "     TAs: ";
                for (auto ta : sec.assistants)
                    cout << (ta ? ta->name : "Unknown") << ", ";
                cout << "\n\n";
            }
        }
    }

    void GenerateWeeklyWorkLogReport(WorkLogDetails *logDetails)
    {
        cout << "\n[HOD REPORT] Weekly Filled Time Sheets\n";
        vector<WorkLog> &logs = logDetails->GetAllEntries();
        cout << left << setw(8) << "Lab ID" << setw(10) << "Section" << setw(15) << "Date" << setw(10) << "Status" << endl;
        for (auto &t : logs)
        {
            cout << left << setw(8) << t.labId << setw(10) << t.sectionName << setw(15) << t.actualTiming.date
                 << setw(10) << (t.isLeave ? "LEAVE" : "PRESENT") << endl;
        }
    }

public:
    HOD() : Person("Head of Department") {}

    void ShowMenu(LabDetails *lDetails, WorkLogDetails *wDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- HOD DASHBOARD ---\n1. View Schedule\n2. View Time Sheets\n3. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1)
                GenerateScheduleReport(lDetails);
            else if (choice == 2)
                GenerateWeeklyWorkLogReport(wDetails);
            else
                return;
        }
    }
};

class AcademicOfficer : public Person
{
private:
    void ScheduleSection(LabDetails *lDetails, VenueDetails *vDetails, FacultyDetails *fDetails)
    {
        int labId, teacherId, bId, rId, taCount;
        string code, secName, day, s, e;

        cout << "Lab ID: ";
        InputOutput::SafeReadInt(labId);
        cout << "Course Code: ";
        InputOutput::SafeReadString(code);
        cout << "Section Name: ";
        InputOutput::SafeReadString(secName);
        cout << "Instructor/Teacher ID (1 or 2): ";
        InputOutput::SafeReadInt(teacherId);
        cout << "Building ID (1 or 2): ";
        InputOutput::SafeReadInt(bId);
        cout << "Room ID (101 or 102): ";
        InputOutput::SafeReadInt(rId);
        cout << "Day: ";
        InputOutput::SafeReadString(day);
        cout << "Start (HH:MM): ";
        InputOutput::SafeReadString(s);
        cout << "End (HH:MM): ";
        InputOutput::SafeReadString(e);

        UniversityTeacher *teach = fDetails->FindTeacher(teacherId);
        CampusBlock *build = vDetails->FindBuilding(bId);
        LectureHall *room = vDetails->FindRoom(rId);

        if (!teach || !build || !room)
        {
            cout << "Error: Invalid Teacher, Building, or Room ID.\n";
            return;
        }

        ClassSection newSec;
        newSec.SetDetails(secName, teach, build, room);
        newSec.scheduleTime.Set(day, s, e);

        cout << "How many TAs? ";
        InputOutput::SafeReadInt(taCount);
        for (int i = 0; i < taCount; i++)
        {
            int taId;
            cout << "TA ID (101-103): ";
            InputOutput::SafeReadInt(taId);
            TeachingAssistant *ta = fDetails->FindTA(taId);
            if (ta)
                newSec.AddTA(ta);
        }

        CourseLaboratory *lab = lDetails->FindLab(labId);
        if (lab)
        {
            lab->AddSection(newSec);
            lDetails->UpdateLab(*lab);
        }
        else
        {
            CourseLaboratory newLab;
            newLab.labId = labId;
            newLab.courseCode = code;
            newLab.AddSection(newSec);
            lDetails->AddLab(newLab);
        }
        cout << "Section Scheduled.\n";
    }

public:
    AcademicOfficer() : Person("Academic Officer") {}

    void ShowMenu(LabDetails *lDetails, VenueDetails *vDetails, FacultyDetails *fDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- ACADEMIC OFFICER ---\n1. Schedule Section\n2. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1)
                ScheduleSection(lDetails, vDetails, fDetails);
            else
                return;
        }
    }
};

class Instructor : public Person
{
private:
    void RequestMakeupLab(LabDetails *lDetails, int labId, const string &secName, const string &s, const string &e)
    {
        CourseLaboratory *l = lDetails->FindLab(labId);
        if (l)
        {
            ClassSection *sec = l->FindSection(secName);
            if (sec)
            {
                sec->scheduleTime.startTime = s;
                sec->scheduleTime.endTime = e;
                cout << "Makeup Requested/Updated.\n";
            }
        }
    }

public:
    Instructor() : Person("Instructor") {}

    void ShowMenu(LabDetails *lDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- INSTRUCTOR DASHBOARD ---\n1. Request Makeup\n2. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1)
            {
                int id;
                string sec, s, e;
                cout << "Lab ID: ";
                InputOutput::SafeReadInt(id);
                cout << "Section: ";
                InputOutput::SafeReadString(sec);
                cout << "Start: ";
                InputOutput::SafeReadString(s);
                cout << "End: ";
                InputOutput::SafeReadString(e);
                RequestMakeupLab(lDetails, id, sec, s, e);
            }
            else
                return;
        }
    }
};

class Attendant : public Person
{
private:
    void FillTimeSheet(WorkLogDetails *logDetails, int labId, const string &secName, const string &d, const string &s, const string &e, bool leave)
    {
        WorkLog entry;
        entry.labId = labId;
        entry.sectionName = secName;
        entry.actualTiming.Set(d, s, e);
        entry.isLeave = leave;
        logDetails->AddEntry(entry);
        cout << "Time Sheet Filled.\n";
    }

public:
    Attendant() : Person("Attendant") {}

    void ShowMenu(WorkLogDetails *logDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- ATTENDANT DASHBOARD ---\n1. Fill Time Sheet\n2. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1)
            {
                int id, leave;
                string sec, d, s, e;
                cout << "Lab ID: ";
                InputOutput::SafeReadInt(id);
                cout << "Section: ";
                InputOutput::SafeReadString(sec);
                cout << "Date: ";
                InputOutput::SafeReadString(d);
                cout << "Leave? (1/0): ";
                InputOutput::SafeReadInt(leave);
                if (!leave)
                {
                    cout << "Start: ";
                    InputOutput::SafeReadString(s);
                    cout << "End: ";
                    InputOutput::SafeReadString(e);
                }
                FillTimeSheet(logDetails, id, sec, d, s, e, leave);
            }
            else
                return;
        }
    }
};

// STORAGE MANAGER (Serialization)

class StorageManager
{
    LabDetails *labDetails;
    VenueDetails *venueDetails;
    FacultyDetails *facultyDetails;
    WorkLogDetails *logDetails;

public:
    StorageManager(LabDetails *l, VenueDetails *v, FacultyDetails *f, WorkLogDetails *w)
        : labDetails(l), venueDetails(v), facultyDetails(f), logDetails(w) {}

    void Save()
    {
        ofstream out("labs_data.txt");
        vector<CourseLaboratory> &labs = labDetails->GetAllLabs();

        out << labs.size() << endl;
        for (const auto &lab : labs)
        {
            out << lab.labId << " " << lab.courseCode << " " << lab.sections.size() << endl;
            for (const auto &sec : lab.sections)
            {
                out << sec.sectionName << endl;
                out << (sec.teacher ? sec.teacher->teacherId : -1) << endl;
                out << (sec.building ? sec.building->buildingId : -1) << endl;
                out << (sec.room ? sec.room->roomId : -1) << endl;
                out << sec.scheduleTime.date << " " << sec.scheduleTime.startTime << " " << sec.scheduleTime.endTime << endl;

                out << sec.assistants.size() << endl;
                for (auto ta : sec.assistants)
                {
                    out << (ta ? ta->taId : -1) << " ";
                }
                out << endl;
            }
        }
        out.close();

        // Save Work Logs
        ofstream wout("logs_data.txt");
        vector<WorkLog> &logs = logDetails->GetAllEntries();
        wout << logs.size() << endl;
        for (const auto &log : logs)
        {
            wout << log.labId << " " << log.sectionName << " " << log.isLeave << " "
                 << log.actualTiming.date << " " << log.actualTiming.startTime << " " << log.actualTiming.endTime << endl;
        }
        wout.close();

        cout << "[System] Data saved to text files.\n";
    }

    void Load()
    {
        ifstream in("labs_data.txt");
        if (in)
        {
            int labCount;
            in >> labCount;
            for (int i = 0; i < labCount; i++)
            {
                CourseLaboratory lab;
                int secCount;
                in >> lab.labId >> lab.courseCode >> secCount;

                for (int j = 0; j < secCount; j++)
                {
                    ClassSection sec;
                    int teachId, buildId, roomId, taCount;

                    in >> sec.sectionName;
                    in >> teachId >> buildId >> roomId;
                    in >> sec.scheduleTime.date >> sec.scheduleTime.startTime >> sec.scheduleTime.endTime;

                    sec.teacher = facultyDetails->FindTeacher(teachId);
                    sec.building = venueDetails->FindBuilding(buildId);
                    sec.room = venueDetails->FindRoom(roomId);

                    in >> taCount;
                    for (int k = 0; k < taCount; k++)
                    {
                        int taId;
                        in >> taId;
                        TeachingAssistant *ta = facultyDetails->FindTA(taId);
                        if (ta)
                            sec.AddTA(ta);
                    }
                    lab.AddSection(sec);
                }
                labDetails->AddLab(lab);
            }
            in.close();
        }

        ifstream win("logs_data.txt");
        if (win)
        {
            int logCount;
            win >> logCount;
            for (int i = 0; i < logCount; i++)
            {
                WorkLog log;
                win >> log.labId >> log.sectionName >> log.isLeave >> log.actualTiming.date >> log.actualTiming.startTime >> log.actualTiming.endTime;
                logDetails->AddEntry(log);
            }
            win.close();
        }
    }
};

// MAIN PROGRAM

int main()
{
    InMemoryLabDetails labDetails;
    InMemoryVenueDetails venueDetails;
    InMemoryFacultyDetails facultyDetails;
    InMemoryWorkLogDetails logDetails;

    venueDetails.SeedData();
    facultyDetails.SeedData();

    StorageManager storage(&labDetails, &venueDetails, &facultyDetails, &logDetails);
    storage.Load();

    HOD hod;
    AcademicOfficer officer;
    Instructor instructor;
    Attendant attendant;

    while (true)
    {
        cout << "\nSYSTEM\n1. HOD\n2. Academic Officer\n3. Instructor\n4. Attendant\n5. Save & Exit\nSelect: ";
        int role;
        InputOutput::SafeReadInt(role);

        switch (role)
        {
        case 1:
            hod.ShowMenu(&labDetails, &logDetails);
            break;
        case 2:
            officer.ShowMenu(&labDetails, &venueDetails, &facultyDetails);
            break;
        case 3:
            instructor.ShowMenu(&labDetails);
            break;
        case 4:
            attendant.ShowMenu(&logDetails);
            break;
        case 5:
            storage.Save();
            return 0;
        }
    }
}