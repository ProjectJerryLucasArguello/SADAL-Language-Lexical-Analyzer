#include "lex.h"
#include <cctype>
#include <unordered_map>
//You copied another lexitem getnexttoken idoit!!!
//We have to find a way to do the following:
//How to read a string and classify certain features of it for instance: 15.7E should be tokenize as: FCONST(15.7) IDENT: <e>
//More examples:
/*
    put("Value of z= "); | 96 KEYWORD LEFTPAR SCCONST RIGHTPAR SEMICOLON
	putline(z); | 101 | KEYWORD LEFTPAR IDENT RIGHTPAR 
*/
// When reading a string it has to be lowercase and when outputting it should also be in lowercaps
// We have issue classify between identifiers and keywords
//Reaching an ERR or DONE token completely stops calling on the getNextToken and print out whatever was read so far
//We are also counting lines incorrectly. The issue is we can't assign the linenumber = 1. So probably the placement of the increment count

/*
Single character tokens, such as +, -, *, /, =, <, >, . (dot), (, ):
? These are easy to recognize.
? Two or more characters tokens such as “**“, “>=” and “/=”
? First character in “**“ is an MULT token, the second character ‘*’
appended to the first forms the EXP token.
? For multi-character tokens, such as IDENT, ICONST, FCONST, and
SCONST, create a state for each type of token, for example INID for
recognizing identifiers (IDENT), ININT for recognizing integer
constants (ICONST), and INSTRING for recognizing string literals
(SCONST) .
? Transition from one state to another state is possible. For example from
the state of recognizing ICONST to the state of recognizing FCONST
*/

//Helper functions
/*
Helper functions may be a good choice considering that are each character we look at it's either
A letter, number, or an operator

Recall: "2x__3"

if(isdigit(ch)){
    lexeme += ch
    if(We peek() and what we find is not the continutation of the handleNumber's qualities){
        Classify that part of the lexeme as of course an ICONST
        but now we dive into the handleIdentifier() helper function
    }
}

Inside the handleIdentifier()...
if(isalpha(ch)){
    lexeme += ch where we started a new lexeme with not relation to the pervious lexeme "2"
    if(We peek() and find a '_' it is still a valid IDENT as we add to the lexeme){
        lexeme += ch //BUT IT'S THEN AFTER WE ALWAYS EXPECT EITHER A LETTER OR NUMBER
        if(We peek again and there is not number or letter we end the reading of the lexeme){
            we classify the lexeme, giving it a token
        }
    }

At this point we are look at the '_' which is not a number, not a letter, and not an operator nor a delimiter

Hence we classify it as an ERR ultimately ending the program and reading what was classified so far
}

Okay so what about 15.7E

if(isdigit(ch)){
    lexeme += ch
    if(isdigit(ch)){
        lexeme += ch
    }
    
    if(ch == '.'){
        lexeme += ch
    }
    
    ...
    
    We know "15.7" should be classified as a FCONST
    Because when we reach E or e we have to peek if there is either a '+' or '-' operator AND that there is one digit after
    
    FCONST ::= Digit+ \. Digit+ Exponent?
    ICONST ::= Digit+ Exponent?
    Exponent ::= E ( +? | - ) Digit+
    
    
}


*/
// Again for the case of "2x__3" assuming we had tokenize ICONST(2) and we are on "x"
—--------------------------------------------------------------------------------------------------------------------------
LexItem handleIdentifier(std::istream& in, char c, int linenum) { 
    std::string lexeme;
    lexeme += std::tolower(c); // Convert first letter to lowercase and store it

    while (isalnum(in.peek()) || in.peek() == '_') { 
        char nextChar = in.get(); // Read the next character

        if (isalnum(nextChar)) {
            lexeme += std::tolower(nextChar); // Convert to lowercase and add
        } 
        else if (nextChar == '_') {
            lexeme += nextChar; // Add first underscore

            // If next character is NOT a digit or letter, break and return lexeme
            if (!isalpha(in.peek()) && !isdigit(in.peek())) {
                return LexItem(IDENT, lexeme, linenum); 
            }

            // If another `_` follows, it's an error
            if (in.peek() == '_') { 
                return LexItem(ERR, "_", linenum);
            }
        }
    }
    
    return id_or_kw(lexeme, linenum); // Classify as IDENT, keyword, or BCONST
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%       
/*if(c == "."
      And there after we peek there is a digit){
        //Would probably need to consider if there are more digits after the parenthesis
        //Futhermore we need to consider ERR such as 5.7.4*/
      
          /*We read what we have so far in the lexeme return it's respective token and 
              since we are have a letter  THAT IS NOT EITHER "E" or "e" we call on the handleIdentifier
          */
          
          /*
          Okay so instead of have two if statements we can combine them where if the next character we peek isalpha(c) True AND it's either "E" or "e"
          Then we have an exponent in our hands
          And since we have an exponent in our hands from the exponent 
          We have to peek if there is either a '+' or a '-' BUT THEN we need to look ahead once more to see if an digit --> FCONST
          But if it there is not a '+' or a '-' then we can classify that 'E' or 'e' as an IDENT within the handleIdentifer helper function
          Futhermore we need to consider ERR such as 5E7E4
          
          Else then we call on the handleIdentifier helper function after we finished returning the return(Token, lexeme, linenum)
          Or just returning is enough where we break outside go through the loop and find the identifer
          
          
In realerr file we should have 0.758. as the identified error not 0.758
Maybe in the lex.ccp we failed to handle a case of 0.758.3
From it we should return 0.758. as an error
Because since there are two decimal places and there is a number after that SECOND decimal place
that would result in an error

          */
—----------------------------------------------------------------------------------------------------------------------                 
LexItem handleNumber(std::istream& in, char ch, int linenum) {
    std::string lexeme;
    lexeme += ch; // Start with the first digit
    bool hasDot = false, hasExponent = false;

    while (isdigit(in.peek()) || in.peek() == '.' || in.peek() == 'E' || in.peek() == 'e') {
        char nextChar = in.get();

        if (isdigit(nextChar)) {
            lexeme += nextChar;
        } 
        /*
        In realerr file we should have 0.758. as the identified error not 0.758
        Maybe in the lex.ccp we failed to handle a case of 0.758.3
        From it we should return 0.758. as an error
        Because since there are two decimal places and there is a number after that SECOND decimal place
        that would result in an error
        */
        else if (nextChar == '.') {
            if (hasDot) { 
                if (in.peek() == '.') { // Case: "2.."
                    in.putback('.');  // Put back second dot
                    return LexItem(ICONST, lexeme, linenum); // Return "2" as ICONST
                }
                return LexItem(ERR, lexeme + ".", linenum); // Case: "3.4.5" should return an ERR "3.4."
            }

            hasDot = true;
            lexeme += nextChar;

            if (!isdigit(in.peek())) { 
                if (in.peek() == '.') { // If we find another dot ? it's "2.."
                    in.putback('.');   // Put back second dot
                    lexeme.pop_back(); // Remove the dot from lexeme
                    return LexItem(ICONST, lexeme, linenum); // Return "2"
                }
                return LexItem(ERR, lexeme, linenum); // Case: "5." (invalid trailing dot)
            }
        } 
        else if (nextChar == 'E' || nextChar == 'e') {
            if (hasExponent) {
                return LexItem(ERR, lexeme, linenum); // Case: "5E7E4"
            }
            hasExponent = true;
            lexeme += nextChar;

            if (in.peek() == '+' || in.peek() == '-') {
                lexeme += in.get();
            }

            if (!isdigit(in.peek())) {
                in.putback(nextChar);  // Put back `E`
                return LexItem(FCONST, lexeme.substr(0, lexeme.length() - 1), linenum);
            }
        }
        else if (isalpha(nextChar)) {
            // We reached a letter, meaning the number is done
            in.putback(nextChar); // Put it back for the next handler
            return hasDot ? LexItem(FCONST, lexeme, linenum) : LexItem(ICONST, lexeme, linenum);
        }
    }

    return hasDot ? LexItem(FCONST, lexeme, linenum) : LexItem(ICONST, lexeme, linenum);
}
//The handleStringLiteral should
//1. Identifty the quotations be it '\'' or a '"'
//2. If we have contents or words within the quotations (assuming it's correctly closed) we count those and these are CASE SENSITIVE
//a. If there is one character between single quotation marks then it's a CCONST
//b. If there are more characters between double quotations marks then it's a SCONST
//3. Now we check the end to see if they match
//Looking at invchar1 and invchar2 I notced the following to be ERR of CCONST
/*
The single quotatiions must resign on the same line, we cannot have one single quote start and another end on the next line 
A result of that is an ERR say New line is nan invalid character constant

CCONST must constain exactly ONE character within the single quotations
Any more that would result in an ERR and from the looks of it we extract the first characters excluding the last character
*/

/*if (ch == '\'' || ch == '"') {
            char quoteType = ch;
            lexeme = "";
            while (in.get(ch) && ch != quoteType) {
                if (ch == '\\') {
                    if (!in.get(ch)) break;
                    switch (ch) {
                        case 'n': lexeme += '\n'; break;
                        case 't': lexeme += '\t'; break;
                        case '\\': lexeme += '\\'; break;
                        case '"': lexeme += '"'; break;
                        case '\'': lexeme += '\''; break;
                        default: lexeme += ch; break;
                    }
                } else {
                    lexeme += ch;
                }
            }
            if (ch != quoteType) {
                return LexItem(ERR, lexeme, linenum);  // Unterminated string
            }
            return LexItem(quoteType == '\'' ? CCONST : SCONST, lexeme, linenumber);
        }*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
LexItem handleStringLiteral(std::istream& in, char ch, int linenum){
              std::string lexeme;
              char quoteType = ch;  // Store whether it's ' or "
    
    if (quoteType != '\'' && quoteType != '"') {
        return LexItem(ERR, std::string(1, ch), linenum);  // Invalid case
    }

    //invstr1 and invstr2 both display the same message as invchar1
    //Why?
    /*
    "The center of the 
	 circle through these points is" --> Should be {Invalid string constant "THe center of the}
     
     "Its radius is' <-- ERR token due to not matching quotations
         Should be Invalid string constant "Its radius is'
    
      Maybe we want to have two possible cases:
          The first charcter is a single quote then it will hold these conditions and erros
          If the first character is a double quote then it will hold these conditions and errors
          
          So  if (ch == '\n') {  // String literals must stay on the same line
            return LexItem(ERR, "New line is an invalid character constant.", linenum);
        } ONLY APPLIES TO SINGLE QUOTATIONS
    */   
    while (in.get(ch)) {
        if (ch == '\n') {  
            return LexItem(ERR, quoteType == '"' ? 
                " Invalid string constant \"" + lexeme : 
                "New line is an invalid character constant.", 
                linenum);
        }

        if (ch == '\\') {  // Handle escape sequences
            if (!in.get(ch)) break;  // Read next character
            switch (ch) {
                    //We cannot have a newline between "\'" in invchar1 it says New Line is an invalid character constant
                case 'n': lexeme += '\n'; break;
                case 't': lexeme += '\t'; break;
                case '\\': lexeme += '\\'; break;
                case '"': lexeme += '"'; break;
                case '\'': lexeme += '\''; break;
                default: lexeme += ch; break;  // Unknown escape sequence (handled literally)
            }
        } 
        
        //So that condition should be applied here since it focus is on whether the single quotations are formmated correctly
        else if (ch == quoteType) {  // Closing quote found
            if (quoteType == '\'' && lexeme.length() != 1) {
                //In invchar 2 the error message displays 'Th' not The 
                //We appear to need to display the first to characters in the error not the last character
      
                return LexItem(ERR, " Invalid character constant '" + lexeme.substr(0, 2) + "'", linenum);  // CCONST must have exactly 1 character
                
            }
            //Check the invstr2 ERR in the corrected file
            //Same for the invstr1 
            return LexItem(quoteType == '"' ? SCONST : CCONST, lexeme, linenum);
        } 
        else {
            lexeme += ch;
        }
    }
    // If we exit the loop without a closing quote
    return LexItem(ERR, " Invalid string constant \"" + lexeme + "\"", linenum);
    
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
       
//If we have a comment then we are not returning anything we are passing over the remainder contents in that line after the "--"
// And then proceeding with the nextline incrementing linenum
//In prog1 i am receiveing an error of an unrecognizable lexeme {{} where "{" is the unrecongnizable 
//The issue is that I have a comment line defined by two "-"
//I assume that maybe in the operatorsanddelimiters function that 
/*
In the LexItem getNextToken
SOMETHING IS DEFINITYELY WRONG WITH OUR COMMENT OR RECONGINZING THAT "--" is a comment
*/
void handleComment(std::istream& in, int& linenum) {
    char ch;
    in.get();  // Consume the second '-'
    
    // Read until the end of the line
    while (in.get(ch)) {
        if (ch == '\n') {
            linenum++;  // Update line count
            return;
        }
    }
}
         
// For the helperFunctions, handleOperator and handleDelimiter in the LexItem getNextToken
// We need the condition for the two where for the character we extracted we refer to the respective maps
// Now for what about the second character after?
//It appears that the handleOperatorAndDelimiter are classified as a keyword
LexItem handleOperatorAndDelimiter(std::istream& in, char ch, int linenum) {
    std::unordered_map<std::string, Token> operators = {
        {"+", PLUS}, {"-", MINUS}, {"*", MULT}, {"**", EXP}, {"/", DIV}, {":=", ASSOP},
        {"=", EQ}, {"/=", NEQ}, {"<", LTHAN}, {">", GTHAN}, {"<=", LTE},
        {">=", GTE}, {"mod", MOD}, {"&", CONCAT}, {"and", AND}, {"or", OR},
        {"not", NOT}
    };
    
    std::unordered_map<std::string, Token> delimiters = {
        {",", COMMA}, {";", SEMICOL}, {"(", LPAREN}, {")", RPAREN},
        {".", DOT}, {":", COLON}
    };

    std::string lexeme(1, ch);  // Start lexeme with the first character

    // Ignore `{}` (Assuming they are ignored characters)
    if (ch == '{' || ch == '}') {
        return getNextToken(in, linenum); // Skip and continue
    }

    // If it's an alphabetic character, check for keywords/operators like "mod", "and", "or", "not"
    if (isalpha(ch)) {
        while (isalnum(in.peek())) {
            lexeme += in.get();
        }
        // Check in operator map first (ensuring mod, and, or, not are handled here)
        if (operators.count(lexeme)) {
            return LexItem(operators[lexeme], lexeme, linenum);
        }
        // If it's not an operator, pass it on to id_or_kw
        return id_or_kw(lexeme, linenum);
    }

    // Check for multi-character operators
    char nextChar = in.peek();
    if (!in.eof() && ispunct(nextChar)) {
        lexeme += nextChar;
        if (operators.count(lexeme)) {  
            in.get();  // Consume the second character
            return LexItem(operators[lexeme], lexeme, linenum);
        }
        lexeme.pop_back(); // Revert to single-character lexeme if not found
    }

    // Check for single-character operators
    if (operators.count(lexeme)) {
        return LexItem(operators[lexeme], lexeme, linenum);
    }

    // Check for delimiters
    if (delimiters.count(lexeme)) {
        return LexItem(delimiters[lexeme], lexeme, linenum);
    }

    // If unrecognized, return an error
    return LexItem(ERR, lexeme, linenum);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

LexItem getNextToken(std::istream& in, int& linenum) {
    char ch;
    
    while (in.get(ch)) {  
        // Skip whitespace and count new lines
        if (isspace(ch)) {
            if (ch == '\n') {
                linenum++;
            }
            continue;  
        }

        // Handle comments: '--' starts a comment, ensure we read both dashes
        if (ch == '-') {
            if (in.peek() == '-') { // Check if next character is also '-'
                handleComment(in, linenum);
                continue;  // Skip the comment and move to the next token
            }
        }

        // Handle identifiers and keywords (starts with a letter) But some reservedwords also start with a letter but they should
        //Actually be run though the return handleOperatorAndDelimiter
        // So words like mod, and, or, AND not are reversed words also known as keywords, not identifiers
        if (isalpha(ch)) {
            return handleIdentifier(in, ch, linenum);
        }

        // Handle numbers (starts with a digit)
        if (isdigit(ch)) {
            return handleNumber(in, ch, linenum);
        }

        // Handle string literals (starts with a double quote)
        if (ch == '"' || ch == '\'') {
            return handleStringLiteral(in, ch, linenum);
        }

        // Handle operators and delimiters (special characters) but there are also reserved words that are classified here
        if (ispunct(ch)) {
            return handleOperatorAndDelimiter(in, ch, linenum);
        }

        // If no valid token is found, return an error token
        return LexItem(ERR, std::string(1, ch), linenum);
    }

    // If we reach the end of the file, return the DONE token
    return LexItem(DONE, "", linenum);
}

//-------------------------------------------------------------------------------------------------------
//The pervious issue I had with this code was that it was not able to correctly count the number of identifiers and keywords
//Or Rather everything was classified to be a identifier
// SO keywords such as "if" "else" "elsif" we considered identifiers
//But the last attempt I used that the order were not lowercased
// and I used id_or_kw method for handling identifiers
/*
    IF, ELSE, ELSIF, PUT, PUTLN, GET, INT, FLOAT,
	CHAR, STRING, BOOL, PROCEDURE, TRUE, FALSE, END,
	IS, BEGIN, THEN, CONST,
*/
LexItem id_or_kw(const std::string& lexeme, int linenum) {
    static const std::unordered_map<std::string, Token> keywordMap = {
        {"if", IF}, {"else", ELSE}, {"elsif", ELSIF}, {"put", PUT}, 
        {"putline", PUTLN}, {"get", GET}, {"integer", INT}, {"float", FLOAT},
        {"character", CHAR}, {"string", STRING}, {"boolean", BOOL}, 
        {"procedure", PROCEDURE}, {"true", TRUE}, {"false", FALSE},
        {"end", END}, {"is", IS}, {"begin", BEGIN}, {"then", THEN}, 
        {"constant", CONST}
    };

    static const std::unordered_map<std::string, Token> operators = {
        {"mod", MOD}, {"and", AND}, {"or", OR}, {"not", NOT} // Added operator keywords
    };

    // Check if the lexeme is a keyword
    auto it = keywordMap.find(lexeme);
    if (it != keywordMap.end()) {
        if (it->second == TRUE || it->second == FALSE) {
            return LexItem(BCONST, lexeme, linenum);
        }
        return LexItem(it->second, lexeme, linenum);
    }

    // Check if the lexeme is an operator (to catch "mod", "and", "or", "not")
    auto opIt = operators.find(lexeme);
    if (opIt != operators.end()) {
        return LexItem(opIt->second, lexeme, linenum);
    }

    // Otherwise, it's an identifier
    return LexItem(IDENT, lexeme, linenum);
}

//-----------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const LexItem& tok) {
    Token tokenType = tok.GetToken();
    std::string lexeme = tok.GetLexeme();
    int lineNumber = tok.GetLinenum();

    // Print token type as a string
    switch (tokenType) {
        case ICONST: os << "ICONST"; break;
        case FCONST: os << "FCONST"; break;
        case BCONST: os << "BCONST"; break;
        case IDENT: os << "IDENT"; break;
        case SCONST: os << "SCONST"; break;
        case CCONST: os << "CCONST"; break;
        case ERR: os << "ERR"; break;
        case PLUS: os << "PLUS"; break;
        case MINUS: os << "MINUS"; break;
        case MULT: os << "MULT"; break;
        case DIV: os << "DIV"; break;
        case EQ: os << "EQ"; break;
        case NEQ: os << "NEQ"; break;
        case LTHAN: os << "LTHAN"; break;
        case GTHAN: os << "GTHAN"; break;
        case LTE: os << "LTE"; break;
        case GTE: os << "GTE"; break;
        case ASSOP: os << "ASSOP"; break;
        case MOD: os << "MOD"; break;
        case CONCAT: os << "CONCAT"; break;
        case AND: os << "AND"; break;
        case OR: os << "OR"; break;
        case NOT: os << "NOT"; break;
        case EXP: os << "EXP"; break;
        case COMMA: os << "COMMA"; break;
        case SEMICOL: os << "SEMICOL"; break;
        case LPAREN: os << "LPAREN"; break;
        case RPAREN: os << "RPAREN"; break;
        case DOT: os << "DOT"; break;
        case COLON: os << "COLON"; break;
        case IF: os << "IF"; break;
        case ELSE: os << "ELSE"; break;
        case ELSIF: os << "ELSIF"; break;
        case PUT: os << "PUT"; break;
        case PUTLN: os << "PUTLN"; break;
        case GET: os << "GET"; break;
        case INT: os << "INT"; break;
        case FLOAT: os << "FLOAT"; break;
        case CHAR: os << "CHAR"; break;
        case STRING: os << "STRING"; break;
        case BOOL: os << "BOOL"; break;
        case PROCEDURE: os << "PROCEDURE"; break;
        case TRUE: os << "TRUE"; break;
        case FALSE: os << "FALSE"; break;
        case END: os << "END"; break;
        case IS: os << "IS"; break;
        case BEGIN: os << "BEGIN"; break;
        case THEN: os << "THEN"; break;
        case CONST: os << "CONST"; break;
        case DONE: os << "DONE"; break;
        default: os << "UNKNOWN"; break;
    }

    // Append lexeme formatting based on token type
    switch (tokenType) {
        case ICONST:
        case FCONST:
        case BCONST:
            os << ": (" << lexeme << ")";
            break;
        case IDENT:// mod, and, or, AND  not are not IDENT they are keywords also known as reserved words
            os << ": <" << lexeme << ">";
            break;
        case SCONST:
            os << ": \"" << lexeme << "\"";
            break;
        case CCONST:
            os << ": '" << lexeme << "'";
            break;
        case ERR:
            os << "\nERR: Unrecognized Lexeme {" << lexeme << "} in line " << lineNumber;
            break;
        default:
            break;
    }

    os << std::endl;
    return os;
}
