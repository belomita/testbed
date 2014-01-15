#ifndef _SMART_ASSERT_H_
#define _SMART_ASSERT_H_

#include <string>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <stdexcept>

namespace utility
{

// ValuePair stores (variable_name, value).
typedef std::pair<std::string, std::string> ValuePair;
typedef std::vector<ValuePair> ValuesArray;

class AssertContext
{
public:
    // Where the assertion locates: filename & line number.
    std::string GetFilename() const
    {
        return filename_;
    }

    void SetFilename(const std::string& filename)
    {
        filename_ = filename;
    }

    int GetLineNumber() const
    {
        return line_number_;
    }
    void SetLineNumber(const int line_number)
    {
        line_number_ = line_number;
    }

    // Get/ set the expression to be asserted.
    void SetExpression( const std::string & expression)
    {
        expression_ = expression;
    }
    const std::string & GetExpression() const
    {
        return expression_;
    }

    const ValuesArray & GetValuesArray() const
    {
        return val_array_;
    }

    // Adds one variable name and its corresponding value.
    void AddValue(const std::string & variable_name,
                  const std::string & value)
    {
        val_array_.push_back(ValuePair(variable_name, value));
    }

    void Print(std::ostream& out)
    {
        out << "ASSERTION FAILED in "
            << filename_ << ":" << line_number_ << ":" << std::endl;
        out << "> Expression: \'" << expression_ << "\'" << std::endl;

        if (val_array_.size() > 0)
        {
            out << "> Values:" << std::endl;
            std::for_each(val_array_.begin(), val_array_.end(),
            [&](ValuePair pair)
            {
                out << "    " << pair.first
                    << " = " << pair.second << std::endl;
            });
        }
    }

protected:
    std::string filename_;
    int line_number_;
    std::string expression_;
    ValuesArray val_array_;
};

class Assert
{
protected:
    AssertContext context_;
public:
    Assert(const std::string& expression)
        : SMART_ASSERT_A(*this), SMART_ASSERT_B(*this)
    {
        context_.SetExpression(expression);
    }

    Assert& SMART_ASSERT_A; // clever macro A
    Assert& SMART_ASSERT_B; // clever macro B

    // Print out result
    template <typename T>
    Assert& AddValue(T val, const std::string& variable_name)
    {
        std::ostringstream ss;
        ss << val;
        context_.AddValue(variable_name, ss.str());
        return *this;
    }

    // For printing out line number, function, etc.
    Assert& SetContext(const std::string& filename, const int line)
    {
        context_.SetFilename(filename);
        context_.SetLineNumber(line);
        return *this;
    }

    AssertContext GetContext()
    {
        return context_;
    }

    void msg(const std::string& msg)
    {
        context_.Print(std::cout);
        std::cout << msg << std::endl;
    }
};

// Make an assertion
static Assert make_assert(const std::string& expression)
{
    return Assert(expression);
}

} // namespace utility


// Must define the macros afterwards
// Clever macros
#define SMART_ASSERT_A(x) SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x) SMART_ASSERT_OP(x, A)

// Clever macro recursion
#define SMART_ASSERT_OP(x, next) \
SMART_ASSERT_A.AddValue((x), #x).SMART_ASSERT_ ## next

#define MAKE_ASSERT(expr)\
    utility::make_assert(#expr).SetContext(__FILE__, __LINE__).SMART_ASSERT_A

#define SMART_ASSERT(expr, msg, ...)       \
    if ( (expr) ) ;             \
    else                        \
    {                           \
        utility::AssertContext _context##__LINE__ = \
            MAKE_ASSERT(expr) __VA_ARGS__ .GetContext(); \
        _context##__LINE__.Print(std::cout);   \
        throw std::runtime_error(msg);     \
    }

#endif