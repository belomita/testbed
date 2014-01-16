#ifndef SMART_ASSERT_HEADER_FROM_CORE_INCLUDED
#define SMART_ASSERT_HEADER_FROM_CORE_INCLUDED

#include <string>
#include <vector>
#include <exception>
#include <sstream>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// SMART_ASSERT - redesign of ideas, described by Andrei Alexandrescu and
// John Torjo in.
// http://www.cuj.com/documents/s=8464/cujcexp0308alexandr/cujcexp0308alexandr.html
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum EAssertLevel{
	lvl_warn,						// just logging
	lvl_error,						// throw exception
	lvl_fatal,						// full logging and aborting
	lvl_total  
};

class AssertContext{
public:
	AssertContext();

	void					setLevel( const EAssertLevel );
	EAssertLevel			getLevel() const;
	void					setFile( const std::string& );
	const std::string&		getFile() const;
	void					setLine( const unsigned int );
	unsigned int 			getLine() const;
	void					setDesc( const std::string& );
	const std::string&		getDesc() const;
	void					setExpr( const std::string& );
	const std::string&		getExpr() const;

	typedef std::pair<std::string, std::string>	ValueAndString_t;
	typedef std::vector<ValueAndString_t>		ValsArray_t;

	void					addValue( const std::string& name, const std::string& value );
	const ValsArray_t&		getValues() const;

	// return user-friendly assert message
	const std::string		getMsg() const;

protected:
	EAssertLevel			m_level;
	std::string 			m_file;
	unsigned int 			m_line;
	std::string 			m_desc;
	std::string 			m_expr;
	ValsArray_t				m_values;	
};


// as usual exception, but keep context of assertion 
struct AssertException : public std::exception {

	AssertException( const AssertContext& );
	virtual 				~AssertException() throw();

	virtual const char* 	what() const throw();
protected:
	AssertContext			m_context;
};

// Helper for unrolling complex SMART_ASSERT macroses
class SmartAssert
{
public:
	SmartAssert( const std::string& expr ):
	  SMART_ASSERT_A( *this ),SMART_ASSERT_B( *this )
	  {
		  m_context.setExpr( expr );
	  }

	  ~SmartAssert();

	  SmartAssert& 	setContext( const std::string& file, const unsigned int line );
	  SmartAssert& 	setExpr( const std::string& );

	  template<class type>
	  SmartAssert& 	addValue( const std::string& name, const type& value ){
		  std::ostringstream ss;
		  ss << value;
		  m_context.addValue( name, ss.str());
		  
		  return ( *this );
	  }

	  SmartAssert& 	warn( const std::string& );    
	  SmartAssert& 	error( const std::string& );
	  SmartAssert& 	fatal( const std::string& );

	  // NULL as callback will disable handling specified level of assertions  
	  typedef void (*HandlerCallback_t)( const AssertContext& ); 
	  static void		setHandler(	EAssertLevel, HandlerCallback_t );

private:
	static void 	defaultWarnHandler( const AssertContext& );
	static void 	defaultErrorHandler( const AssertContext& );
	static void 	defaultFatalHandler( const AssertContext& ); 

public:
	SmartAssert& SMART_ASSERT_A;
	SmartAssert& SMART_ASSERT_B;

private:
	AssertContext 			m_context;
	static HandlerCallback_t	m_callbacks[lvl_total];
}; // class Assert_c

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// These macroses will help us to unfold complex assertions, like
// SMART_ASSERT( a >= 0 )( a ).warn( "A must be >=0" );
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define SMART_ASSERT_A(x) SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x) SMART_ASSERT_OP(x, A)

#define SMART_ASSERT_OP(x, next) \
	SMART_ASSERT_A.addValue( #x, (x) ).SMART_ASSERT_ ## next

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   
#ifndef NDEBUG
#define SMART_ASSERT(expr) \
	if ( expr ) \
		{ ; } \
	else \
		SmartAssert( #expr ).setContext( __FILE__, __LINE__ ).SMART_ASSERT_A

#define SMART_VERIFY(expr) \
	if ( expr ) \
		{ ; } \
	else \
		SmartAssert( #expr ).setContext( __FILE__, __LINE__ ).SMART_ASSERT_A

#else
#define SMART_ASSERT(expr) \
	if ( true ) \
		{;} \
	else \
		SmartAssert("").SMART_ASSERT_A

#define SMART_VERIFY(expr) \
	if ( true ) \
		{ (void)(expr); } \
	else \
		SmartAssert("").SMART_ASSERT_A

#endif // ifndef NDEBUG

#define  ENSURE SMART_ASSERT
#endif /* SMART_ASSERT_HEADER_FROM_CORE_INCLUDED */

