//Name: Heiton Zang HXZ190015 and David Murray DWM170230
//Date: created: 09/29/2022, updated 11/06/2022
//Desc: frauddetect
//Prog: 1st input file name, output ids

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>

using namespace std;

// Struct for CSV data
struct Transaction {
public:
    Transaction(int cust, int trans, int date, double a, double lon, double lat) {
        custID = cust; transID = trans; dateTime = date;
        amt = a; longitude = lon; latitude = lat;
    }
    bool operator <(const Transaction & transObj) const {
        if(custID == transObj.custID)
            return dateTime < transObj.dateTime;
        return custID < transObj.custID;
    }
    int custID, transID,dateTime;
    double amt, longitude, latitude;

};

int main(int argc, char **argv)
{
    // Initializing variables
    string filename = argv[1];
    string line;
    ifstream file1 (filename);
    list<Transaction> transList;
    int custID, transID,dateTime;
    double amt, longitude, latitude;

    // Open file and read CSV values and insert into struct list
    if (file1.is_open()) {
        while(getline(file1, line)) { //create sorted list of entries
            istringstream s(line);
            string tmp;
            getline(s, tmp, ',');
            custID = stoi(tmp);
            getline(s, tmp, ',');
            transID = stoi(tmp);
            getline(s, tmp, ',');
            dateTime = stoi(tmp);
            getline(s, tmp, ',');
            amt = stod(tmp);
            getline(s, tmp, ',');
            longitude = stod(tmp);
            getline(s, tmp, ',');
            latitude = stod(tmp);
            Transaction temp = Transaction(custID, transID, dateTime, amt, longitude, latitude);
            transList.push_back(temp);
        }
        file1.close();
    }

    transList.sort();

    dateTime = 0;
    int fraudType = 0;
    int transCount = 0;
    double pastAmt = -1.00;
    int pastAmtEqual = 0;
    custID = -1;

    /*
    */
    for(const auto& transTrav : transList) {
        if(custID != transTrav.custID) {
            custID = transTrav.custID;
            longitude = 0;
            latitude = 0;
            pastAmt = -1;
            pastAmtEqual = 0;
            dateTime = 0;
        }
        // Add , for csv list at end of each non first line
        if (custID != -1)
            cout << ",";

        // Check for fraud based on transaction amount
        if (transTrav.amt >= 200)
            fraudType += 2;
        //Check for previous repeats
        else if(transTrav.amt == pastAmt) {
            pastAmtEqual++;
            if(pastAmtEqual >= 3) {
                fraudType+=2;
            }
        }

        // Check for fraud based on datetime delta from last transaction
        if (transTrav.dateTime - dateTime < 20)
            fraudType += 1;

        // Check for fraud based on geolocation delta from last transaction
        if (transTrav.latitude / (latitude/((double)transCount)) < .5 || transTrav.latitude / (latitude/((double)transCount)) > 1.5) {
            fraudType += 4;
        } else if (transTrav.longitude / (longitude/((double)transCount)) < .5 || transTrav.longitude / (longitude/((double)transCount)) > 1.5) {
            fraudType += 4;
        }

        // Send results to std out
        cout << transTrav.custID << "," << transTrav.transID << "," << transTrav.dateTime << "," <<
                to_string((bool) (fraudType != 0)) << "," << to_string(fraudType);

        // Updates values for next check if fraud was not found
        if (fraudType == 0) {
            dateTime = transTrav.dateTime;
            transCount++;
            pastAmt = transTrav.amt;
            longitude = (longitude + transTrav.longitude); //is total of all long, divide by transAmt to find avg
            latitude = (latitude + transTrav.latitude); //total of all lat, divide by transAmt to find avg
        }
        fraudType = 0;  
    }
}
