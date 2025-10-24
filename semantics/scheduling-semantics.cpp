#include <iostream>
#include <vector>
#include "../settings/settings.h"
using namespace std;

vector<int> requestUserVariables() {
    vector<int> userVariables;

    // Request amount of days
    int days;
    cout << "How many days can be used for scheduling?\n";
    cin >> days;
    userVariables.push_back(days);
    
    // Request amount of hours
    int hours;
    cout << "How many hours are available in a day?\n";
    cin >> hours;
    userVariables.push_back(hours);
    
    // Request amount of classrooms
    int classRooms;
    cout << "How many class rooms are available?\n";
    cin >> classRooms;
    userVariables.push_back(classRooms);
    
    // Request amount of courses
    int courses;
    cout << "How many courses must be scheduled?\n";
    cin >> courses;
    userVariables.push_back(courses);
    
    // Request amount of course hours
    int courseHours;
    cout << "How many hours must be scheduled per course?\n";
    cin >> courseHours;
    userVariables.push_back(courseHours);

    return userVariables;
}

void enterSchedulingGenerator(){

    vector<int> userVariables = requestUserVariables();
    int days = userVariables[0];
    int hours = userVariables[1];
    int classRooms = userVariables[2];
    int courses = userVariables[3];
    int courseHours = userVariables[4];

    int timeSlots = days * hours;
    int totalCourseHours = courses * courseHours;

    cout << "Creating clauses for " << courseHours << " course hours with " << timeSlots << " available timeslots and " << classRooms << " classrooms.\n";
    
    
    //

    /* 
    Literals indicate whether a class is held in a specified room in a specified timeslot
    Constraints:
    1. Two classes can't have the same room
    2. Every class must have a classroom
    */

    cout << "Creating clauses for " << courseHours << " hours per course ("
         << totalCourseHours << " course-hours total) with "
         << timeSlots << " available timeslots and "
         << classRooms << " classrooms.\n";

    // Create 3D vector of literals:
    vector<vector<vector<int>>> literals;
    literals.resize((size_t)totalCourseHours);
    int literalCounter = 1;
    for (long long c = 0; c < totalCourseHours; c++) {
        literals[(size_t)c].resize((size_t)classRooms);
        for (int r = 0; r < classRooms; r++) {
            literals[(size_t)c][r].resize((size_t)timeSlots);
            for (int t = 0; t < timeSlots; t++) {
                if (debug) {
                    cout << "[DEBUG] Creating literal " << literalCounter
                         << " for courseHour=" << c << " room=" << r << " timeslot=" << t << "\n";
                }
                literals[(size_t)c][r][t] = literalCounter++;
            }
        }
    }

    long long vars = (long long)literalCounter - 1;
    if (debug) cout << "[DEBUG] Total variables (literals) = " << vars << "\n";

    // Clause vectors
    vector<vector<int>> mustHaveRoomClauses;
    vector<vector<int>> atMostOneClauses;
    vector<vector<int>> roomConflictClauses;

    // Build must-have clauses: for each course-hour, at least one (room,timeslot)
    mustHaveRoomClauses.reserve((size_t)totalCourseHours);
    for (long long c = 0; c < totalCourseHours; c++) {
        vector<int> clause;
        clause.reserve((size_t)classRooms * (size_t)timeSlots);
        for (int r = 0; r < classRooms; r++) {
            for (int t = 0; t < timeSlots; t++) {
                clause.push_back(literals[(size_t)c][r][t]); // positive literal
            }
        }
        mustHaveRoomClauses.push_back(std::move(clause));
    }

    // Build at-most-one clauses: for each course-hour, pairwise negative literals
    // (i.e., a single course-hour cannot occupy two different room-timeslot literals)
    atMostOneClauses.reserve((size_t)totalCourseHours * 10); // soft reserve; may grow more
    for (long long c = 0; c < totalCourseHours; c++) {
        // flatten this course-hour's literals to a vector to produce pairs
        vector<int> flat;
        flat.reserve((size_t)classRooms * (size_t)timeSlots);
        for (int r = 0; r < classRooms; r++) {
            for (int t = 0; t < timeSlots; t++) {
                flat.push_back(literals[(size_t)c][r][t]);
            }
        }
        for (size_t i = 0; i < flat.size(); ++i) {
            for (size_t j = i + 1; j < flat.size(); ++j) {
                atMostOneClauses.push_back(vector<int>{ -flat[i], -flat[j] });
            }
        }
    }

    // Build room conflict clauses: two DIFFERENT course-hours cannot occupy same room AND timeslot
    // For each pair of different course-hours (a,b), and for each (room, timeslot): (-litA OR -litB)
    roomConflictClauses.reserve( (size_t)( (totalCourseHours*(totalCourseHours-1)/2) * classRooms * timeSlots / 10 + 1) );
    for (long long a = 0; a < totalCourseHours; a++) {
        for (long long b = a + 1; b < totalCourseHours; b++) {
            for (int r = 0; r < classRooms; r++) {
                for (int t = 0; t < timeSlots; t++) {
                    int litA = literals[(size_t)a][r][t];
                    int litB = literals[(size_t)b][r][t];
                    roomConflictClauses.push_back(vector<int>{ -litA, -litB });
                }
            }
        }
    }

    // Count clauses
    long long clauseCount = 0;
    clauseCount += (long long)mustHaveRoomClauses.size();
    clauseCount += (long long)atMostOneClauses.size();
    clauseCount += (long long)roomConflictClauses.size();

    // Output in DIMACS CNF format
    cout << "p cnf " << vars << " " << clauseCount << "\n";

    // print must-have (positive) clauses
    for (const auto &cl : mustHaveRoomClauses) {
        for (int lit : cl) cout << lit << " ";
        cout << "0\n";
    }

    // print at-most-one clauses
    for (const auto &cl : atMostOneClauses) {
        for (int lit : cl) cout << lit << " ";
        cout << "0\n";
    }

    // print room conflict clauses
    for (const auto &cl : roomConflictClauses) {
        for (int lit : cl) cout << lit << " ";
        cout << "0\n";
    }

    if (debug) {
        cout << "[DEBUG] Clauses: mustHave=" << mustHaveRoomClauses.size()
             << " atMostOne=" << atMostOneClauses.size()
             << " roomConflicts=" << roomConflictClauses.size()
             << " total=" << clauseCount << "\n";
    }
}
