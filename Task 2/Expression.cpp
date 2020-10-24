#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
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
    virtual Expression* clone() const = 0;
    virtual bool equal_to(Expression *ex) const = 0;
    friend Expression* simplify(Expression *ex);
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

    //derivative builder
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
    
    //clone function
    Expression* clone() const
    {
        return new Number(num);
    }
    
    //Simplification
    Expression* simplification() const
    {
        return clone();
    }
    
    //Overloaded ==
    bool equal_to(Expression* ex) const
    {
        Number* nn = dynamic_cast<Number*>(ex);
        if (nn == nullptr)
            return false;
    else
    return nn->num == num ;
    }
    
    friend Expression* simplify(Expression *ex);
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
        if(s == "")
            throw "Wrong evaluation";
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
    
    Expression* clone() const
    {
        return new Variable(str);
    }
    
    bool equal_to(Expression* ex) const
    {
        Variable* nn = dynamic_cast<Variable*>(ex);
        if (nn == nullptr)
            return false;
    else
    return nn->str == str;
    }
    
     friend Expression* simplify(Expression *ex);
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
    
    Expression* clone() const
    {
        return new Add(left->clone(), right->clone());
    }
    

   bool equal_to(Expression* ex) const
   {
       Add* nn = dynamic_cast<Add*>(ex);
       if (nn == nullptr)
           return false;
   else
   return (nn->left)->equal_to(left) && (nn->right)->equal_to(right);
   }
    
   friend Expression* simplify(Expression *ex);
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
    
    Expression* clone() const
    {
        return new Sub(left->clone(), right->clone());
    }
    
    bool equal_to(Expression* ex) const
    {
        Sub* nn = dynamic_cast<Sub*>(ex);
        if (nn == nullptr)
            return false;
        else
            return (nn->left)->equal_to(left) && (nn->right)->equal_to(right);
    }
    
     friend Expression* simplify(Expression *ex);
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
        return new Add(new Mul(left->derivative(str), right->clone()), new Mul(left->clone(), right->derivative(str)));
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
    
    Expression* clone() const
    {
        return new Mul(left->clone(), right->clone());
    }
    
    bool equal_to(Expression* ex) const
    {
        Mul* nn = dynamic_cast<Mul*>(ex);
        if (nn == nullptr)
            return false;
    else
    return (nn->left)->equal_to(left) && (nn->right)->equal_to(right);
    }
    
     friend Expression* simplify(Expression *ex);
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
        return new Div(new Sub(new Mul(numerator->derivative(str), denominator->clone()), new Mul(numerator->clone(), denominator->derivative(str))),
        new Mul(denominator->clone(), denominator->clone()));
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
    
    Expression* clone() const
    {
        return new Div(numerator->clone(), denominator->clone());
    }
    
    bool equal_to(Expression* ex) const
    {
        Div* nn = dynamic_cast<Div*>(ex);
        if (nn == nullptr)
            return false;
    else
    return (nn->numerator)->equal_to(numerator) && (nn->denominator)->equal_to(denominator);
    }
    
     friend Expression* simplify(Expression *ex);
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

Expression* calculation(Expression* ex)
{
    double x;
    try
    {
        x = ex->eval("");
        return new Number(x);
    }
    catch(const char *s)
    {
        throw "Cannot calculate expression. Try to simplify it!";
    }
}

Expression* simplify(Expression *ex)
{
    Variable* variable = dynamic_cast<Variable*>(ex);
    Number* number = dynamic_cast<Number*>(ex);
    Add* add = dynamic_cast<Add*>(ex);
    Mul* mul = dynamic_cast<Mul*>(ex);
    Sub* sub = dynamic_cast<Sub*>(ex);
    Div* div = dynamic_cast<Div*>(ex);
    if(number != nullptr)
        return ex->clone();
    if(variable != nullptr)
        return ex->clone();
    if(add != nullptr)
        {
            add->left = simplify(add->left);
            add->right = simplify(add->right);
            Number *l = dynamic_cast<Number*>(add->left);
            Number *r = dynamic_cast<Number*>(add->right);
            if(l != nullptr)
                if(l->num == 0)
                    return (add->right)->clone();
            if(r != nullptr)
                if(r->num == 0)
                    return (add->left)->clone();
        }
        
        if(sub != nullptr)
        {
            sub->left = simplify(sub->left);
            sub->right = simplify(sub->right);
            if((sub->left)->equal_to(sub->right))
                return new Number(0);
        }
        
        if(mul != nullptr)
        {
            mul->left = simplify(mul->left);
            mul->right = simplify(mul->right);
            Number* l = dynamic_cast<Number*>(mul->left);
            Number* r = dynamic_cast<Number*>(mul->right);
            if(l != nullptr)
            {
                if(l->num == 1)
                    return (mul->right)->clone();
                if(l->num == 0)
                    return new Number(0);
            }
            if(r != nullptr)
            {
                if(r->num == 1)
                    return (mul->left)->clone();
                if(r->num == 0)
                    return new Number(0);
            }
        }
        
        if(div != nullptr)
        {
            div->numerator = simplify(div->numerator);
            div->denominator = simplify(div->denominator);
        }
        return ex->clone();
}

int main()
{
    string str;
    cin >> str;
    Expression* e1 = get_expression(str);
    Expression* e2 = simplify(e1);
    try
    {
        double x = e2->eval("");
        cout << "result of expression: " << x << endl;
    } catch (const char *s)
    {
        cout << "Cannot calculate expression: " << endl;
        e2->print(cout);
    }
    delete e1;
    delete e2;
    return 0;
}
