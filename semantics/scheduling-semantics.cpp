#include <iostream>
#include <vector>
#include "../settings/settings.h"
using namespace std;

void enterSchedulingGenerator(){
    std::vector< int > clauses;

    // Request amount of hours
    int hours;
    cout << "How many hours are there in a day?\n";
    cin >> hours;
    // Request amount of classrooms
    int classRooms;
    cout << "How many class rooms are available?\n";
    cin >> classRooms;

    cout << "Creating clauses for " << hours << " hours and " << classRooms << " classrooms.\n";
    if (debug) {
        cout << "[DEBUG] Debug mode is ON\n";
    }
    // Add clause for each hour
    // Add clause for each classroom
};