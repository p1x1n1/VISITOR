﻿// patterns.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <cassert>
struct Transformer//это реализация паттерна проектирования Visitor
{
    virtual ~Transformer() {}
    virtual Expression* transformNumber(Number const*) = 0;
    virtual Expression* transformBinaryOperation(BinaryOperation const*) = 0;
    virtual Expression* transformFunctionCall(FunctionCall const*) = 0;
    virtual Expression* transformVariable(Variable const*) = 0;
};
struct CopySyntaxTree :Transformer {
    Expression* transformNumber(Number const* number)
    {
        return;
    }
    Expression* transformBinaryOperation(BinaryOperation const* binop)
    {
        return;
    }
    Expression* transformFunctionCall(FunctionCall const* fcall)
    {
        return;
    }
    Expression* transformVariable(Variable const* var)
    {
        return ; 
    }

};

struct Expression //базовая абстрактная структура представляет абстрактное
    //синтаксическое дерево(АСД)
{
    virtual ~Expression() { } //виртуальный деструктор
    virtual double evaluate() const = 0; //абстрактный метод «вычислить»
    virtual Expression* transform(Transformer* tr) const = 0;//acceptVisitor
};

struct Number : Expression // стуктура «Число»
{
    Number(double value) : value_(value) {} //конструктор
    double value() const { return value_; } // метод чтения значения числа
    double evaluate() const { return value_; } // реализация виртуального метода «вычислить»
    ~Number() {}//деструктор, тоже виртуальный
    Expression* transform(Transformer* tr) const
    {
        return tr->transformNumber(this);
    }

private:
    double value_; // само вещественное число
};

struct BinaryOperation : Expression // «Бинарная операция»
{
    enum { // перечислим константы, которыми зашифруем символы операций
        PLUS = '+',
        MINUS = '-',
        DIV = '/',
        MUL = '*'
    };
    // в конструкторе надо указать 2 операнда — левый и правый, а также сам символ операции
    BinaryOperation(Expression const* left, int op, Expression const* right)
        : left_(left), op_(op), right_(right)
    {
        assert(left_ && right_);//проверка корректности данных
    }
    ~BinaryOperation() //в деструкторе освободим занятую память
    {
        delete left_;
        delete right_;
    }
    Expression const* left() const { return left_; } // чтение левого операнда
    Expression const* right() const { return right_; } // чтение правого операнда
    int operation() const { return op_; } // чтение символа операции
    double evaluate() const // реализация виртуального метода «вычислить»
    {
        double left = left_->evaluate(); // вычисляем левую часть
        double right = right_->evaluate(); // вычисляем правую часть
        switch (op_) // в зависимости от вида операции, складываем, вычитаем, умножаем // или делим левую и правую части
        {
        case PLUS: return left + right;
        case MINUS: return left - right;
        case DIV: return left / right;
        case MUL: return left * right;
        }
    }
    Expression* transform(Transformer* tr) const
    {
        return tr->transformBinaryOperation(this);
    }
private:
    Expression const* left_; // указатель на левый операнд
    Expression const* right_; // указатель на правый операнд
    int op_; // символ операции
};

struct FunctionCall : Expression // структура «Вызов функции»
{
    // в конструкторе надо учесть имя функции и ее аргумент
    FunctionCall(std::string const& name, Expression const* arg) : name_(name),arg_(arg)
    {
        assert(arg_);
        assert(name_ == "sqrt" || name_ == "abs");
    } // разрешены только вызов //sqrt и abs
    std::string const& name() const
    {
        return name_;
    }
    Expression const* arg() const // чтение аргумента функции
    {
        return arg_;
    }
    Expression* transform(Transformer* tr) const
    {
        return tr->transformFunctionCall(this);
    }
    ~FunctionCall() { delete arg_; } // освобождаем память в деструкторе
    virtual double evaluate() const { // реализация виртуального метода
        //«вычислить»
        if (name_ == "sqrt")
            return sqrt(arg_->evaluate()); // либо вычисляем корень квадратный
        else return fabs(arg_->evaluate());
    } // либо модуль — остальные функции //запрещены
private:
    std::string const name_; // имя функции
    Expression const* arg_; // указатель на ее аргумент
};

struct Variable : Expression // структура «Переменная»
{
    Variable(std::string const& name) : name_(name) { } //в конструкторе надо
    //указать ее имя
    std::string const& name() const { return name_; } // чтение имени переменной
    double evaluate() const // реализация виртуального метода «вычислить»
    {
        return 0.0;
    }
private:
    std::string const name_; // имя переменной
};

int main() {
    Expression* e1 = new Number(1.234);
    Expression* e2 = new Number(-1.234);
    Expression* e3 = new BinaryOperation(e1, BinaryOperation::DIV, e2);
    std::cout << e3->evaluate() << std::endl;
    //------------------------------------------------------------------------------
    Expression* n32 = new Number(32.0);
    Expression* n16 = new Number(16.0);
    Expression* minus = new BinaryOperation(n32, BinaryOperation::MINUS, n16);
    Expression* callSqrt = new FunctionCall("sqrt", minus);
    Expression* n2 = new Number(2.0);
    Expression* mult = new BinaryOperation(n2, BinaryOperation::MUL, callSqrt);
    Expression* callAbs = new FunctionCall("abs", mult);
    std::cout << callAbs->evaluate() << std::endl;
    //------------------------------------------------------------------------------
    Expression* expression = new Number(10.0);
    Transformer* transformer = new CopySyntaxTree();
    Expression* new_expression = expression->transform(transformer);

}

/*
Суть вот в чём: есть класс – визитор,
который содержит методы для работы с каждой из конкретных реализаций нашей абстракции.
А каждая конкретная реализация содержит метод,
который делает одну единственную вещь — передаёт себя соответствующему методу визитора.

будет возвращать полностью новое (т. е. все узлы должны копироваться, даже
 если никаких изменений в дереве не требуется) АСД, построенное с помощью реализации
 интерфейса Transformer из исходного дерева*/