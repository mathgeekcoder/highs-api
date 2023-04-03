#pragma once

class ModelLinearExpression
{
private:
    double constant, rhs;

    enum class Inequality { Unknown, EQ, LE, GE };
    Inequality type;

    std::vector<double> coeffs;
    std::vector<ModelVar> vars;
    void multAdd(double m, const ModelLinearExpression& expr);

  public:
    friend class OptimizationModel;

    ModelLinearExpression(double constant=0.0);
    ModelLinearExpression(ModelVar var, double coeff=1.0);

    ModelLinearExpression operator==(double rhs)
    {
        if (type != Inequality::Unknown)
            throw std::runtime_error("Inequality already specified");

        this->rhs = rhs;
        type = Inequality::EQ;
        return *this;
    }

    ModelLinearExpression operator<= (double rhs)
    {
        if (type != Inequality::Unknown)
            throw std::runtime_error("Inequality already specified");

        this->rhs = rhs;
        type = Inequality::LE;
        return *this;
    }

    ModelLinearExpression operator>= (double rhs)
    {
        if (type != Inequality::Unknown)
            throw std::runtime_error("Inequality already specified");

        this->rhs = rhs;
        type = Inequality::GE;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream &stream, ModelLinearExpression expr);
    friend ModelLinearExpression operator+(const ModelLinearExpression& x, const ModelLinearExpression& y);
    friend ModelLinearExpression operator+(const ModelLinearExpression& x);
    friend ModelLinearExpression operator+(ModelVar x, ModelVar y);
    friend ModelLinearExpression operator+(ModelVar x, double a);
    friend ModelLinearExpression operator+(double a, ModelVar x);
    friend ModelLinearExpression operator-(const ModelLinearExpression& x, const ModelLinearExpression& y);
    friend ModelLinearExpression operator-(const ModelLinearExpression& x);
    friend ModelLinearExpression operator-(ModelVar x);
    friend ModelLinearExpression operator-(ModelVar x, ModelVar y);
    friend ModelLinearExpression operator-(ModelVar x, double a);
    friend ModelLinearExpression operator-(double a, ModelVar x);
    friend ModelLinearExpression operator*(double a, ModelVar x);
    friend ModelLinearExpression operator*(ModelVar x, double a);
    friend ModelLinearExpression operator*(const ModelLinearExpression& x, double a);
    friend ModelLinearExpression operator*(double a, const ModelLinearExpression& x);
    friend ModelLinearExpression operator/(ModelVar x, double a);
    friend ModelLinearExpression operator/(const ModelLinearExpression& x, double a);

    friend ModelLinearExpression operator==(double rhs, const ModelLinearExpression& that);
    friend ModelLinearExpression operator<=(double rhs, const ModelLinearExpression& that);
    friend ModelLinearExpression operator>=(double rhs, const ModelLinearExpression& that);
    friend ModelLinearExpression operator==(double rhs, ModelVar x);
    friend ModelLinearExpression operator<=(double rhs, ModelVar x);
    friend ModelLinearExpression operator>=(double rhs, ModelVar x);
    friend ModelLinearExpression operator==(ModelVar x, double rhs);
    friend ModelLinearExpression operator<=(ModelVar x, double rhs);
    friend ModelLinearExpression operator>=(ModelVar x, double rhs);

    unsigned int size(void) const;
    ModelVar getVar(int i) const;
    double getCoeff(int i) const;
    double getConstant() const { return constant; }
    double getRHS() const { return rhs; }
    //double getValue() const;

    void addTerms(const double* coeff, const ModelVar* var, int cnt);
    ModelLinearExpression operator=(const ModelLinearExpression& rhs);
    void operator+=(const ModelLinearExpression& expr);
    void operator-=(const ModelLinearExpression& expr);
    void operator*=(double mult);
    void operator/=(double a);
    ModelLinearExpression operator+(const ModelLinearExpression& rhs);
    ModelLinearExpression operator-(const ModelLinearExpression& rhs);
    void remove(int i);
    bool remove(ModelVar v);

    void clear();
};

//std::ostream& operator<<(std::ostream &stream, ModelLinearExpression expr);
ModelLinearExpression operator+(const ModelLinearExpression& x, const ModelLinearExpression& y);
ModelLinearExpression operator+(const ModelLinearExpression& x);
ModelLinearExpression operator+(ModelVar x, ModelVar y);
ModelLinearExpression operator+(ModelVar x, double a);
ModelLinearExpression operator+(double a, ModelVar x);
ModelLinearExpression operator-(const ModelLinearExpression& x, const ModelLinearExpression& y);
ModelLinearExpression operator-(const ModelLinearExpression& x);
ModelLinearExpression operator-(ModelVar x);
ModelLinearExpression operator-(ModelVar x, ModelVar y);
ModelLinearExpression operator-(ModelVar x, double a);
ModelLinearExpression operator-(double a, ModelVar x);
ModelLinearExpression operator*(double a, ModelVar x);
ModelLinearExpression operator*(ModelVar x, double a);
ModelLinearExpression operator*(const ModelLinearExpression& x, double a);
ModelLinearExpression operator*(double a, const ModelLinearExpression& x);
ModelLinearExpression operator/(ModelVar x, double a);
ModelLinearExpression operator/(const ModelLinearExpression& x, double a);
ModelLinearExpression operator==(double rhs, const ModelLinearExpression& that);
ModelLinearExpression operator<=(double rhs, const ModelLinearExpression& that);
ModelLinearExpression operator>=(double rhs, const ModelLinearExpression& that);
ModelLinearExpression operator==(double rhs, ModelVar x);
ModelLinearExpression operator<=(double rhs, ModelVar x);
ModelLinearExpression operator>=(double rhs, ModelVar x);
ModelLinearExpression operator==(ModelVar x, double rhs);
ModelLinearExpression operator<=(ModelVar x, double rhs);
ModelLinearExpression operator>=(ModelVar x, double rhs);


ModelLinearExpression quicksum(const std::vector<ModelVar> vars);
ModelLinearExpression quicksum(const std::vector<ModelLinearExpression> vars);

// takes a lambda predicate to conditionally sum over a vector of variables
ModelLinearExpression quicksum_if(const std::vector<ModelVar>& vars, std::function<bool(ModelVar)> pred);