//////////////////////////////////////////////////////////////////////////////
//
//  --- CheckError.h ---
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __MRKCHECKERROR_H__
#define __MRKCHECKERROR_H__

#include <stdio.h>
#include <GL/gl.h>

//----------------------------------------------------------------------------

static const char*
MRKErrorString( GLenum error )
{
    const char*  msg;
    switch( error ) {
#define Case( Token )  case Token: msg = #Token; break;
	Case( GL_NO_ERROR );
	Case( GL_INVALID_VALUE );
	Case( GL_INVALID_ENUM );
	Case( GL_INVALID_OPERATION );
	Case( GL_STACK_OVERFLOW );
	Case( GL_STACK_UNDERFLOW );
	Case( GL_OUT_OF_MEMORY );
#undef Case	
    }

    return msg;
}

//----------------------------------------------------------------------------

static void
_MRKCheckError( const char* file, int line )
{
    GLenum  error = glGetError();

    while ((error = glGetError()) != GL_NO_ERROR )
    {
        fprintf( stderr, "[%s:%d] %s\n", file, line, ErrorString(error) );
    }
}

//----------------------------------------------------------------------------

#define MRKCheckError()  _MRKCheckError( __FILE__, __LINE__ )

//----------------------------------------------------------------------------

#endif // !__CHECKERROR_H__
