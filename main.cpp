/*
 Developers:
 Peyton Hecht
 Revanth Yalamanchili
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
#include <ctime>
#include <limits>

using namespace std;

struct Flight {
    string carrier;
    string airport_name;
    int arr_delay;         // delay time in minutes
};

// check whitespaces from string data
string string_format(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");  // find first non-whitespace character
    if (first == string::npos)
        return "";  // return empty string if no non-whitespace character found
    size_t last = str.find_last_not_of(" \t\r\n");   // find last non-whitespace character
    return str.substr(first, (last - first + 1));  // return the formatted string
}

vector<string> parseCSVLine(const string& line, char delimiter) {
    vector<string> item;  // vector to store the parsed items
    string currItem;           // current item being processed
    bool inside_quotes = false;     // flag to check if the current character is inside quotes

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
            item.push_back(string_format(currItem));  // add currItem to the vector and clear it
            currItem.clear();
        } else {
            currItem += c;  // add the character to the currItem
        }
    }
    item.push_back(string_format(currItem));  // add the last currItem
    return item;
}

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

    int carrier_id = -1, airport_name_id = -1, arr_delay_id = -1;
    // find the indices of the required columns based on header names
    for (int i = 0; i < headers.size(); ++i) {
        string header_lower = headers[i];
        transform(header_lower.begin(), header_lower.end(), header_lower.begin(), ::tolower);  // convert to lowercase
        header_lower = string_format(header_lower);
        if (header_lower == "carrier") carrier_id = i;
        else if (header_lower == "airport_name") airport_name_id = i;
        else if (header_lower == "arr_delay") arr_delay_id = i;
    }

    // if any required column is missing, return an empty vector
    if (carrier_id == -1 || airport_name_id == -1 || arr_delay_id == -1) {
        cerr << "Required columns not found in the CSV file." << endl;
        return flights;
    }

    while (getline(file, line)) {  // read flight data lines
        if (line.empty()) continue;  // skip empty lines

        vector<string> tokens = parseCSVLine(line, comma);  // parse the line into tokens

        if (tokens.size() <= max({carrier_id, airport_name_id, arr_delay_id})) {  // skip invalid lines
            continue;
        }

        Flight flight;
        flight.carrier = tokens[carrier_id];  // store the carrier code
        flight.airport_name = tokens[airport_name_id];  // store the airport name

        try {
            flight.arr_delay = stoi(tokens[arr_delay_id]);  // parse the arrival delay as an integer
        } catch (const invalid_argument& e) {
            continue;
        } catch (const out_of_range& e) {
            continue;
        }
        flights.push_back(flight);  // add the flight to the list
    }

    file.close();  // close the file
    return flights;  // return the list of flights
}

// QuickSort function to sort the flights based on their arrival delay
void quickSort(vector<Flight>& flights, int low, int high) {
    if (low < high) {

        // randomly select pivot index, swap to end
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
        int x = i + 1;

        quickSort(flights, low, x - 1);  // recursively sort the left partition

        quickSort(flights, x + 1, high);  // recursively sort the right partition
    }
}

// Merge function used by Merge Sort
void merge(vector<Flight>& flights, int left, int mid, int right) {  // merge two sorted subarrays
    int n1 = mid - left + 1;  // size of left subarray
    int n2 = right - mid;  // size of right subarray
    vector<Flight> leftArray(n1);  // temporary array for left subarray
    vector<Flight> rightArray(n2);  // temporary array for right subarray

    for (int i = 0; i < n1; i++) {  // copy elements to left subarray
        leftArray[i] = flights[left + i];
    }
    for (int i = 0; i < n2; i++) {  // copy elements to right subarray
        rightArray[i] = flights[mid + 1 + i];
    }

    int i_left = 0, j_right = 0, k = left;  // indices for left, right, and merged array

    while (i_left < n1 && j_right < n2) {  // merge while both subarrays have elements
        if (leftArray[i_left].arr_delay <= rightArray[j_right].arr_delay) {  // compare delays
            flights[k] = leftArray[i_left];  // copy from left array
            i_left++;
        } else {
            flights[k] = rightArray[j_right];
            j_right++;
        }
        k++;
    }

    while (i_left < n1) {  // copy remaining elements from left array
        flights[k] = leftArray[i_left];
        i_left++;
        k++;
    }

    while (j_right < n2) {  // copy remaining elements from right array
        flights[k] = rightArray[j_right];
        j_right++;
        k++;
    }
}

void mergeSort(vector<Flight>& flights, int left, int right) {  // recursive merge sort
    if (left < right) {  // check if subarray has more than one element
        int mid = left + (right - left) / 2;  // calculate middle index
        mergeSort(flights, left, mid);  // sort left subarray
        mergeSort(flights, mid + 1, right);  // sort right subarray
        merge(flights, left, mid, right);  // merge sorted subarrays
    }
}

vector<Flight> getBestCase(const vector<Flight>& flights) {
    vector<Flight> sortedFlights = flights;
    sort(sortedFlights.begin(), sortedFlights.end(), [](const Flight& a, const Flight& b) {
        return a.arr_delay < b.arr_delay;  // sort by ascending arrival delay
    });
    return sortedFlights;
}

vector<Flight> getWorstCase(const vector<Flight>& flights) {
    vector<Flight> sortedFlights = flights;
    sort(sortedFlights.begin(), sortedFlights.end(), [](const Flight& a, const Flight& b) {
        return a.arr_delay > b.arr_delay;  // descending arrival delay
    });
    return sortedFlights;
}

void randomize(vector<Flight>& flights) {
    if (flights.size() < 2) return;
    for (int i = flights.size() - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(flights[i], flights[j]);
    }
}

// Function to return the average case (shuffled data)
vector<Flight> getAverageCase(const vector<Flight>& flights) {
    vector<Flight> shuffledFlights = flights;
    randomize(shuffledFlights);  // randomize the data
    return shuffledFlights;
}

int main() {
    cout << "                                                                                 " << endl;
    cout << "                                                                                 " << endl;
    cout << "                                                                                 " << endl;
    cout << "                                                                                 " << endl;
    cout << "                                                                                 " << endl;
    cout << "                                                                   ,/(///.       " << endl;
    cout << "                                                                  /(/   /.       " << endl;
    cout << "                                                                /(/(   /.        " << endl;
    cout << "                                                               /((/    /.        " << endl;
    cout << "  //((//(/.                                          .// /.//./((/     //((/////  " << endl;
    cout << " /(///       //,                              // /////                          //// " << endl;
    cout << "    /(////,         .//.                // ///                   ///*/*//*///    " << endl;
    cout << "    /(/(/(/(//,           //(      // ///                //(///(/(/             " << endl;
    cout << "           /(/(/(//       /(/, ////             , /(/./(/(/(/                    " << endl;
    cout << "             .//(/(/(.                    /.(/ /(/(/(/(/                       " << endl;
    cout << "           ///                    . ,//,//.////                                " << endl;
    cout << "      ////                   ,// /(/(/(/(//                                  " << endl;
    cout << "    ///              ///     ////(/(        /(/                                        " << endl;
    cout << "   /// /(/(/(/(/    ///////////(/(/(/(/       (/(//                                 " << endl;
    cout << "  ////          //////                 ///        /(/(/               " << endl;
    cout << " //// ///////(//////                       /*//      //(/                " << endl;
    cout << " ///////////                                   ////      ///                        " << endl;
    cout << "                                                  //(//(/(//                      " << endl;
    cout << "                                                                                 " << endl;
    std::string ascii_art = R"(
 _______  __       __    _______  __    __  .___________.    _______   _______  __          ___   ____    ____
|   ____||  |     |  |  /  _____||  |  |  | |           |   |       \ |   ____||  |        /   \  \   \  /   /
|  |__   |  |     |  | |  |  __  |  |__|  | `---|  |----`   |  .--.  ||  |__   |  |       /  ^  \  \   \/   /
|   __|  |  |     |  | |  | |_ | |   __   |     |  |        |  |  |  ||   __|  |  |      /  /_\  \  \_    _/
|  |     |  `----.|  | |  |__| | |  |  |  |     |  |        |  '--'  ||  |____ |  `----./  _____  \   |  |
|__|     |_______||__|  \______| |__|  |__|     |__|        |_______/ |_______||_______/__/     \__\  |__|


    )";

    // Print the ASCII art
    std::cout << ascii_art << std::endl;

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    string filename;
    cout << "Enter the path to the flight data CSV file: ";
    cin >> filename;
    vector<Flight> flights = readFlightData(filename);  // read flight data from the file

    if (flights.empty()) {  // if no data is read, terminate
        cerr << "No data to sort." << endl;
        return 1;
    }

    set<string> validCarriers;  // set to store valid airline carriers
    for (const auto& flight : flights) {
        validCarriers.insert(flight.carrier);  // add unique carriers to the set
    }

    char choice = 'Y';
    while (choice == 'Y' || choice == 'y') {
        int method = 0;
        cout << "\nSelect the sorting method to test:\n";
        cout << "1. Quick Sort\n";
        cout << "2. Merge Sort\n";
        cout << "Enter your choice (1 or 2): ";
        cin >> method;  // user input for sorting method

        // Validate sorting method input
        while (method != 1 && method != 2) {
            cout << "Invalid choice. Please enter 1 or 2: ";
            cin >> method;
        }

        int option = 0;
        cout << "\nDo you want to sort delays based on:\n";
        cout << "1. An airline carrier\n";
        cout << "2. An airport (city name)\n";
        cout << "3. All data\n";
        cout << "Enter a number (1, 2, or 3): ";
        cin >> option;  // user input for filter choice

        // Validate filter choice input
        while (option != 1 && option != 2 && option != 3) {
            cout << "Invalid choice. Please enter 1, 2, or 3: ";
            cin >> option;
        }

        vector<Flight> identified;

        // Clear any leftover input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Filter the flights based on the user's choice (airline or airport)
        if (option == 1) {
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
                    identified.push_back(flight);
                }
            }

            if (identified.empty()) {
                cout << "No flights found for the airline: " << airlineName << endl;
                // Ask if the user wants to try again
                cout << "Do you want to try again? (Y/N): ";
                cin >> choice;
                continue;
            }
        } else if (option == 2) {
            string airportName;
            cout << "Enter the airport city name (e.g., Chicago, Birmingham): ";
            getline(cin, airportName);  // allow input with spaces

            string lower_name = airportName;
            transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);

            for (const auto& flight : flights) {
                string flightair = flight.airport_name;
                transform(flightair.begin(), flightair.end(), flightair.begin(), ::tolower);

                if (flightair.find(lower_name) != string::npos) {  // if airport matches
                    identified.push_back(flight);
                }
            }

            if (identified.empty()) {
                cout << "No flights found for airport city containing: " << airportName << endl;
                // Ask if the user wants to try again
                cout << "Do you want to try again? (Y/N): ";
                cin >> choice;
                continue;
            }
        } else if (option == 3) {
            // User chose to sort all data
            identified = flights;
        }

        // Display sorting method chosen
        if (method == 1) {
            cout << "\nYou selected Quick Sort.\n";
        } else if (method == 2) {
            cout << "\nYou selected Merge Sort.\n";
        }

        cout << fixed << setprecision(2);  // format the output to 2 decimal places

        // Best case sorting (already sorted data)
        vector<Flight> bestCaseData = getBestCase(identified);
        auto start = chrono::high_resolution_clock::now();  // start timing

        if (method == 1) {
            quickSort(bestCaseData, 0, bestCaseData.size() - 1);  // perform quick sort
        } else {
            mergeSort(bestCaseData, 0, bestCaseData.size() - 1);  // perform merge sort
        }

        auto end = chrono::high_resolution_clock::now();  // end timing
        auto durationBest = chrono::duration<double, milli>(end - start);
        cout << "\nBest Case (Already Sorted) Sorting Time: " << durationBest.count() << " ms" << endl;


        if (!bestCaseData.empty()) {
            cout << "Shortest delay: " << bestCaseData.front().arr_delay << " minutes" << endl;  // display shortest delay
            cout << "Longest delay: " << bestCaseData.back().arr_delay << " minutes" << endl;  // display longest delay
        }

        // Worst case sorting (reverse sorted data)
        vector<Flight> worstCaseData = getWorstCase(identified);
        start = chrono::high_resolution_clock::now();

        if (method == 1) {
            quickSort(worstCaseData, 0, worstCaseData.size() - 1);
        } else {
            mergeSort(worstCaseData, 0, worstCaseData.size() - 1);
        }

        end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> durationWorst;
        durationWorst = chrono::duration<double, milli>(end - start);
        cout << "\nWorst Case (Reverse Sorted) Sorting Time: " << durationWorst.count() << " ms" << endl;

        if (!worstCaseData.empty()) {
            cout << "Shortest delay: " << worstCaseData.front().arr_delay << " minutes" << endl;
            cout << "Longest delay: " << worstCaseData.back().arr_delay << " minutes" << endl;
        }

        // Average case sorting (shuffled data)
        vector<Flight> averageCaseData = getAverageCase(identified);
        start = chrono::high_resolution_clock::now();

        if (method == 1) {
            quickSort(averageCaseData, 0, averageCaseData.size() - 1);
        } else {
            mergeSort(averageCaseData, 0, averageCaseData.size() - 1);
        }

        end = chrono::high_resolution_clock::now();
        auto durationAverage = chrono::duration<double, milli>(end - start);
        cout << "\nAverage Case (Random Order) Sorting Time: " << durationAverage.count() << " ms" << endl;

        if (!averageCaseData.empty()) {
            cout << "Shortest delay: " << averageCaseData.front().arr_delay << " minutes" << endl;
            cout << "Longest delay: " << averageCaseData.back().arr_delay << " minutes" << endl;
        }

        // Ask the user if they want to perform another operation
        cout << "\nDo you want to perform another operation? (Y/N): ";
        cin >> choice;

        // Validate user input
        while (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n') {
            cout << "Invalid choice. Please enter Y or N: ";
            cin >> choice;
        }
    }

    cout << "Exiting the program. Goodbye!" << endl;
    return 0;
}

// Reference

/*
*   Title: Flight Delay Data for U.S. Airports by Carrier August 2013 - August 2023
*   Author: Sri Harsha Eedala
*   Date: 2023
*   Availability: https://www.kaggle.com/datasets/sriharshaeedala/airline-delay/data
*/
