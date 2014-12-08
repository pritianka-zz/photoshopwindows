//-------------------------------------------------------------------------------
//
//	File:
//		WakaTimePlugin.h
//
//	Description:
//		Header file for the WakaTime plug in.
//
//-------------------------------------------------------------------------------

#ifndef __WakaTime_Plugin_Header		// Has this not been defined yet?
#define __WakaTime_Plugin_Header		// Only include this once by predefining it

#define _MSL_MACHEADERS_INCLUDES_CMATH 1

#if defined(__cplusplus) && TARGET_API_MAC_CARBON && _MSL_MACHEADERS_INCLUDES_CMATH
#include <cmath>
#endif

#include "PIDefines.h"
#include "SPTypes.h"
#include "SPAccess.h"
#include "SPInterf.h"
#include "PIUActionUtils.h" // this needs to be first so the AEObjects.h gets precompiled first

#include "WakaTimeDefines.h"		// Terminology for this plug-in.
#include "PIActionsPlugIn.h"			// Actions plug-in Photoshop header file

#include "PIActions.h"
#include "PIUSuites.h"
#include "PIUActions.h"
#include "PIUGet.h"
#if __PIMac__
#include <Folders.h>
#include <TextUtils.h>
#include <stdio.h>
#endif

//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------
typedef struct Listener_t
{
	string 		actionName;
	string 		actionSet;
	DescriptorEventID 	eventID;
	Listener_t*			next;
} Listener_t;

extern string* gActionName;
extern string* gActionSet;
extern DescriptorEventID gEventID;
extern Listener_t* gListenerList;

extern SPBasicSuite* sSPBasic;
extern SPPluginRef gPlugInRef;
//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------
// Main routine.
SPErr Execute(PIActionParameters* actionParams);
// Read any scripting params.
SPErr ReadScriptParams(PIActionParameters* actionParams);
// Write any scripting params.
SPErr WriteScriptParams(PIActionParameters* actionParams);

Listener_t* FindListenerListEnd(void);

//-------------------------------------------------------------------------------
//	Constants.
//-------------------------------------------------------------------------------
const int32 kMaxStr255Len = 255; // Maximum standard string length. (Pascal, etc.)

//-------------------------------------------------------------------------------

#endif 
