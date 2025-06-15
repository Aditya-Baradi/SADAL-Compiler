/* Implementation of Interpreter
   for the Simple Ada-Like (SADAL) Language
 * parserInterp.cpp
 * Programming Assignment 3
 * Spring 2025
*/
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>
#include <map>
#include <string>
#include "parserInterp.h"
#include "lex.h"

map<string, bool> defVar;
map<string, Token> SymbolTable;
map<string, Value> TempRes;

vector<string> * Id_List;

namespace Parser {
    bool pushed_back = false;
    LexItem pushed_tok;

    static LexItem GetNextToken(istream& in, int& l) {
        if( pushed_back ) {
            pushed_back = false;
            return pushed_tok;
        }
        return getNextToken(in, l);
    }

    static void PushBackToken(LexItem & t) {
        if( pushed_back ) {
            abort();
        }
        pushed_back = true;
        pushed_tok = t;
    }
}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int l, string message)
{
    ++error_count;
    cout << l << ": " << message << endl;
}


ValType TokenToValType(Token tok) {
    switch (tok) {
        case INT: return VINT;
        case FLOAT: return VREAL;
        case STRING: return VSTRING;
        case CHAR: return VCHAR;
        case BOOL: return VBOOL;
        default: return VERR;
    }
}


bool VarInitialized(const string& varName) {
    if (TempRes.find(varName) == TempRes.end()) {
        return false;
    }
    return !TempRes[varName].IsErr();
}


bool Prog(istream& in, int& l)
{
    bool f1;
    LexItem tok = Parser::GetNextToken(in, l);
    if (tok.GetToken() == PROCEDURE) {
        tok = Parser::GetNextToken(in, l);
        if (tok.GetToken() == IDENT) {
            string identstr = tok.GetLexeme();
            if (defVar.find(identstr) == defVar.end()) {
                defVar[identstr] = true;
            }
            tok = Parser::GetNextToken(in, l);
            if (tok.GetToken() == IS) {
                f1 = ProcBody(in, l);
                if(f1) {
    tok = Parser::GetNextToken(in, l);
    if(tok.GetToken() == DONE) {
       cout << endl;
       cout << "(DONE)" << endl;
        return true;
    }
    else
    {
        ParseError(l, "Incorrect compilation file.");
        return false;
    }
}
                else
                {
                    ParseError(l, "Incorrect Procedure Definition.");
                    return false;
                }
            }
            else
            {
                ParseError(l, "Incorrect Procedure Header Format.");
                return false;
            }
        }
        else
        {
            ParseError(l, "Missing Procedure Name.");
            return false;
        }
    }
    ParseError(l, "Incorrect compilation file.");
    return false;
}


bool ProcBody(istream& in, int& l)
{
    bool status = false, f2;
    LexItem tok;
    string procName;
    

    for (auto& var : defVar) {
        procName = var.first;
        break;
    }
    
    status = DeclPart(in, l);
    if (!status)
    {
        ParseError(l, "Incorrect Declaration Part.");
        return status;
    }
    tok = Parser::GetNextToken(in, l);
    if(tok == BEGIN)
    {
        f2 = StmtList(in, l);
        if(!f2)
        {
            ParseError(l, "Incorrect Procedure Body.");
            return false;
        }
        tok = Parser::GetNextToken(in, l);
        if (tok.GetToken() == END)
        {
            tok = Parser::GetNextToken(in, l);
            if (tok.GetToken() == IDENT)
            {

                if (tok.GetLexeme() != procName) {
                    ParseError(l, "Procedure name mismatch with closing end identifier.");
                    return false;
                }
                
                tok = Parser::GetNextToken(in, l);
                if (tok.GetToken() == SEMICOL)
                {
                    return true;
                }
                else
                {
                    ParseError(l, "Missing end of procedure semicolon.");
                    return false;
                }
            }
            else
            {
                ParseError(l, "Missing END of procedure name.");
                return false;
            }
        }
        else
        {
            ParseError(l, "Missing END of Procedure Keyword.");
            return false;
        }
    }
    ParseError(l, "Incorrect procedure body.");
    return false;
}





bool StmtList(istream& in, int& l)
{
    bool status;
    LexItem tok;
    
    status = Stmt(in, l);
    if (!status) {
        return false;
    }
    
    tok = Parser::GetNextToken(in, l);
    while (tok != END && tok != ELSIF && tok != ELSE) {
        Parser::PushBackToken(tok);
        status = Stmt(in, l);
        if (!status) {
            return false;
        }
        tok = Parser::GetNextToken(in, l);
        if (tok.GetToken() == DONE) {
            break;
        }
    }
    
    Parser::PushBackToken(tok);
    return true;
}


bool DeclPart(istream& in, int& l) {
    bool status = false;
    LexItem tok;
    status = DeclStmt(in, l);
    if(status)
    {
        tok = Parser::GetNextToken(in, l);
        if(tok == BEGIN)
        {
            Parser::PushBackToken(tok);
            return true;
        }
        else
        {
            Parser::PushBackToken(tok);
            status = DeclPart(in, l);
        }
    }
    else
    {
        ParseError(l, "Non-recognizable Declaration Part.");
        return false;
    }
    return true;
}


bool DeclStmt(istream& in, int& l)
{
    LexItem t;
    Id_List = new vector<string>;
    bool status = IdentList(in, l);
    bool f;
    Value initVal;
    Token type;
    
    if (!status)
    {
        ParseError(l, "Incorrect identifiers list in Declaration Statement.");
        return status;
    }
    t = Parser::GetNextToken(in, l);
    if(t == COLON)
    {
        t = Parser::GetNextToken(in, l);
        if(t == CONST)
        {
            t = Parser::GetNextToken(in, l);
        }
        type = t.GetToken();
        
        if(t == INT || t == FLOAT || t == STRING || t == BOOL || t == CHAR)
        {
            t = Parser::GetNextToken(in, l);
            if(t == LPAREN)
            {
                Value rangeVal1, rangeVal2;
                status = Range(in, l, rangeVal1, rangeVal2);
                if(!status)
                {
                    ParseError(l, "Incorrect definition of a range in declaration statement");
                    return false;
                }
                t = Parser::GetNextToken(in, l);
                if(t != RPAREN)
                {
                    ParseError(l, "Incorrect syntax for a range in declaration statement");
                    return false;
                }
                t = Parser::GetNextToken(in, l);
            }
            if(t == ASSOP)
            {

                f = Expr(in, l, initVal);
                if(!f)
                {
                    ParseError(l, "Incorrect initialization expression.");
                    return false;
                }
                

                if ((type == INT && !initVal.IsInt()) ||
                    (type == FLOAT && !initVal.IsReal()) ||
                    (type == STRING && !initVal.IsString()) ||
                    (type == CHAR && !initVal.IsChar()) ||
                    (type == BOOL && !initVal.IsBool())) {
                    ParseError(l, "Initialization value type does not match variable type.");
                    return false;
                }
                

                for (const string& varName : *Id_List) {
                    SymbolTable[varName] = type;
                    

                    switch (type) {
                        case INT:
                            TempRes[varName] = Value(initVal.GetInt());
                            break;
                        case FLOAT:
                            TempRes[varName] = Value(initVal.GetReal());
                            break;
                        case STRING:
                            TempRes[varName] = Value(initVal.GetString());
                            break;
                        case CHAR:
                            TempRes[varName] = Value(initVal.GetChar());
                            break;
                        case BOOL:
                            TempRes[varName] = Value(initVal.GetBool());
                            break;
                        default:
                            TempRes[varName] = Value();
                    }
                }
            }
            else
            {

                Parser::PushBackToken(t);
                

                for (const string& varName : *Id_List) {
                    SymbolTable[varName] = type;
                    

                    TempRes[varName] = Value();
                }
            }
            t = Parser::GetNextToken(in, l);
            if(t == SEMICOL)
            {
                return true;
            }
            else
            {
                l--;
                ParseError(l, "Missing semicolon at end of statement");
                return false;
            }
        }
        else
        {
            ParseError(l, "Incorrect Declaration Type.");
            return false;
        }
    }
    else
    {
        Parser::PushBackToken(t);
        ParseError(l, "Incorrect Declaration Statement Syntax.");
        return false;
    }
}


bool IdentList(istream& in, int& l) {
    bool status;
    string identstr;
    LexItem tok = Parser::GetNextToken(in, l);
    if(tok == IDENT)
    {
        identstr = tok.GetLexeme();
        if (defVar.find(identstr) == defVar.end())
        {
            defVar[identstr] = true;
            Id_List->push_back(identstr);
        }
        else
        {
            ParseError(l, "Variable Redefinition");
            return false;
        }
    }
    else
    {
        Parser::PushBackToken(tok);
        return true;
    }
    tok = Parser::GetNextToken(in, l);
    if (tok == COMMA) {
        status = IdentList(in, l);
    }
    else if(tok == COLON)
    {
        Parser::PushBackToken(tok);
        return true;
    }
    else if(tok == IDENT)
    {
        ParseError(l, "Missing comma in declaration statement.");
        return false;
    }
    else {
        ParseError(l, "Invalid name for an Identifier:");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return status;
}


bool Stmt(istream& in, int& l) {
    bool status = false;
    LexItem t = Parser::GetNextToken(in, l);
    switch( t.GetToken() ) {
    case IDENT:
        Parser::PushBackToken(t);
        status = AssignStmt(in, l);
        if(!status)
        {
            ParseError(l, "Invalid assignment statement.");
            return false;
        }
        break;
    case IF:
        Parser::PushBackToken(t);
        status = IfStmt(in, l);
        if(!status)
        {
            ParseError(l, "Invalid If statement.");
            return false;
        }
        break;
    case PUT: case PUTLN:
        Parser::PushBackToken(t);
        status = PrintStmts(in, l);
        if(!status)
        {
            ParseError(l, "Invalid put statement.");
            return false;
        }
        break;
    case GET:
        Parser::PushBackToken(t);
        status = GetStmt(in, l);
        if(!status)
        {
            ParseError(l, "Invalid get statement.");
            return false;
        }
        break;
    default:
        Parser::PushBackToken(t);
        return false;
    }
    return status;
}


bool PrintStmts(istream& in, int& l) {
    LexItem t;
    Value printvalue;
    bool isPutLine = false;
    
    t = Parser::GetNextToken(in, l);
    if (t == PUTLN) {
        isPutLine = true;
    }
    else if (t != PUT) {
        ParseError(l, "Missing Put or PutLine Keyword");
        return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if( t != LPAREN ) {
        ParseError(l, "Missing Left Parenthesis");
        return false;
    }
    
    bool ex = Expr(in, l, printvalue);
    if (!ex) {
        ParseError(l, "Missing expression for an output statement");
        return false;
    }
    

    cout << printvalue;
    if (isPutLine) {
        cout << endl;
    }
    
    t = Parser::GetNextToken(in, l);
    if(t != RPAREN ) {
        ParseError(l, "Missing Right Parenthesis");
        return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if(t != SEMICOL) {
        l--;
        ParseError(l, "Missing semicolon at end of statement");
        return false;
    }
    
    return true;
}


bool IfStmt(istream& in, int& l) {
    bool ex=false, status;
    LexItem t;
    Value condvalue;
    bool ConditionIsMet = false;
    
    t = Parser::GetNextToken(in, l);
    if(t != IF) {
        ParseError(l, "Missing IF Keyword");
        return false;
    }
    
    ex = Expr(in, l, condvalue);
    if(!ex) {
        ParseError(l, "Missing if statement condition");
        return false;
    }
    

    if(!condvalue.IsBool()) {
        ParseError(l, "Run-Time Error-Illegal Expression type for an If statement condition");
        return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if(t != THEN) {
        ParseError(l, "If-Stmt Syntax Error");
        return false;
    }
    

    if(condvalue.GetBool()) {
        ConditionIsMet = true;
        status = StmtList(in, l);
        if(!status) {
            ParseError(l, "Missing Statement for If-Stmt Then-clause");
            return false;
        }
    } else {

        int nestedLevel = 0;
        while(true) {
            t = Parser::GetNextToken(in, l);
            if(t == IF) {
                nestedLevel++;
            } else if(t == END && nestedLevel > 0) {
                t = Parser::GetNextToken(in, l);
                if(t == IF) {
                    nestedLevel--;
                }
                Parser::PushBackToken(t);
            } else if(t == ELSIF || t == ELSE || t == END) {
                break;
            } else if(t == DONE) {
                ParseError(l, "Missing END IF");
                return false;
            }
        }
        Parser::PushBackToken(t);
    }
    

    t = Parser::GetNextToken(in, l);
    while(t == ELSIF && !ConditionIsMet) {
        ex = Expr(in, l, condvalue);
        if(!ex) {
            ParseError(l, "Missing Elsif statement condition");
            return false;
        }
        

        if(!condvalue.IsBool()) {
            ParseError(l, "Run-Time Error-Illegal Expression type for an elsif-clause condition");
            return false;
        }
        
        t = Parser::GetNextToken(in, l);
        if(t != THEN) {
            ParseError(l, "Elsif-Stmt Syntax Error");
            return false;
        }
        

        if(condvalue.GetBool()) {
            ConditionIsMet = true;
            status = StmtList(in, l);
            if(!status) {
                ParseError(l, "Missing Statement for If-Stmt Else-If-clause");
                return false;
            }
        } else {

            int nestedLevel = 0;
            while(true) {
                t = Parser::GetNextToken(in, l);
                if(t == IF) {
                    nestedLevel++;
                } else if(t == END && nestedLevel > 0) {
                    t = Parser::GetNextToken(in, l);
                    if(t == IF) {
                        nestedLevel--;
                    }
                    Parser::PushBackToken(t);
                } else if(t == ELSIF || t == ELSE || t == END) {
                    break;
                } else if(t == DONE) {
                    ParseError(l, "Missing END IF");
                    return false;
                }
            }
            Parser::PushBackToken(t);
        }
        
        t = Parser::GetNextToken(in, l);
    }


    
    

    if(t == ELSE && !ConditionIsMet) {
        status = StmtList(in, l);
        if(!status) {
            ParseError(l, "Missing Statement for If-Stmt Else-clause");
            return false;
        }
        t = Parser::GetNextToken(in, l);
    } else if(t == ELSE && ConditionIsMet) {

        int nestedLevel = 0;
        while(true) {
            t = Parser::GetNextToken(in, l);
            if(t == IF) {
                nestedLevel++;
            } else if(t == END && nestedLevel > 0) {
                t = Parser::GetNextToken(in, l);
                if(t == IF) {
                    nestedLevel--;
                }
                Parser::PushBackToken(t);
            } else if(t == END) {
                break;
            } else if(t == DONE) {
                ParseError(l, "Missing END IF");
                return false;
            }
        }
    }
    
    if(ConditionIsMet)
    {
        while(t != END)
        {
            t = Parser::GetNextToken(in,l);
        }
    }
    

    if(t != END) {
        ParseError(l, "Missing END keyword for If-statement.");
        return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if(t != IF) {
        ParseError(l, "Missing IF keyword after END for If-statement.");
        return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if(t != SEMICOL) {
        l--;
        ParseError(l, "Missing semicolon at end of statement");
        return false;
    }
    
    return true;
}


bool GetStmt(istream& in, int& l)
{
    LexItem t, idtok;
    string varName;
    
    t = Parser::GetNextToken(in, l);
    if (t != GET)
    {
        ParseError(l, "Missing Get Keyword");
        return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if (t != LPAREN) {
        ParseError(l, "Missing Left Parenthesis");
        return false;
    }
    
    bool ex = Var(in, l, idtok);
    if (!ex) {
        ParseError(l, "Missing a variable for an input statement");
        return false;
    }
    
    varName = idtok.GetLexeme();
    

    Token varType = SymbolTable[varName];
    string inputStr;
    
    switch (varType) {
        case INT: {
            int value;
            cout << "";
            cin >> value;
            TempRes[varName] = Value(value);
            break;
        }
        case FLOAT: {
            double value;
            cout << "";
            cin >> value;
            TempRes[varName] = Value(value);
            break;
        }
        case STRING: {
            string value;
            cout << "";
            cin >> value;
            TempRes[varName] = Value(value);
            break;
        }
        case CHAR: {
            char value;
            cout << "Enter a character value: ";
            cin >> value;
            TempRes[varName] = Value(value);
            break;
        }
        case BOOL: {
            bool value;
            string input;
            cout << "Enter a boolean value (true/false): ";
            cin >> input;
            value = (input == "true" || input == "TRUE" || input == "True" || input == "1");
            TempRes[varName] = Value(value);
            break;
        }
        default:
            ParseError(l, "Invalid variable type for input");
            return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if(t != RPAREN) {
        ParseError(l, "Missing Right Parenthesis");
        return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if(t != SEMICOL)
    {
        l--;
        ParseError(l, "Missing semicolon at end of statement");
        return false;
    }
    
    return true;
}


bool Var(istream& in, int& l, LexItem & idtok)
{
    string identstr;
    idtok = Parser::GetNextToken(in, l);
    
    if (idtok == IDENT) {
        identstr = idtok.GetLexeme();
        if (defVar.find(identstr) == defVar.end() || !defVar[identstr])
        {
            ParseError(l, "Undeclared Variable");
            return false;
        }
        return true;
    }
    else if (idtok.GetToken() == ERR) {
        ParseError(l, "Unrecognized Input Pattern");
        cout << "(" << idtok.GetLexeme() << ")" << endl;
        return false;
    }
    return false;
}


bool AssignStmt(istream& in, int& l) {
    bool varstatus = false;
    bool status = false;
    LexItem t, idtok;
    int currentLine;
    string varName;
    Value exprvalue;
    
    varstatus = Var(in, l, idtok);
    varName = idtok.GetLexeme();
    
    currentLine = l;
    if (varstatus) {
        t = Parser::GetNextToken(in, l);
        if (t == ASSOP) {
            status = Expr(in, l, exprvalue);
            if (!status) {
                ParseError(l, "Missing Expression in Assignment Statement");
                return status;
            }
            

            Token varType = SymbolTable[varName];
            switch (varType) {
                case INT:
                    if (!exprvalue.IsInt()) {
                        ParseError(l, "Run-Time Error-Illegal Expression type for the assigned variable");
                        return false;
                    }
                    TempRes[varName] = Value(exprvalue.GetInt());
                    break;
                case FLOAT:
                    if (!exprvalue.IsReal()) {
                        cout << exprvalue << endl;
                        ParseError(l, "Run-Time Error-Illegal Expression type for the assigned variable");
                        return false;
                    }
                    TempRes[varName] = Value(exprvalue.GetReal());
                    break;
                case STRING:
                    if (!exprvalue.IsString()) {
                        ParseError(l, "Run-Time Error-Illegal Expression type for the assigned variable");
                        return false;
                    }
                    TempRes[varName] = Value(exprvalue.GetString());
                    break;
                case CHAR:
                    if (!exprvalue.IsChar()) {
                        ParseError(l, "Run-Time Error-Illegal Expression type for the assigned variable");
                        return false;
                    }
                    TempRes[varName] = Value(exprvalue.GetChar());
                    break;
                case BOOL:
                    if (!exprvalue.IsBool()) {
                        ParseError(l, "Run-Time Error-Illegal Expression type for the assigned variable");
                        return false;
                    }
                    TempRes[varName] = Value(exprvalue.GetBool());
                    break;
                default:
                    ParseError(l, "Invalid variable type for assignment");
                    return false;
            }
        }
        else if (t.GetToken() == ERR) {
            ParseError(l, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        }
        else {
            ParseError(l, "Missing Assignment Operator");
            return false;
        }
    }
    else {
        ParseError(l, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    
    t = Parser::GetNextToken(in, l);
    if (t != SEMICOL) {
        if (currentLine != l) {
            l--;
            ParseError(l, "Missing semicolon at end of statement");
        }
        else {
            ParseError(l, "Illegal expression for an assignment statement");
        }
        return false;
    }
    
    return status;
}


bool Expr(istream& in, int& l, Value & retVal) {
    LexItem tok;
    Value value1, value2;
    
    bool t1 = Relation(in, l, value1);
    if (!t1) {
        return false;
    }
    
    retVal = value1;
    
    tok = Parser::GetNextToken(in, l);
    if (tok.GetToken() == ERR) {
        ParseError(l, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    
    while (tok == OR || tok == AND) {
        t1 = Relation(in, l, value2);
        if (!t1) {
            ParseError(l, "Missing operand after operator");
            return false;
        }
        

        if (!retVal.IsBool() || !value2.IsBool()) {
            ParseError(l, "Run-Time Error-Illegal operand types for logical operator");
            return false;
        }
        

        if (tok == AND) {
            retVal = retVal && value2;
        }
        else if (tok == OR) {
            retVal = retVal || value2;
        }
        
        tok = Parser::GetNextToken(in, l);
        if (tok.GetToken() == ERR) {
            ParseError(l, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    
    Parser::PushBackToken(tok);

    return true;
}


bool Relation(istream& in, int& l, Value & retVal) {
    LexItem tok;
    Value value1, value2;
    
    bool t1 = SimpleExpr(in, l, value1);
    if (!t1) {
        return false;
    }
    
   retVal = value1;
    
    tok = Parser::GetNextToken(in, l);
    if (tok.GetToken() == ERR) {
        ParseError(l, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    

    if (tok == EQ || tok == NEQ || tok == LTHAN || tok == GTHAN || tok == LTE || tok == GTE) {
        t1 = SimpleExpr(in, l, value2);
        if (!t1) {
            ParseError(l, "Missing operand after operator");
            return false;
        }
        

        if (value1.GetType() != value2.GetType()) {
            ParseError(l, "Run-Time Error-Illegal Mixed Type Operands");
            return false;
        }
        

        if (tok == EQ) {
            retVal = value1 == value2;
        }
        else if (tok == NEQ) {
            retVal = value1 != value2;
        }
        else if (tok == LTHAN) {
            retVal = value1 < value2;
        }
        else if (tok == GTHAN) {
            retVal = value1 > value2;
        }
        else if (tok == LTE) {
            retVal = value1 <= value2;
        }
        else if (tok == GTE) {
            retVal = value1 >= value2;
        }
        
        tok = Parser::GetNextToken(in, l);
    }
    
    Parser::PushBackToken(tok);

    return true;
}


bool SimpleExpr(istream& in, int& l, Value & retVal) {
    Value value1, value2;
    LexItem tok;
    
    bool t1 = STerm(in, l, value1);
    if (!t1) {
        return false;
    }
    
    retVal = value1;
    
    tok = Parser::GetNextToken(in, l);
    if (tok.GetToken() == ERR) {
        ParseError(l, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    
    while (tok == PLUS || tok == MINUS || tok == CONCAT) {
        t1 = STerm(in, l, value2);
        if (!t1) {
            ParseError(l, "Missing operand after operator");
            return false;
        }
        

        if (tok == PLUS) {

            if ((retVal.IsInt() && value2.IsInt()) ||
                (retVal.IsReal() && value2.IsReal())) {
                retVal = retVal + value2;
            }
            else {
                ParseError(l, "Run-Time Error-Illegal Mixed Type Operands");
                return false;
            }
        }
        else if (tok == MINUS) {

            if ((retVal.IsInt() && value2.IsInt()) ||
                (retVal.IsReal() && value2.IsReal())) {
                retVal = retVal - value2;
            }
            else {
                ParseError(l, "Run-Time Error-Illegal Mixed Type Operands");
                return false;
            }
        }
        else if (tok == CONCAT) {

            if ((retVal.IsString() || retVal.IsChar()) &&
                (value2.IsString() || value2.IsChar())) {
                retVal = retVal.Concat(value2);
            }
            else {
                ParseError(l, "Run-Time Error-Illegal Operand Types for Concatenation");
                return false;
            }
        }
        
        tok = Parser::GetNextToken(in, l);
        if (tok.GetToken() == ERR) {
            ParseError(l, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    
    Parser::PushBackToken(tok);

    return true;
}


bool STerm(istream& in, int& l, Value & retVal) {
    LexItem t = Parser::GetNextToken(in, l);
    bool status;
    int sign = 0;
    Value value;
    
    if (t == MINUS) {
        sign = -1;
    }
    else if (t == PLUS) {
        sign = 1;
    }
    else if (t == NOT) {
        sign = 0;
        status = Term(in, l, sign, value);
        if (!status) {
            return false;
        }
        

        if (!value.IsBool()) {
            ParseError(l, "Run-Time Error-Illegal operand type for NOT operator");
            return false;
        }
        

        retVal = !value;
        return true;
    }
    else {
        Parser::PushBackToken(t);
    }
    
    status = Term(in, l, sign, retVal);

    return status;
}


bool Term(istream& in, int& l, int sign, Value & retVal) {
    Value value1, value2;
    LexItem tok;
    
    bool t1 = Factor(in, l, sign, value1);
    if (!t1) {
        return false;
    }
    
    retVal = value1;
    
    tok = Parser::GetNextToken(in, l);
    if (tok.GetToken() == ERR) {
        ParseError(l, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    
    while (tok == MULT || tok == DIV || tok == MOD) {
        t1 = Factor(in, l, sign, value2);
        if (!t1) {
            ParseError(l, "Missing operand after operator");
            return false;
        }
        

        if (tok == MULT) {

            if ((retVal.IsInt() && value2.IsInt()) ||
                (retVal.IsReal() && value2.IsReal())) {
                retVal = retVal * value2;
            }
            else {
                ParseError(l, "Run-Time Error-Illegal Mixed Type Operands");
                return false;
            }
        }
        else if (tok == DIV) {

            if ((value2.IsInt() && value2.GetInt() == 0) ||
                (value2.IsReal() && value2.GetReal() == 0.0)) {
                ParseError(l, "Run-Time Error-Illegal Division by Zero");
                return false;
            }
            

            if ((retVal.IsInt() && value2.IsInt()) ||
                (retVal.IsReal() && value2.IsReal())) {
                retVal = retVal / value2;
            }
            else {
                ParseError(l, "Run-Time Error-Illegal Mixed Type Operands");
                return false;
            }
        }
        else if (tok == MOD) {

            if (retVal.IsInt() && value2.IsInt()) {

                if (value2.GetInt() == 0) {
                    ParseError(l, "Run-Time Error-Illegal Division by Zero");
                    return false;
                }
                retVal = retVal % value2;
            }
            else {
                ParseError(l, "Run-Time Error-Illegal operand types for MOD operator");
                return false;
            }
        }
        
        tok = Parser::GetNextToken(in, l);
        if (tok.GetToken() == ERR) {
            ParseError(l, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    
    Parser::PushBackToken(tok);

    return true;
}


bool Factor(istream& in, int& l, int sign, Value & retVal) {
    LexItem tok;
    bool t1;
    Value value1, value2;
    
    tok = Parser::GetNextToken(in, l);
    if (tok == NOT) {
        t1 = Primary(in, l, sign, value1);
        if (!t1) {
            ParseError(l, "Incorrect operand for NOT operator");
            return false;
        }
        

        if (!value1.IsBool()) {
            ParseError(l, "Run-Time Error-Illegal operand type for NOT operator");
            return false;
        }
        

        retVal = !value1;
        return true;
    }
    else {
        Parser::PushBackToken(tok);
    }
    
    t1 = Primary(in, l, sign, value1);
    if (!t1) {
        ParseError(l, "Incorrect operand");
        return false;
    }
    

    if (sign == -1) {
        if (value1.IsInt()) {
            retVal = Value(-value1.GetInt());
        }
        else if (value1.IsReal()) {
            retVal = Value(-value1.GetReal());
        }
        else {
            ParseError(l, "Run-Time Error-Illegal operand type for Sign operation");
            return false;
        }
    }
    else if (sign == 1) {
        if (value1.IsInt() || value1.IsReal()) {
            retVal = value1;
        }
        else {
            ParseError(l, "Run-Time Error-Illegal operand type for Sign operation");
            return false;
        }
    }
    else {
        retVal = value1;
    }
    
    tok = Parser::GetNextToken(in, l);
    if (tok == EXP) {
        t1 = Primary(in, l, sign, value2);
        if (!t1) {
            ParseError(l, "Missing raised power for exponent operator");
            return false;
        }
        

        if (!retVal.IsReal()) {
            ParseError(l, "Run-Time Error-Illegal Base for Exponentiation (must be Float)");
            return false;
        }
        

        retVal = retVal.Exp(value2);
        return true;
    }
    
    Parser::PushBackToken(tok);

    return true;
}

bool Primary(istream& in, int& l, int sign, Value & retVal) {
    LexItem tok = Parser::GetNextToken(in, l);
    bool status;
    
    if (tok == IDENT) {
        string varName = tok.GetLexeme();
        Parser::PushBackToken(tok);
        status = Name(in, l, sign, retVal);
        if(!status) {
            ParseError(l, "Invalid reference to a variable.");
            return false;
        }
        

        if (!VarInitialized(varName)) {
            ParseError(l, "Run-Time Error-Using uninitialized variable");
            return false;
        }
        
        return true;
    }
    else if (tok == ICONST) {
        int value = stoi(tok.GetLexeme());
        retVal = Value(value);
        return true;
    }
    else if (tok == SCONST) {
        string value = tok.GetLexeme();
        retVal = Value(value);
        return true;
    }
    else if (tok == FCONST) {
        double value = stod(tok.GetLexeme());
        retVal = Value(value);
        return true;
    }
    else if (tok == BCONST) {
        string lexeme = tok.GetLexeme();
        for (char& c : lexeme) c = tolower(c);
        bool value = (lexeme == "true");
        retVal = Value(value);
        return true;
    }
    else if (tok == CCONST) {
        char value = tok.GetLexeme()[0];
        retVal = Value(value);
        return true;
    }
    else if (tok == LPAREN) {
        status = Expr(in, l, retVal);
        if (!status) {
            ParseError(l, "Invalid expression after left parenthesis");
            return false;
        }
        
        if (Parser::GetNextToken(in, l) == RPAREN)
            return status;
        else {
            Parser::PushBackToken(tok);
            ParseError(l, "Missing right parenthesis after expression");
            return false;
        }
    }
    else if (tok.GetToken() == ERR) {
        ParseError(l, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    
    Parser::PushBackToken(tok);
    ParseError(l, "Invalid Expression");

    return false;
}

bool Name(istream& in, int& l, int sign, Value & retVal) {
    bool status;
    LexItem tok = Parser::GetNextToken(in, l);
    string lexeme = tok.GetLexeme();
    
    if (defVar.find(lexeme) == defVar.end() || !defVar[lexeme]) 
    {
        ParseError(l, "Using Undefined Variable");
        return false;
    }
    
     if (TempRes.find(lexeme) == TempRes.end())
     {
        ParseError(l, "Invalid use of an unintialized variable.");
        return false;
     }
    
    retVal = TempRes[lexeme];
    
    tok = Parser::GetNextToken(in, l);
    if (tok == LPAREN) {

        Value index1, index2;
        status = Range(in, l, index1, index2);
        if (!status) {
            ParseError(l, "Invalid range definition");
            return false;
        }
        
        tok = Parser::GetNextToken(in, l);
        if (tok != RPAREN) {
            ParseError(l, "Invalid syntax for an index or range definition.");
            return false;
        }
        

        if (!retVal.IsString()) {
            ParseError(l, "Run-Time Error-Index/Range operation on non-string variable");
            return false;
        }
        
        string strVal = retVal.GetString();
        

        if (index2.IsErr()) {

            if (!index1.IsInt()) {
                ParseError(l, "Run-Time Error-Index must be an integer");
                return false;
            }
            
            int idx = index1.GetInt();
            

            if (idx < 0 || idx >= static_cast<int>(strVal.length())) {
                ParseError(l, "Run-Time Error-Index out of bounds");
                return false;
            }
            

            retVal = Value(strVal[idx]);
        }
        else {

            if (!index1.IsInt() || !index2.IsInt()) {
                ParseError(l, "Run-Time Error-Range bounds must be integers");
                return false;
            }
            
            int start = index1.GetInt();
            int end = index2.GetInt();
            

            if (start > end) {
                ParseError(l, "Run-Time Error-Invalid range: start > end");
                return false;
            }
            

            if (start < 0 || end >= static_cast<int>(strVal.length())) {
                ParseError(l, "Run-Time Error-Range out of bounds");
                return false;
            }
            

            retVal = Value(strVal.substr(start, end - start + 1));
        }
        
        return true;
    }
    
    Parser::PushBackToken(tok);

    return true;
}

bool Range(istream& in, int& l, Value & retVal1, Value & retVal2) {
    bool status;
    LexItem tok;
    
    status = SimpleExpr(in, l, retVal1);
    if (!status) {
        ParseError(l, "Invalid expression for an index or a lower bound definition of a range.");
        return false;
    }
    
    tok = Parser::GetNextToken(in, l);
    if (tok == DOT) {
        tok = Parser::GetNextToken(in, l);
        if (tok == DOT) {
            status = SimpleExpr(in, l, retVal2);
            if (!status) {
                ParseError(l, "Invalid expression for an upper bound definition of a range.");
                return false;
            }
        }
        else {
            ParseError(l, "Invalid definition of a range.");
            return false;
        }
        return true;
    }
    

    retVal2 = Value();
    Parser::PushBackToken(tok);
    return true;
}

