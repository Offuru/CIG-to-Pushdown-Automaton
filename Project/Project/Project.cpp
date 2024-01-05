#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "Grammar.h"

void printMenu()
{
    std::cout << "1 - Print the Grammar;\n";
    std::cout << "2 n - Generate n words in the current Grammar;\n";
    std::cout << "3 - Simplify Grammar;\n";
    std::cout << "4 - FNC convert\n";
    std::cout << "5 - Generate a random word in current Grammar and verify if it's accepted by the equivalent automaton;\n";
    std::cout << "6 - Clear the screen;\n";
    std::cout << "0 - End.\n\n";
}

int main()
{
    Grammar G;
    std::ifstream in("grammar.in");
    G.ReadGrammar(in);
    in.close();
    if (!G.VerifyGrammar(std::cout))
        return 0;

    int option, noWords;
    std::string word;
    bool createdAutomaton = false;

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
            std::cin >> noWords;
            for (int i = 0; i < noWords; ++i)
            {
                G.GenerateWord();
                std::cout << "\n\n";
            }
            break;
        case 3:
            G.simplifyGrammar();
            break;
        case 4:
            G.FNGconvert();
            break;
        case 6:
            system("CLS");
            break;
        default:
            continue;
        }
    }

    return 0;
}
