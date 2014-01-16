#include "SmartAssert.h"

#include <iostream>
#include <sstream>

AssertContext::AssertContext()
	:	m_level	( lvl_error )
	,	m_line	( 0 )
{
}

void AssertContext::setLevel( const EAssertLevel lvl )
{
	m_level = lvl;
}

EAssertLevel AssertContext::getLevel() const
{
	return m_level;
}

void AssertContext::setFile( const std::string& file )
{
	m_file = file;
}

const std::string& AssertContext::getFile() const
{
	return m_file;
}

void AssertContext::setLine( const unsigned int line )
{
	m_line = line;
}

unsigned int AssertContext::getLine() const
{
	return m_line;
}

void AssertContext::setDesc( const std::string& desc )
{
	m_desc = desc;
}

const std::string& AssertContext::getDesc() const
{
	return m_desc;
}

void AssertContext::setExpr( const std::string& expr )
{
	m_expr = expr;
}

const std::string& AssertContext::getExpr() const
{
	return m_expr;
}

void AssertContext::addValue( const std::string& name, const std::string& value )
{
	m_values.push_back( std::make_pair( name, value ) );
}

const AssertContext::ValsArray_t& AssertContext::getValues() const
{
	return m_values;
}

const std::string AssertContext::getMsg() const
{
	std::ostringstream out;
	try{
		out << "ASSERTION FAILED in [" << m_file << ":" << m_line << "]: "<<std::endl;
		out << "> ";

		switch ( m_level ){
		case lvl_warn:		out << "Warning: ";		break;
		case lvl_error:		out << "Error: ";		break;
		case lvl_fatal:		out << "Fatal: ";		break;
		default:			out << "Unknown: ";		break;
		}
		out << m_desc << std::endl;

		out << "> Expression: \"" << m_expr << "\" failed." << std::endl;
		if(!m_values.empty())
			out << "> Values: " << std::endl;
		for( ValsArray_t::const_iterator it = m_values.begin(); it != m_values.end(); ++it )
			out << "\t" << (*it).first << " = " << (*it).second << std::endl;
		out << std::flush;
	}
	catch( ... )
	{
		// guarantee stability
	}

	return out.str();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AssertException::AssertException( const AssertContext& context )
	:	m_context( context )
{
}

AssertException::~AssertException() throw()
{}

const char* AssertException::what() const throw()
{
	return m_context.getMsg().c_str();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SmartAssert::HandlerCallback_t SmartAssert::m_callbacks[lvl_total] =
{	defaultWarnHandler, defaultErrorHandler, defaultFatalHandler	};

SmartAssert::~SmartAssert()
{
	const EAssertLevel level = m_context.getLevel();
	if ( ( level >= lvl_warn ) && ( level < lvl_total ) )
	{
		if ( m_callbacks[level] ) 
			m_callbacks[level]( m_context );
	}
	else
		abort();	// something gonna very bad
}

SmartAssert& SmartAssert::setContext( const std::string& file, const unsigned int line )
{
	m_context.setFile( file );
	m_context.setLine( line );
	return( *this );
}

SmartAssert& SmartAssert::setExpr( const std::string& expr )
{
	m_context.setExpr( expr );	
	return( *this );
}

SmartAssert& SmartAssert::warn( const std::string& desc )
{
	m_context.setLevel( lvl_warn );
	m_context.setDesc( desc );
	return( *this );
}

SmartAssert& SmartAssert::error( const std::string& desc )
{
	m_context.setLevel( lvl_error );
	m_context.setDesc( desc );
	return( *this );
}

SmartAssert& SmartAssert::fatal( const std::string& desc )
{
	m_context.setLevel( lvl_fatal );
	m_context.setDesc( desc );
	return( *this );
}

void SmartAssert::defaultWarnHandler( const AssertContext& context )
{
	std::cerr << context.getMsg();
}

void SmartAssert::defaultErrorHandler( const AssertContext& context )
{
	std::cerr << context.getMsg();
	throw AssertException( context );
}

void SmartAssert::defaultFatalHandler( const AssertContext& context )
{
	std::cerr << context.getMsg();
	abort();
}

void SmartAssert::setHandler(	EAssertLevel level, HandlerCallback_t callback )
{
	SMART_ASSERT( ( level >= lvl_warn ) && ( level < lvl_total ) )
		.error( "Bad assertion level." ).addValue( "level", level );

	SMART_ASSERT( callback != NULL )
		.warn( "Assertion handling for specified level will be disabled" )
		.addValue( "level", level );

	m_callbacks[level] = callback;
}

