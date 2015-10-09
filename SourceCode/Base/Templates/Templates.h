/*
=============================================================================
	File:	Templates.h
	Desc:	
=============================================================================
*/

#ifndef __MX_TEMPLATES_H__
#define __MX_TEMPLATES_H__


#if CPP_0X
	#define foreach( variable, container )	\
		for( auto variable = (container).GetStart();	\
			variable != (container).GetEnd();	\
			++variable )
#else
	#warning foreach not implemented
#endif



#endif // !__MX_TEMPLATES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
