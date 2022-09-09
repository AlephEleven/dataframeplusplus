#include <iostream>
#include <fstream>

#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>

#include <limits>
#include <climits>

#include <math.h>

#define INT_REGEX  "^[-+]?[0-9]+$"
#define FLOAT_REGEX "[+-]?([0-9]*[.])?[0-9]+"

using namespace std;

class DataFrame {
    
    public:

    string fp;

    void ReadFile(char = ',', int = false, int = true, int = 10);
    void info();

    template <class T1, class T2>
    vector<T1> loc(string colName, int rowA, int rowB, T1 (*func)(T2), char delim = ',');

    template<class T>
    vector<vector<int>> bins(vector<T> series);

    template<class T>
    T max(vector<T> series);

    int bytes;
    vector<int> shape;
    vector<string> labelTypes;
    vector<string> labelNames;

    DataFrame(string filepath, int delim=',', int debug=false, int read=true){
        fp = filepath;
        if(read) ReadFile(delim=delim, debug=debug);
    }
};

/*

    Reads file given during class init. and generates values for class
    params:
    - delim: delimiter for file, ',' for comma-separated files
    - debug: if enabled, shows process of parsing file.
    - hasLabels: uses first row for list of label names
    - predictType: predicts column type, based on regex & boolean true/falses, otherwise defaults to string
    otherwise looks through whole table get all indexes.

*/
void DataFrame::ReadFile(char delim, int debug, int hasLabels, int predictType){

    if(debug) cout << "Reading " << fp << "..." << endl;

    ifstream file;
    string line;
    file.open(fp);
    int rows = 0; int cols = 0;

    if(file.is_open()){
        while(getline(file, line)){

            //Get Column names and push to labelNames vector
            if(rows == 0 && hasLabels){

                istringstream ss(line);
                string token;
                
                if(debug) cout << "Columns Found:" << endl;
                while(getline(ss, token, delim)){
                    if(debug) cout << " " << token << endl;
                    labelNames.push_back(token);
                    labelTypes.push_back("string");
                    cols++;
                }

            }

            //Predict type of each column, for a certain amount of times, otherwise default to string
            if(((rows==0 && !hasLabels) || rows>0) && predictType > 0){
                istringstream ss(line);
                string token;
                int col_ind = 0;

                while(getline(ss, token, delim)){
                    if(col_ind == cols){
                        break;
                    }

                    //If labeltype is string, attempt to match with better type.
                    if(labelTypes.at(col_ind)=="string"){

                        if(regex_match(token,regex(INT_REGEX))){
                            labelTypes.at(col_ind) = "int";

                        }
                        else 
                        if(regex_match(token,regex(FLOAT_REGEX))){
                            labelTypes.at(col_ind) = "float";

                        }
                        else
                        if(token=="True"||token=="False"||token=="TRUE"||token=="FALSE"||token=="true"||token=="false"){
                            labelTypes.at(col_ind) = "bool";
                        }
                        col_ind++;
                    }
                }
                predictType--;
            }



            bytes = file.tellg();
            rows += 1;
        }
        rows = (hasLabels) ? rows-1 : rows;
        if(debug) cout << "Finshed reading " << fp << "!\nFound " << rows << " lines.\n" << bytes << " bytes read" << endl;
        shape = {rows, cols};

        file.close();
    }
    else{
        cout << "Failed to open " << fp << endl;
        shape = {0,0};
    }
}

//Get info about dataframe
void DataFrame::info(){
    cout << "Shape: (" << shape.at(0) << "x" << shape.at(1) << ")" << endl;
    for(int i = 0; i < labelNames.size(); i++){
        cout << "Column " << i << ":" << endl;
        cout << "   name: " << labelNames.at(i) << endl;
        cout << "   type: " << labelTypes.at(i) << endl;
    }
}

/*

    Gets series of values based on column name, and row indexes, and applies a function to each value.
    params:
    - colName: column name
    - rowA/rowB: start/end index, rowA=0, rowB=df.shape(0) to get the whole column
    - func: function that is applied to each row, data is represented as strings, so converting to type is intended
    - delim: delimiter, default is ','

*/
template <class T1, class T2>
vector<T1> DataFrame::loc(string colName, int rowA, int rowB, T1 (*func)(T2), char delim){
    ifstream file;
    string line;
    file.open(fp);

    vector<T1> res;

    int colNum = 0; int rowNum = -1;
    for(int i = 0; i < labelNames.size(); i++){
        if(colName == labelNames.at(i)){
            colNum = i;
            break;
        }
    }

    if(file.is_open()){
        while(getline(file, line)){
            if(rowNum >= rowA && rowNum < rowB){
                istringstream ss(line);
                string token;
                int col_ind = 0;

                while(getline(ss, token, delim)){
                    if(col_ind == colNum){
                        res.push_back(func(token));
                        break;
                    }
                    col_ind++;
                }
                col_ind = 0;
            }
            if(rowNum > rowB){
                break;
            }
            rowNum++;
        }
        file.close();
    }
    else{
        cout << "Failed to open " << fp << endl;
    }


    return res;

}

//Gets max of series of values
template<class T>
T DataFrame::max(vector<T> series){
    return *max_element(series.begin(), series.end());

}

//Creates bins for histogram based on series values, may be skewed for un-normal data
template<class T>
vector<vector<int>> DataFrame::bins(vector<T> series){
    double bins = floor(log2(series.size())+1);
    double max_bin = ceil(max(series));
    double cutoffs = max_bin/bins;
    vector<double> cutoffs_vec = {};
    vector<int> hist_vals = {};
    vector<int> hist_totals = {};

    for(int i = 0; i < bins-1; i++){
        cutoffs_vec.push_back(cutoffs*(i+1));
        hist_totals.push_back(0);
    }
    cutoffs_vec.push_back(max_bin);
    hist_totals.push_back(0);


    for(int i = 0; i < series.size(); i++){
        for(int j = 0; j < cutoffs_vec.size(); j++){
            if(series.at(i) <= cutoffs_vec.at(j)){
                hist_vals.push_back(j);
                hist_totals.at(j) += 1;
                break;
            }
        }
    }

    return {hist_vals, hist_totals};
}

int strToInt(string x){
    return (x == "") ? 0 : stoi(x);
}



int main(int argc, char** argv){

    DataFrame df("data/House_Rent_Dataset.csv");

    //df.info();

    vector<int> rents = df.loc("Rent", 0, df.shape.at(0), &strToInt);

    vector<vector<int>> rent_bins = df.bins(rents);

    for(int i = 0; i < rent_bins.at(1).size(); i++){
        cout << "Bins: " << rent_bins.at(1).at(i) << endl;
    }

    

    return 0;

}