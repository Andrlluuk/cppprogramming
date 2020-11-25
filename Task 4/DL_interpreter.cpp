#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

bool is_digit(char c)
{
    return isdigit(c) || (c == '-');
}
class Expression
{
public:
    virtual ~Expression(){}
    virtual Expression* eval() = 0;
    virtual Expression* clone() const = 0;
    virtual ostream& print(ostream& out) const = 0;
};

class Env
{
public:
    unordered_map<string, Expression*> env;

    Expression* fromEnv(string& s)
    {
        return env[s];
    }
};


class Val: public Expression
{
public:
    int val;
    Env* data;
    
    Val(int i, Env* data):val(i), data(data){};
    
    Expression* eval()
    {
        return clone();
    }
    
    Expression* clone() const
    {
        return new Val(val, data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(val " << val << ")";
        return out;
    }
};

int getValue(Expression *ex)
{
    Val *i = dynamic_cast<Val*>(ex);
    if(i != nullptr)
        return i->val;
    else
        throw "Call getValue function from non-val object";
}


class Var :public Expression
{
protected:
    string id;
    Env* data;
    friend class Call;
public:
    
    Var(string s, Env* e): id(s), data(e)
    {}
    
    Expression* eval()
    {
        try
        {
            Expression* x = data->fromEnv(id);
            return x;
        }
        catch(...)
        {
            throw "Haven't found expression for this id";
        }
    }
    
    Expression* clone() const
    {
        return new Var(id, data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(var " << id << ")";
        return out;
    }
    
};

class Add: public Expression
{
    Expression* right;
    Expression* left;
    Env* data;
public:
    Add(Expression* left, Expression* right, Env* data):left(left), right(right), data(data){}
    
    ~Add()
    {
        delete left;
        delete right;
    }
    
    Expression* eval()
    {
        return new Val(getValue(left->eval()) + getValue(right->eval()), data);
    }
    
    Expression* clone() const
    {
        return new Add(left->clone(), right->clone(), data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(add ";
        right->print(out);
        out << " ";
        left->print(out);
        out << ")";
        return out;
    }
};


class If : public Expression
{
    Expression* left_cond;
    Expression* right_cond;
    Expression* e_then;
    Expression* e_else;
    Env* data;

public:
    If(Expression* left_cond, Expression* right_cond, Expression* e_then, Expression* e_else, Env* data):left_cond(left_cond), right_cond(right_cond), e_then(e_then), e_else(e_else), data(data){}
    
    ~If()
    {
        delete left_cond;
        delete right_cond;
        delete e_then;
        delete e_else;
    }
    
    Expression* eval()
    {
        if(getValue(left_cond->eval()) > getValue(right_cond->eval()))
            return e_then->eval();
        else
            return e_else->eval();
    }
    Expression* clone() const
    {
        return new If(left_cond->clone(), right_cond->clone(), e_then->clone(), e_else->clone(), data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(if ";
        left_cond->print(out);
        out << " ";
        right_cond->print(out);
        out << ") then ";
        e_then->print(out);
        out << " else ";
        e_else->print(out);
        return out;
    }
};


class Let:public Expression
{
    string id;
    Expression *e1;
    Expression *e2;
    Env* data;
    
public:
    Let(string id, Expression *e1, Expression *e2, Env* data): id(id), e1(e1), e2(e2), data(data){}
    
    ~Let()
    {
        delete e1;
        delete e2;
    }
    
    Expression* eval()
    {
        Expression *ex = e1->eval();
        data->env[id] = ex;
        return e2->eval();
    }
    
    Expression* clone() const
    {
        return new Let(id, e1->clone(), e2->clone(),data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(let " << id << " = ";
        e1->print(out);
        out << " in ";
        e2->print(out);
        return out;
    }
};

class Function:public Expression
{
    string id;
    Expression *ex1;
    Env* data;
    friend class Call;
public:
    Function(string id, Expression *ex1, Env* data):id(id), ex1(ex1), data(data){}
    
    ~Function()
    {
        delete ex1;
    }
    
    Expression *eval()
    {
        return this;
    }
    
    Expression* clone() const
    {
        return new Function(id, ex1->clone(), data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(function " << id << " ";
        ex1->print(out);
        out << ")";
        return out;
    }
};


class Call: public Expression
{
    Expression *f_expr;
    Expression *arg_expr;
    Env *data;
    
public:
    Call(Expression *e1, Expression *e2, Env* data):f_expr(e1), arg_expr(e2), data(data){}
    
    ~Call()
    {
        delete f_expr;
        delete arg_expr;
    }

    Expression *eval()
    {
        Expression* e_n = f_expr->eval();
        Val *f = dynamic_cast<Val*>(e_n);
        if(f != nullptr)
        {
            throw "Invalid eval procedure call";
        }
        else
        {
            Function *func = dynamic_cast<Function*>(e_n);
            string id = func->id;
            Expression* body = func->ex1;
            data->env[id] = arg_expr->eval();
            return body->eval();
        }
    }
    
    Expression *clone() const
    {
        return new Call(f_expr->clone(), arg_expr->clone(), data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(call ";
        f_expr->print(out);
        out << " ";
        arg_expr->print(out);
        out << ")";
        return out;
    }
};

class Set: public Expression
{
    string id;
    Expression *expr;
    Env *data;
    
public:
    Set(string id, Expression* expr, Env* data):id(id), expr(expr), data(data){};
    
    ~Set()
    {
        delete expr;
    }
    
    Expression *eval()
    {
        data->env[id] = expr->eval();
        return this;
    }
    Expression *clone() const
    {
        return new Set(id, expr->clone(), data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(set " << id << " ";
        expr->print(out);
        out << ")";
        return out;
    }
};

class Block: public Expression
{
    vector<Expression*> exprs;
    Env* data;
    
public:
    
    Block(vector<Expression*> &s, Env* data):data(data)
    {
        for(int i = 0; i < s.size(); i++)
            exprs.push_back(s[i]);
    };
    
    ~Block()
    {}
    
    Expression *eval()
    {
        vector<Expression*> evaluations;
        for(int i = 0 ; i < exprs.size(); i++)
            evaluations.push_back(exprs[i]->eval());
        return evaluations[exprs.size() - 1];
        
    }
    
    Expression *clone() const
    {
        vector<Expression*> evs;
        for(int i = 0; i < exprs.size(); i++)
            evs.push_back(exprs[i]->clone());
        return new Block(evs, data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(block ";
        for(int i = 0; i < exprs.size(); i++)
        {
            exprs[i]->print(out);
            out << " ";
        }
        out << ")";
        return out;
    }
};

class Arr: public Expression
{
    vector<Expression*> arr;
    Env* data;
    
public:
    
    Arr(vector<Expression*> &exprs, Env *data):data(data)
    {
        for(int i = 0; i < exprs.size(); i++)
            arr.push_back(exprs[i]);
    }
    
    ~Arr()
    {}
    
    Expression *eval()
    {
        vector<Expression*> evals;
        for(int i = 0; i < arr.size(); i++)
            evals.push_back(arr[i]->eval());
        Expression *arr_eval = new Arr(evals, data);
        return arr_eval;
    }
    
    Expression *clone() const
    {
        vector<Expression*> evs;
        for(int i = 0; i < evs.size(); i++)
            evs.push_back(evs[i]->clone());
        return new Arr(evs, data);
    }
    
    int get_size()
    {
        return arr.size();
    }
    
    Expression* operator[](int idx)
    {
        return arr[idx];
    }
    
    ostream& print(ostream& out) const
    {
        out << "(arr ";
        for(int i = 0; i < arr.size(); i++)
        {
            arr[i]->print(out);
            out << " ";
        }
        out << ")";
        return out;
    }
};

class Gen: public Expression
{
    Expression* e_length;
    Expression* e_function;
    Env* data;
    
public:
    Gen(Expression *e_length, Expression* e_function, Env* data):e_length(e_length), e_function(e_function), data(data){}
    
    ~Gen()
    {
        delete e_length;
        delete e_function;
    }
    
    Expression* clone() const
    {
        return new Gen(e_length->clone(), e_function->clone(), data);
    }
    
    Expression* eval()
    {
        vector<Expression*> expr;
        int len = getValue(e_length->eval());
        for(int i = 0; i < len; i++)
        {
            Expression* arr_el = new Call(e_function, new Val(i,data), data);
            expr.push_back(arr_el->eval());
        }
        Expression *ret_arr = new Arr(expr, data);
        return ret_arr;
    }
    
    ostream& print(ostream& out) const
    {
        out << "(gen ";
        e_length->print(out);
        out << " ";
        e_function->print(out);
        out << ")";
        return out;
    }
};

class At: public Expression
{
    Expression* array;
    Expression* index;
    Env* data;

public:
    At(Expression* array, Expression* index, Env* data):array(array), index(index), data(data){}
    
    ~At()
    {
        delete array;
        delete index;
    }
    
    Expression *eval()
    {
        Expression* ret_arr = array->eval();
        Arr* arr = dynamic_cast<Arr*>(ret_arr);
        if(arr == nullptr)
            throw "Got invalid structure in At class. Need an array!";
        int x = getValue(index->eval());
        if((x < 0) && (x >= arr->get_size()))
           throw "Got invalid index!";
           return (*arr)[x];
    }
    
    Expression* clone() const
    {
        return new At(array, index, data);
    }
    
    ostream& print(ostream& out) const
    {
        out << "(at ";
        array->print(out);
        out << " ";
        index->print(out) ;
        out << ")";
        return out;
    }
};


istream& operator >> (istream &in,  string &s)
{
    char ss[16];
    in.getline(ss, 16);
    s += ss;
    in.clear();
    while(strlen(ss))
    {
        in.getline(ss, 16);
        in.clear();
        s += ss;
    }
    return in;
}


string& get_expr_from_string(string& s)
{
    int cnt = 0;
    int begin = 0;
    int end = begin;
    while(!((s[begin] >= '(') && (s[begin] <= 'z')))
        begin++;
    if(s[begin] == '(')
    {
        cnt++;
        end = begin;
        end++;
        while(cnt != 0)
        {
            if(s[end] == '(')
                cnt++;
            if(s[end] == ')')
                cnt--;
            end++;
        }
        
    }
    else
    {
        end = begin;
        while((s[end] !=' ') && (end != s.length() - 1))
            end++;
    }
    if(end == begin)
    {
        s = s.substr(begin, end+1);
        return s;
    }
    if(end == s.length() - 1)
    {
        string *t = new string(s);
        return *t;
    }
    string *u = new string(s.substr(begin, end));
    s = s.substr(min(end, (int) s.length()), s.length());
    return *u;
}

int calculate_number_of_expressions(vector<string> &s, int beginning)
{
    int cnt = 0;
    int idx = beginning;
    int number_of_expressions = 0;
    while(cnt != -1)
    {
        if(s[idx] == "(")
            cnt++;
        if(s[idx] == ")")
            cnt--;
        if(cnt == 0)
            number_of_expressions++;
        idx++;
    }
    return number_of_expressions;
}


Expression* parse(vector<string>& s, Env* e, int &beginning)
{
    Expression* res;
    
    beginning++;

    if (s[beginning] == "val")
    {
        beginning++;
        res = new Val(stoi(s[beginning]), e);
        beginning++;
    }
    else if (s[beginning] == "var")
    {
        beginning++;
        res = new Var(s[beginning], e);
        beginning++;
    }
    else if (s[beginning] == "add")
    {
        beginning++;
        Expression* e1 = parse(s, e, beginning);
        Expression* e2 = parse(s, e, beginning);
        res = new Add(e1, e2, e);
    }
    else if (s[beginning] == "if")
    {
        beginning++;
        Expression* e1 = parse(s, e, beginning);
        Expression* e2 = parse(s, e, beginning);
        beginning++;
        Expression* e_then = parse(s, e, beginning);
        beginning++;
        Expression* e_else = parse(s, e, beginning);
        res = new If(e1, e2, e_then, e_else, e);
    }
    else if (s[beginning] == "let")
    {
        beginning++;
        string id = s[beginning];
        beginning++;
        beginning++;
        Expression* e_val = parse(s,e, beginning);
        beginning++;
        Expression* e_body = parse(s,e, beginning);
        res = new Let(id, e_val, e_body, e);
    }
    else if (s[beginning] == "function")
    {
        beginning++;
        string id = s[beginning];
        beginning++;
        Expression* f_body = parse(s, e, beginning);
        res = new Function(id, f_body, e);
    }
    else if (s[beginning] == "call")
    {
        beginning++;
        Expression* e1 = parse(s, e, beginning);
        Expression* e2 = parse(s, e, beginning);
        res = new Call(e1, e2, e);
    }
    else if (s[beginning] == "set")
    {
        beginning++;
        string id = s[beginning];
        beginning++;
        Expression* e1 = parse(s, e, beginning);
        res = new Set(id, e1, e);
    }
    else if (s[beginning] == "block")
    {
        beginning++;
        int x = calculate_number_of_expressions(s, beginning);
        vector<Expression*> ex;
        for(int i = 0; i < x; i++)
            ex.push_back(parse(s, e, beginning));
        res = new Block(ex, e);
    }
    else if (s[beginning] == "arr")
    {
        beginning++;
        int x = calculate_number_of_expressions(s, beginning);
        vector<Expression*> ex;
        for(int i = 0; i < x; i++)
            ex.push_back(parse(s, e, beginning));
        res = new Arr(ex, e);
    }
    else if (s[beginning] == "gen")
    {
        beginning++;
        Expression* e1 = parse(s, e, beginning);
        Expression* e2 = parse(s, e, beginning);
        res = new Gen(e1, e2, e);
    }
    else if (s[beginning] == "at")
    {
        beginning++;
        Expression *arr = parse(s, e, beginning);
        Expression *at = parse(s, e, beginning);
        res = new At(arr, at, e);
    }
    beginning++;
    return res;
}

vector<string> get(istream& in)
{
    vector<string> fin;
    string ss;
    in >> ss;
    string phrase;
    if (!isspace(ss[0]))
        phrase += ss[0];
    int len = ss.length();
    for (int i = 1; i < len; i++)
    {
        if (is_digit(ss[i - 1]) && is_digit(ss[i]))
        {
            phrase += ss[i];
        }
        else if(isalpha(ss[i-1]) && isalpha(ss[i]))
        {
            phrase += ss[i];
        }
        else if (isspace(ss[i]))
        {
            if(phrase.length())
                fin.push_back(phrase);
            phrase = "";
        }
        else
        {
            if (phrase.length())
                fin.push_back(phrase);
            phrase = "";
            phrase += ss[i];
        }
    }
    if (!phrase.empty())
        fin.push_back(phrase);
return fin;
}

int main()
{
    ifstream in;
    in.open("input.txt");
    ofstream out;
    out.open("output.txt");
    Env e;
    int beginning = 0;
    vector<string> program;
    program = get(in);
    Expression *ex = parse(program, &e, beginning);
    try
    {
        Expression *ex1 = ex->eval();
        ex1->print(out);
        if(getValue(ex1))
            out << "(val " << getValue(ex1) << ")";
    }
    catch(...)
    {
        out << "ERROR" << endl;
    }
    delete ex;
    in.close();
    out.close();
    return 0;
}
