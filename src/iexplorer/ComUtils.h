///////////////////////////////////////////////////////////////
//
// ComUtils.h
//
// Created: 25/05/2003
// Copyright (c) 2003 Ralph Hare (ralph.hare@ysgyfarnog.co.uk)
// All rights reserved.
//
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
//
///////////////////////////////////////////////////////////////

#ifndef __COMUTILS_H_509491D1_A5FB_46D7_87AA_8E5744BB7F1A_
#define __COMUTILS_H_509491D1_A5FB_46D7_87AA_8E5744BB7F1A_

/**
 * Class to test the value of the return code from a COM method
 * call and throw a HRESULT in the event that the call failed.
 * Suggested usage:
 *
 * <code>
 *      ThrowHResult    hr;
 *      try
 *      {
 *          hr = pObj->Method1();
 *          hr = pObj->Method2();
 *      }
 *      catch( HRESULT hr )
 *      {
 *          // do something with the hr here
 *      }
 *      
 *      // or do something with the hr here
 *  </code>
 **/

class ThrowHResult
{
public:
    explicit ThrowHResult( HRESULT hr = S_OK ) :
        m_hr( hr )
    {
        if( FAILED( hr ) )
        {
            ATLASSERT( FALSE );
            throw hr;
        }
    }

    operator HRESULT ()
    {
        return m_hr;
    }

    HRESULT operator = ( HRESULT hr )
    {
        m_hr = hr;

        if( FAILED( m_hr ) )
        {
            ATLASSERT( FALSE );
            throw m_hr;
        }

        return m_hr;
    }

private:
    HRESULT     m_hr;
};

#endif // __COMUTILS_H_509491D1_A5FB_46D7_87AA_8E5744BB7F1A_
