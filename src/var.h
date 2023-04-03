#pragma once
#include <memory>
#include <string>

#include "highs/Highs.h"

class OptimizationModel;

class ModelColumn
{
  private:
    OptimizationModel* model;
    int col;

  public:
    friend class ModelVar;
    friend class OptimizationModel;

    std::string name;
    double lb, ub;
    bool integer;

    ModelColumn(OptimizationModel* model, int col_no, std::string name = "") : model(model), col(col_no), name(name) {
        lb = -kHighsInf; 
        ub = kHighsInf; 
        integer = false;
    }
};

class ModelVar
{
  private:
    std::shared_ptr<ModelColumn> column;

  public:
    friend class OptimizationModel;
    friend class ModelLinearExpression;

    ModelVar() {}
    ModelVar(const ModelVar& var) : column(var.column) {}
    ModelVar(std::shared_ptr<ModelColumn> ptr) : column(ptr) { }

    ModelVar& operator=(const ModelVar& var) {
        column = var.column;
        return *this;
    }

    int index() const {
        return column ? column->col : -2;
    }

    void remove() {
        throw std::runtime_error("Not implemented");

        //if (column) {
        //    column->model->remove(*this);
        //    column.reset();
        //}
    }

    //double getValue() const { 
    //    return column->model->highs.getSolution().col_value[index()];
    //}

    std::string getName() const {
        return column->name;
    }

    bool sameAs(ModelVar v2) {
        return column.get() == v2.column.get();
    }
};
