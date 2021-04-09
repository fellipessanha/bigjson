
#include <bigjson/BigJSON.hpp>

#include <iostream>

int main()
{
    std::ifstream if_test("demo/test.json");
    bigjson::BigJSON bigj(if_test);
    std::cout << "LOADED #KEYS = " << bigj.size() << std::endl;

    std::ifstream if_test2("demo/test2.json");
    bigjson::BigJSON bigj2(if_test2);
    std::cout << "LOADED #KEYS = " << bigj2.size() << std::endl; // 3
    std::cout << bigj2["A"] << std::endl;
    std::cout << bigj2["B"]["B2"] << std::endl;
    std::cout << bigj2["Z"] << std::endl;

    return 0;
}
