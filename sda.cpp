#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <map>
#include <set>

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

class MakeupLabRequest
{
public:
    int labId;
    string sectionName;
    string requestedStartTime;
    string requestedEndTime;
    string requestedDate;

    MakeupLabRequest() : labId(0) {}
    MakeupLabRequest(int id, const string &sec, const string &date, const string &start, const string &end)
        : labId(id), sectionName(sec), requestedDate(date), requestedStartTime(start), requestedEndTime(end) {}
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

    // Helper function to extract day from date string (assuming format like "Monday", "Tuesday", etc.)
    string ExtractDay(const string &date)
    {
        // If date contains day name, extract it; otherwise return the date as-is
        vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        for (const string &day : days)
        {
            if (date.find(day) != string::npos)
                return day;
        }
        return date; // Return original if no day name found
    }

    // Helper function to check if a date falls within a given week
    bool IsDateInWeek(const string &date, const string &weekIdentifier)
    {
        // Simple implementation: check if week identifier is in the date string
        // This can be enhanced with proper date parsing
        return date.find(weekIdentifier) != string::npos || weekIdentifier == "all";
    }

    void GenerateCompleteWeeklySchedule(LabDetails *lDetails)
    {
        cout << "\n========================================\n";
        cout << "    COMPLETE LAB SCHEDULE - ENTIRE WEEK\n";
        cout << "========================================\n\n";
        
        vector<CourseLaboratory> &labs = lDetails->GetAllLabs();
        
        if (labs.empty())
        {
            cout << "No labs scheduled for the week.\n";
            cout << "\n========================================\n";
            return;
        }
        
        // Group sections by day
        map<string, vector<pair<CourseLaboratory*, ClassSection*>>> scheduleByDay;
        
        for (auto &lab : labs)
        {
            for (auto &sec : lab.sections)
            {
                string day = ExtractDay(sec.scheduleTime.date);
                scheduleByDay[day].push_back({&lab, &sec});
            }
        }
        
        // Define day order
        vector<string> dayOrder = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        
        // Display schedule organized by day
        for (const string &day : dayOrder)
        {
            if (scheduleByDay.find(day) != scheduleByDay.end())
            {
                cout << "\n=== " << day << " ===\n";
                cout << string(50, '-') << "\n";
                
                for (const auto &entry : scheduleByDay[day])
                {
                    CourseLaboratory *lab = entry.first;
                    ClassSection *sec = entry.second;
                    
                    cout << "\nLab ID: " << lab->labId << " | Course: " << lab->courseCode << "\n";
                    cout << "  Section: " << sec->sectionName << "\n";
                    cout << "  Time: " << sec->scheduleTime.startTime << " - " << sec->scheduleTime.endTime << "\n";
                    cout << "  Venue: " << (sec->building ? sec->building->name : "N/A")
                         << " - Room " << (sec->room ? to_string(sec->room->roomNumber) : "N/A") << "\n";
                    cout << "  Instructor: " << (sec->teacher ? sec->teacher->name : "Unassigned") << "\n";
                    cout << "  TAs: ";
                    if (sec->assistants.empty())
                    {
                        cout << "None assigned\n";
                    }
                    else
                    {
                        for (size_t i = 0; i < sec->assistants.size(); i++)
                        {
                            cout << (sec->assistants[i] ? sec->assistants[i]->name : "Unknown");
                            if (i < sec->assistants.size() - 1)
                                cout << ", ";
                        }
                        cout << "\n";
                    }
                }
            }
        }
        
        // Display any sections with unrecognized day format
        for (const auto &entry : scheduleByDay)
        {
            bool found = false;
            for (const string &day : dayOrder)
            {
                if (entry.first == day)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                cout << "\n=== " << entry.first << " ===\n";
                cout << string(50, '-') << "\n";
                
                for (const auto &pair : entry.second)
                {
                    CourseLaboratory *lab = pair.first;
                    ClassSection *sec = pair.second;
                    
                    cout << "\nLab ID: " << lab->labId << " | Course: " << lab->courseCode << "\n";
                    cout << "  Section: " << sec->sectionName << "\n";
                    cout << "  Time: " << sec->scheduleTime.startTime << " - " << sec->scheduleTime.endTime << "\n";
                    cout << "  Venue: " << (sec->building ? sec->building->name : "N/A")
                         << " - Room " << (sec->room ? to_string(sec->room->roomNumber) : "N/A") << "\n";
                    cout << "  Instructor: " << (sec->teacher ? sec->teacher->name : "Unassigned") << "\n";
                }
            }
        }
        
        cout << "\n========================================\n";
    }

    void GenerateWeeklyTimeSheetReport(LabDetails *lDetails, WorkLogDetails *logDetails)
    {
        cout << "\n========================================\n";
        cout << "    FILLED TIME SHEETS - WEEKLY REPORT\n";
        cout << "========================================\n\n";
        
        string weekInput;
        cout << "Enter week identifier (e.g., 'Week1', 'Week2', or 'all' for all weeks): ";
        InputOutput::SafeReadString(weekInput);
        
        vector<WorkLog> &logs = logDetails->GetAllEntries();
        vector<WorkLog> filteredLogs;
        
        // Filter logs for the specified week
        for (const auto &log : logs)
        {
            if (IsDateInWeek(log.actualTiming.date, weekInput))
            {
                filteredLogs.push_back(log);
            }
        }
        
        if (filteredLogs.empty())
        {
            cout << "No time sheet entries found for " << weekInput << ".\n";
            cout << "\n========================================\n";
            return;
        }
        
        cout << "\nTime Sheet Entries for " << weekInput << ":\n";
        cout << string(80, '-') << "\n";
        cout << left << setw(8) << "Lab ID" << setw(15) << "Course Code" << setw(12) << "Section" 
             << setw(15) << "Date" << setw(12) << "Start Time" << setw(12) << "End Time" 
             << setw(10) << "Status" << setw(15) << "Instructor" << endl;
        cout << string(80, '-') << "\n";
        
        for (const auto &log : filteredLogs)
        {
            CourseLaboratory *lab = lDetails->FindLab(log.labId);
            string courseCode = lab ? lab->courseCode : "N/A";
            
            ClassSection *sec = nullptr;
            string instructorName = "N/A";
            if (lab)
            {
                sec = lab->FindSection(log.sectionName);
                if (sec && sec->teacher)
                {
                    instructorName = sec->teacher->name;
                }
            }
            
            cout << left << setw(8) << log.labId 
                 << setw(15) << courseCode
                 << setw(12) << log.sectionName
                 << setw(15) << log.actualTiming.date
                 << setw(12) << (log.isLeave ? "N/A" : log.actualTiming.startTime)
                 << setw(12) << (log.isLeave ? "N/A" : log.actualTiming.endTime)
                 << setw(10) << (log.isLeave ? "LEAVE" : "PRESENT")
                 << setw(15) << instructorName << endl;
        }
        
        // Summary statistics
        int presentCount = 0;
        int leaveCount = 0;
        float totalHours = 0.0f;
        
        for (const auto &log : filteredLogs)
        {
            if (log.isLeave)
                leaveCount++;
            else
            {
                presentCount++;
                totalHours += CalculateHours(log.actualTiming.startTime, log.actualTiming.endTime);
            }
        }
        
        cout << "\n" << string(80, '-') << "\n";
        cout << "SUMMARY:\n";
        cout << "  Total Entries: " << filteredLogs.size() << "\n";
        cout << "  Present: " << presentCount << "\n";
        cout << "  Leaves: " << leaveCount << "\n";
        cout << "  Total Contact Hours: " << fixed << setprecision(2) << totalHours << " hours\n";
        cout << "\n========================================\n";
    }

    void GenerateLabSpecificTimeSheet(LabDetails *lDetails, WorkLogDetails *logDetails)
    {
        cout << "\n========================================\n";
        cout << "    LAB-SPECIFIC TIME SHEET REPORT\n";
        cout << "========================================\n\n";
        
        int labId;
        cout << "Enter Lab ID: ";
        InputOutput::SafeReadInt(labId);
        
        CourseLaboratory *lab = lDetails->FindLab(labId);
        if (!lab)
        {
            cout << "Error: Lab ID " << labId << " not found.\n";
            cout << "\n========================================\n";
            return;
        }
        
        cout << "\nLab ID: " << lab->labId << " | Course: " << lab->courseCode << "\n";
        cout << string(80, '-') << "\n\n";
        
        vector<WorkLog> &logs = logDetails->GetAllEntries();
        vector<WorkLog> labLogs;
        
        // Filter logs for this specific lab
        for (const auto &log : logs)
        {
            if (log.labId == labId)
            {
                labLogs.push_back(log);
            }
        }
        
        if (labLogs.empty())
        {
            cout << "No time sheet entries found for this lab.\n";
            cout << "\n========================================\n";
            return;
        }
        
        // Display all sessions for this lab
        cout << "ALL SESSIONS TIME SHEET:\n";
        cout << string(80, '-') << "\n";
        cout << left << setw(12) << "Section" << setw(15) << "Date" 
             << setw(12) << "Start Time" << setw(12) << "End Time" 
             << setw(10) << "Status" << setw(15) << "Instructor" << endl;
        cout << string(80, '-') << "\n";
        
        float totalContactHours = 0.0f;
        int totalLeaves = 0;
        int totalPresent = 0;
        map<string, int> sectionLeaves; // section -> leave count
        map<string, float> sectionHours; // section -> total hours
        
        for (const auto &log : labLogs)
        {
            ClassSection *sec = lab->FindSection(log.sectionName);
            string instructorName = "N/A";
            if (sec && sec->teacher)
            {
                instructorName = sec->teacher->name;
            }
            
            float hours = 0.0f;
            if (!log.isLeave)
            {
                hours = CalculateHours(log.actualTiming.startTime, log.actualTiming.endTime);
                totalContactHours += hours;
                totalPresent++;
                sectionHours[log.sectionName] += hours;
            }
            else
            {
                totalLeaves++;
                sectionLeaves[log.sectionName]++;
            }
            
            cout << left << setw(12) << log.sectionName
                 << setw(15) << log.actualTiming.date
                 << setw(12) << (log.isLeave ? "N/A" : log.actualTiming.startTime)
                 << setw(12) << (log.isLeave ? "N/A" : log.actualTiming.endTime)
                 << setw(10) << (log.isLeave ? "LEAVE" : "PRESENT")
                 << setw(15) << instructorName;
            if (!log.isLeave)
                cout << " (" << fixed << setprecision(2) << hours << " hrs)";
            cout << endl;
        }
        
        // Summary by section
        cout << "\n" << string(80, '-') << "\n";
        cout << "SECTION-WISE SUMMARY:\n";
        cout << string(80, '-') << "\n";
        cout << left << setw(12) << "Section" << setw(15) << "Total Hours" 
             << setw(12) << "Present" << setw(12) << "Leaves" << endl;
        cout << string(80, '-') << "\n";
        
        set<string> allSections;
        for (const auto &log : labLogs)
        {
            allSections.insert(log.sectionName);
        }
        
        for (const string &secName : allSections)
        {
            int presentCount = 0;
            for (const auto &log : labLogs)
            {
                if (log.sectionName == secName && !log.isLeave)
                    presentCount++;
            }
            
            cout << left << setw(12) << secName
                 << setw(15) << fixed << setprecision(2) << sectionHours[secName]
                 << setw(12) << presentCount
                 << setw(12) << sectionLeaves[secName] << endl;
        }
        
        // Overall summary
        cout << "\n" << string(80, '-') << "\n";
        cout << "OVERALL SUMMARY FOR LAB " << labId << " (" << lab->courseCode << "):\n";
        cout << string(80, '-') << "\n";
        cout << "  Total Sessions: " << labLogs.size() << "\n";
        cout << "  Present Sessions: " << totalPresent << "\n";
        cout << "  Leave Sessions: " << totalLeaves << "\n";
        cout << "  Total Contact Hours: " << fixed << setprecision(2) << totalContactHours << " hours\n";
        cout << "  Average Hours per Session: " << fixed << setprecision(2) 
             << (totalPresent > 0 ? totalContactHours / totalPresent : 0.0f) << " hours\n";
        cout << "\n========================================\n";
    }

public:
    HOD() : Person("Head of Department") {}

    void ShowMenu(LabDetails *lDetails, WorkLogDetails *wDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- HOD DASHBOARD ---\n";
            cout << "1. Complete Lab Schedule for Entire Week\n";
            cout << "2. Filled Time Sheet for All Labs in a Given Week\n";
            cout << "3. Time Sheet for All Sessions of a Particular Lab\n";
            cout << "4. Logout\n";
            cout << "Select: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1)
                GenerateCompleteWeeklySchedule(lDetails);
            else if (choice == 2)
                GenerateWeeklyTimeSheetReport(lDetails, wDetails);
            else if (choice == 3)
                GenerateLabSpecificTimeSheet(lDetails, wDetails);
            else
                return;
        }
    }
};

class AcademicOfficer : public Person
{
private:
    vector<MakeupLabRequest> ReadMakeupRequestsFromBinary()
    {
        vector<MakeupLabRequest> requests;
        ifstream in("makeup_requests.dat", ios::binary);
        
        if (!in.is_open())
        {
            return requests; // Return empty vector if file doesn't exist
        }

        while (in.peek() != EOF)
        {
            MakeupLabRequest request;
            
            // Read labId
            in.read(reinterpret_cast<char *>(&request.labId), sizeof(int));
            if (in.eof())
                break;
            
            // Read sectionName
            int secLen;
            in.read(reinterpret_cast<char *>(&secLen), sizeof(int));
            if (in.eof())
                break;
            char *secBuf = new char[secLen + 1];
            in.read(secBuf, secLen);
            secBuf[secLen] = '\0';
            request.sectionName = string(secBuf);
            delete[] secBuf;
            
            // Read requestedDate
            int dateLen;
            in.read(reinterpret_cast<char *>(&dateLen), sizeof(int));
            if (in.eof())
                break;
            char *dateBuf = new char[dateLen + 1];
            in.read(dateBuf, dateLen);
            dateBuf[dateLen] = '\0';
            request.requestedDate = string(dateBuf);
            delete[] dateBuf;
            
            // Read requestedStartTime
            int startLen;
            in.read(reinterpret_cast<char *>(&startLen), sizeof(int));
            if (in.eof())
                break;
            char *startBuf = new char[startLen + 1];
            in.read(startBuf, startLen);
            startBuf[startLen] = '\0';
            request.requestedStartTime = string(startBuf);
            delete[] startBuf;
            
            // Read requestedEndTime
            int endLen;
            in.read(reinterpret_cast<char *>(&endLen), sizeof(int));
            if (in.eof())
                break;
            char *endBuf = new char[endLen + 1];
            in.read(endBuf, endLen);
            endBuf[endLen] = '\0';
            request.requestedEndTime = string(endBuf);
            delete[] endBuf;
            
            requests.push_back(request);
        }
        
        in.close();
        return requests;
    }

    void ViewMakeupRequests()
    {
        vector<MakeupLabRequest> requests = ReadMakeupRequestsFromBinary();
        
        if (requests.empty())
        {
            cout << "\nNo makeup lab requests found.\n";
            return;
        }
        
        cout << "\n--- MAKEUP LAB REQUESTS ---\n";
        cout << left << setw(8) << "Lab ID" << setw(12) << "Section" << setw(15) << "Date" 
             << setw(12) << "Start Time" << setw(12) << "End Time" << endl;
        cout << string(60, '-') << endl;
        
        for (const auto &req : requests)
        {
            cout << left << setw(8) << req.labId << setw(12) << req.sectionName 
                 << setw(15) << req.requestedDate << setw(12) << req.requestedStartTime 
                 << setw(12) << req.requestedEndTime << endl;
        }
    }

    void ScheduleMakeupLab(LabDetails *lDetails, VenueDetails *vDetails, FacultyDetails *fDetails)
    {
        vector<MakeupLabRequest> requests = ReadMakeupRequestsFromBinary();
        
        if (requests.empty())
        {
            cout << "\nNo makeup lab requests found.\n";
            return;
        }
        
        cout << "\n--- AVAILABLE MAKEUP REQUESTS ---\n";
        for (size_t i = 0; i < requests.size(); i++)
        {
            cout << (i + 1) << ". Lab ID: " << requests[i].labId 
                 << ", Section: " << requests[i].sectionName 
                 << ", Date: " << requests[i].requestedDate
                 << ", Time: " << requests[i].requestedStartTime 
                 << "-" << requests[i].requestedEndTime << endl;
        }
        
        int choice;
        cout << "\nSelect request number to schedule (0 to cancel): ";
        InputOutput::SafeReadInt(choice);
        
        if (choice < 1 || choice > (int)requests.size())
        {
            cout << "Invalid selection.\n";
            return;
        }
        
        MakeupLabRequest selected = requests[choice - 1];
        
        // Get scheduling details
        int teacherId, bId, rId, taCount;
        string day;
        
        cout << "Instructor/Teacher ID (1 or 2): ";
        InputOutput::SafeReadInt(teacherId);
        cout << "Building ID (1 or 2): ";
        InputOutput::SafeReadInt(bId);
        cout << "Room ID (101 or 102): ";
        InputOutput::SafeReadInt(rId);
        cout << "Day: ";
        InputOutput::SafeReadString(day);
        
        UniversityTeacher *teach = fDetails->FindTeacher(teacherId);
        CampusBlock *build = vDetails->FindBuilding(bId);
        LectureHall *room = vDetails->FindRoom(rId);
        
        if (!teach || !build || !room)
        {
            cout << "Error: Invalid Teacher, Building, or Room ID.\n";
            return;
        }
        
        // Create new section for makeup lab
        ClassSection makeupSec;
        makeupSec.SetDetails(selected.sectionName + "_MAKEUP", teach, build, room);
        makeupSec.scheduleTime.Set(selected.requestedDate, selected.requestedStartTime, selected.requestedEndTime);
        
        cout << "How many TAs? ";
        InputOutput::SafeReadInt(taCount);
        for (int i = 0; i < taCount; i++)
        {
            int taId;
            cout << "TA ID (101-103): ";
            InputOutput::SafeReadInt(taId);
            TeachingAssistant *ta = fDetails->FindTA(taId);
            if (ta)
                makeupSec.AddTA(ta);
        }
        
        CourseLaboratory *lab = lDetails->FindLab(selected.labId);
        if (lab)
        {
            lab->AddSection(makeupSec);
            lDetails->UpdateLab(*lab);
        }
        else
        {
            cout << "Error: Original lab not found.\n";
            return;
        }
        
        cout << "Makeup lab scheduled successfully.\n";
    }

    void ViewCompleteLabDetails(LabDetails *lDetails, VenueDetails *vDetails, FacultyDetails *fDetails)
    {
        cout << "\n--- COMPLETE LAB SCHEDULING INFORMATION ---\n\n";
        
        // Display all scheduled labs
        cout << "=== SCHEDULED LABS ===\n";
        vector<CourseLaboratory> &labs = lDetails->GetAllLabs();
        if (labs.empty())
        {
            cout << "No labs scheduled yet.\n";
        }
        else
        {
            for (const auto &lab : labs)
            {
                cout << "\nLab ID: " << lab.labId << " | Course Code: " << lab.courseCode << "\n";
                cout << "  Sections: ";
                for (size_t i = 0; i < lab.sections.size(); i++)
                {
                    cout << lab.sections[i].sectionName;
                    if (i < lab.sections.size() - 1)
                        cout << ", ";
                }
                cout << "\n";
            }
        }
        
        // Display all available instructors
        cout << "\n=== AVAILABLE INSTRUCTORS ===\n";
        // We need to access the faculty details - let's create a method to get all teachers
        // For now, we'll show the seeded data structure
        cout << "ID: 1 - Dr. Smith\n";
        cout << "ID: 2 - Dr. Jones\n";
        cout << "(Note: Additional instructors can be added to the system)\n";
        
        // Display all available TAs
        cout << "\n=== AVAILABLE TEACHING ASSISTANTS ===\n";
        cout << "ID: 101 - Alice\n";
        cout << "ID: 102 - Bob\n";
        cout << "ID: 103 - Charlie\n";
        cout << "(Note: Additional TAs can be added to the system)\n";
        
        // Display all available venues
        cout << "\n=== AVAILABLE BUILDINGS ===\n";
        cout << "ID: 1 - CS Block\n";
        cout << "ID: 2 - EE Block\n";
        
        cout << "\n=== AVAILABLE ROOMS ===\n";
        cout << "ID: 101 - Room 101\n";
        cout << "ID: 102 - Room 102\n";
        
        // Display detailed section information
        cout << "\n=== DETAILED SECTION INFORMATION ===\n";
        if (labs.empty())
        {
            cout << "No sections scheduled.\n";
        }
        else
        {
            for (const auto &lab : labs)
            {
                for (const auto &sec : lab.sections)
                {
                    cout << "\nLab ID: " << lab.labId << " | Section: " << sec.sectionName << "\n";
                    cout << "  Course: " << lab.courseCode << "\n";
                    cout << "  Schedule: " << sec.scheduleTime.date 
                         << " | " << sec.scheduleTime.startTime 
                         << " - " << sec.scheduleTime.endTime << "\n";
                    cout << "  Instructor: " << (sec.teacher ? sec.teacher->name + " (ID: " + to_string(sec.teacher->teacherId) + ")" : "Unassigned") << "\n";
                    cout << "  Building: " << (sec.building ? sec.building->name + " (ID: " + to_string(sec.building->buildingId) + ")" : "N/A") << "\n";
                    cout << "  Room: " << (sec.room ? "Room " + to_string(sec.room->roomNumber) + " (ID: " + to_string(sec.room->roomId) + ")" : "N/A") << "\n";
                    cout << "  TAs: ";
                    if (sec.assistants.empty())
                    {
                        cout << "None assigned\n";
                    }
                    else
                    {
                        for (size_t i = 0; i < sec.assistants.size(); i++)
                        {
                            if (sec.assistants[i])
                                cout << sec.assistants[i]->name << " (ID: " << sec.assistants[i]->taId << ")";
                            if (i < sec.assistants.size() - 1)
                                cout << ", ";
                        }
                        cout << "\n";
                    }
                }
            }
        }
        cout << "\n========================================\n";
    }

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
            cout << "\n--- ACADEMIC OFFICER ---\n1. View Complete Lab Details\n2. Schedule Section\n3. View Makeup Requests\n4. Schedule Makeup Lab\n5. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1)
                ViewCompleteLabDetails(lDetails, vDetails, fDetails);
            else if (choice == 2)
                ScheduleSection(lDetails, vDetails, fDetails);
            else if (choice == 3)
                ViewMakeupRequests();
            else if (choice == 4)
                ScheduleMakeupLab(lDetails, vDetails, fDetails);
            else
                return;
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
            // Write labId
            out.write(reinterpret_cast<const char *>(&request.labId), sizeof(int));
            
            // Write sectionName length and string
            int secLen = request.sectionName.length();
            out.write(reinterpret_cast<const char *>(&secLen), sizeof(int));
            out.write(request.sectionName.c_str(), secLen);
            
            // Write requestedDate length and string
            int dateLen = request.requestedDate.length();
            out.write(reinterpret_cast<const char *>(&dateLen), sizeof(int));
            out.write(request.requestedDate.c_str(), dateLen);
            
            // Write requestedStartTime length and string
            int startLen = request.requestedStartTime.length();
            out.write(reinterpret_cast<const char *>(&startLen), sizeof(int));
            out.write(request.requestedStartTime.c_str(), startLen);
            
            // Write requestedEndTime length and string
            int endLen = request.requestedEndTime.length();
            out.write(reinterpret_cast<const char *>(&endLen), sizeof(int));
            out.write(request.requestedEndTime.c_str(), endLen);
            
            out.close();
        }
    }

    void RequestMakeupLab(LabDetails *lDetails, int labId, const string &secName, const string &date, const string &s, const string &e)
    {
        // Check if the lab and section are scheduled
        CourseLaboratory *l = lDetails->FindLab(labId);
        if (!l)
        {
            cout << "Error: Lab ID " << labId << " not found. Makeup requests can only be made for scheduled labs.\n";
            return;
        }
        
        ClassSection *sec = l->FindSection(secName);
        if (!sec)
        {
            cout << "Error: Section " << secName << " not found for Lab ID " << labId << ". Makeup requests can only be made for scheduled sections.\n";
            return;
        }

        // Create and save makeup request to binary file
        MakeupLabRequest request(labId, secName, date, s, e);
        SaveMakeupRequestToBinary(request);
        cout << "Makeup Lab Request submitted successfully.\n";
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
                string sec, date, s, e;
                cout << "Lab ID: ";
                InputOutput::SafeReadInt(id);
                cout << "Section: ";
                InputOutput::SafeReadString(sec);
                cout << "Requested Date: ";
                InputOutput::SafeReadString(date);
                cout << "Requested Start Time (HH:MM): ";
                InputOutput::SafeReadString(s);
                cout << "Requested End Time (HH:MM): ";
                InputOutput::SafeReadString(e);
                RequestMakeupLab(lDetails, id, sec, date, s, e);
            }
            else
                return;
        }
    }
};

class Attendant : public Person
{
private:
    bool IsLabSectionScheduled(LabDetails *lDetails, int labId, const string &secName)
    {
        CourseLaboratory *lab = lDetails->FindLab(labId);
        if (!lab)
            return false;
        ClassSection *sec = lab->FindSection(secName);
        return sec != nullptr;
    }

    void ViewScheduledLabs(LabDetails *lDetails)
    {
        cout << "\n--- SCHEDULED LABS DETAILS ---\n";
        vector<CourseLaboratory> &labs = lDetails->GetAllLabs();
        
        if (labs.empty())
        {
            cout << "No labs have been scheduled yet.\n";
            return;
        }

        for (const auto &lab : labs)
        {
            cout << "\n========================================\n";
            cout << "COURSE: " << lab.courseCode << " (Lab ID: " << lab.labId << ")\n";
            cout << "========================================\n";
            
            for (const auto &sec : lab.sections)
            {
                cout << "\n  Section: " << sec.sectionName << "\n";
                cout << "  Schedule: " << sec.scheduleTime.date 
                     << " | " << sec.scheduleTime.startTime 
                     << " - " << sec.scheduleTime.endTime << "\n";
                cout << "  Venue: " << (sec.building ? sec.building->name : "N/A")
                     << " - Room " << (sec.room ? to_string(sec.room->roomNumber) : "N/A") << "\n";
                cout << "  Instructor: " << (sec.teacher ? sec.teacher->name : "Unassigned") << "\n";
                cout << "  Teaching Assistants: ";
                if (sec.assistants.empty())
                {
                    cout << "None assigned\n";
                }
                else
                {
                    for (size_t i = 0; i < sec.assistants.size(); i++)
                    {
                        cout << (sec.assistants[i] ? sec.assistants[i]->name : "Unknown");
                        if (i < sec.assistants.size() - 1)
                            cout << ", ";
                    }
                    cout << "\n";
                }
            }
        }
        cout << "\n========================================\n";
    }

    void FillTimeSheet(LabDetails *lDetails, WorkLogDetails *logDetails, int labId, const string &secName, const string &d, const string &s, const string &e, bool leave)
    {
        // Check if the lab and section are scheduled
        if (!IsLabSectionScheduled(lDetails, labId, secName))
        {
            cout << "Error: This lab ID and section combination has not been scheduled by Academic Officer.\n";
            return;
        }

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

    void ShowMenu(LabDetails *lDetails, WorkLogDetails *logDetails)
    {
        int choice;
        while (true)
        {
            cout << "\n--- ATTENDANT DASHBOARD ---\n1. View Scheduled Labs\n2. Fill Time Sheet\n3. Logout\nSelect: ";
            InputOutput::SafeReadInt(choice);
            if (choice == 1)
                ViewScheduledLabs(lDetails);
            else if (choice == 2)
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
                FillTimeSheet(lDetails, logDetails, id, sec, d, s, e, leave);
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

private:
    // Helper function to write string to binary file
    void WriteString(ofstream &out, const string &str)
    {
        int len = str.length();
        out.write(reinterpret_cast<const char *>(&len), sizeof(int));
        if (len > 0)
        {
            out.write(str.c_str(), len);
        }
    }

    // Helper function to read string from binary file
    string ReadString(ifstream &in)
    {
        int len;
        in.read(reinterpret_cast<char *>(&len), sizeof(int));
        if (len <= 0 || in.eof())
            return "";
        
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
        ofstream out("labs_data.dat", ios::binary);
        if (!out.is_open())
        {
            cout << "[System] Error: Could not open labs_data.dat for writing.\n";
            return;
        }

        vector<CourseLaboratory> &labs = labDetails->GetAllLabs();
        
        // Write number of labs
        int labCount = labs.size();
        out.write(reinterpret_cast<const char *>(&labCount), sizeof(int));

        for (const auto &lab : labs)
        {
            // Write lab ID
            out.write(reinterpret_cast<const char *>(&lab.labId), sizeof(int));
            
            // Write course code
            WriteString(out, lab.courseCode);
            
            // Write number of sections
            int secCount = lab.sections.size();
            out.write(reinterpret_cast<const char *>(&secCount), sizeof(int));

            for (const auto &sec : lab.sections)
            {
                // Write section name
                WriteString(out, sec.sectionName);
                
                // Write teacher ID
                int teachId = (sec.teacher ? sec.teacher->teacherId : -1);
                out.write(reinterpret_cast<const char *>(&teachId), sizeof(int));
                
                // Write building ID
                int buildId = (sec.building ? sec.building->buildingId : -1);
                out.write(reinterpret_cast<const char *>(&buildId), sizeof(int));
                
                // Write room ID
                int roomId = (sec.room ? sec.room->roomId : -1);
                out.write(reinterpret_cast<const char *>(&roomId), sizeof(int));
                
                // Write schedule time
                WriteString(out, sec.scheduleTime.date);
                WriteString(out, sec.scheduleTime.startTime);
                WriteString(out, sec.scheduleTime.endTime);

                // Write number of TAs
                int taCount = sec.assistants.size();
                out.write(reinterpret_cast<const char *>(&taCount), sizeof(int));
                
                // Write TA IDs
                for (auto ta : sec.assistants)
                {
                    int taId = (ta ? ta->taId : -1);
                    out.write(reinterpret_cast<const char *>(&taId), sizeof(int));
                }
            }
        }
        out.close();

        // Save Work Logs
        ofstream wout("logs_data.dat", ios::binary);
        if (!wout.is_open())
        {
            cout << "[System] Error: Could not open logs_data.dat for writing.\n";
            return;
        }

        vector<WorkLog> &logs = logDetails->GetAllEntries();
        
        // Write number of logs
        int logCount = logs.size();
        wout.write(reinterpret_cast<const char *>(&logCount), sizeof(int));
        
        for (const auto &log : logs)
        {
            // Write lab ID
            wout.write(reinterpret_cast<const char *>(&log.labId), sizeof(int));
            
            // Write section name
            WriteString(wout, log.sectionName);
            
            // Write isLeave flag
            wout.write(reinterpret_cast<const char *>(&log.isLeave), sizeof(bool));
            
            // Write timing information
            WriteString(wout, log.actualTiming.date);
            WriteString(wout, log.actualTiming.startTime);
            WriteString(wout, log.actualTiming.endTime);
        }
        wout.close();

        cout << "[System] Data saved to binary files.\n";
    }

    void Load()
    {
        ifstream in("labs_data.dat", ios::binary);
        if (in.is_open())
        {
            int labCount;
            in.read(reinterpret_cast<char *>(&labCount), sizeof(int));
            
            for (int i = 0; i < labCount && !in.eof(); i++)
            {
                CourseLaboratory lab;
                int secCount;
                
                // Read lab ID
                in.read(reinterpret_cast<char *>(&lab.labId), sizeof(int));
                if (in.eof())
                    break;
                
                // Read course code
                lab.courseCode = ReadString(in);
                if (in.eof())
                    break;
                
                // Read number of sections
                in.read(reinterpret_cast<char *>(&secCount), sizeof(int));
                if (in.eof())
                    break;

                for (int j = 0; j < secCount && !in.eof(); j++)
                {
                    ClassSection sec;
                    int teachId, buildId, roomId, taCount;

                    // Read section name
                    sec.sectionName = ReadString(in);
                    if (in.eof())
                        break;
                    
                    // Read teacher, building, room IDs
                    in.read(reinterpret_cast<char *>(&teachId), sizeof(int));
                    in.read(reinterpret_cast<char *>(&buildId), sizeof(int));
                    in.read(reinterpret_cast<char *>(&roomId), sizeof(int));
                    if (in.eof())
                        break;
                    
                    // Read schedule time
                    sec.scheduleTime.date = ReadString(in);
                    sec.scheduleTime.startTime = ReadString(in);
                    sec.scheduleTime.endTime = ReadString(in);
                    if (in.eof())
                        break;

                    // Set pointers
                    sec.teacher = facultyDetails->FindTeacher(teachId);
                    sec.building = venueDetails->FindBuilding(buildId);
                    sec.room = venueDetails->FindRoom(roomId);

                    // Read number of TAs
                    in.read(reinterpret_cast<char *>(&taCount), sizeof(int));
                    if (in.eof())
                        break;
                    
                    // Read TA IDs
                    for (int k = 0; k < taCount && !in.eof(); k++)
                    {
                        int taId;
                        in.read(reinterpret_cast<char *>(&taId), sizeof(int));
                        if (in.eof())
                            break;
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

        ifstream win("logs_data.dat", ios::binary);
        if (win.is_open())
        {
            int logCount;
            win.read(reinterpret_cast<char *>(&logCount), sizeof(int));
            
            for (int i = 0; i < logCount && !win.eof(); i++)
            {
                WorkLog log;
                
                // Read lab ID
                win.read(reinterpret_cast<char *>(&log.labId), sizeof(int));
                if (win.eof())
                    break;
                
                // Read section name
                log.sectionName = ReadString(win);
                if (win.eof())
                    break;
                
                // Read isLeave flag
                win.read(reinterpret_cast<char *>(&log.isLeave), sizeof(bool));
                if (win.eof())
                    break;
                
                // Read timing information
                log.actualTiming.date = ReadString(win);
                log.actualTiming.startTime = ReadString(win);
                log.actualTiming.endTime = ReadString(win);
                if (win.eof())
                    break;
                
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
            attendant.ShowMenu(&labDetails, &logDetails);
            break;
        case 5:
            storage.Save();
            return 0;
        }
    }
}