/*
 Developers:
 Peyton Hecht
 Revanth Yalamanch
*/


#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <set>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Struct to hold data related to a flight
struct Flight {
    string carrier;        // airline carrier code
    string airport_name;   // airport where the flight is scheduled to land
    int arr_delay;         // arrival delay time in minutes
};

// Function to trim leading and trailing whitespaces from a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");  // find first non-whitespace character
    if (first == string::npos)
        return "";  // return empty string if no non-whitespace character found
    size_t last = str.find_last_not_of(" \t\r\n");   // find last non-whitespace character
    return str.substr(first, (last - first + 1));  // return the formatted string
}

// Function to parse a CSV line into tokens separated by a delimiter
vector<string> parseCSVLine(const string& line, char delimiter) {
    vector<string> item;  // vector to store the parsed item
    string currItem;           // current currItem being processed
    bool inside_quotes = false;  // flag to check if the current character is inside quotes

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == '"') {  // if quote character is encountered
            if (inside_quotes && i + 1 < line.length() && line[i + 1] == '"') {
                currItem += '"';  // escaped quote inside quoted field
                ++i;
            } else {
                inside_quotes = !inside_quotes;  // toggle inside_quotes flag
            }
        } else if (c == delimiter && !inside_quotes) {  // if delimiter is found outside quotes
            item.push_back(trim(currItem));  // add currItem to the vector and clear it
            currItem.clear();
        } else {
            currItem += c;  // otherwise, add the character to the currItem
        }
    }
    item.push_back(trim(currItem));  // add the last currItem
    return item;
}

// Function to read flight data from a CSV file
vector<Flight> readFlightData(const string& filename) {
    vector<Flight> flights;  // vector to store the flight data
    ifstream file(filename);  // open the file

    if (!file.is_open()) {  // check if the file is opened
        cerr << "Failed to open file: " << filename << endl;
        return flights;  // return an empty vector if file cannot be opened
    }

    string line;
    if (!getline(file, line)) {  // read the header line
        return flights;  // return an empty vector if header cannot be read
    }

    char comma = ',';  // default delimiter is a comma
    if (line.find('\t') != string::npos) {
        comma = '\t';  // if tab is found, change delimiter to tab
    }

    vector<string> headers = parseCSVLine(line, comma);  // parse the header line into columns

//    cout << "Headers:" << endl;
//    for (size_t i = 0; i < headers.size(); ++i) {  // output the headers
//        cout << i << ": '" << headers[i] << "'" << endl;
//    }

    int carrier_idx = -1, airport_name_idx = -1, arr_delay_idx = -1;
    // find the indices of the required columns based on header names
    for (size_t i = 0; i < headers.size(); ++i) {
        string header_lower = headers[i];
        transform(header_lower.begin(), header_lower.end(), header_lower.begin(), ::tolower);  // convert to lowercase
        header_lower = trim(header_lower);
        if (header_lower == "carrier") carrier_idx = i;
        else if (header_lower == "airport_name") airport_name_idx = i;
        else if (header_lower == "arr_delay") arr_delay_idx = i;
    }

//    cout << "carrier_idx: " << carrier_idx << ", airport_name_idx: " << airport_name_idx << ", arr_delay_idx: " << arr_delay_idx << endl;

    // if any required column is missing, return an empty vector
    if (carrier_idx == -1 || airport_name_idx == -1 || arr_delay_idx == -1) {
        cerr << "Required columns not found in the CSV file." << endl;
        return flights;
    }

    while (getline(file, line)) {  // read flight data lines
        if (line.empty()) continue;  // skip empty lines

        vector<string> tokens = parseCSVLine(line, comma);  // parse the line into tokens

        if (tokens.size() <= max({carrier_idx, airport_name_idx, arr_delay_idx})) {  // skip invalid lines
            continue;
        }

        Flight flight;
        flight.carrier = tokens[carrier_idx];  // store the carrier code
        flight.airport_name = tokens[airport_name_idx];  // store the airport name

        try {
            flight.arr_delay = static_cast<int>(stod(tokens[arr_delay_idx]));  // parse the arrival delay as an integer
        } catch (const invalid_argument& e) {
            continue;  // skip lines with invalid delay values
        }
        flights.push_back(flight);  // add the flight to the list
    }

    file.close();  // close the file
    return flights;  // return the list of flights
}

// QuickSort function to sort the flights based on their arrival delay
void quickSort(vector<Flight>& flights, int low, int high) {
    if (low < high) {
        // Randomly select a pivot index and swap it to the end
        int pivotIndex = low + rand() % (high - low + 1);
        swap(flights[pivotIndex], flights[high]);

        int pivot = flights[high].arr_delay;  // pivot value is the arrival delay
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (flights[j].arr_delay <= pivot) {
                ++i;
                swap(flights[i], flights[j]);  // swap to arrange smaller values before pivot
            }
        }
        swap(flights[i + 1], flights[high]);  // place pivot in its correct position
        int pi = i + 1;

        quickSort(flights, low, pi - 1);  // recursively sort the left partition
        quickSort(flights, pi + 1, high);  // recursively sort the right partition
    }
}

// Placeholder for merge sort implementation
void mergeSort(vector<Flight>& flights, int left, int right) {
    void merge(vector<Flight>& flights, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Temporary arrays
    vector<Flight> leftArray(n1);
    vector<Flight> rightArray(n2);

    // Copy data to temp arrays
    for (int i = 0; i < n1; i++) {
        leftArray[i] = flights[left + i];
    }
    for (int i = 0; i < n2; i++) {
        rightArray[i] = flights[mid + 1 + i];
    }

    int i = 0; // Initial index of left array
    int j = 0; // Initial index of right array
    int k = left; // Initial index of merged array

    // Merge the temp arrays back into flights
    while (i < n1 && j < n2) {
        if (leftArray[i].arr_delay <= rightArray[j].arr_delay) {
            flights[k] = leftArray[i];
            i++;
        } else {
            flights[k] = rightArray[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of leftArray
    while (i < n1) {
        flights[k] = leftArray[i];
        i++;
        k++;
    }

    // Copy the remaining elements of rightArray
    while (j < n2) {
        flights[k] = rightArray[j];
        j++;
        k++;
    }
}

// Merge sort function
void mergeSort(vector<Flight>& flights, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Recursively sort first and second halves
        mergeSort(flights, left, mid);
        mergeSort(flights, mid + 1, right);

        // Merge the sorted halves
        merge(flights, left, mid, right);
    }
}
}

// Function to return the best case (already sorted data)
vector<Flight> getBestCase(const vector<Flight>& flights) {
    vector<Flight> sortedFlights = flights;
    sort(sortedFlights.begin(), sortedFlights.end(), [](const Flight& a, const Flight& b) {
        return a.arr_delay < b.arr_delay;  // sort by ascending arrival delay
    });
    return sortedFlights;
}

// Function to return the worst case (reverse sorted data)
vector<Flight> getWorstCase(const vector<Flight>& flights) {
    vector<Flight> sortedFlights = flights;
    sort(sortedFlights.begin(), sortedFlights.end(), [](const Flight& a, const Flight& b) {
        return a.arr_delay > b.arr_delay;  // sort by descending arrival delay
    });
    return sortedFlights;
}

// Function to return the average case (shuffled data)
vector<Flight> getAverageCase(const vector<Flight>& flights) {
    vector<Flight> shuffledFlights = flights;
    random_shuffle(shuffledFlights.begin(), shuffledFlights.end());  // shuffle the data
    return shuffledFlights;
}

int main() {
    string filename = "C:/Users/peyto/Airline_Delay_Cause.csv";  // input CSV file name
    vector<Flight> flights = readFlightData(filename);  // read flight data from the file

    if (flights.empty()) {  // if no data is read, terminate
        cerr << "No data to sort." << endl;
        return 1;
    }

    set<string> validCarriers;  // set to store valid airline carriers
    for (const auto& flight : flights) {
        validCarriers.insert(flight.carrier);  // add unique carriers to the set
    }

    int sortingMethod = 0;
    cout << "Select the sorting method to test:\n";
    cout << "1. Quick Sort\n";
    cout << "2. Merge Sort\n";
    cout << "Enter your choice (1 or 2): ";
    cin >> sortingMethod;  // user input for sorting method

    // Validate sorting method input
    while (sortingMethod != 1 && sortingMethod != 2) {
        cout << "Invalid choice. Please enter 1 or 2: ";
        cin >> sortingMethod;
    }

    int filterChoice = 0;
    cout << "Do you want to sort delays based on:\n";
    cout << "1. An airline carrier\n";
    cout << "2. An airport (city name)\n";
    cout << "Enter a number (1 or 2): ";
    cin >> filterChoice;  // user input for filter choice

    while (filterChoice != 1 && filterChoice != 2) {
        cout << "Invalid choice. Please enter 1 or 2: ";
        cin >> filterChoice;
    }

    vector<Flight> selectedFlights;

    // Filter the flights based on the user's choice (airline or airport)
    if (filterChoice == 1) {
        string airlineName;
        cout << "Enter the airline carrier code (e.g., AA, DL, UA): ";
        cin >> airlineName;

        // Validate airline carrier code input
        while (validCarriers.find(airlineName) == validCarriers.end()) {
            cout << "Invalid airline carrier code. Please enter a valid airline code: ";
            cin >> airlineName;
        }

        for (const auto& flight : flights) {
            if (flight.carrier == airlineName) {  // add flights that match the carrier
                selectedFlights.push_back(flight);
            }
        }

        if (selectedFlights.empty()) {
            cout << "No flights found for the airline: " << airlineName << endl;
            return 1;
        }
    } else if (filterChoice == 2) {
        string airportName;
        cout << "Enter the airport city name (e.g., Chicago, Birmingham): ";
        cin.ignore();
        getline(cin, airportName);  // allow input with spaces

        // Convert airport name to lowercase
        string airportNameLower = airportName;
        transform(airportNameLower.begin(), airportNameLower.end(), airportNameLower.begin(), ::tolower);

        for (const auto& flight : flights) {
            string flightAirportNameLower = flight.airport_name;
            transform(flightAirportNameLower.begin(), flightAirportNameLower.end(), flightAirportNameLower.begin(), ::tolower);

            if (flightAirportNameLower.find(airportNameLower) != string::npos) {  // if airport matches
                selectedFlights.push_back(flight);
            }
        }

        if (selectedFlights.empty()) {
            cout << "No flights found for airport city containing: " << airportName << endl;
            return 1;
        }
    }

    // Display sorting method chosen
    if (sortingMethod == 1) {
        cout << "\nYou selected Quick Sort.\n";
    } else if (sortingMethod == 2) {
        cout << "\nYou selected Merge Sort.\n";
    }

    cout << fixed << setprecision(2);  // format the output to 2 decimal places

    // Best case sorting (already sorted data)
    vector<Flight> bestCaseData = getBestCase(selectedFlights);
    auto start = high_resolution_clock::now();  // start timing

    if (sortingMethod == 1) {
        quickSort(bestCaseData, 0, bestCaseData.size() - 1);  // perform quick sort
    } else {
        // FIXME: implement merge sort call
        mergeSort(bestCaseData, 0, bestCaseData.size() - 1);
    }

    auto end = high_resolution_clock::now();  // end timing
    auto durationBest = duration<double, milli>(end - start);
    cout << "\nBest Case (Already Sorted) Sorting Time: " << durationBest.count() << " ms" << endl;

    if (!bestCaseData.empty()) {
        cout << "Shortest delay: " << bestCaseData.front().arr_delay << " minutes" << endl;  // display shortest delay
        cout << "Longest delay: " << bestCaseData.back().arr_delay << " minutes" << endl;  // display longest delay
    }

    // Worst case sorting (reverse sorted data)
    vector<Flight> worstCaseData = getWorstCase(selectedFlights);
    start = high_resolution_clock::now();

    if (sortingMethod == 1) {
        quickSort(worstCaseData, 0, worstCaseData.size() - 1);
    } else {
        // FIXME: implement merge sort call
        mergeSort(worstCaseData, 0, worstCaseData.size() - 1);
    }

    end = high_resolution_clock::now();
    auto durationWorst = duration<double, milli>(end - start);
    cout << "\nWorst Case (Reverse Sorted) Sorting Time: " << durationWorst.count() << " ms" << endl;

    if (!worstCaseData.empty()) {
        cout << "Shortest delay: " << worstCaseData.front().arr_delay << " minutes" << endl;
        cout << "Longest delay: " << worstCaseData.back().arr_delay << " minutes" << endl;
    }

    // Average case sorting (shuffled data)
    vector<Flight> averageCaseData = getAverageCase(selectedFlights);
    start = high_resolution_clock::now();

    if (sortingMethod == 1) {
        quickSort(averageCaseData, 0, averageCaseData.size() - 1);
    } else {
        // FIXME: implement merge sort call
        mergeSort(averageCaseData, 0, averageCaseData.size() - 1);
    }

    end = high_resolution_clock::now();
    auto durationAverage = duration<double, milli>(end - start);
    cout << "\nAverage Case (Random Order) Sorting Time: " << durationAverage.count() << " ms" << endl;

    if (!averageCaseData.empty()) {
        cout << "Shortest delay: " << averageCaseData.front().arr_delay << " minutes" << endl;
        cout << "Longest delay: " << averageCaseData.back().arr_delay << " minutes" << endl;
    }

    return 0;
}

// Reference

/*
*   Title: Flight Delay Data for U.S. Airports by Carrier August 2013 - August 2023
*    Author: Sri Harsha Eedala
*    Date: 2023
*    Availability: https://www.kaggle.com/datasets/sriharshaeedala/airline-delay/data
 */
