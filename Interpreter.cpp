// Interpreter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <boost/lexical_cast.hpp>
struct Token {
    enum Type{ integer, plus, minus, lparen, rparen} type;
    std::string text;

    explicit Token(Type type, const std::string& text) :
        type{ type }, text{ text } {}

    friend std::ostream& operator << (std::ostream& os, const Token& obj) {
        return os << " " << obj.text << " ";
    }
};

std::vector<Token> lex(const std::string& input) {
    std::vector<Token> result;

    for (int i = 0; i < input.size(); ++i) {
        switch (input[i]) {
        case '+':
            result.push_back(Token{ Token::plus, "+" });
            break;
        case '-':
            result.push_back(Token{ Token::minus, "-" });
            break;
        case '(':
            result.push_back(Token{ Token::lparen, "(" });
            break;
        case ')':
            result.push_back(Token{ Token::rparen, ")" });
            break;
        default:
            //number ???
            std::ostringstream buffer;
            buffer << input[i];
            for (int j = i + 1; j < input.size(); ++j) {
                if (isdigit(input[j])) {
                    buffer << input[j];
                    ++i;
                }
                else {
                    result.push_back(Token{ Token::integer, buffer.str() });
                    break;
                }
            }
        }
    }
    return result;
}

struct Element
{
    virtual int eval() const = 0;
};

struct Integer : Element {
    int value;

    explicit Integer(const int value) : value(value) {}

    int eval() const override { return value; }
};

struct BinaryOperation : Element {
    enum Type { addition, subtraction} type;
    std::shared_ptr<Element> lhs, rhs;

    int eval() const override {
        if (type == addition)
            return lhs->eval() + rhs->eval();
        return lhs->eval() - rhs->eval();

    }
};

std::shared_ptr<Element> parse(const std::vector<Token>& tokens) {
    auto result = std::make_unique<BinaryOperation>();
    bool have_lhs = false; //this will need some explaining :)
    for (size_t i = 0; i < tokens.size(); i++)
    {
        auto token = tokens[i];
        switch (token.type) {
        case Token::integer:
        {
            int value = boost::lexical_cast<int>(token.text);
            auto integer = std::make_shared<Integer>(value);
            if (!have_lhs) {
                result->lhs = integer;
                have_lhs = true;
            }
            else result->rhs = integer;
        }
        case Token::plus:
            result->type = BinaryOperation::addition;
            break;
        case Token::minus:
            result->type = BinaryOperation::subtraction;
            break;
        case Token::lparen:
        {
            int j = i;
            for (; tokens.size(); ++j)
                if (tokens[j].type == Token::rparen)
                    break; //found it

            std::vector<Token> subexpression(&tokens[i + 1], &tokens[j]);
            auto element = parse(subexpression); //recursive call
            if (!have_lhs) {
                result->lhs = element;
                have_lhs = true;
            }
            else {
                result->rhs = element;
            }
            i = j;
        }
        }
    }
    return result;
}



int main()
{
    std::string input{ "(13-4)-(12+1)" };
    auto tokens = lex(input);
    auto parsed = parse(tokens);
    std::cout << input << "=" << parsed->eval() << std::endl;
    //prints "(13-4)-(12+1)=-4"
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer win.dow to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
