#pragma once
#include <memory>
#include "linear_expression.h"

class OptimizationModel;

class ModelRow
{
  private:
    OptimizationModel* model;
    int row;

  public:
    friend class ModelConstraint;
    friend class OptimizationModel;

    ModelRow(OptimizationModel* model, int row, std::string name = "") : model(model), row(row), name(name) {}
    std::string name;
    ModelLinearExpression expr;
};

class ModelConstraint
{
  private:
    std::shared_ptr<ModelRow> row;

  public:
    friend class OptimizationModel;
    friend class ModelColumn;

    ModelConstraint() {}
    ModelConstraint(const ModelConstraint& var) : row(var.row) {}
    ModelConstraint(std::shared_ptr<ModelRow> ptr) : row(ptr) { }

    ModelConstraint& operator=(const ModelConstraint& var) {
        row = var.row;
    	return *this;
    }

    int index() const {
        return row ? row->row : -2;
    }

    void remove() {
        throw std::runtime_error("Not implemented");

        //if (row) {
        //    row->model->remove(*this);
        //    row.reset();
        //}
    }

    bool sameAs(ModelConstraint c2) {
        return (row.get() == c2.row.get());
    }
};
