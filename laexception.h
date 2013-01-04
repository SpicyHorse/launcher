#ifndef LAEXCEPTION_H
#define LAEXCEPTION_H

#include <exception>

class LaException : public std::exception
{
const char* error;

public:
    LaException(const char* s) throw() { error = s; }
    virtual ~LaException() throw() {}
    virtual const char* what() const throw() { return error; }
};

#endif // AVEXCEPTION_H
