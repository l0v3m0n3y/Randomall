# Randomall
api for randomall.ru big random generator site
# main
```cpp
#include "Randomall.h"
#include <iostream>

int main() {
   Randomall api;

    auto result = api.generate_result(11880).then([](json::value result) {
        std::cout << "Search results: " << result.serialize() << std::endl;
    });
    result.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```
