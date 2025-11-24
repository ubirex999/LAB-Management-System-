#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

// Utility class for Input/Output operations
class InputOutput
{
public:
    static void SafeReadInt(int &val)
    {
        while (!(cin >> val))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid integer input. Please try again: ";
        }
        cin.ignore(1000, '\n');
    }

    static void SafeReadString(string &val)
    {
        getline(cin, val);
    }
};

// Data Validation logic
class DataValidator
{
public:
    static bool IsValidTime(const string &t)
    {
        if (t.length() != 5 || t[2] != ':') return false;
        if (!isdigit(t[0]) || !isdigit(t[1]) || !isdigit(t[3]) || !isdigit(t[4])) return false;
        try {
            int h = stoi(t.substr(0, 2));
            int m = stoi(t.substr(3, 2));
            return (h >= 0 && h < 24 && m >= 0 && m < 60);
        } catch (...) { return false; }
    }

    static bool IsStartBeforeEnd(const string &start, const string &end)
    {
        if (!IsValidTime(start) || !IsValidTime(end)) return false;
        int h1 = stoi(start.substr(0, 2));
        int m1 = stoi(start.substr(3, 2));
        int h2 = stoi(end.substr(0, 2));
        int m2 = stoi(end.substr(3, 2));
        return (h1 * 60 + m1) < (h2 * 60 + m2);
    }

    static bool IsNonEmptyString(const string &s) {
        return !s.empty() && s.find_first_not_of(" \t\n\r") != string::npos;
    }

    static bool IsValidID(int id) {
        return id > 0;
    }

    static bool IsLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    static bool IsValidDate(const string &d) {
        // Expected Format: YYYY-MM-DD
        if (d.length() != 10) return false;
        if (d[4] != '-' || d[7] != '-') return false;
        
        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7) continue;
            if (!isdigit(d[i])) return false;
        }

        try {
            int year = stoi(d.substr(0, 4));
            int month = stoi(d.substr(5, 2));
            int day = stoi(d.substr(8, 2));

            if (year < 1900 || year > 2100) return false;
            if (month < 1 || month > 12) return false;

            int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (IsLeapYear(year)) daysInMonth[2] = 29;

            return day >= 1 && day <= daysInMonth[month];
        } catch (...) {
            return false;
        }
    }
};

// Core Domain Entities

class DateAndTime
{
private:
    string date;
    string startTime;
    string endTime;

public:
    void Set(const string &d, const string &s, const string &e)
    {
        date = d;
        startTime = s;
        endTime = e;
    }
    string GetDate() const { return date; }
    string GetStartTime() const { return startTime; }
    string GetEndTime() const { return endTime; }
};

class CampusBlock
{
private:
    int buildingId;
    string name;

public:
    CampusBlock() : buildingId(0) {}
    CampusBlock(int id, string n) : buildingId(id), name(n) {}

    int GetId() const { return buildingId; }
    string GetName() const { return name; }
};

class LectureHall
{
private:
    int roomId;
    string roomNumber;
    int buildingId;
    CampusBlock *building;

public:
    LectureHall() : roomId(0), roomNumber(""), buildingId(0), building(nullptr) {}
    LectureHall(int id, string num, int bId, CampusBlock *bldg)
        : roomId(id), roomNumber(num), buildingId(bId), building(bldg) {}

    int GetId() const { return roomId; }
    string GetRoomNumber() const { return roomNumber; }
    int GetBuildingId() const { return buildingId; }
    CampusBlock *GetBuilding() const { return building; }
    
    void SetBuilding(CampusBlock* b) { building = b; }
};

class UniversityTeacher
{
private:
    int teacherId;
    string name;

public:
    UniversityTeacher() : teacherId(0) {}
    UniversityTeacher(int id, string n) : teacherId(id), name(n) {}

    int GetId() const { return teacherId; }
    string GetName() const { return name; }
};

class TeachingAssistant
{
private:
    int taId;
    string name;

public:
    TeachingAssistant() : taId(0) {}
    TeachingAssistant(int id, string n) : taId(id), name(n) {}

    int GetId() const { return taId; }
    string GetName() const { return name; }
};

class ClassSection
{
private:
    string sectionName;
    UniversityTeacher *teacher;
    vector<TeachingAssistant *> assistants;
    CampusBlock *building;
    LectureHall *room;
    DateAndTime scheduleTime;

public:
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

    string GetSectionName() const { return sectionName; }
    UniversityTeacher *GetTeacher() const { return teacher; }
    const vector<TeachingAssistant *> &GetAssistants() const { return assistants; }
    CampusBlock *GetBuilding() const { return building; }
    LectureHall *GetRoom() const { return room; }
    DateAndTime &GetScheduleTime() { return scheduleTime; }
    const DateAndTime &GetScheduleTime() const { return scheduleTime; }

    void SetSectionName(const string &n) { sectionName = n; }
    void SetTeacher(UniversityTeacher *t) { teacher = t; }
    void SetBuilding(CampusBlock *b) { building = b; }
    void SetRoom(LectureHall *r) { room = r; }
};

class CourseLaboratory
{
private:
    int labId;
    string courseCode;
    vector<ClassSection> sections;

public:
    CourseLaboratory() : labId(0) {}

    void AddSection(const ClassSection &s)
    {
        sections.push_back(s);
    }

    ClassSection *FindSection(const string &secName)
    {
        for (auto &s : sections)
        {
            if (s.GetSectionName() == secName)
                return &s;
        }
        return nullptr;
    }

    int GetLabId() const { return labId; }
    void SetLabId(int id) { labId = id; }
    string GetCourseCode() const { return courseCode; }
    void SetCourseCode(const string &code) { courseCode = code; }
    vector<ClassSection> &GetSections() { return sections; }
    const vector<ClassSection> &GetSections() const { return sections; }
};

class WorkLog
{
private:
    int labId;
    string sectionName;
    DateAndTime actualTiming;
    bool isLeave;

public:
    WorkLog() : labId(0), isLeave(false) {}

    int GetLabId() const { return labId; }
    void SetLabId(int id) { labId = id; }
    string GetSectionName() const { return sectionName; }
    void SetSectionName(const string &n) { sectionName = n; }
    const DateAndTime &GetActualTiming() const { return actualTiming; }
    DateAndTime &GetActualTiming() { return actualTiming; }
    bool GetIsLeave() const { return isLeave; }
    void SetIsLeave(bool l) { isLeave = l; }
};

class MakeupLabRequest
{
private:
    CourseLaboratory *lab;
    string sectionName;
    string requestedStartTime;
    string requestedEndTime;
    string requestedDate;

public:
    MakeupLabRequest() : lab(nullptr) {}
    MakeupLabRequest(CourseLaboratory *l, const string &sec, const string &date, const string &start, const string &end)
        : lab(l), sectionName(sec), requestedDate(date), requestedStartTime(start), requestedEndTime(end) {}

    CourseLaboratory *GetLab() const { return lab; }
    void SetLab(CourseLaboratory *l) { lab = l; }
    string GetSectionName() const { return sectionName; }
    void SetSectionName(const string &s) { sectionName = s; }
    string GetRequestedDate() const { return requestedDate; }
    void SetRequestedDate(const string &d) { requestedDate = d; }
    string GetRequestedStartTime() const { return requestedStartTime; }
    void SetRequestedStartTime(const string &s) { requestedStartTime = s; }
    string GetRequestedEndTime() const { return requestedEndTime; }
    void SetRequestedEndTime(const string &e) { requestedEndTime = e; }
};

// Interfaces and Implementations

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
    virtual void AddBuilding(const CampusBlock &b) = 0;
    virtual void AddRoom(const LectureHall &r) = 0;
    virtual CampusBlock *FindBuilding(int id) = 0;
    virtual LectureHall *FindRoom(int id) = 0;
    virtual vector<CampusBlock>& GetAllBuildings() = 0;
    virtual vector<LectureHall>& GetAllRooms() = 0;
};

class FacultyDetails
{
public:
    virtual void AddTeacher(const UniversityTeacher &t) = 0;
    virtual void AddTA(const TeachingAssistant &t) = 0;
    virtual UniversityTeacher *FindTeacher(int id) = 0;
    virtual TeachingAssistant *FindTA(int id) = 0;
    virtual vector<UniversityTeacher>& GetAllTeachers() = 0;
    virtual vector<TeachingAssistant>& GetAllTAs() = 0;
};

class InMemoryLabDetails : public LabDetails
{
    vector<CourseLaboratory> labs;
public:
    void AddLab(const CourseLaboratory &l) override { labs.push_back(l); }
    void UpdateLab(const CourseLaboratory &l) override
    {
        for (auto &existing : labs) {
            if (existing.GetLabId() == l.GetLabId()) {
                existing = l;
                return;
            }
        }
    }
    CourseLaboratory *FindLab(int id) override
    {
        for (auto &l : labs) if (l.GetLabId() == id) return &l;
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
    void AddBuilding(const CampusBlock &b) override { buildings.push_back(b); }
    void AddRoom(const LectureHall &r) override { rooms.push_back(r); }
    
    CampusBlock *FindBuilding(int id) override
    {
        for (auto &b : buildings) if (b.GetId() == id) return &b;
        return nullptr;
    }
    LectureHall *FindRoom(int id) override
    {
        for (auto &r : rooms) if (r.GetId() == id) return &r;
        return nullptr;
    }
    vector<CampusBlock>& GetAllBuildings() override { return buildings; }
    vector<LectureHall>& GetAllRooms() override { return rooms; }
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
        for (auto &t : teachers) if (t.GetId() == id) return &t;
        return nullptr;
    }
    TeachingAssistant *FindTA(int id) override
    {
        for (auto &t : tas) if (t.GetId() == id) return &t;
        return nullptr;
    }
    vector<UniversityTeacher>& GetAllTeachers() override { return teachers; }
    vector<TeachingAssistant>& GetAllTAs() override { return tas; }
};

// Actor Roles

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

    string ExtractDay(const string &date)
    {
        vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        for (const string &day : days)
        {
            if (date.find(day) != string::npos)
                return day;
        }
        return date;
    }

    bool IsDateInWeek(const string &date, const string &weekIdentifier)
    {
        return date.find(weekIdentifier) != string::npos || weekIdentifier == "all";
    }

    void GenerateCompleteWeeklySchedule(LabDetails *lDetails)
    {
        cout << "\nComplete Lab Schedule - Entire Week\n";
        cout << string(40, '=') << "\n";

        vector<CourseLaboratory> &labs = lDetails->GetAllLabs();

        if (labs.empty())
        {
            cout << "No labs scheduled for the week.\n";
            return;
        }

        map<string, vector<pair<CourseLaboratory *, ClassSection *>>> scheduleByDay;
        for (auto &lab : labs)
        {
            for (auto &sec : lab.GetSections())
            {
                string day = ExtractDay(sec.GetScheduleTime().GetDate());
                scheduleByDay[day].push_back({&lab, &sec});
            }
        }

        vector<string> dayOrder = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        set<string> processedDays;

        for (const string &day : dayOrder)
        {
            if (scheduleByDay.find(day) != scheduleByDay.end())
            {
                processedDays.insert(day);
                cout << "\n--- " << day << " ---\n";
                for (const auto &entry : scheduleByDay[day])
                {
                    CourseLaboratory *lab = entry.first;
                    ClassSection *sec = entry.second;

                    cout << "Lab ID: " << lab->GetLabId() << " | Course: " << lab->GetCourseCode() << "\n";
                    cout << "  Section: " << sec->GetSectionName() << "\n";
                    cout << "  Time: " << sec->GetScheduleTime().GetStartTime() << " - " << sec->GetScheduleTime().GetEndTime() << "\n";
                    cout << "  Venue: " << (sec->GetBuilding() ? sec->GetBuilding()->GetName() : "N/A")
                         << " - Room " << (sec->GetRoom() ? sec->GetRoom()->GetRoomNumber() : "N/A") << "\n";
                    cout << "  Instructor: " << (sec->GetTeacher() ? sec->GetTeacher()->GetName() : "Unassigned") << "\n";
                }
            }
        }

        for (const auto& pair : scheduleByDay) {
            if (processedDays.find(pair.first) == processedDays.end()) {
                cout << "\n--- " << pair.first << " ---\n";
                for (const auto &entry : pair.second)
                {
                    CourseLaboratory *lab = entry.first;
                    ClassSection *sec = entry.second;
                    cout << "Lab ID: " << lab->GetLabId() << " | Course: " << lab->GetCourseCode() << "\n";
                    cout << "  Section: " << sec->GetSectionName() << "\n";
                    cout << "  Time: " << sec->GetScheduleTime().GetStartTime() << " - " << sec->GetScheduleTime().GetEndTime() << "\n";
                    cout << "  Venue: " << (sec->GetBuilding() ? sec->GetBuilding()->GetName() : "N/A")
                         << " - Room " << (sec->GetRoom() ? sec->GetRoom()->GetRoomNumber() : "N/A") << "\n";
                    cout << "  Instructor: " << (sec->GetTeacher() ? sec->GetTeacher()->GetName() : "Unassigned") << "\n";
                }
            }
        }
        cout << string(40, '=') << "\n";
    }

    void GenerateWeeklyTimeSheetReport(LabDetails *lDetails, WorkLogDetails *logDetails)
    {
        cout << "\nFilled Time Sheets Report\n";
        cout << string(40, '=') << "\n";

        string weekInput;
        cout << "Enter week identifier (e.g., 'Week1', 'all'): ";
        InputOutput::SafeReadString(weekInput);

        vector<WorkLog> &logs = logDetails->GetAllEntries();
        vector<WorkLog> filteredLogs;

        for (const auto &log : logs)
        {
            if (IsDateInWeek(log.GetActualTiming().GetDate(), weekInput)) filteredLogs.push_back(log);
        }

        if (filteredLogs.empty())
        {
            cout << "No entries found.\n";
            return;
        }

        cout << left << setw(8) << "LabID" << setw(12) << "Section" << setw(15) << "Date" << "Status" << endl;
        for (const auto &log : filteredLogs)
        {
            cout << left << setw(8) << log.GetLabId() << setw(12) << log.GetSectionName()
                 << setw(15) << log.GetActualTiming().GetDate()
                 << (log.GetIsLeave() ? "LEAVE" : "PRESENT") << endl;
        }
    }

    void GenerateLabSpecificTimeSheet(LabDetails *lDetails, WorkLogDetails *logDetails)
    {
        int labId;
        cout << "Enter Lab ID: "; InputOutput::SafeReadInt(labId);
        if (!lDetails->FindLab(labId)) { cout << "Error: Lab ID not found.\n"; return; }

        vector<WorkLog> &logs = logDetails->GetAllEntries();
        vector<WorkLog> labLogs;
        for (const auto &log : logs) if (log.GetLabId() == labId) labLogs.push_back(log);

        if (labLogs.empty()) { cout << "No logs for this lab.\n"; return; }

        cout << "Logs for Lab " << labId << ":\n";
        for (const auto &log : labLogs)
        {
            cout << "Sec: " << log.GetSectionName() << " | Date: " << log.GetActualTiming().GetDate() 
                 << " | " << (log.GetIsLeave() ? "LEAVE" : "PRESENT") << endl;
        }
    }

public:
    HOD() : Person("HOD") {}
    void ShowMenu(LabDetails *lDetails, WorkLogDetails *wDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- HOD DASHBOARD ---\n";
            cout << "1. Weekly Schedule\n2. Weekly Time Sheet Report\n3. Lab Specific Report\n4. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1) GenerateCompleteWeeklySchedule(lDetails);
            else if (choice == 2) GenerateWeeklyTimeSheetReport(lDetails, wDetails);
            else if (choice == 3) GenerateLabSpecificTimeSheet(lDetails, wDetails);
            else return;
        }
    }
};

class AcademicOfficer : public Person
{
private:
    vector<MakeupLabRequest> ReadMakeupRequestsFromBinary(LabDetails *lDetails)
    {
        vector<MakeupLabRequest> requests;
        ifstream in("makeup_requests.dat", ios::binary);
        if (!in.is_open()) return requests;

        while (in.peek() != EOF)
        {
            MakeupLabRequest request;
            int labId; in.read(reinterpret_cast<char *>(&labId), sizeof(int));
            if (in.eof()) break;

            request.SetLab(lDetails->FindLab(labId));

            int secLen; in.read(reinterpret_cast<char *>(&secLen), sizeof(int));
            if (in.eof()) break;
            char *secBuf = new char[secLen + 1]; in.read(secBuf, secLen); secBuf[secLen] = '\0';
            request.SetSectionName(string(secBuf)); delete[] secBuf;

            int dateLen; in.read(reinterpret_cast<char *>(&dateLen), sizeof(int));
            if (in.eof()) break;
            char *dateBuf = new char[dateLen + 1]; in.read(dateBuf, dateLen); dateBuf[dateLen] = '\0';
            request.SetRequestedDate(string(dateBuf)); delete[] dateBuf;

            int startLen; in.read(reinterpret_cast<char *>(&startLen), sizeof(int));
            if (in.eof()) break;
            char *startBuf = new char[startLen + 1]; in.read(startBuf, startLen); startBuf[startLen] = '\0';
            request.SetRequestedStartTime(string(startBuf)); delete[] startBuf;

            int endLen; in.read(reinterpret_cast<char *>(&endLen), sizeof(int));
            if (in.eof()) break;
            char *endBuf = new char[endLen + 1]; in.read(endBuf, endLen); endBuf[endLen] = '\0';
            request.SetRequestedEndTime(string(endBuf)); delete[] endBuf;

            requests.push_back(request);
        }
        in.close();
        return requests;
    }

    void WriteMakeupRequestsToBinary(const vector<MakeupLabRequest> &requests)
    {
        ofstream out("makeup_requests.dat", ios::binary | ios::trunc);
        if (!out.is_open()) return;

        for (const auto &request : requests)
        {
            int labId = (request.GetLab() ? request.GetLab()->GetLabId() : -1);
            out.write(reinterpret_cast<const char *>(&labId), sizeof(int));

            string secName = request.GetSectionName(); int secLen = secName.length();
            out.write(reinterpret_cast<const char *>(&secLen), sizeof(int)); out.write(secName.c_str(), secLen);

            string date = request.GetRequestedDate(); int dateLen = date.length();
            out.write(reinterpret_cast<const char *>(&dateLen), sizeof(int)); out.write(date.c_str(), dateLen);

            string start = request.GetRequestedStartTime(); int startLen = start.length();
            out.write(reinterpret_cast<const char *>(&startLen), sizeof(int)); out.write(start.c_str(), startLen);

            string end = request.GetRequestedEndTime(); int endLen = end.length();
            out.write(reinterpret_cast<const char *>(&endLen), sizeof(int)); out.write(end.c_str(), endLen);
        }
        out.close();
    }

public:
    AcademicOfficer() : Person("Academic Officer") {}

    void AddBuilding(VenueDetails *vDetails) {
        int id; string name;
        do {
            cout << "Enter Building ID (>0): "; InputOutput::SafeReadInt(id);
            if (!DataValidator::IsValidID(id)) cout << "Invalid ID. Must be > 0.\n";
        } while (!DataValidator::IsValidID(id));
        
        if(vDetails->FindBuilding(id)) { cout << "ID exists.\n"; return; }
        
        do {
            cout << "Enter Building Name: "; InputOutput::SafeReadString(name);
            if (!DataValidator::IsNonEmptyString(name)) cout << "Name cannot be empty.\n";
        } while (!DataValidator::IsNonEmptyString(name));
        
        vDetails->AddBuilding(CampusBlock(id, name));
        cout << "Building Added.\n";
    }

    void AddRoom(VenueDetails *vDetails) {
        int id, bId; string num;
        do {
            cout << "Enter Room ID (>0): "; InputOutput::SafeReadInt(id);
            if (!DataValidator::IsValidID(id)) cout << "Invalid ID. Must be > 0.\n";
        } while (!DataValidator::IsValidID(id));
        
        if(vDetails->FindRoom(id)) { cout << "ID exists.\n"; return; }
        
        do {
            cout << "Enter Room Number/Name: "; InputOutput::SafeReadString(num);
            if (!DataValidator::IsNonEmptyString(num)) cout << "Cannot be empty.\n";
        } while (!DataValidator::IsNonEmptyString(num));
        
        do {
            cout << "Enter Building ID (>0): "; InputOutput::SafeReadInt(bId);
            if (!DataValidator::IsValidID(bId)) cout << "Invalid ID.\n";
        } while (!DataValidator::IsValidID(bId));
        
        CampusBlock* b = vDetails->FindBuilding(bId);
        if(!b) { cout << "Building ID not found.\n"; return; }
        
        vDetails->AddRoom(LectureHall(id, num, bId, b));
        cout << "Room Added.\n";
    }

    void AddTeacher(FacultyDetails *fDetails) {
        int id; string name;
        do {
            cout << "Enter Teacher ID (>0): "; InputOutput::SafeReadInt(id);
            if (!DataValidator::IsValidID(id)) cout << "Invalid ID.\n";
        } while (!DataValidator::IsValidID(id));
        
        if(fDetails->FindTeacher(id)) { cout << "ID exists.\n"; return; }
        
        do {
            cout << "Enter Name: "; InputOutput::SafeReadString(name);
            if (!DataValidator::IsNonEmptyString(name)) cout << "Cannot be empty.\n";
        } while (!DataValidator::IsNonEmptyString(name));
        
        fDetails->AddTeacher(UniversityTeacher(id, name));
        cout << "Teacher Added.\n";
    }

    void AddTA(FacultyDetails *fDetails) {
        int id; string name;
        do {
            cout << "Enter TA ID (>0): "; InputOutput::SafeReadInt(id);
            if (!DataValidator::IsValidID(id)) cout << "Invalid ID.\n";
        } while (!DataValidator::IsValidID(id));
        
        if(fDetails->FindTA(id)) { cout << "ID exists.\n"; return; }
        
        do {
            cout << "Enter Name: "; InputOutput::SafeReadString(name);
            if (!DataValidator::IsNonEmptyString(name)) cout << "Cannot be empty.\n";
        } while (!DataValidator::IsNonEmptyString(name));
        
        fDetails->AddTA(TeachingAssistant(id, name));
        cout << "TA Added.\n";
    }

    void ViewInfrastructure(VenueDetails *v, FacultyDetails *f) {
        cout << "\n--- VIEW INFRASTRUCTURE ---\n";
        cout << "1. Buildings\n2. Rooms\n3. Teachers\n4. TAs\nSelect: ";
        int choice;
        InputOutput::SafeReadInt(choice);

        if (choice == 1) {
            auto& bldgs = v->GetAllBuildings();
            if (bldgs.empty()) cout << "No buildings recorded.\n";
            else for (auto& b : bldgs) cout << "ID: " << b.GetId() << " | Name: " << b.GetName() << endl;
        }
        else if (choice == 2) {
            auto& rooms = v->GetAllRooms();
            if (rooms.empty()) cout << "No rooms recorded.\n";
            else for (auto& r : rooms) cout << "ID: " << r.GetId() << " | Room: " << r.GetRoomNumber() << " | Building ID: " << r.GetBuildingId() << endl;
        }
        else if (choice == 3) {
            auto& teachers = f->GetAllTeachers();
            if (teachers.empty()) cout << "No teachers recorded.\n";
            else for (auto& t : teachers) cout << "ID: " << t.GetId() << " | Name: " << t.GetName() << endl;
        }
        else if (choice == 4) {
            auto& tas = f->GetAllTAs();
            if (tas.empty()) cout << "No TAs recorded.\n";
            else for (auto& t : tas) cout << "ID: " << t.GetId() << " | Name: " << t.GetName() << endl;
        }
    }

    void ViewCompleteLabDetails(LabDetails *lDetails)
    {
        cout << "\nComplete Lab Schedule\n";
        vector<CourseLaboratory> &labs = lDetails->GetAllLabs();
        if (labs.empty()) { cout << "No labs scheduled.\n"; return; }

        for (const auto &lab : labs)
        {
            cout << "\nLab ID: " << lab.GetLabId() << " | Code: " << lab.GetCourseCode() << "\n";
            for (const auto &sec : lab.GetSections())
            {
                cout << "  Section: " << sec.GetSectionName() << "\n";
                cout << "  Instructor: " << (sec.GetTeacher() ? sec.GetTeacher()->GetName() : "Unassigned") << "\n";
                cout << "  Room: " << (sec.GetRoom() ? sec.GetRoom()->GetRoomNumber() : "N/A") << "\n";
                cout << "  Time: " << sec.GetScheduleTime().GetDate() << " " << sec.GetScheduleTime().GetStartTime() << "-" << sec.GetScheduleTime().GetEndTime() << "\n";
            }
        }
    }

    void ScheduleSection(LabDetails *lDetails, VenueDetails *vDetails, FacultyDetails *fDetails)
    {
        int labId, teacherId, bId, rId, taCount;
        string code, secName, day, s, e;

        do { cout << "Lab ID: "; InputOutput::SafeReadInt(labId); if (!DataValidator::IsValidID(labId)) cout << "Invalid ID.\n"; } while(!DataValidator::IsValidID(labId));
        do { cout << "Course Code: "; InputOutput::SafeReadString(code); if (!DataValidator::IsNonEmptyString(code)) cout << "Cannot be empty.\n"; } while(!DataValidator::IsNonEmptyString(code));
        do { cout << "Section Name: "; InputOutput::SafeReadString(secName); if (!DataValidator::IsNonEmptyString(secName)) cout << "Cannot be empty.\n"; } while(!DataValidator::IsNonEmptyString(secName));
        
        do {
            cout << "Teacher ID: "; InputOutput::SafeReadInt(teacherId);
            if (!fDetails->FindTeacher(teacherId)) cout << "Teacher ID not found.\n";
        } while(!fDetails->FindTeacher(teacherId));

        do {
            cout << "Building ID: "; InputOutput::SafeReadInt(bId);
            if (!vDetails->FindBuilding(bId)) cout << "Building ID not found.\n";
        } while(!vDetails->FindBuilding(bId));

        do {
            cout << "Room ID: "; InputOutput::SafeReadInt(rId);
            LectureHall* r = vDetails->FindRoom(rId);
            if (!r) cout << "Room ID not found.\n";
            else if (r->GetBuildingId() != bId) { cout << "Room does not belong to the selected building.\n"; r = nullptr; }
            if (!r) continue; else break;
        } while(true);
        
        do { cout << "Day/Date (YYYY-MM-DD or Weekday): "; InputOutput::SafeReadString(day); if (!DataValidator::IsNonEmptyString(day)) cout << "Cannot be empty.\n"; } while(!DataValidator::IsNonEmptyString(day));
        
        while(true) {
            cout << "Start (HH:MM): "; InputOutput::SafeReadString(s);
            cout << "End (HH:MM): "; InputOutput::SafeReadString(e);
            if (!DataValidator::IsValidTime(s) || !DataValidator::IsValidTime(e)) {
                cout << "Invalid time format. Use HH:MM.\n";
            } else if (!DataValidator::IsStartBeforeEnd(s, e)) {
                cout << "Start time must be strictly before End time.\n";
            } else {
                break;
            }
        }

        UniversityTeacher *t = fDetails->FindTeacher(teacherId);
        CampusBlock *b = vDetails->FindBuilding(bId);
        LectureHall *r = vDetails->FindRoom(rId);

        ClassSection sec;
        sec.SetDetails(secName, t, b, r);
        sec.GetScheduleTime().Set(day, s, e);

        cout << "Num TAs: "; InputOutput::SafeReadInt(taCount);
        for(int i=0; i<taCount; i++) {
            int taId; cout << "TA ID: "; InputOutput::SafeReadInt(taId);
            TeachingAssistant* ta = fDetails->FindTA(taId);
            if(ta) sec.AddTA(ta);
            else cout << "TA ID " << taId << " not found, skipping.\n";
        }

        CourseLaboratory *lab = lDetails->FindLab(labId);
        if (lab) {
            lab->AddSection(sec);
            lDetails->UpdateLab(*lab);
        } else {
            CourseLaboratory newLab;
            newLab.SetLabId(labId);
            newLab.SetCourseCode(code);
            newLab.AddSection(sec);
            lDetails->AddLab(newLab);
        }
        cout << "Scheduled.\n";
    }

    void ViewMakeupRequests(LabDetails *lDetails)
    {
        vector<MakeupLabRequest> requests = ReadMakeupRequestsFromBinary(lDetails);
        if (requests.empty()) { cout << "\nNo makeup requests.\n"; return; }

        cout << "\nMakeup Requests\n";
        for (const auto &req : requests)
        {
            int labId = (req.GetLab() ? req.GetLab()->GetLabId() : -1);
            cout << "Lab: " << labId << " | Sec: " << req.GetSectionName() << " | Date: " << req.GetRequestedDate() << "\n";
        }
    }

    void ScheduleMakeupLab(LabDetails *lDetails, VenueDetails *vDetails, FacultyDetails *fDetails)
    {
        vector<MakeupLabRequest> requests = ReadMakeupRequestsFromBinary(lDetails);
        if (requests.empty()) { cout << "\nNo makeup requests.\n"; return; }

        cout << "\nAvailable Makeup Requests\n";
        for (size_t i = 0; i < requests.size(); i++)
        {
            int labId = (requests[i].GetLab() ? requests[i].GetLab()->GetLabId() : -1);
            cout << (i + 1) << ". Lab ID: " << labId << ", Sec: " << requests[i].GetSectionName() << endl;
        }

        int choice; cout << "\nSelect request number (0 to cancel): "; InputOutput::SafeReadInt(choice);
        if (choice < 1 || choice > (int)requests.size()) return;

        MakeupLabRequest selected = requests[choice - 1];

        int teacherId, bId, rId, taCount;
        
        do {
            cout << "Instructor ID: "; InputOutput::SafeReadInt(teacherId);
            if(!fDetails->FindTeacher(teacherId)) cout << "Teacher not found.\n";
        } while(!fDetails->FindTeacher(teacherId));

        do {
            cout << "Building ID: "; InputOutput::SafeReadInt(bId);
            if(!vDetails->FindBuilding(bId)) cout << "Building not found.\n";
        } while(!vDetails->FindBuilding(bId));

        do {
            cout << "Room ID: "; InputOutput::SafeReadInt(rId);
            LectureHall* r = vDetails->FindRoom(rId);
            if(!r) cout << "Room not found.\n";
            else if (r->GetBuildingId() != bId) { cout << "Room not in building.\n"; r = nullptr; }
            if(r) break;
        } while(true);

        UniversityTeacher *t = fDetails->FindTeacher(teacherId);
        CampusBlock *b = vDetails->FindBuilding(bId);
        LectureHall *r = vDetails->FindRoom(rId);

        ClassSection makeupSec;
        makeupSec.SetDetails(selected.GetSectionName() + "_MAKEUP", t, b, r);
        makeupSec.GetScheduleTime().Set(selected.GetRequestedDate(), selected.GetRequestedStartTime(), selected.GetRequestedEndTime());

        cout << "Num TAs: "; InputOutput::SafeReadInt(taCount);
        for(int i=0; i<taCount; i++) {
            int taId; cout << "TA ID: "; InputOutput::SafeReadInt(taId);
            TeachingAssistant* ta = fDetails->FindTA(taId);
            if(ta) makeupSec.AddTA(ta);
        }

        selected.GetLab()->AddSection(makeupSec);
        lDetails->UpdateLab(*selected.GetLab());

        requests.erase(requests.begin() + (choice - 1));
        WriteMakeupRequestsToBinary(requests);
        cout << "Makeup Scheduled.\n";
    }

    void ShowMenu(LabDetails *l, VenueDetails *v, FacultyDetails *f) {
        while(true) {
            cout << "\n--- ACADEMIC OFFICER DASHBOARD ---\n";
            cout << "1. Add Infrastructure (Building/Room/Faculty)\n";
            cout << "2. Schedule Section\n";
            cout << "3. View Complete Lab Schedule\n";
            cout << "4. View Existing Infrastructure\n";
            cout << "5. View Makeup Requests\n";
            cout << "6. Schedule Makeup Lab\n";
            cout << "7. Logout\n";
            cout << "Select: ";
            
            int ch; InputOutput::SafeReadInt(ch);
            
            if(ch==1) {
                cout << "\n--- ADD INFRASTRUCTURE ---\n";
                cout << "1. Add Building\n";
                cout << "2. Add Room\n";
                cout << "3. Add Teacher\n";
                cout << "4. Add Teaching Assistant\n";
                cout << "0. Back\n";
                cout << "Select: ";
                
                int sub; InputOutput::SafeReadInt(sub);
                if(sub==1) AddBuilding(v);
                else if(sub==2) AddRoom(v);
                else if(sub==3) AddTeacher(f);
                else if(sub==4) AddTA(f);
            }
            else if(ch==2) ScheduleSection(l, v, f);
            else if(ch==3) ViewCompleteLabDetails(l);
            else if(ch==4) ViewInfrastructure(v, f);
            else if(ch==5) ViewMakeupRequests(l);
            else if(ch==6) ScheduleMakeupLab(l, v, f);
            else return;
        }
    }
};

class Instructor : public Person
{
private:
    void SaveMakeupRequestToBinary(const MakeupLabRequest &request)
    {
        ofstream out("makeup_requests.dat", ios::binary | ios::app);
        if (out.is_open())
        {
            int labId = (request.GetLab() ? request.GetLab()->GetLabId() : -1);
            out.write(reinterpret_cast<const char *>(&labId), sizeof(int));
            string secName = request.GetSectionName(); int secLen = secName.length();
            out.write(reinterpret_cast<const char *>(&secLen), sizeof(int)); out.write(secName.c_str(), secLen);
            string date = request.GetRequestedDate(); int dateLen = date.length();
            out.write(reinterpret_cast<const char *>(&dateLen), sizeof(int)); out.write(date.c_str(), dateLen);
            string start = request.GetRequestedStartTime(); int startLen = start.length();
            out.write(reinterpret_cast<const char *>(&startLen), sizeof(int)); out.write(start.c_str(), startLen);
            string end = request.GetRequestedEndTime(); int endLen = end.length();
            out.write(reinterpret_cast<const char *>(&endLen), sizeof(int)); out.write(end.c_str(), endLen);
            out.close();
        }
    }

    void RequestMakeupLab(LabDetails *lDetails, int labId, const string &secName, const string &date, const string &s, const string &e)
    {
        MakeupLabRequest request(lDetails->FindLab(labId), secName, date, s, e);
        SaveMakeupRequestToBinary(request);
        cout << "Request Submitted.\n";
    }

public:
    Instructor() : Person("Instructor") {}
    void ShowMenu(LabDetails *lDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- INSTRUCTOR ---\n1. Request Makeup\n2. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1)
            {
                int id; string sec, date, s, e;
                CourseLaboratory* l = nullptr;
                ClassSection* section = nullptr;

                do {
                    cout << "Lab ID: "; InputOutput::SafeReadInt(id);
                    l = lDetails->FindLab(id);
                    if (!l) cout << "Error: Lab ID not found. Can only request makeup for scheduled labs.\n";
                } while (!l);

                do {
                    cout << "Section: "; InputOutput::SafeReadString(sec);
                    section = l->FindSection(sec);
                    if (!section) cout << "Error: Section not found in this Lab.\n";
                } while (!section);
                
                do { 
                    cout << "Date (YYYY-MM-DD): "; InputOutput::SafeReadString(date); 
                    if (!DataValidator::IsValidDate(date)) cout << "Invalid Date Format or Value (Use YYYY-MM-DD).\n";
                } while(!DataValidator::IsValidDate(date));

                while(true) {
                    cout << "Start (HH:MM): "; InputOutput::SafeReadString(s);
                    cout << "End (HH:MM): "; InputOutput::SafeReadString(e);
                    if (!DataValidator::IsValidTime(s) || !DataValidator::IsValidTime(e)) cout << "Invalid time format.\n";
                    else if (!DataValidator::IsStartBeforeEnd(s, e)) cout << "Start time must be strictly before End time.\n";
                    else break;
                }

                RequestMakeupLab(lDetails, id, sec, date, s, e);
            }
            else return;
        }
    }
};

class Attendant : public Person
{
private:
    bool IsLabSectionScheduled(LabDetails *lDetails, int labId, const string &secName)
    {
        CourseLaboratory *lab = lDetails->FindLab(labId);
        if (!lab) return false;
        ClassSection *sec = lab->FindSection(secName);
        return sec != nullptr;
    }

    void ViewScheduledLabs(LabDetails *lDetails)
    {
        cout << "\n--- SCHEDULED LABS ---\n";
        vector<CourseLaboratory> &labs = lDetails->GetAllLabs();
        if (labs.empty()) { cout << "No labs scheduled.\n"; return; }

        for (const auto &lab : labs)
        {
            cout << "\nCourse: " << lab.GetCourseCode() << " (ID: " << lab.GetLabId() << ")\n";
            for (const auto &sec : lab.GetSections())
            {
                cout << "  Sec: " << sec.GetSectionName() << " | Time: " << sec.GetScheduleTime().GetDate() << " " << sec.GetScheduleTime().GetStartTime() << "-" << sec.GetScheduleTime().GetEndTime() << "\n";
            }
        }
    }

    void FillTimeSheet(LabDetails *lDetails, WorkLogDetails *logDetails, int labId, const string &secName, const string &d, const string &s, const string &e, bool leave)
    {
        WorkLog entry;
        entry.SetLabId(labId); entry.SetSectionName(secName);
        entry.GetActualTiming().Set(d, s, e); entry.SetIsLeave(leave);
        logDetails->AddEntry(entry);
        cout << "Time Sheet Filled.\n";
    }

public:
    Attendant() : Person("Attendant") {}
    void ShowMenu(LabDetails *lDetails, WorkLogDetails *logDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- ATTENDANT ---\n1. View Labs\n2. Fill Time Sheet\n3. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1) ViewScheduledLabs(lDetails);
            else if (choice == 2)
            {
                int id, leave; string sec, d, s, e;
                CourseLaboratory* l = nullptr;
                ClassSection* section = nullptr;

                do {
                    cout << "Lab ID: "; InputOutput::SafeReadInt(id);
                    l = lDetails->FindLab(id);
                    if (!l) cout << "Error: Lab ID not found.\n";
                } while(!l);

                do {
                    cout << "Section: "; InputOutput::SafeReadString(sec);
                    section = l->FindSection(sec);
                    if (!section) cout << "Error: Section not found.\n";
                } while(!section);

                do { 
                    cout << "Date (YYYY-MM-DD): "; InputOutput::SafeReadString(d); 
                    if(!DataValidator::IsValidDate(d)) cout << "Invalid Date Format or Value.\n";
                } while(!DataValidator::IsValidDate(d));
                
                cout << "Leave? (1/0): "; InputOutput::SafeReadInt(leave);
                
                if (!leave) {
                    while(true) {
                        cout << "Start (HH:MM): "; InputOutput::SafeReadString(s);
                        cout << "End (HH:MM): "; InputOutput::SafeReadString(e);
                        if (!DataValidator::IsValidTime(s) || !DataValidator::IsValidTime(e)) cout << "Invalid time format.\n";
                        else if (!DataValidator::IsStartBeforeEnd(s, e)) cout << "Start time must be strictly before End time.\n";
                        else break;
                    }
                }
                FillTimeSheet(lDetails, logDetails, id, sec, d, s, e, leave);
            }
            else return;
        }
    }
};

class StorageManager
{
    LabDetails *labDetails;
    VenueDetails *venueDetails;
    FacultyDetails *facultyDetails;
    WorkLogDetails *logDetails;

    void WriteString(ofstream &out, const string &str)
    {
        int len = str.length();
        out.write(reinterpret_cast<const char *>(&len), sizeof(int));
        if (len > 0) out.write(str.c_str(), len);
    }

    string ReadString(ifstream &in)
    {
        int len;
        in.read(reinterpret_cast<char *>(&len), sizeof(int));
        if (len <= 0 || in.eof()) return "";
        char *buf = new char[len + 1];
        in.read(buf, len);
        buf[len] = '\0';
        string result(buf);
        delete[] buf;
        return result;
    }

public:
    StorageManager(LabDetails *l, VenueDetails *v, FacultyDetails *f, WorkLogDetails *w)
        : labDetails(l), venueDetails(v), facultyDetails(f), logDetails(w) {}

    void Save()
    {
        ofstream vOut("venue.dat", ios::binary);
        if (vOut.is_open()) {
            auto& bldgs = venueDetails->GetAllBuildings();
            int bCount = bldgs.size();
            vOut.write(reinterpret_cast<const char*>(&bCount), sizeof(int));
            for(auto& b : bldgs) {
                int id = b.GetId();
                vOut.write(reinterpret_cast<const char*>(&id), sizeof(int));
                WriteString(vOut, b.GetName());
            }

            auto& rooms = venueDetails->GetAllRooms();
            int rCount = rooms.size();
            vOut.write(reinterpret_cast<const char*>(&rCount), sizeof(int));
            for(auto& r : rooms) {
                int id = r.GetId();
                int bId = r.GetBuildingId(); 
                vOut.write(reinterpret_cast<const char*>(&id), sizeof(int));
                WriteString(vOut, r.GetRoomNumber());
                vOut.write(reinterpret_cast<const char*>(&bId), sizeof(int));
            }
            vOut.close();
        }

        ofstream fOut("faculty.dat", ios::binary);
        if(fOut.is_open()) {
            auto& teachers = facultyDetails->GetAllTeachers();
            int tCount = teachers.size();
            fOut.write(reinterpret_cast<const char*>(&tCount), sizeof(int));
            for(auto& t : teachers) {
                int id = t.GetId();
                fOut.write(reinterpret_cast<const char*>(&id), sizeof(int));
                WriteString(fOut, t.GetName());
            }

            auto& tas = facultyDetails->GetAllTAs();
            int taCount = tas.size();
            fOut.write(reinterpret_cast<const char*>(&taCount), sizeof(int));
            for(auto& t : tas) {
                int id = t.GetId();
                fOut.write(reinterpret_cast<const char*>(&id), sizeof(int));
                WriteString(fOut, t.GetName());
            }
            fOut.close();
        }

        ofstream sOut("schedule.dat", ios::binary);
        if(sOut.is_open()) {
            auto& labs = labDetails->GetAllLabs();
            int lCount = labs.size();
            sOut.write(reinterpret_cast<const char*>(&lCount), sizeof(int));
            for(auto& lab : labs) {
                int id = lab.GetLabId();
                sOut.write(reinterpret_cast<const char*>(&id), sizeof(int));
                WriteString(sOut, lab.GetCourseCode());

                auto& secs = lab.GetSections();
                int sCount = secs.size();
                sOut.write(reinterpret_cast<const char*>(&sCount), sizeof(int));
                for(auto& sec : secs) {
                    WriteString(sOut, sec.GetSectionName());
                    
                    int tId = (sec.GetTeacher() ? sec.GetTeacher()->GetId() : -1);
                    int bId = (sec.GetBuilding() ? sec.GetBuilding()->GetId() : -1);
                    int rId = (sec.GetRoom() ? sec.GetRoom()->GetId() : -1);
                    
                    sOut.write(reinterpret_cast<const char*>(&tId), sizeof(int));
                    sOut.write(reinterpret_cast<const char*>(&bId), sizeof(int));
                    sOut.write(reinterpret_cast<const char*>(&rId), sizeof(int));

                    WriteString(sOut, sec.GetScheduleTime().GetDate());
                    WriteString(sOut, sec.GetScheduleTime().GetStartTime());
                    WriteString(sOut, sec.GetScheduleTime().GetEndTime());

                    auto& tas = sec.GetAssistants();
                    int taCount = tas.size();
                    sOut.write(reinterpret_cast<const char*>(&taCount), sizeof(int));
                    for(auto* ta : tas) {
                        int taId = ta ? ta->GetId() : -1;
                        sOut.write(reinterpret_cast<const char*>(&taId), sizeof(int));
                    }
                }
            }
            sOut.close();
        }
        
        ofstream lOut("logs.dat", ios::binary);
        if(lOut.is_open()) {
             auto& logs = logDetails->GetAllEntries();
             int logCount = logs.size();
             lOut.write(reinterpret_cast<const char*>(&logCount), sizeof(int));
             for(auto& log : logs) {
                 int id = log.GetLabId();
                 lOut.write(reinterpret_cast<const char*>(&id), sizeof(int));
                 WriteString(lOut, log.GetSectionName());
                 bool leave = log.GetIsLeave();
                 lOut.write(reinterpret_cast<const char*>(&leave), sizeof(bool));
                 WriteString(lOut, log.GetActualTiming().GetDate());
                 WriteString(lOut, log.GetActualTiming().GetStartTime());
                 WriteString(lOut, log.GetActualTiming().GetEndTime());
             }
             lOut.close();
        }
        
        cout << "Data Saved Successfully.\n";
    }

    void Load()
    {
        ifstream vIn("venue.dat", ios::binary);
        if(vIn.is_open()) {
            int bCount; vIn.read(reinterpret_cast<char*>(&bCount), sizeof(int));
            for(int i=0; i<bCount && !vIn.eof(); i++) {
                int id; vIn.read(reinterpret_cast<char*>(&id), sizeof(int));
                string name = ReadString(vIn);
                venueDetails->AddBuilding(CampusBlock(id, name));
            }

            int rCount; vIn.read(reinterpret_cast<char*>(&rCount), sizeof(int));
            for(int i=0; i<rCount && !vIn.eof(); i++) {
                int id, bId;
                vIn.read(reinterpret_cast<char*>(&id), sizeof(int));
                string num = ReadString(vIn); 
                vIn.read(reinterpret_cast<char*>(&bId), sizeof(int));
                
                CampusBlock* b = venueDetails->FindBuilding(bId);
                venueDetails->AddRoom(LectureHall(id, num, bId, b));
            }
            vIn.close();
        }

        ifstream fIn("faculty.dat", ios::binary);
        if(fIn.is_open()) {
            int tCount; fIn.read(reinterpret_cast<char*>(&tCount), sizeof(int));
            for(int i=0; i<tCount && !fIn.eof(); i++) {
                int id; fIn.read(reinterpret_cast<char*>(&id), sizeof(int));
                string name = ReadString(fIn);
                facultyDetails->AddTeacher(UniversityTeacher(id, name));
            }
            int taCount; fIn.read(reinterpret_cast<char*>(&taCount), sizeof(int));
            for(int i=0; i<taCount && !fIn.eof(); i++) {
                int id; fIn.read(reinterpret_cast<char*>(&id), sizeof(int));
                string name = ReadString(fIn);
                facultyDetails->AddTA(TeachingAssistant(id, name));
            }
            fIn.close();
        }

        ifstream sIn("schedule.dat", ios::binary);
        if(sIn.is_open()) {
            int lCount; sIn.read(reinterpret_cast<char*>(&lCount), sizeof(int));
            for(int i=0; i<lCount && !sIn.eof(); i++) {
                CourseLaboratory lab;
                int id; sIn.read(reinterpret_cast<char*>(&id), sizeof(int));
                lab.SetLabId(id);
                lab.SetCourseCode(ReadString(sIn));

                int sCount; sIn.read(reinterpret_cast<char*>(&sCount), sizeof(int));
                for(int j=0; j<sCount; j++) {
                    ClassSection sec;
                    sec.SetSectionName(ReadString(sIn));
                    
                    int tId, bId, rId;
                    sIn.read(reinterpret_cast<char*>(&tId), sizeof(int));
                    sIn.read(reinterpret_cast<char*>(&bId), sizeof(int));
                    sIn.read(reinterpret_cast<char*>(&rId), sizeof(int));

                    sec.SetTeacher(facultyDetails->FindTeacher(tId));
                    sec.SetBuilding(venueDetails->FindBuilding(bId));
                    sec.SetRoom(venueDetails->FindRoom(rId));

                    string d = ReadString(sIn);
                    string s = ReadString(sIn);
                    string e = ReadString(sIn);
                    sec.GetScheduleTime().Set(d,s,e);

                    int taCount; sIn.read(reinterpret_cast<char*>(&taCount), sizeof(int));
                    for(int k=0; k<taCount; k++) {
                        int taId; sIn.read(reinterpret_cast<char*>(&taId), sizeof(int));
                        TeachingAssistant* ta = facultyDetails->FindTA(taId);
                        if(ta) sec.AddTA(ta);
                    }
                    lab.AddSection(sec);
                }
                labDetails->AddLab(lab);
            }
            sIn.close();
        }
        
        ifstream lIn("logs.dat", ios::binary);
        if(lIn.is_open()) {
            int count; lIn.read(reinterpret_cast<char*>(&count), sizeof(int));
            for(int i=0; i<count && !lIn.eof(); i++) {
                WorkLog log;
                int id; lIn.read(reinterpret_cast<char*>(&id), sizeof(int));
                log.SetLabId(id);
                log.SetSectionName(ReadString(lIn));
                bool leave; lIn.read(reinterpret_cast<char*>(&leave), sizeof(bool));
                log.SetIsLeave(leave);
                
                string d = ReadString(lIn);
                string s = ReadString(lIn);
                string e = ReadString(lIn);
                log.GetActualTiming().Set(d,s,e);
                logDetails->AddEntry(log);
            }
            lIn.close();
        }
        
        cout << "Data Loaded.\n";
    }
};

int main()
{
    InMemoryLabDetails labDetails;
    InMemoryVenueDetails venueDetails;
    InMemoryFacultyDetails facultyDetails;
    InMemoryWorkLogDetails logDetails;

    StorageManager storage(&labDetails, &venueDetails, &facultyDetails, &logDetails);
    storage.Load();

    HOD hod;
    AcademicOfficer officer;
    Instructor instructor;
    Attendant attendant;

    while (true)
    {
        cout << "\n--- UNIVERSITY SYSTEM ---\n";
        cout << "1. HOD\n2. Academic Officer (Manage Infrastructure & Schedule)\n3. Instructor\n4. Attendant\n5. Save & Exit\nSelect: ";
        int role;
        InputOutput::SafeReadInt(role);

        switch (role)
        {
        case 1: hod.ShowMenu(&labDetails, &logDetails); break;
        case 2: officer.ShowMenu(&labDetails, &venueDetails, &facultyDetails); break;
        case 3: instructor.ShowMenu(&labDetails); break;
        case 4: attendant.ShowMenu(&labDetails, &logDetails); break;
        case 5: storage.Save(); return 0;
        }
    }
}