#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "highs/Highs.h"

#include "var.h"
#include "constraint.h"

class OptimizationModel
{
  private:
    std::vector<std::shared_ptr<ModelRow>> rows;
    std::vector<std::shared_ptr<ModelColumn>> columns;

    ModelLinearExpression objective;
    ObjSense sense;
    
    std::unique_ptr<HighsModel> model;

    OptimizationModel(const HighsModel& m);
    ModelConstraint addConstr(const ModelLinearExpression&  expr, char sense, double lhs, double rhs, const std::string& cname);
    ModelVar addVar(double lb, double ub, bool integer, std::string name);

  public:
    Highs highs;

    OptimizationModel() : sense(ObjSense::kMinimize) { }
    OptimizationModel(const std::string& filename);
    OptimizationModel(const OptimizationModel& xmodel);

    void read(const std::string& filename);
    void write(const std::string& filename);

    OptimizationModel relax();
    OptimizationModel presolve();

    void update();
    void optimize();

    void minimize(ModelLinearExpression obj);
    void maximize(ModelLinearExpression obj);

    void computeIIS();

    ModelLinearExpression getObjective() const;
    void setObjective(ModelLinearExpression obje, ObjSense sense=ObjSense::kMinimize);

    ModelVar getVar(int i) const;
    ModelConstraint getConstr(int i) const;

    ModelVar addVar(double lb=0, double ub=kHighsInf, std::string name="") { return addVar(lb, ub, false, name); }
    ModelVar addIntegral(double lb=0, double ub=kHighsInf, std::string name="") { return addVar(lb, ub, true, name); }
    ModelVar addBinary(std::string name="") { return addVar(0, 1, true, name); }

    std::vector<ModelVar> addBinaries(int size, std::string prefix="") 
    {
        std::vector<ModelVar> vars;
        
        for (int i = 0; i < size; i++)
			vars.push_back(addBinary(prefix + std::to_string(i)));

        return std::move(vars); 
    }


    ModelConstraint addConstr(ModelLinearExpression expr, std::string name="");

    void remove(ModelVar v);
    void remove(ModelConstraint c);

    void chgCoeff(ModelConstraint c, ModelVar v, double val);
    double getCoeff(ModelConstraint c, ModelVar v) const;

    //std::vector<ModelLinearExpression> getCol(ModelVar v);
    ModelLinearExpression getRow(ModelConstraint c);

    HighsSolution getSolution();

    std::string getJSONSolution(void);
};
