// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		ListenerScripting.cpp
//
//	Description:
//		Persistent plug-in scripting source.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "WakaTimePlugin.h"


//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//
//	ReadScriptParams
//
//	Checks the parameters against scripting-returned parameters, if any, and
//	updates the globals to match ones provided by the scripting system.
//
//-------------------------------------------------------------------------------

SPErr ReadScriptParams( PIActionParameters* actionParams )
{
	SPErr error = kSPNoError;
	
	// We should put this whole list of listeners, but instead we'll put just
	// the last listener from the list:
	
	PIActionDescriptor descriptor = actionParams->descriptor;
				
	// If we got a valid descriptor, grab our key out of it:
	if ( descriptor != NULL )
	{
		uint32 stringLength = 0;
		error = sPSActionDescriptor->GetStringLength( descriptor, 
													  keyPIActionSet, 
													  &stringLength );
		if ( error == kSPNoError )
			gActionSet = new string;
		
		if ( gActionSet == NULL )
			error = kSPOutOfMemoryError;

		vector<char> vc( stringLength + 1 );
		
		if ( error == kSPNoError )
			error = sPSActionDescriptor->GetString( descriptor, 
													keyPIActionSet, 
													&vc[0],
													stringLength );
		
		if ( error == kSPNoError )
			gActionSet->assign( vc.begin(), vc.begin() + stringLength );

		stringLength = 0;
				
		if ( error == kSPNoError )
			error = sPSActionDescriptor->GetStringLength( descriptor, 
														  keyPIAction, 
														  &stringLength );
		if ( error == kSPNoError )
			gActionName = new string;

		if ( gActionName == NULL )
			error = kSPOutOfMemoryError;
			
		vector<char> vc2( stringLength + 1 );

		if ( error == kSPNoError )
			error = sPSActionDescriptor->GetString( descriptor, 
													keyPIAction, 
													&vc2[0],
													stringLength );
		if ( error == kSPNoError )
			gActionName->assign( vc2.begin(), vc2.begin() + stringLength );

	}

	return error;
	
} // end ReadScriptParams
		
//-------------------------------------------------------------------------------
//
//	WriteScriptParams
//
//	Takes our parameters from our global variables and writes them out to the
//	scripting system, which hands it all back to the host.
//
//-------------------------------------------------------------------------------

SPErr WriteScriptParams( PIActionParameters* actionParams )
{
	SPErr error = kSPNoError; // we'll return any error with this

	Listener_t* listener = FindListenerListEnd();

	if (sPSActionDescriptor != NULL && listener != NULL)
	{ // Make sure we have a valid suite before doing this.
		PIActionDescriptor	descriptor = 0;	// token to write our parameters to
		
		error = sPSActionDescriptor->Make(&descriptor);
		
		if (error == kSPNoError && descriptor != 0)
		{
			error = sPSActionDescriptor->PutString( descriptor,
													keyPIActionSet,
													(char*)(listener->actionSet.c_str()) );
				
			if (error == kSPNoError)
			{
				error = sPSActionDescriptor->PutString( descriptor,
					                                    keyPIAction,
														(char*)(listener->actionName.c_str()) );
			}
				
		}
			
		// Now stuff our return descriptor for return to the host:
		if (error == 0 && actionParams != NULL)
		{
			if (actionParams->descriptor)
				sPSActionDescriptor->Free( actionParams->descriptor );

			actionParams->descriptor = descriptor;
			actionParams->recordInfo = plugInDialogOptional;
		}
	}
	
	return error;
		
} // end WriteScriptParams
	
//-------------------------------------------------------------------------------

// end ListenerScripting.cpp
