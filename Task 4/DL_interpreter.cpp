#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <sstream>
#include <stack>
using namespace std;

bool is_digit(char c)
{
    return isdigit(c) || (c == '-');
}


class Expression;

typedef unordered_map<string, Expression*> Env;

stack<Expression*> memory_stack;

void deallocate(stack<Expression*> mem_stack)
{
    int x = 0;
    while(!mem_stack.empty())
    {
        Expression *ex = mem_stack.top();
        if(ex != nullptr)
        {
            delete ex;
        }
        mem_stack.pop();
        x++;
    }
}
class Expression
{
public:
    virtual ~Expression(){}
    virtual Expression* eval(Env& env) = 0;
    virtual Expression* clone() const = 0;
    virtual ostream& print(ostream& out) const = 0;
};


class Val: public Expression
{

    int val;
    
public:
    
    Val(int i):val(i){};
    
    Expression* eval(Env& env)
    {
        return clone();
    }
    
    Expression* clone() const
    {
        Expression *x = new Val(val);
        memory_stack.push(x);
        return x;
    }
    
    ostream& print(ostream& out) const
    {
        out << "(val " << val << ")";
        return out;
    }
    
    friend int getValue(Expression* ex);
    
    ~Val()
    {}
};

int getValue(Expression *ex)
{
    Val *i = dynamic_cast<Val*>(ex);
    if(i != nullptr)
    {
        int x = i->val;
        return x;
    }
    else
    {
        ostringstream out;
        ex->print(out);
        throw invalid_argument("Try to call getValue function from " + out.str());
    }
        
}


class Var :public Expression
{
protected:
    string id;
    friend class Call;
public:
    
    Var(string s): id(s)
    {}
    
    Expression* eval(Env& env)
    {
        Expression* x = env[id];
        if(x != NULL)
            return x;
        else
        {
            ostringstream out;
            this->print(out);
            throw invalid_argument("Try to call eval function in Var class from " + out.str() + " and didn't find evaluation of this expression");
        }
    }
    
    Expression* clone() const
    {
        Expression* x = new Var(id);
        memory_stack.push(x);
        return x;
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
public:
    Add(Expression* left, Expression* right): left(left), right(right){}
    
    ~Add()
    {
        delete left;
        delete right;
    }
    
    Expression* eval(Env& env)
    {
        Expression* x = new Val(getValue(left->eval(env)) + getValue(right->eval(env)));
        memory_stack.push(x);
        return x;
    }
    
    Expression* clone() const
    {
        Expression* x = new Add(left->clone(), right->clone());
        memory_stack.push(x);
        return x;
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

public:
    If(Expression* left_cond, Expression* right_cond, Expression* e_then, Expression* e_else):left_cond(left_cond), right_cond(right_cond), e_then(e_then), e_else(e_else){}
    
    ~If()
    {
        delete left_cond;
        delete right_cond;
        delete e_then;
        delete e_else;
    }
    
    Expression* eval(Env& env)
    {
        if(getValue(left_cond->eval(env)) > getValue(right_cond->eval(env)))
            return e_then->eval(env);
        else
            return e_else->eval(env);
    }
    
    Expression* clone() const
    {
        Expression* x = new If(left_cond->clone(), right_cond->clone(), e_then->clone(), e_else->clone());
        memory_stack.push(x);
        return x;
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
    
public:
    Let(string id, Expression *e1, Expression *e2): id(id), e1(e1), e2(e2){}
    
    ~Let()
    {
        delete e1;
        delete e2;
    }
    
    Expression* eval(Env& env)
    {
        Env new_env = env;
        new_env[id] = e1->eval(env);
        return e2->eval(new_env);
    }
    
    Expression* clone() const
    {
        Expression* x = new Let(id, e1->clone(), e2->clone());
        memory_stack.push(x);
        return x;
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
    friend class Call;
    
public:
    Function(string id, Expression *ex1):id(id), ex1(ex1){}
    
    ~Function()
    {
        delete ex1;
    }
    
    Expression *eval(Env& env)
    {
        return clone();
    }
    
    Expression* clone() const
    {
        Expression *x = new Function(id, ex1->clone());
        memory_stack.push(x);
        return x;
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
    
public:
    Call(Expression *e1, Expression *e2):f_expr(e1), arg_expr(e2)
    {}
    
    ~Call()
    {
        delete f_expr;
        delete arg_expr;
    }

    Expression *eval(Env& env)
    {
        Expression* e_n = f_expr->eval(env);
        Val *f = dynamic_cast<Val*>(e_n);
        if(f != nullptr)
        {
            throw "Invalid eval procedure call";
        }
        else
        {
            Env new_env = env;
            Function *func = dynamic_cast<Function*>(e_n);
            string id = func->id;
            Expression* body = func->ex1;
            new_env[id] = arg_expr->eval(env);
            return body->eval(new_env);
        }
    }
    
    Expression *clone() const
    {
        Expression *x = new Call(f_expr->clone(), arg_expr->clone());
        memory_stack.push(x);
        return x;
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
    
public:
    Set(string id, Expression* expr):id(id), expr(expr){};
    
    ~Set()
    {
        delete expr;
    }
    
    Expression *eval(Env& env)
    {
        Env new_env;
        new_env[id] = expr->eval(env);
        return this;
    }
    Expression *clone() const
    {
        Expression *x = new Set(id, expr->clone());
        memory_stack.push(x);
        return x;
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
    
public:
    
    Block(vector<Expression*> &s)
    {
        exprs.resize(s.size());
        for(int i = 0; i < s.size(); i++)
            exprs[i] = s[i];
    };
    
    ~Block()
    {}
    
    Expression *eval(Env& env)
    {
        vector<Expression*> evaluations;
        evaluations.resize(exprs.size());
        for(int i = 0 ; i < exprs.size(); i++)
            evaluations[i] = (exprs[i])->eval(env);
        return evaluations[exprs.size() - 1];
        
    }
    
    Expression *clone() const
    {
        vector<Expression*> evs;
        evs.resize(exprs.size());
        for(int i = 0; i < exprs.size(); i++)
            evs[i] = exprs[i]->clone();
        Expression *x = new Block(evs);
        memory_stack.push(x);
        return x;
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
    
public:
    
    Arr(vector<Expression*> &exprs)
    {
        arr.resize(exprs.size());
        for(int i = 0; i < exprs.size(); i++)
            arr[i] = exprs[i];
    }
    
    ~Arr()
    {}
    
    Expression *eval(Env& env)
    {
        vector<Expression*> evals;
        evals.resize(arr.size());
        for(int i = 0; i < arr.size(); i++)
            evals[i] = arr[i]->eval(env);
        Expression *arr_eval = new Arr(evals);
        memory_stack.push(arr_eval);
        return arr_eval;
    }
    
    Expression *clone() const
    {
        vector<Expression*> evs;
        for(int i = 0; i < evs.size(); i++)
            evs.push_back(evs[i]->clone());
        Expression *ex = new Arr(evs);
        memory_stack.push(ex);
        return ex;
    }
    
    int get_size()
    {
        return arr.size();
    }
    
    Expression* operator[](int idx)
    {
        return arr.at(idx);
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
    
public:
    Gen(Expression *e_length, Expression* e_function):e_length(e_length), e_function(e_function){}
    
    ~Gen()
    {
        delete e_length;
        delete e_function;
    }
    
    Expression* clone() const
    {
        Expression *ex = new Gen(e_length->clone(), e_function->clone());
        memory_stack.push(ex);
        return ex;
    }
    
    Expression* eval(Env& env)
    {
        vector<Expression*> expr;
        int len = getValue(e_length->eval(env));
        for(int i = 0; i < len; i++)
        {
            Expression* arr_el = new Call(e_function, new Val(i));
            expr.push_back(arr_el->eval(env));
        }
        Expression *ret_arr = new Arr(expr);
        memory_stack.push(ret_arr);
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

public:
    At(Expression* array, Expression* index):array(array), index(index){}
    
    ~At()
    {
        delete array;
        delete index;
    }
    
    Expression *eval(Env& env)
    {
        Expression* ret_arr = array->eval(env);
        Arr* arr = dynamic_cast<Arr*>(ret_arr);
        if(arr == nullptr)
            throw "Got invalid structure in At class. Need an array!";
        int x = getValue(index->eval(env));
        if((x < 0) && (x >= arr->get_size()))
           throw "Got invalid index!";
           return (*arr)[x];
    }
    
    Expression* clone() const
    {
        Expression *ex = new At(array, index);
        memory_stack.push(ex);
        return ex;
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


Expression* parse(vector<string>& s, int &beginning)
{
    Expression* res;
    
    beginning++;

    if (s[beginning] == "val")
    {
        beginning++;
        res = new Val(stoi(s[beginning]));
        beginning++;
    }
    else if (s[beginning] == "var")
    {
        beginning++;
        res = new Var(s[beginning]);
        beginning++;
    }
    else if (s[beginning] == "add")
    {
        beginning++;
        Expression* e1 = parse(s, beginning);
        Expression* e2 = parse(s, beginning);
        res = new Add(e1, e2);
    }
    else if (s[beginning] == "if")
    {
        beginning++;
        Expression* e1 = parse(s, beginning);
        Expression* e2 = parse(s, beginning);
        beginning++;
        Expression* e_then = parse(s, beginning);
        beginning++;
        Expression* e_else = parse(s, beginning);
        res = new If(e1, e2, e_then, e_else);
    }
    else if (s[beginning] == "let")
    {
        beginning++;
        string id = s[beginning];
        beginning++;
        beginning++;
        Expression* e_val = parse(s, beginning);
        beginning++;
        Expression* e_body = parse(s, beginning);
        res = new Let(id, e_val, e_body);
    }
    else if (s[beginning] == "function")
    {
        beginning++;
        string id = s[beginning];
        beginning++;
        Expression* f_body = parse(s, beginning);
        res = new Function(id, f_body);
    }
    else if (s[beginning] == "call")
    {
        beginning++;
        Expression* e1 = parse(s, beginning);
        Expression* e2 = parse(s, beginning);
        res = new Call(e1, e2);
    }
    else if (s[beginning] == "set")
    {
        beginning++;
        string id = s[beginning];
        beginning++;
        Expression* e1 = parse(s, beginning);
        res = new Set(id, e1);
    }
    else if (s[beginning] == "block")
    {
        beginning++;
        int x = calculate_number_of_expressions(s, beginning);
        vector<Expression*> ex;
        for(int i = 0; i < x; i++)
            ex.push_back(parse(s, beginning));
        res = new Block(ex);
    }
    else if (s[beginning] == "arr")
    {
        beginning++;
        int x = calculate_number_of_expressions(s, beginning);
        vector<Expression*> ex;
        for(int i = 0; i < x; i++)
            ex.push_back(parse(s, beginning));
        res = new Arr(ex);
    }
    else if (s[beginning] == "gen")
    {
        beginning++;
        Expression* e1 = parse(s, beginning);
        Expression* e2 = parse(s, beginning);
        res = new Gen(e1, e2);
    }
    else if (s[beginning] == "at")
    {
        beginning++;
        Expression *arr = parse(s, beginning);
        Expression *at = parse(s, beginning);
        res = new At(arr, at);
    }
    beginning++;
    memory_stack.push(res);
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
    int beginning = 0;
    vector<string> program;
    program = get(in);
    Env env;
    Expression *ex = parse(program, beginning);
    try
    {
        Expression *ex1 = ex->eval(env);
        out << "(val " << getValue(ex1) << ")";
    }
    catch(invalid_argument& e)
    {
        out << "ERROR" << endl;
        cout << e.what();
    }
    in.close();
    out.close();
    deallocate(memory_stack);
    return 0;
}
