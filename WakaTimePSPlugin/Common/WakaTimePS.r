//-------------------------------------------------------------------------------
//
//	File:
//		WakaTimePS.r
//
//	Description:
//		This is a special file that gets converted to required resource in
//		the windows project. (It will be used as is in the Mac build.)
//
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName			"WakaTime"
#define plugInAETEComment 	"Analytics for programmers"

#define plugInSuiteID		'WTPSG'
#define plugInClassID		plugInSuiteID
#define plugInEventID		'WTPSN'

//-------------------------------------------------------------------------------
//	Set up included files for Macintosh and Windows.
//-------------------------------------------------------------------------------

#include "PIDefines.h"

#ifdef __PIMac__
	#include "Types.r"
	#include "SysTypes.r"
	#include "WakaTimeDefines.h"
	#include "PIGeneral.r"
	#include "PIUtilities.r"
#elif defined(__PIWin__)
	#include "WakaTimeDefines.h"
	#define Rez
	 #include "PIGeneral.h"
	 #include "PIUtilities.r"
#endif

#include "PITerminology.h"
#include "PIActions.h"


//-------------------------------------------------------------------------------
//	PiPL resource
//-------------------------------------------------------------------------------

resource 'PiPL' (ListenerResourceID, plugInName " PiPL", purgeable)
	{
		{
		Kind { Actions },
		Name { plugInName "..." },
		Category { vendorName },
		Version { (latestActionsPlugInVersion << 16) | latestActionsPlugInSubVersion },

		#ifdef __PIMac__
			#if (defined(__x86_64__))
				CodeMacIntel64 { "AutoPluginMain" },
			#endif
			#if (defined(__i386__))
				CodeMacIntel32 { "AutoPluginMain" },
			#endif
		#else
			#if defined(_WIN64)
				CodeWin64X86 { "AutoPluginMain" },
			#else
				CodeWin32X86 { "AutoPluginMain" },
			#endif
		#endif

		HasTerminology
			{ 
			plugInClassID, 
			plugInEventID, 
			ListenerResourceID, 
			vendorName " " plugInName	// Unique string.
			},
		
		// Include "Persistent" if you want your plug-in to stay loaded
		// until Photoshop quits:
		Persistent {},
		
		// Messages {} is only honored if your plug-in is Persistent:
		Messages
			{
			startupRequired,
			doesNotPurgeCache,
			shutdownRequired,
			acceptProperty
			},
		
		// If you want this on all the time, remove the EnableInfo property (such
		// as for help menu entries.)  To have this on according to document
		// open, close, and mode guidelines, provide a minimal EnableInfo, such
		// as the one here:
		// I'm going to comment EnableInfo out so that this plug-in
		// is available all the time (absence of EnableInfo = always available).
		// EnableInfo { "true" },
		
		}
	};

//-------------------------------------------------------------------------------
//	Dictionary (scripting) resource
//-------------------------------------------------------------------------------

resource 'aete' (ListenerResourceID, plugInName " dictionary", purgeable)
	{
	1, 0, english, roman,					/* aete version and language specifiers */
		{
		vendorName,							/* vendor suite name */
		"Adobe example plug-ins",			/* optional description */
		plugInSuiteID,						/* suite ID */
		1,									/* suite code, must be 1 */
		1,									/* suite level, must be 1 */
			{								/* structure for automation */
			plugInName,						/* name */
			plugInAETEComment,				/* optional description */
			plugInClassID,					/* class ID, must be unique or Suite ID */
			plugInEventID,					/* event ID, must be unique */

			NO_REPLY,						/* never a reply */
			IMAGE_DIRECT_PARAMETER,			/* direct parameter, used by Photoshop */
				{							// filter or selection class here:
				// name:
				"action set",
				// key ID:
				keyPIActionSet,
				// type ID:
				typeChar,
				// optional description:
				"",
				// flags:
				flagsSingleParameter,

				// name:
				"action",
				// key ID:
				keyPIAction,
				// type ID:
				typeChar,
				// optional description:
				"",
				// flags:
				flagsSingleParameter
				}
			},
			{},	/* non-filter/automation plug-in class here */
			{}, /* comparison ops (not supported) */
			{ // Enumerations go here:
			}	/* end of any enumerations */
		}
	};

resource 'STR ' (kListenerAboutID, plugInName " About Text", purgeable)
{
	plugInName "\n\n"
	"Version " VersionString " "
	"Release " ReleaseString "\n"
	"Copyright " plugInCopyrightYear "-" CurrentYear ", WakaTime.\n"
	"All Rights Reserved.\n\n"
	plugInDescription
};
//-------------------------------------------------------------------------------

// end Listener.r
