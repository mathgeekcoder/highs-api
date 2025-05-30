#include <iostream>
#include "highs-wrapper/optimization_model.h"

//
// min  x0 + x1 + 3
// s.t.   
//         5 <= x0 + 2x1
//  x0 + 2x1 <= 15
//         6 <= 3x0 + 2x1
//   0 <= x0 <= 4 
//   1 <= x1 <= 7
//
int main(int argc, char* argv[])
{
    try {
        OptimizationModel m;

        auto x0 = m.addVar(0, 4, "x0");
        auto x1 = m.addVar(1, 7, "x1");

        m.addConstr(x0 + 2*x1 >= 5);
        m.addConstr(x0 + 2*x1 <= 15);
        m.addConstr(        6 <= 3*x0 + 2*x1);

        m.minimize(x0 + x1 + 3);


        // Get the solution  
        auto x = m.getSolution();  

        // Print the solution  
        std::cout << "Solution: ";  
        for (auto i : x.col_value) {  
            std::cout << i << " ";  
        }  
        std::cout << std::endl;  
    }
    catch (std::exception& e)   {
    	std::cout << e.what() << std::endl;
    }

    return 0;
}
