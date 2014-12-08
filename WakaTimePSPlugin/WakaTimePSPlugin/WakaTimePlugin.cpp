//-------------------------------------------------------------------------------
//
//	File:
//		WakaTimePlugin.cpp
// 
//	Description:
//		This is the main file of our plugin. Every event that we are interested
//		in should be handled here.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "WakaTimePlugin.h"
#include "View.h"
#include "PITerminology.h"

#ifndef MAX_PATH
#define MAX_PATH	256
#endif

// global suite pointers for our plug-in to use
SPBasicSuite			*sSPBasic = NULL; // this is part of the AutoPluginMain message
SPPluginRef				gPlugInRef = NULL;

//-------------------------------------------------------------------------------
//	Globals -- Define global variables for plug-in scope.
//-------------------------------------------------------------------------------
std::string * gActionName = NULL;
std::string * gActionSet = NULL;
DescriptorEventID gEventID = 0;

static bool notifierOn = false;

Listener_t* gListenerList = NULL;

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

// Initialize our parameters:
static void Initialize(void);

// Our listening notifier:
static void Listener
(
/* IN */	DescriptorEventID		event,		// Incoming event.
/* IN */	PIActionDescriptor		descriptor,	// Event descriptor.
/* IN */	PIDialogRecordOptions	options,	// Outgoing dialog options.
/* IN */	void*					data		// Your user data. 
);

// Our Event Dump notifier:
static void EventDumper
(
/* IN */	DescriptorEventID		event,		// Incoming event.
/* IN */	PIActionDescriptor		descriptor,	// Event descriptor.
/* IN */	PIDialogRecordOptions	options,	// Outgoing dialog options.
/* IN */	void*					data		// Your user data. 
);

// Startup call (used to install notifier):
static SPErr Startup(void);

// Shutdown call (used to remove notifier):
static SPErr Shutdown(void);

// Register to receive a notification:
static SPErr DoRegister(void);

// Track listeners in a list:
static Listener_t* AddListenerToList(void);

//-------------------------------------------------------------------------------
//
//	AutoPluginMain
//
//	All calls to the plug-in module come through this routine.
//	It must be placed first in the resource.  To achieve this,
//	most development systems require this be the first routine
//	in the source.
//
//	The entrypoint will be "pascal void" for Macintosh,
//	"void" for Windows.
//
//-------------------------------------------------------------------------------
DLLExport SPAPI SPErr AutoPluginMain(
	const char* caller,	// who is calling
	const char* selector, // what do they want
	void* message	// what is the message
	)
{
	SPErr error = kSPNoError;

	try
	{
		//all messages contain a SPMessageData*
		SPMessageData* basicMessage;
		basicMessage = (SPMessageData*)message;
		sSPBasic = basicMessage->basic;
		gPlugInRef = basicMessage->self;

		// check for SP interface callers
		if (sSPBasic->IsEqual(caller, kSPInterfaceCaller))
		{
			// pop an about box
			if (sSPBasic->IsEqual(selector, kSPInterfaceAboutSelector))
			{
				ShowWakaTimeAbtDialog(gPlugInRef, kListenerAboutID);
			}
			// time to start our plug in
			if (sSPBasic->IsEqual(selector, kSPInterfaceStartupSelector))
				error = Startup();

			// time to end our plug in
			if (sSPBasic->IsEqual(selector, kSPInterfaceShutdownSelector))
				error = Shutdown();
		}

		// Photoshop is calling us
		if (sSPBasic->IsEqual(caller, kPSPhotoshopCaller))
		{
			// the one and only message 
			if (sSPBasic->IsEqual(selector, kPSDoIt))
			{
				//now that we know more we can cast the message
				//to a PSActionsPlugInMessage* and get the action parameters
				PSActionsPlugInMessage* actionsMessage;
				actionsMessage = (PSActionsPlugInMessage*)message;
				PIActionParameters* actionParams;
				actionParams = actionsMessage->actionParameters;
				error = Execute(actionParams);
			}
		}
	}

	catch (...)
	{
		if (error == 0)
			error = kSPBadParameterError;
	}

	return error;
}

//-------------------------------------------------------------------------------
//
//	Execute
//
//	Main routine.
//
//-------------------------------------------------------------------------------
SPErr Execute(PIActionParameters* actionParams)
{
	SPErr error = kSPNoError;

	// Default parameters for all globals.
	Initialize();

	// Override globals with new descriptor info.
	ReadScriptParams(actionParams);

	// Determine if we need to pop our dialog:
	PIDialogPlayOptions playInfo = actionParams->playInfo;

	// Others = plugInDialogDontDisplay / plugInDialogSilent
	if (playInfo == plugInDialogDisplay)
	{
		// Go ahead and display a dialog:
		error = ShowWakaTimeAPIKeyDialog();
		if (error != kSPNoError)
		{
			// delete the globals created by the UI
			// I should remove this global crap
			if (gActionSet != NULL)
			{
				delete gActionSet;
				gActionSet = NULL;
			}
			if (gActionName != NULL)
			{
				delete gActionName;
				gActionName = NULL;
			}
		}
	}

	// register for this event
	if (error == kSPNoError)
		error = DoRegister();

	// write our script parameters to the scripting system
	if (error == kSPNoError)
		WriteScriptParams(actionParams);

	return error;
}

//-------------------------------------------------------------------------------
//
//	Initialize
//
//	Reset parameters.
//
//-------------------------------------------------------------------------------
static void Initialize(void)
{
	// Listeners are only loaded (initialized) once, so don't re-initialize
	// parameters here each time we're called but instead favor file-scope
	// initialization.
}

//-------------------------------------------------------------------------------
//
//	Listener
//
//	This is the routine that gets notified for any events that we've
//	registered for.
//
//	You can only have one listening proc registered per event, but you
//	can have the same listening proc for multiple events.
//
//	Use "eventAll" to get notification of all actions events.
//
//	MACINTOSH WARNING: Due to resource management problems, when this
//	routine is called, **YOUR RESOURCE FORK IS CLOSED**.  If you need
//	resources from your fork, load them during startup or call your
//	own plug-in using a Play() command.
//
//	The notifier is intended to be used to make intelligent decisions
//	and dispatch to other things such as external apps.  Keep its
//	execution time down, especially if you're catching "eventAll" or
//	the user will notice an overall slowdown and bang their machine
//	thinking something's wrong.
//	
//-------------------------------------------------------------------------------
static void Listener
(
/* IN */	DescriptorEventID		/* event */,		// Incoming event.
/* IN */	PIActionDescriptor		/* descriptor */,	// Event descriptor.
/* IN */	PIDialogRecordOptions	/* options */,		// Outgoing dialog options.
/* IN */	void*					data				// Your user data. 
)
{
	// This listener dispatches an action, stored in data:
	if (data != NULL)
	{
		Listener_t* listener = (Listener_t*)data;

		if (listener != NULL)
			/* SPErr error = */ PIUActionsPlayByName((char*)(listener->actionSet.c_str()),
			(char*)(listener->actionName.c_str()));
	}
}

//-------------------------------------------------------------------------------
//
//	EventDumper
//
//	This is the routine that gets notified for all events from Photoshop.
//
//	You can only have one listening proc registered per event, but you
//	can have the same listening proc for multiple events.
//
//	Use "eventAll" to get notification of all actions events.   
//
//	MACINTOSH WARNING: Due to resource management problems, when this
//	routine is called, **YOUR RESOURCE FORK IS CLOSED**.  If you need
//	resources from your fork, load them during startup or call your
//	own plug-in using a Play() command.
//
//	The notifier is intended to show an automation plug in programmer
//	how to put event descriptors together. The DumpDescriptor routine
//	takes the event that just happened and walks through the descriptor
//	dumping everything that it finds.
//
//	NOTE: THIS IS FOR THE DEBUG BUILD OF THIS PLUG IN ONLY. THE DEBUG
//	LIBRARY IS RATHER LARGE AND YOU PROBABLY DON'T WANT IT IN YOUR
//	SHIPPING PLUG IN.  
//	
//-------------------------------------------------------------------------------
static void EventDumper
(
/* IN */	DescriptorEventID		 event,		// Incoming event.
/* IN */	PIActionDescriptor		 descriptor,	// Event descriptor.
/* IN */	PIDialogRecordOptions	 /*options*/,		// Outgoing dialog options.
/* IN */	void*					/*data*/			// Your user data. 
)
{
	char logfilename[MAX_PATH];
	char* filename = (char*)"Listener.log";

	SPErr error = GetFullPathToDesktop(logfilename, MAX_PATH);
	if (error)
	{
		logfilename[0] = '\0';
		error = 0;
	}

	strncat(logfilename, filename, MAX_PATH - 1);
	logfilename[MAX_PATH - 1] = '\0';

	PIUDumpDescriptor(event, descriptor, logfilename);
}

//-------------------------------------------------------------------------------
//
//	Startup
//   
//	On the startup call we register our notifier to catch any actions
//	that we're interested in.
//
//	The startup sequence is based on your PiPL.  If you have
//
//		Persistent {},
//
//	Your plug-in will stay resident until Photoshop is quit.  If you have
//
//		Messages { startupRequired, doesNotPurgeCache, shutdownRequired, acceptProperty },
//
//	Your plug-in will receive startup and shutdown messages.  You can also
//	create a combination of the startup/shutdown calls by putting "no" in front
//	of the unnecessary call, such as "noStartupRequired".  purgeCache and
//	acceptProperty are required parameters that are not used in Photoshop 5.0
//	but may be in the future.
//
//	You must have a Persistent property for the Messages property to be
//	respected.
//
//	DO NOT do long processes at Startup.  The user will think something
//	happened during loading and bonk their machine, getting irate.
//
//	The normal calling process for a persistent plug-in with messages:
//		Reload
//		Startup
//		Execute (if user selects you or you are played)
//		Shutdown
//		Unload
//
//	GUARANTEES:
//		If you return from Reload with no error,
//			you are guaranteed an Unload call.
//		If you return from Startup with no error,
//			you are guaranteed a Shutdown call (if you asked for one.)
//
//-------------------------------------------------------------------------------
static SPErr Startup(void)
{
	SPErr error = kSPNoError;

	// Lets register our actions notification routine.  NOTE: It is our
	// responsibility to unregister this when we're done with it.  If we
	// don't and this plug-in gets unloaded, its a ticking time bomb --
	// whatever event we've registered for will eventually happen and the
	// host will call this routine which won't be there and it'll all go
	// boom.
	if (!notifierOn)
	{
		error = sPSActionControl->AddNotify(gPlugInRef,
			eventAll, // event you want to listen for
			EventDumper, // Proc for listening routine.
			NULL); // User data.

		if (!error)
			notifierOn = true;
	}

	return error;
}

//-------------------------------------------------------------------------------
//
//	Shutdown
//
//	It's our responsibility to unregister any notifiers or memory we allocated
//	at startup.  So we iterate through the listener list, both removing
//	any notifiers and cleaning out memory.
//
//-------------------------------------------------------------------------------
static SPErr Shutdown(void)
{
	SPErr error = kSPNoError;

	Listener_t* thisList = gListenerList;

	while (thisList != NULL)
	{
		gEventID = thisList->eventID;

		error = sPSActionControl->RemoveNotify(gPlugInRef,
			gEventID);

		gEventID = 0;
		thisList->eventID = 0;

		Listener_t* nextList = thisList->next;
		delete thisList;

		thisList = nextList;
	}

	if (notifierOn)
		error = sPSActionControl->RemoveNotify(
		gPlugInRef,
		eventAll);				// Event we registered.

	// clean up after ourselves
	PIUSuitesRelease();

	return error;
}

//-------------------------------------------------------------------------------
//
//	DoRegister
//
//	This registers to receive notifications of an event.
//
//-------------------------------------------------------------------------------
static SPErr DoRegister(void)
{
	SPErr error = kSPNoError;
	/*
	// Once you run this, gActionName, gActionSet, gEventID are all
	// set to NULL (so they aren't accidentally deleted by someone)
	// and you must use newListener->actionName, newListener->actionSet,
	// and newListener->eventID to get at this info:
	Listener_t* newListener = AddListenerToList();

	error = sPSActionControl->AddNotify(gPlugInRef,
		newListener->eventID, // event to listen for
		Listener, // Proc for listening routine.
		(void*)newListener); // User data.
	*/
	return error;
}

//-------------------------------------------------------------------------------
//
//	ListenerUtils
//
//	Utility routines to track a list of listener events and actions.
//
//-------------------------------------------------------------------------------
Listener_t* FindListenerListEnd(void)
{
	Listener_t* thisList = gListenerList;

	while (thisList != NULL && thisList->next != NULL)
		thisList = thisList->next;

	return thisList;
}

static Listener_t* AddListenerToList(void)
{
	Listener_t* listEnd = FindListenerListEnd();

	Listener_t* newListener = new Listener_t;

	newListener->eventID = gEventID;
	gEventID = 0;

	// Same for action name:
	newListener->actionName = *gActionName;
	gActionName = NULL;

	newListener->actionSet = *gActionSet;
	gActionSet = NULL;

	newListener->next = NULL;

	if (listEnd != NULL)
		listEnd->next = newListener;
	else
		gListenerList = newListener;

	return newListener;
}

#if Macintosh
SPErr DoUI(void)
{
	return kSPUserCanceledError; /* kSPUnimplementedError */
}
void DoAbout(SPPluginRef plugin, int dialogID)
{
}

#endif
// end Listener.cpp