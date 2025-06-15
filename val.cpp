#include "val.h"

Value Value::operator+(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp + op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(Rtemp + op.GetReal());
    }
    else {
        return Value();  
    }
}

Value Value::operator*(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp * op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(Rtemp * op.GetReal());
    }
    else {
        return Value();
    }
}

Value Value::operator-(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp - op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        return Value(Rtemp - op.GetReal());
    }
    else {
        return Value();
    }
}

Value Value::operator%(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        if (op.GetInt() == 0) {
            return Value(); 
        }
        return Value(Itemp % op.GetInt());
    }
    else {
        return Value();  
    }
}

Value Value::operator/(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        if (op.GetInt() == 0) {
            return Value();
        }
        return Value(Itemp / op.GetInt());
    }
    else if (IsReal() && op.IsReal()) {
        if (op.GetReal() == 0.0) {
            return Value();
        }
        return Value(Rtemp / op.GetReal());
    }
    else {
        return Value();
    }
}
Value Value::operator!=(const Value& op) const {
    if (GetType() != op.GetType()) {
        return Value(); 
    }
    if (IsInt()) {
        return Value(Itemp != op.GetInt());
    }
    else if (IsReal()) {
        return Value(Rtemp != op.GetReal());
    }
    else if (IsChar()) {
        return Value(Ctemp != op.GetChar());
    }
    else if (IsString()) {
        return Value(Stemp != op.GetString());
    }
    else if (IsBool()) {
        return Value(Btemp != op.GetBool());
    }
    else {
        return Value(); 
    }
}



Value Value::operator>(const Value& op) const {
    if (GetType() != op.GetType()) {
        return Value(); 
    }
    
    if (IsInt()) {
        return Value(Itemp > op.GetInt());
    }
    else if (IsReal()) {
        return Value(Rtemp > op.GetReal());
    }
    else if (IsChar()) {
        return Value(Ctemp > op.GetChar());
    }
    else if (IsString()) {
        return Value(Stemp > op.GetString());
    }
    else {
        return Value(); 
    }
}
Value Value::operator==(const Value& op) const {
    if (GetType() != op.GetType()) {
        return Value();
    }
    if (IsInt()) {
        return Value(Itemp == op.GetInt());
    }
    else if (IsReal()) {
        return Value(Rtemp == op.GetReal());
    }
    else if (IsChar()) {
        return Value(Ctemp == op.GetChar());
    }
    else if (IsString()) {
        return Value(Stemp == op.GetString());
    }
    else if (IsBool()) {
        return Value(Btemp == op.GetBool());
    }
    else {
        return Value();
    }
}

Value Value::Concat(const Value& op) const {
    if (IsString() && op.IsString()) {
        return Value(Stemp + op.GetString());
    }
    else if (IsString() && op.IsChar()) {
        string result = Stemp + op.GetChar();
        return Value(result);
    }
    else if (IsChar() && op.IsString()) {
        string result(1, Ctemp);
        result += op.GetString();
        return Value(result);
    }
    else if (IsChar() && op.IsChar()) {
        string result(1, Ctemp);
        result += op.GetChar();
        return Value(result);
    }
    else {
        return Value();
    }
}

Value Value::operator>=(const Value& op) const {
    if (GetType() != op.GetType()) {
        return Value();
    }
    if (IsInt()) {
        return Value(Itemp >= op.GetInt());
    }
    else if (IsReal()) {
        return Value(Rtemp >= op.GetReal());
    }
    else if (IsChar()) {
        return Value(Ctemp >= op.GetChar());
    }
    else if (IsString()) {
        return Value(Stemp >= op.GetString());
    }
    else {
        return Value();
    }
}

Value Value::operator<(const Value& op) const {
    if (GetType() != op.GetType()) {
        return Value(); 
    }
    if (IsInt()) {
        return Value(Itemp < op.GetInt());
    }
    else if (IsReal()) {
        return Value(Rtemp < op.GetReal());
    }
    else if (IsChar()) {
        return Value(Ctemp < op.GetChar());
    }
    else if (IsString()) {
        return Value(Stemp < op.GetString());
    }
    else {
        return Value(); 
    }
}


Value Value::operator&&(const Value& op) const {
    if (IsBool() && op.IsBool()) {
        return Value(Btemp && op.GetBool());
    }
    else {
        return Value();
    }
}

Value Value::operator<=(const Value& op) const {
    if (GetType() != op.GetType()) {
        return Value(); 
    }
    if (IsInt()) {
        return Value(Itemp <= op.GetInt());
    }
    else if (IsReal()) {
        return Value(Rtemp <= op.GetReal());
    }
    else if (IsChar()) {
        return Value(Ctemp <= op.GetChar());
    }
    else if (IsString()) {
        return Value(Stemp <= op.GetString());
    }
    else {
        return Value(); 
    }
}



Value Value::operator!() const {
    if (IsBool()) {
        return Value(!Btemp);
    }
    else {
        return Value(); 
    }
}

Value Value::operator||(const Value& op) const {

    if (IsBool() && op.IsBool()) {
        return Value(Btemp || op.GetBool());
    }
    else {
        return Value();
    }
}

Value Value::Exp(const Value& op) const {
    if (IsReal()) {
        double base = Rtemp;
        if (!op.IsInt() && !op.IsReal()) {
            return Value(); 
        }
        
        double exponent;
        if (op.IsInt()) {
            exponent = static_cast<double>(op.GetInt());
        }
        else { 
            exponent = op.GetReal();
        }
        
       
        if (base == 0.0) {
            if (exponent == 0.0) {
                return Value(1.0); 
            }
            return Value(0.0);
        }
        if (exponent == 0.0) {
            return Value(1.0); 
        }
        if (exponent < 0) {
            double result = 1.0 / pow(base, -exponent);
            return Value(result);
        }
        return Value(pow(base, exponent));
    }
    else {
        return Value(); 
    }
}



