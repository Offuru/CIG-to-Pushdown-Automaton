#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "Grammar.h"
#include "APD.h"

void printMenu();

int main()
{
    Grammar G;
    std::ifstream in("grammar.in");
    G.ReadGrammar(in);
    in.close();
    if (!G.VerifyGrammar(std::cout))
        return 0;

    if (!G.IsContextIndependent())
    {
        std::cout << "The grammar isn't context independent.\n";
        return 0;
    }

    int option;
    std::string word;
    APD automaton = APD(G);

    if (automaton.isDeterministic())
        std::cout << "The automaton is deterministic!\n\n";
    else
        std::cout << "The automaton isn't deterministic!\n\n";

    while (true)
    {
        printMenu();
        std::cin >> option;
        switch (option)
        {
        case 0:
            return 0;
            break;
        case 1:
            G.PrintGrammar();
            std::cout << "\n";
            break;
        case 2:
            G.GenerateWord();
            std::cout << "\n\n";
            break;
        case 3:
            G.simplifyGrammar();
            G.PrintGrammar();
            break;
        case 4:
            G.FNGconvert();
            G.PrintGrammar();
            break;
        case 5:
            word = G.GenerateWord(false);
            std::cout << "The word is: " << word << "\n";
            if (automaton.checkWord(word))
                std::cout << "The word is valid!\n\n";
            else
                std::cout << "The word is not valid!\n\n";
            break;
        case 6:
            std::cout << "Type word: ";
            std::cin >> word;
            if (automaton.checkWord(word))
                std::cout << "The word is valid!\n\n";
            else
                std::cout << "The word is not valid!\n\n";
            break;
        case 7:
            std::cout << automaton << "\n\n";
            break;
        default:
            continue;
        }
    }

    return 0;
}

void printMenu()
{
    std::cout << "1: Print the Grammar;\n";
    std::cout << "2: Generate a word in the current Grammar;\n";
    std::cout << "3: Print the simplified grammar;\n";
    std::cout << "4: Print grammar converted to FNG grammar;\n";
    std::cout << "5: Generate a random word in current Grammar and verify if it's accepted by the equivalent automaton;\n";
    std::cout << "6: Check word;\n";
    std::cout << "7: Print automaton.\n";
    std::cout << "0: EXIT.\n\n";
}