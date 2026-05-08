// ==========================================================================
// 							Class Specification : COXBitBuffer
// ==========================================================================

// Header file : OXBITBUF.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class allows bitwase access to a buffer of bytes

// Remark:
//		

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////
#ifndef __BITBUFF_H__
#define __BITBUFF_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


class OX_CLASS_DECL COXBitBuffer : public CObject
{
DECLARE_DYNAMIC(COXBitBuffer)

// Data members -------------------------------------------------------------
public:
	
protected:
	LPBYTE        m_pBuffer;
	unsigned char m_ucMask;
  	long          m_cByte;
  	int	          m_nRack;
    long 		  m_nLastByte;
    
private:
	
// Member functions ---------------------------------------------------------
public:
	COXBitBuffer(LPBYTE pBuffer);
	// --- In  : pBuffer : The buffer from which the bits will be read or written
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state
    
    COXBitBuffer(LPBYTE pBuffer, int nInLength );
    // --- In  : pBuffer : The buffer from which the bits will be read or written
	//         : InLength: sets the length of PBuffer to detect last byte
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state
        
	void OutputBit(int bit);
	// --- In       : bit, bit to be written
	// --- Out      : 
	// --- Returns  :
	// --- Effect   : writing a bit to the outputbuffer (compressing)
	   
	
	void OutputBits(int code, int count); 
	// --- In       : code    : sequence of bits being written
	//				  count   : number of bits being written
	// ---Out       :
	// ---Returns   :
	// ---Effect    : compressing count bits to the outputbuffer
	
	void OutputBitsEOS();
	// ---In        : code    : sequence of null bits to mark end of buffer
	//				: count   : END OF STREAM, 12 bits
	// ---Out		:
	// ---Returns   : 
	// ---Effect    : terminating with EOS zero bits to mark end of buffer
	 	
	int InputBit();
    // ---In 		:
    // ---Out		:
    // ---Returns	: bit from buffer
    // ---Effect	: reading a bit from the input buffer for expanding
     
	unsigned long InputBits(int bit_count);
	// ---In		: bit_count : number of bit being read
	// ---Out		: 
	// ---Returns   : multiple bits read from buffer
	// ---Effect	: reading multiple bits from the inputbuffer for expanding		: 
    
	long GetCompressedLength() 
		{ 
		return m_cByte; 
		};
	// ---In		:
	// ---Out		:
	// ---Returns	: m_cByte : number of compressed bytes
	// ---Effect	: reach for protected member variable
        
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXBitBuffer();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      

private:
                   
};

#endif
// ==========================================================================
