#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <sstream>
#include <cmath>    
#include "lex.h"


/*
In the main function I have to organize my output to where it does the following:
2) Increasing order for digits
2a) If there are any exponent convert to their regular form so for instance 25E-2 that would be 0.25
3) For the ERRs I need the full error so if I have 0.758. I show the full lexeme not part of it

In realerr file we should have 0.758. as the identified error not 0.758
Maybe in the lex.ccp we failed to handle a case of 0.758.3
From it we should return 0.758. as an error
Because since there are two decimal places and there is a number after that SECOND decimal place
that would result in an error

invstr1 we forgot to include the " in "Its radius is'
And with within the {} it should say { Invalid string constant "Its radius is'}

invstr2 we forgot the " in "The center of the

In lex.ccp we forgot to consider the case of a single quotation starting on one line and ending on another line
If that were the case then the error message reads {New line is an invalid character constant.}

In invchar as said before, we want the first two character to denote a invalid character constant
So the error message should not display {The} it should display {'Th'}

Lastly in the main.ccp we need for the CHARACTERS AND STRINGS:
Just fix it where the last element in the list does not have a comma
What is Token Value Order?
    // keywords OR RESERVED WORDS
	listingorderofKeyWords <mapping> = {IF, ELSE, ELSIF, PUT, PUTLN, GET, INT, FLOAT,
	CHAR, STRING, BOOL, PROCEDURE, TRUE, FALSE, END,
	IS, BEGIN, THEN, CONST}
    
    Plan: I think these keywords should be in one map
    We already had identified all the keywords/reserved words identified in lex.ccp
    It's a manner of us going through the map of the keywords or reservedwords
    So we need a for loop so 
    
    In Python Terms:
    for every ky in listingorderofKeywords:
        if ky exist in the identified keywords
            foundKys.append(ky)
    return foundKys

    We'll start at "if" we'll look if was identified
    if it we place it in keywords
    
    else we continue on the loop
*/
using namespace std;


bool isKeyword(Token tok) {
    return tok == PROCEDURE || tok == STRING || tok == ELSE || tok == IF ||tok == INT ||
        tok == FLOAT || tok == CHAR || tok == PUT || tok == PUTLN ||tok == GET ||
        tok == BOOL || tok == TRUE || tok == FALSE || tok == ELSIF ||tok == IS ||
        tok == END || tok == BEGIN || tok == THEN || tok == CONST ||tok == MOD ||
        tok == AND || tok == OR || tok == NOT;
}

int main(int argc, char* argv[]) {
    
    // Set to store unique values
    set<string> numericConstants;
    set<string> stringConstants;
    set<string> identifiers;
    vector<string> keywordOrder;
    
    std::vector<string> listingorderofKeywords = {
        "if", "else", "elsif", "put", "putline", "get", "integer", "float",
        "character", "string", "boolean", "procedure", "true", "false", "end",
        "is", "begin", "then", "constant", "and","or","not","mod" 
    };
    
    std::unordered_map<std::string, int> keywordCounts;
    
    if (argc < 2) {
        cout << "No specified input file." << endl;
        return 1;
    }



    //Ripped this out of SA 3 & 4 
    vector<LexItem> tokens;
    bool allFlag = false, numFlag = false, strFlag = false, idFlag = false, kwFlag = false;
    string filename;

    // Read the file contents
    for (int i = 1; i < argc; i++) {
    string arg = argv[i];

        // Check if the argument is a flag
        if (arg[0] == '-') {
            if (arg == "-all") allFlag = true;
            else if (arg == "-num") numFlag = true;
            else if (arg == "-str") strFlag = true;
            else if (arg == "-id") idFlag = true;
            else if (arg == "-kw") kwFlag = true;
            else {
                cout << "Unrecognized flag {" << arg << "}" << endl;
                return 1;  // Exit on badarg flag
            }
        }
        
        // If it's a filename
        else {
            if (!filename.empty()) {
                cout << "Only one file name is allowed." << endl;
                return 1;  // Exit on multiple filenames
            }
            filename = arg;
        }
    }

    // Check if no filename was provided
    if (filename.empty()) {
        cout << "No specified input file." << endl;
        return 1;
    }

    // Try opening the file
    ifstream file(filename);
    if (!file) {
        cout << "CANNOT OPEN THE FILE " << filename << endl;
        return 1;
    }

    // Check if the file is empty
    if (file.peek() == EOF) {
        cout << "Empty file." << endl;
        return 1;
    }

    int linenum = 0; //This count I believe is somewhat inaccurate
    //See since I started the count at zero sure it counts ther correct number of lines
    //But when we identify an errors it's n-1 so the error in line 1 it actually noted as line 0
    //int errorlinenum = 1; Maybe initalizing another int to count the errors starting from line 1
    //But how do I icremenet both linenum because it give me the correct count
    //And also increment another int errorlinenum but this one starts at one?
    LexItem token;
   
   //For invstr1 and invstr2 it has a {"Invalid string constant" << """ <<  error}
   //invchar1 display {New line is an invalid character constant.}
   //invchar2 displays {Invalid chracter constant }
    //But how are you suppose to know what error to display?
   //But that's for identifying SCONST and CCONST
    while ((token = getNextToken(file, linenum)).GetToken() != DONE) {
        if (token.GetToken() == ERR) {
            cout << "ERR: in line " << linenum + 1 <<", " << "Error Message "<< "{" <<token.GetLexeme() << "}" << endl;
            return 1;
        }
        tokens.push_back(token);

        // Process token types
        //Sort alphabetically and numerically when we show the output
        //As for the Exp we need to convert it into it's base form
        //In addition in prog1 for a number like 0.0 if there exist after the dot the 0
        //Then when we output the numerical constants it's just the constant
        if (token.GetToken() == ICONST || token.GetToken() == FCONST) {
            numericConstants.insert(token.GetLexeme());
        } else if (token.GetToken() == SCONST || token.GetToken() == CCONST) {
            stringConstants.insert(token.GetLexeme());
        } else if (token.GetToken() == IDENT) {
            identifiers.insert(token.GetLexeme());
        } else if (isKeyword(token.GetToken())) {
           string lexeme = token.GetLexeme();
           keywordCounts[lexeme]++; // Increment the count in the map
        }

        if (allFlag) {
            cout << token;
        }
    }

    // Summary Information
    cout << "\nLines: " << linenum << endl;
    cout << "Total Tokens: " << tokens.size() << endl;
    cout << "Numerals: " << numericConstants.size() << endl;
    cout << "Characters and Strings : " << stringConstants.size() << endl;
    cout << "Identifiers: " << identifiers.size() << endl;
    //Want to diplay each unqiue key words and not the count of each occuring keyword/reserved word
    //Also again to emphasize I need to alphabetize fromtoken value
    /*
    
    */
    cout << "Keywords: " << keywordCounts.size() << endl;
    
    // Process flags
    //For numerical constants we need a conidition where we print something out is if we got a 0 after the constant.0 we just print out the constant
    //If it's in exponent form we convert it into it's regular form
    //And lastly the output should be in increasing order
    if (numFlag && !numericConstants.empty()) {
        cout << "NUMERIC CONSTANTS:\n";
    
        // Convert set of numeric strings to vector of doubles
        vector<double> sortedNumericConstants;
        for (const auto& num : numericConstants) {
            sortedNumericConstants.push_back(stod(num));
        }

        // Sort numerically
        sort(sortedNumericConstants.begin(), sortedNumericConstants.end());

        // Print sorted numeric constants
        bool first = true;
        for (const auto& num : sortedNumericConstants) {
            if (!first) cout << ", ";
            cout << num;
            first = false;
        }
        cout << endl;
    }

    if (strFlag && !stringConstants.empty()) {
        cout << "CHARACTERS AND STRINGS:\n";
        size_t count = 0;
        for (const auto& str : stringConstants) {
            cout << "\"" << str << "\"";
            if (++count < stringConstants.size()) cout << ", ";
        }
        cout << endl;
    }

    if (idFlag && !identifiers.empty()) {
        vector<string> sortedIdentifiers(identifiers.begin(), identifiers.end()); 
        sort(sortedIdentifiers.begin(), sortedIdentifiers.end()); // Sort alphabetically

        cout << "IDENTIFIERS:\n";
        for (size_t i = 0; i < sortedIdentifiers.size(); ++i) {
            if (i > 0) cout << ", ";  // Avoid leading comma
            cout << sortedIdentifiers[i];
        }
        cout << endl;
    }
    
    // Display keywords in order of token value?
    /*
        
    */
    if (kwFlag && !keywordCounts.empty()) {
        cout << "KEYWORDS:\n";
        bool first = true;
        /*
        for (const auto std::string& keyword : keywordCounts) {
            if (keywordCounts.find(keyword) != keywordCounts.end()) {
                if (!first) std::cout << ", ";
                std::cout << keyword.first;
                first = false;
            }
        }
        */
        for (const std::string& keyword : listingorderofKeywords) {
            if (keywordCounts.find(keyword) != keywordCounts.end()) {
                if (!first) std::cout << ", ";
                    std::cout << keyword; // Access the key (string) from the pair
                /*
                So i have all the keyword now how do I sort it based on:
                "IF", "ELSE", "ELSIF", "PUT", "PUTLN", "GET", "INT", "FLOAT",
                "CHAR", "STRING", "BOOL", "PROCEDURE", "TRUE", "FALSE", "END",
                "IS", "BEGIN", "THEN", "CONST" 
                
                */ 
            first = false;
            }
        }
        cout << endl;
    }    
    return 0;
}
