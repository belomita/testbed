#include "ScopeGuard.h"

#include <iostream>
#include <string>

void TestScopeGuard()
{
    int number = 3;
    ON_SCOPE_EXIT([&]{
        std::cout<<"exit function scope "<<std::endl;
    });

    std::cout<<"I am in function scope"<<std::endl;

    do 
    {
        std::string hello("Hello");
        ON_SCOPE_EXIT([&]{
            std::cout<<"exit do while scope "<<hello<<std::endl;
        });

        std::cout<<"I am in do while scope"<<std::endl;
    } while (0);

    std::cout<<"I am after do while scope"<<std::endl;
}
