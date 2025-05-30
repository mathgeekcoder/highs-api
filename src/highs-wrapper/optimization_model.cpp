#include <string.h>
#include <assert.h>
#include <cstdlib>
#include "optimization_model.h"

OptimizationModel::OptimizationModel(const string& filename)
{
	read(filename);
}

OptimizationModel::OptimizationModel(const OptimizationModel& m)
{
	// populate from wrapper class
	for (int col = 0; col < m.columns.size(); ++col)
		columns.push_back(std::shared_ptr<ModelColumn>(new ModelColumn(this, col, m.columns[col]->name)));

	for (int row = 0; row < m.rows.size(); ++row)
		rows.push_back(std::shared_ptr<ModelRow>(new ModelRow(this, row, m.rows[row]->name)));

	objective = m.objective;
}

OptimizationModel::OptimizationModel(const HighsModel& m)
{
	// populate from highs model
	for (int col = 0; col < m.lp_.num_col_; ++col) {
		std::string name = m.lp_.col_names_.size() > col ? m.lp_.col_names_[col] : "";
		columns.push_back(std::shared_ptr<ModelColumn>(new ModelColumn(this, col, name)));
	}

	for (int row = 0; row < m.lp_.num_row_; ++row) {
		std::string name = m.lp_.row_names_.size() > row ? m.lp_.row_names_[row] : "";
		rows.push_back(std::shared_ptr<ModelRow>(new ModelRow(this, row, name)));
	}

	objective.clear();
	const auto& obj = m.lp_.col_cost_;
	objective = m.lp_.offset_;

	for (int i = 0; i < m.lp_.num_col_; i++) {
		if (obj[i] != 0.0) {
			objective += obj[i] * columns[i];
		}
	}
}

void OptimizationModel::read(const string& filename)
{
	model.reset();
	highs.readModel(filename);

	// populate from highs
	for (int col = 0; col < highs.getNumCol(); ++col) {
		std::string name;
		highs.getColName(col, name);
		columns.push_back(std::shared_ptr<ModelColumn>(new ModelColumn(this, col, name)));
	}

	for (int row = 0; row < highs.getNumRow(); ++row) {
		std::string name;
		highs.getRowName(row, name);
		rows.push_back(std::shared_ptr<ModelRow>(new ModelRow(this, row, name)));
	}

	objective.clear();
	const auto& obj = highs.getLp().col_cost_;
	objective = highs.getLp().offset_;

	for (int i = 0; i < highs.getNumCol(); i++) {
		if (obj[i] != 0.0) {
			objective += obj[i] * columns[i];
		}
	}
}

void OptimizationModel::write(const string& filename)
{
	update();
	highs.writeModel(filename);
}

OptimizationModel OptimizationModel::relax()
{
	throw std::logic_error("not implemented");
}

OptimizationModel OptimizationModel::presolve()
{
	update();
	highs.presolve();
	return OptimizationModel(highs.getPresolvedModel());
}

/// <summary>
/// Convert wrapper class to highs model. 
/// This is done lazily, so that the sparse matrix is only updated when needed.
/// </summary>
void OptimizationModel::update()
{
	if (model == nullptr) {
		model.reset(new HighsModel());
		
		model->lp_.num_col_ = columns.size();
		model->lp_.num_row_ = rows.size();
		model->lp_.offset_ = objective.constant;
		model->lp_.sense_ = sense;

		// columns
		model->lp_.col_cost_.resize(columns.size());
		for (int c = 0; c < objective.coeffs.size(); ++c)
			model->lp_.col_cost_[objective.vars[c].index()] = objective.coeffs[c];

		model->lp_.col_lower_.resize(columns.size());
		model->lp_.col_upper_.resize(columns.size());
		model->lp_.integrality_.resize(columns.size());

		bool integer = false;

		for (int c = 0; c < columns.size(); ++c) {
			model->lp_.col_lower_[c] = columns[c]->lb;
			model->lp_.col_upper_[c] = columns[c]->ub;
			model->lp_.integrality_[c] = columns[c]->integer ? HighsVarType::kInteger : HighsVarType::kContinuous;
			integer |= columns[c]->integer;
		}

		// remove integrality vector if model is completely continuous
		if (integer == false)
			model->lp_.integrality_.clear();

		// rows
		model->lp_.row_lower_.resize(rows.size());
		model->lp_.row_upper_.resize(rows.size());

		for (int r = 0; r < rows.size(); ++r) {
			auto& row = rows[r];

			// TODO: check constant vs RHS logic
			if (row->expr.type == ModelLinearExpression::Inequality::EQ) {	    // == rhs
				model->lp_.row_lower_[r] = row->expr.rhs;
				model->lp_.row_upper_[r] = row->expr.rhs;
			}
			else if (row->expr.type == ModelLinearExpression::Inequality::LE) { // <= rhs
				model->lp_.row_lower_[r] = -kHighsInf;
				model->lp_.row_upper_[r] = row->expr.rhs;
			}
			else {															    // >= rhs
				model->lp_.row_lower_[r] = row->expr.rhs;
				model->lp_.row_upper_[r] = kHighsInf;
			}
		}

		// The orientation of the matrix is row-wise
		model->lp_.a_matrix_.format_ = MatrixFormat::kRowwise;

		model->lp_.a_matrix_.start_.resize(rows.size() + 1);
		int nnz = 0;

		for (int r = 0; r < rows.size(); ++r) {
			nnz += rows[r]->expr.coeffs.size();
			model->lp_.a_matrix_.start_[r+1] = nnz;
		}

		model->lp_.a_matrix_.index_.resize(nnz);
		model->lp_.a_matrix_.value_.resize(nnz);
		int offset = 0;

		for (int r = 0; r < rows.size(); ++r) {
			for (int i = 0; i < rows[r]->expr.coeffs.size(); ++i) {
				model->lp_.a_matrix_.index_[offset] = rows[r]->expr.vars[i].index();
				model->lp_.a_matrix_.value_[offset] = rows[r]->expr.coeffs[i];
				++offset;
			}
		}

		highs.passModel(*model);
	}
}

void OptimizationModel::optimize()
{
	update();
	highs.run();
}

void OptimizationModel::minimize(ModelLinearExpression obj)
{
	setObjective(obj, ObjSense::kMinimize);
	optimize();
}

void OptimizationModel::maximize(ModelLinearExpression obj)
{
	setObjective(obj, ObjSense::kMaximize);
	optimize();
}

HighsSolution OptimizationModel::getSolution() {
	return highs.getSolution();
}

void OptimizationModel::computeIIS()
{
	throw std::logic_error("not implemented");
}

ModelLinearExpression OptimizationModel::getObjective() const
{
	return objective;
}

void OptimizationModel::setObjective(ModelLinearExpression obje, ObjSense s)
{
	model.reset();
	objective = obje;
	sense = s;
}

ModelVar OptimizationModel::getVar(int i) const
{
	if (i < 0 || i >= columns.size())
		throw std::invalid_argument("i");
	return columns[i];
}

ModelConstraint OptimizationModel::getConstr(int i) const
{
	if (i < 0 || i >= rows.size())
		throw std::invalid_argument("i");
	return rows[i];
}


ModelVar OptimizationModel::addVar(double lb, double ub, bool integer, std::string name)
{
	model.reset();
	std::shared_ptr<ModelColumn> newCol(new ModelColumn(this, columns.size(), name));

	newCol->lb = lb;
	newCol->ub = ub;
	newCol->integer = integer;

	columns.push_back(newCol);
	return newCol;
}

ModelConstraint OptimizationModel::addConstr(ModelLinearExpression expr, std::string name)
{
	model.reset();
	std::shared_ptr<ModelRow> newRow(new ModelRow(this, rows.size(), name));
	newRow->expr = expr;

	rows.push_back(newRow);
	return newRow;
}

void OptimizationModel::remove(ModelVar v)
{
	throw std::logic_error("not implemented");
}

void OptimizationModel::remove(ModelConstraint c)
{
	throw std::logic_error("not implemented");
}

void OptimizationModel::chgCoeff(ModelConstraint c, ModelVar v, double val)
{
	throw std::logic_error("not implemented");

	//int i = c.getrowno();
	//int j = v.getcolno();
	//if (i < 0 || j < 0) 
	//	throw std::invalid_argument("c or v");

	//highs.changeCoeff(i, j, val);
}

double OptimizationModel::getCoeff(ModelConstraint c, ModelVar v) const
{
	throw std::logic_error("not implemented");

	//int i = c.getrowno();
	//int j = v.getcolno();
	//if (i < 0 || i >= rows || j < 0 || j >= cols) 
	//	throw std::invalid_argument("c or v");

	//throw std::logic_error("not implemented");
}

ModelLinearExpression OptimizationModel::getRow(ModelConstraint c)
{
	if (c.index() < 0 || c.index() >= rows.size())
		throw std::invalid_argument("c");

	return c.row->expr;
}

string OptimizationModel::getJSONSolution(void)
{
	throw std::logic_error("not implemented");
}

