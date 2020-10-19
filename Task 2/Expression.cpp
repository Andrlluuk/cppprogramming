#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//Expression base class
class Expression
{;
public:
    Expression(){}
    virtual ostream& print(ostream& out) const = 0;
    virtual Expression* derivative(string v) const = 0;
    virtual double eval(string str) const = 0;
    virtual ~Expression() {}
};

//Derived classes
class Number: public Expression
{
    double num;
public:
    
    //Constructor
    Number(double num): num(num)
    {}
    
    //output method
    ostream& print(ostream& out) const
    {
        out << num;
        return out;
    }

    Expression* derivative(string str) const
    {
        return new Number(0);
    }
    
    //expression evaluation method
    double eval(string str) const
    {
        return num;
    }
    
    //destructor
    ~Number()
    {}
    
};

//Variable class
class Variable: public Expression
{
    string str;
public:
    Variable(string str): str(str)
    {}

    ostream& print(ostream& out) const
    {
        out << str;
        return out;
    }

    Expression* derivative(string s) const
    {
        if (s == str)
            return new Number(1);
        else
            return new Number(0);
    }

    double eval(string s) const
    {
        size_t pos = s.find(str);
        if(s.find(';', pos + 1) != string::npos)
        {
            size_t end = s.find(";", pos + 1);
            size_t begin = s.find(" ", pos + 1);
            double value = stod(s.substr(begin + 4, end - 1));
            return value;
        }
        size_t begin = s.find(" ", pos + 1);
        double value = stod(s.substr(begin + 4, s.length() - 1));
        return value;
    }
    
    ~Variable()
    {}
};

//addition class
class Add: public Expression
{
    Expression* left;
    Expression* right;
public:
    Add(Expression* left, Expression* right): left(left), right(right)
    {}

    ostream& print(ostream& out) const
    {
        out << "(";
        left->print(out);
        out << "+";
        right->print(out);
        out << ")";
        return out;
    }
    
    Expression* derivative(string str) const
    {
        return new Add(left->derivative(str), right->derivative(str));
    }
    
    double eval(string str) const
    {
        return left->eval(str) + right->eval(str);
    }
    
    ~Add()
    {
        delete left;
        delete right;
    }
};

//Subtraction class
class Sub: public Expression
{
    Expression *left;
    Expression *right;
public:
    Sub(Expression *left, Expression *right): left(left), right(right)
    {}

    ostream& print(ostream& out) const
    {
        out << "(";
        left->print(out);
        out << "-";
        right->print(out);
        out << ")";
        return out;
    }
    
    Expression* derivative(string str) const
    {
        return new Sub(left->derivative(str), right->derivative(str));
    }
    
    double eval(string str) const
    {
         return left->eval(str) - right->eval(str);
    }
    
    ~Sub()
    {
        delete left;
        delete right;
    }
};

//Multiplication class
class Mul: public Expression
{
    Expression *left;
    Expression *right;
public:
    Mul(Expression *left, Expression *right): left(left), right(right)
    {}

    ostream& print(ostream& out) const
    {
        out << "(";
        left->print(out);
        out << "*";
        right->print(out);
        out << ")";
        return out;
    }
    
    Expression* derivative(string str) const
    {
        return new Add(new Mul(left->derivative(str), right), new Mul(left, right->derivative(str)));
    }
    
    double eval(string str) const
    {
         return left->eval(str) * right->eval(str);
    }
    
    ~Mul()
    {
        delete left;
        delete right;
    }
};

//Division class
class Div: public Expression
{
    Expression *numerator;
    Expression *denominator;
public:
    Div(Expression *numerator, Expression *denominator): numerator(numerator), denominator(denominator)
    {}

    ostream& print(ostream& out) const
    {
        out << "(";
        numerator->print(out);
        out << "/";
        denominator->print(out);
        out << ")";
        return out;
    }
    
    Expression* derivative(string str) const
    {
        return new Div(new Sub(new Mul(numerator->derivative(str), denominator), new Mul(numerator, denominator->derivative(str))),
        new Mul(denominator, denominator));
    }
    
    double eval(string str) const
    {
         return numerator->eval(str) / denominator->eval(str);
    }
    
    ~Div()
    {
        delete numerator;
        delete denominator;
    }
    
};

/*get type of expression
 return 1 if expression included outside scopes
 return 0 if not*/
bool type_of_expression(string str)
{
    if((str[0] == '(') && (str[str.length() - 1] == ')'))
    {
        string s = str.substr(1, str.length() - 2);
        bool ind = 1;
        int cnt = 0;
        for(int i = 0; i < s.length(); i++)
        {
            if(s[i] == '(')
                cnt++;
            if(s[i] == ')')
                cnt--;
            if(cnt < 0)
                return 0;
        }
        return 1;
    }
    return 0;
}

//build expression from string
Expression* get_expression(string str){
    int i;
    bool ind = 1;
    for(i = 0; i < str.length(); i++)
        if((str[i] == ')') || (str[i] == '*') || (str[i] == '/') || (str[i] == '+') || (str[i] == '-'))
            ind = 0;
    if(ind)
    {
        if(str[0] < '9')
            return new Number(stod(str));
        return new Variable(str);
    }
    if(type_of_expression(str))
        str = str.substr(1, str.length() - 2);
    i = str.length();
    int cnt = 0;
    if (str[i] == '(')
        cnt++;
    if (str[i] == ')')
        cnt--;
    i--;
    while (((cnt != 0) || ((str[i] != '+') && (str[i] != '-'))) && (i >=0 ))
    {
        if (str[i] == '(')
            cnt++;
        if (str[i] == ')')
            cnt--;
        i--;
    }
        if(i >= 0)
        {
        string left = str.substr(0, i);
        string right = str.substr(i + 1, str.length() - i - 1);
        if (str[i] == '+')
            return new Add(get_expression(left), get_expression(right));
        if (str[i] == '-')
            return new Sub(get_expression(left), get_expression(right));
        }
    {
        cnt = 0;
        i = str.length();
        
        if (str[i] == '(')
            cnt++;
        if (str[i] == ')')
            cnt--;
        i--;
        while ((cnt != 0) || ((str[i] != '*') && (str[i] != '/') ))
        {
            if (str[i] == '(')
                cnt++;
            if (str[i] == ')')
                cnt--;
            i--;
        }
        string left = str.substr(0, i);
        string right = str.substr(i + 1, str.length() - i - 1);
    if (str[i] == '*')
        return new Mul(get_expression(left), get_expression(right));
    return new Div(get_expression(left), get_expression(right));
    }
}


int main()
{
    string str;
    cin >> str;
    
    Expression* e1 = get_expression(str);
    e1->print(cout);
    Expression* de1 = e1->derivative("x");
    de1->print(cout);
    cout << endl;
    double res1 = e1->eval("x <- 10; y <- 13");
    cout << res1;
    delete e1;
    return 0;
}

