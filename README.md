# FYI: Use this API at your own peril
**This is a nice example of using HiGHS via vcpkg.**  The C++ wrapper is somewhat cute, but I honestly don't recommend using it.  At some point I'll update this repo with a completely different API idea that I've been playing with, which is significantly more efficient - yet quite user friendly.


# HiGHS C++ Wrapper
This project is a *prototype* C++ wrapper around the HiGHS optimization solver. It attempts to provide a user-friendly interface to HiGHS, making it easier for developers to integrate the solver into their C++ projects.  It is nowhere near feature complete, but I think it has potential.

# Getting Started
## Dependencies
To use the C++ wrapper, simply add it to your project.  You'll need HiGHS, which you can easily get via vcpkg.  To build the example you'll need:

- cmake
- vcpkg

## Building
To build the example, follow these steps:

1. Clone the repository: git clone https://github.com/mathgeekcoder/highs-api/repo.git
2. Navigate to the repository directory: cd repo
3. Generate the build files: `cmake --preset <linux-release, linux-clang-12, windows-release>`
4. Build the project: `cmake --build out/build/<preset>`

## Usage
To use the HiGHS C++ wrapper, include the optimization_model.h header file in your C++ code. You can then use the OptimizationModel class to create an instance of the HiGHS solver and solve optimization problems.

Here is an example usage:

```C++
#include "optimization_model.h"  
  
int main() {  
    // Create a HiGHS model instance  
    OptimizationModel m;

    // Add variables
    auto x0 = m.addVar(0, 4);
    auto x1 = m.addVar(1, 7);

    // Add constraints
    m.addConstr(x0 + 2*x1 >= 5);
    m.addConstr(x0 + 2*x1 <= 15);
    m.addConstr(        6 <= 3*x0 + 2*x1);  // i.e., variables can be on either side of inequality.

    // Solve the optimization problem  
    m.minimize(x0 + x1 + 3);

    // Get the solution  
    auto x = m.getSolution();  
  
    // Print the solution  
    std::cout << "Solution: ";  
    for (auto i : x.col_value) {  
        std::cout << i << " ";  
    }  
    std::cout << std::endl;  
  
    return 0;  
}
```

## Contributing
If you would like to contribute to this project, please let me know.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
