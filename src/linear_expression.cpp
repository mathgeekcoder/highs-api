#include "var.h"
#include "constraint.h"
#include "linear_expression.h"

ModelLinearExpression::ModelLinearExpression(double xconstant)
{
	constant = xconstant;
	rhs = 0.0;
	type = Inequality::Unknown;
}

ModelLinearExpression::ModelLinearExpression(ModelVar var, double coeff)
{
	constant = 0.0;
	rhs = 0.0;
	type = Inequality::Unknown;
	coeffs.push_back(coeff);
	vars.push_back(var);
}

ModelLinearExpression ModelLinearExpression::operator=(const ModelLinearExpression& rhs)
{
	constant = rhs.constant;
	this->rhs = rhs.rhs;
	coeffs = rhs.coeffs;
	vars = rhs.vars;
	type = rhs.type;

	return *this;
}

void ModelLinearExpression::multAdd(double m, const ModelLinearExpression& expr)
{
	if (m == 0) return;

	ModelLinearExpression tmp;
	const vector<double>* addcoeffs;
	const vector<ModelVar>* addvars;

	/* to avoid an endless loop when adding an expression to itself, we copy
	 * the expr and use the data inside the copy */
	if (&this->coeffs == &expr.coeffs) {
		tmp = expr;
		addcoeffs = &(tmp.coeffs);
		addvars = &(tmp.vars);
	}
	else {
		addcoeffs = &(expr.coeffs);
		addvars = &(expr.vars);
	}

	if (m == 1.0) {
		coeffs.insert(coeffs.end(), addcoeffs->begin(), addcoeffs->end());
	}
	else {
		unsigned int size = (*addcoeffs).size();

		for (unsigned int i = 0; i < size; i++) {
			coeffs.push_back(m * (*addcoeffs)[i]);
		}
	}
	vars.insert(vars.end(), addvars->begin(), addvars->end());
	constant += m * expr.constant;
}

void ModelLinearExpression::operator+=(const ModelLinearExpression& expr)
{
	this->multAdd(1.0, expr);
}

void ModelLinearExpression::operator-=(const ModelLinearExpression& expr)
{
	this->multAdd(-1.0, expr);
}

void ModelLinearExpression::operator*=(double mult)
{
	constant *= mult;
	for (unsigned int i = 0; i < vars.size(); i++) {
		coeffs[i] *= mult;
	}
}

void ModelLinearExpression::operator/=(double a)
{
	constant /= a;
	for (unsigned int i = 0; i < vars.size(); i++) {
		coeffs[i] /= a;
	}
}

ModelLinearExpression ModelLinearExpression::operator+(const ModelLinearExpression& rhs)
{
	ModelLinearExpression result = *this;
	result.multAdd(1.0, rhs);
	return result;
}

ModelLinearExpression ModelLinearExpression::operator-(const ModelLinearExpression& rhs)
{
	ModelLinearExpression result = *this;
	result.multAdd(-1.0, rhs);
	return result;
}

unsigned int ModelLinearExpression::size() const
{
	return vars.size();
}

ModelVar ModelLinearExpression::getVar(int i) const
{
	return vars[i];
}

double ModelLinearExpression::getCoeff(int i) const
{
	return coeffs[i];
}

//double ModelLinearExpression::getValue() const
//{
//	double value = constant;
//	for (unsigned int i = 0; i < vars.size(); i++)
//		value += coeffs[i] * vars[i].getValue();
//	return value;
//}

void ModelLinearExpression::addTerms(const double* coeff, const ModelVar* var, int   cnt)
{
	for (int i = 0; i < cnt; i++) {
		if (coeff == NULL) coeffs.push_back(1.0);
		else               coeffs.push_back(coeff[i]);
		vars.push_back(var[i]);
	}
}

void ModelLinearExpression::remove(int i)
{
	if (i < 0 || i >= (int)vars.size()) 
		throw std::invalid_argument("i");

	coeffs.erase(coeffs.begin() + i);
	vars.erase(vars.begin() + i);
}

bool ModelLinearExpression::remove(ModelVar v)
{
	int size = (int)vars.size();
	for (int i = size - 1; i >= 0; i--) {
		if (v.column == vars[i].column) {
			coeffs.erase(coeffs.begin() + i);
			vars.erase(vars.begin() + i);
		}
	}
	if (size > (int)vars.size()) return true;
	else                          return false;
}

void ModelLinearExpression::clear(void)
{
	constant = 0.0;
	coeffs.clear();
	vars.clear();
}

std::ostream& operator<<(std::ostream& stream, ModelLinearExpression expr)
{
	for (unsigned int i = 0; i < expr.coeffs.size(); i++)
		stream << "+ " << expr.coeffs[i] << " " << expr.vars[i].getName() << " ";
	if (expr.constant != 0)
		stream << "+ " << expr.constant;

	return stream;
}

ModelLinearExpression operator+(const ModelLinearExpression& x, const ModelLinearExpression& y)
{
	ModelLinearExpression result = x;
	result += y;
	return result;
}

ModelLinearExpression operator-(const ModelLinearExpression& x, const ModelLinearExpression& y)
{
	ModelLinearExpression result = x;
	result -= y;
	return result;
}

ModelLinearExpression operator+(const ModelLinearExpression& x)
{
	ModelLinearExpression result = x;
	return result;
}

ModelLinearExpression operator+(ModelVar x, ModelVar y)
{
	ModelLinearExpression result(x);
	result += y;

	return result;
}

ModelLinearExpression operator+(ModelVar x, double a)
{
	ModelLinearExpression result(x);
	result += a;

	return result;
}

ModelLinearExpression operator+(double a, ModelVar x)
{
	ModelLinearExpression result(x);
	result += a;

	return result;
}

ModelLinearExpression operator-(ModelVar x, ModelVar y)
{
	ModelLinearExpression result(x);
	result -= y;

	return result;
}

ModelLinearExpression operator-(ModelVar x, double a)
{
	ModelLinearExpression result(x);
	result -= a;

	return result;
}

ModelLinearExpression operator-(double a, ModelVar x)
{
	ModelLinearExpression result(a);
	result -= x;

	return result;
}

ModelLinearExpression operator-(const ModelLinearExpression& x)
{
	ModelLinearExpression result;
	result -= x;

	return result;
}

ModelLinearExpression operator*(double a, ModelVar x)
{
	return ModelLinearExpression(x, a);
}

ModelLinearExpression operator-(ModelVar x)
{
	return -1.0 * x;
}

ModelLinearExpression operator*(ModelVar x, double a)
{
	return ModelLinearExpression(x, a);
}

ModelLinearExpression operator*(const ModelLinearExpression& x, double a)
{
	ModelLinearExpression result = 0;
	result.multAdd(a, x);
	return result;
}

ModelLinearExpression operator*(double a, const ModelLinearExpression& x)
{
	ModelLinearExpression result = 0;
	result.multAdd(a, x);
	return result;
}

ModelLinearExpression operator/(ModelVar x, double a) {
	return ModelLinearExpression(x, 1.0 / a);
}

ModelLinearExpression operator/(const ModelLinearExpression& x, double a) {
	ModelLinearExpression result = x;
	return (1.0 / a) * result;
}



ModelLinearExpression operator==(double rhs, const ModelLinearExpression& that)
{
	if (that.type != ModelLinearExpression::Inequality::Unknown)
		throw std::invalid_argument("Inequality already set");

	ModelLinearExpression result = that;
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::EQ;
	return result;
}

ModelLinearExpression operator<=(double rhs, const ModelLinearExpression& that)
{
	if (that.type != ModelLinearExpression::Inequality::Unknown)
		throw std::invalid_argument("Inequality already set");

	ModelLinearExpression result = that;
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::GE;
	return result;
}

ModelLinearExpression operator>=(double rhs, const ModelLinearExpression& that)
{
	if (that.type != ModelLinearExpression::Inequality::Unknown)
		throw std::invalid_argument("Inequality already set");

	ModelLinearExpression result = that;
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::LE;
	return result;
}


ModelLinearExpression operator==(double rhs, ModelVar x)
{
	ModelLinearExpression result(x);
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::EQ;
	return result;
}

ModelLinearExpression operator<=(double rhs, ModelVar x)
{
	ModelLinearExpression result(x);
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::GE;
	return result;
}

ModelLinearExpression operator>=(double rhs, ModelVar x)
{
	ModelLinearExpression result(x);
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::LE;
	return result;
}


ModelLinearExpression operator==(ModelVar x, double rhs)
{
	ModelLinearExpression result(x);
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::EQ;
	return result;
}

ModelLinearExpression operator<=(ModelVar x, double rhs)
{
	ModelLinearExpression result(x);
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::LE;
	return result;
}

ModelLinearExpression operator>=(ModelVar x, double rhs)
{
	ModelLinearExpression result(x);
	result.rhs = rhs;
	result.type = ModelLinearExpression::Inequality::GE;
	return result;
}

ModelLinearExpression quicksum(const std::vector<ModelVar> vars) {
	ModelLinearExpression expr;

	for (auto var : vars) {
		expr += var;
	}

	return expr;
}

ModelLinearExpression quicksum(const std::vector<ModelLinearExpression> vars) {
	ModelLinearExpression expr;

	for (auto var : vars) {
		expr += var;
	}

	return expr;
}

// takes a lambda predicate to conditionally sum over a vector of variables
ModelLinearExpression quicksum_if(const std::vector<ModelVar>& vars, std::function<bool(ModelVar)> pred) {
	ModelLinearExpression expr;

	for (auto var : vars) {
		if (pred(var)) {
			expr += var;
		}
	}

	return expr;
}