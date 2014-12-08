//-------------------------------------------------------------------------------
//
//	File:
//		WakaTimeDefines.h
//
//	Description:
//		Resource and code definitions for WakaTime plug-in. 
//
//-------------------------------------------------------------------------------

#ifndef __WakaTime_Defines_h
#define __WakaTime_Defines_h



//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName			"WakaTime"
#define plugInCopyrightYear	"2014"
#define plugInDescription 	"Analytics for programmers"

//-------------------------------------------------------------------------------
//	Definitions -- Scripting keys
//-------------------------------------------------------------------------------

#define keyPIAction		'actN'
#define keyPIActionSet	'actS'

//-------------------------------------------------------------------------------
//	Definitions -- Resources
//-------------------------------------------------------------------------------
// Have to do it this way, and not with an enum, because the Windows
// precompiler does not do the right thing when precompiling the .r file:
#define ListenerResourceID		18500
#define kListenerDialogID		ListenerResourceID+1

#define kListenerAboutID		kListenerDialogID+1

//-------------------------------------------------------------------------------

#endif // __WakaTime_Defines_h