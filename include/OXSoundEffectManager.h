// ==========================================================================
//						   Class Specification : 
//				COXSoundEffectManager & COXSoundEffectOrganizer
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

/*

We introduce three classes that were designed to simplify process of creating 
applications with sound effects functionality. Nowadays, more and more 
computers are sold with sound card installed and sound effects can add another
professional feature that distinguish your software. Of course, sound effect is
a must for multimedia application but sometimes it's good idea to add some sounds
in usual application too. Wouldn't it be nice to hear some sound when you click 
over button or play some sound when mouse moves over some object. Using our new 
classes it takes only one line of code to associate a sound effect with any object
so this effect will be played when specified event is fired.

We already have in our library the class that is capable of playing sound: COXSound. 
And now we move two steps further and provide for you COXSoundEffectManager,
COXSoundEffectOrganizer and COXSoundCustomizeDlg classes. 




//////////////////////////////
COXSoundEffectManager

We noted that sounds are usually played in result of some user action (e.g. mouse 
left button was clicked or specic keyboard button was pressed) or in result of some 
event (e.g. expiring of some period of time). Any event which eventually forces 
an application to play sound we call sound event. To uniquely identify any such
event we define sound event ID. This approach goes perefectly fine with natural 
events such as mouse or keyboard events. In the case when sound played in result of 
application specific event why don't we define it as application specific message
(anyway, windows everywhere). We arrived to the point where we associate with 
any sound played some windows message. 
Now the idea that stands behind COXSoundEffectManager class should be clear. 
Why don't we create the class that will manage sound effects and sound events? 
It could be only one function to register a sound to be played when particular 
events is fired! And as long as sound events are defined as window messages we 
can come up with the class that will work with any object (static, edit, combobox, 
button, listbox, tree, list controls are windows, aren't they). 

In our library we have very powerful class that was designed specifically to hook
any window and handle any message in any specific way. This is COXHookWnd class and
we use it as parent class of COXSoundEffectManager. 

Now, everything is clear. We hook a window and trace all messages. As long as the 
currently handling message is the registered as sound event one we start to play 
associated sound using our COXSound class. Pretty simple.


Now let's explore how to use the functionality provided by COXSoundEffectManager 
class.

First of all as long as COXSoundEffectManager is derived from COXHookWnd class
any target window must be attached to COXSoundEffectManager object using next 
function:

	BOOL Attach(CWnd* pWnd);
	// --- In  :	pWnd		-	pointer to created window to be hooked
	// --- Returns:	TRUE if pWnd was successfully hooked, otherwise FALSE
	// --- Effect : Hooks window

After window was successfully attached you can register whatever sound events 
and any sound to be played when corresponding event is fired. Use next functions
to do that:

	BOOL RegisterPlayEvent(UINT nMsgIDtoStartPlaying, LPCTSTR pszFileName, 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	BOOL RegisterPlayEvent(UINT nMsgIDtoStartPlaying, UINT nSoundResource, 
		HINSTANCE hResInstance = AfxGetResourceHandle(), 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	BOOL RegisterPlayEvent(UINT nMsgIDtoStartPlaying, CFile* pOpenedFile, 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	// --- In  :	nMsgIDtoStartPlaying	-	message ID to start playing sound
	//				pszFileName				-	complete path and filename for a 
	//											WAVE file to open
	//				nSoundResource			-	resourse ID to load WAVE
	//				hResInstance			-	handle to the application that contains
	//											specified resourse
	//				pOpenedFile				-	pointer to CFile object from which to 
	//											read WAVE data
	//				bLoop					-	if TRUE then specified sound will be 
	//											played in loop, otherwise the specified 
	//											sound will be played only once
	//				nLoopCount				-	if bLoop is TRUE then this parameter 
	//											specifies the number of times it will 
	//											be played (note that if you specify 0 the
	//											sound will be played once anyway). If you
	//											set it to -1 the specified sound will be 
	//											played infinitely.
	//				bForceToStart			-	if TRUE then if nMsgIDtoStartPlaying 
	//											message was fired then any sound effect 
	//											that is playing at the moment will be
	//											stoped and the specified sound effect 
	//											will be played, otherwise if 
	//											nMsgIDtoStartPlaying message was fired 
	//											and if any sound effect is playing at 
	//											the moment then the specified sound 
	//											effect won't be played
	//
	// --- Returns: TRUE if sound was registereded successfully to be played if 
	//				nMsgIDtoStartPlaying is fired or FALSE otherwise
	// --- Effect : Register sound effect
	

As you can see you can use as source of sound an existing sound file or resource
from your application and you can set additional options that define the way sound 
is played: sound can be played only once or in loop infinetely or for concrete times.
Also you can specify if sound should be started playing even though another can be
played at the same moment. 

Some sound can be pretty lengthy (especially if you used loop infinitely option).
Although any playing sound will be stopped when hooked window is destroyed or detached 
using Detach function sometimes it's good idea to define some events that would mean
after been fired that any playing sound should be stoped. We call such events as
sound stop events (so the sound events that were described before we call sound play 
event). To register such event use next function:

	BOOL RegisterStopEvent(UINT nMsgIDtoStopPlaying);
	// --- In  :	nMsgIDtoStopPlaying	-	message ID to stop playing sound
	// --- Out : 
	// --- Returns: TRUE if the specified event was registereded successfully 
	//				or FALSE otherwise
	// --- Effect : Register stop event which whould stop playing any effect 
	//				if have been fired

Of course the same event cannot be registered to play different sounds and 
cannot be registered as play and stop event simultenuosely.

Any stop or play sound events can be unregistered using next functions:

	BOOL UnregisterPlayEvent(UINT nMsgIDtoStartPlaying);
	// --- In  :	nMsgIDtoStartPlaying	-	registered message ID to start playing 
	//											sound
	// --- Returns: TRUE if sound was unregistereded successfully or FALSE otherwise
	// --- Effect : Unregister sound effect

	BOOL UnregisterStopEvent(UINT nMsgIDtoStopPlaying);
	// --- In  :	nMsgIDtoStopPlaying	-	registered message ID to stop playing 
	//										any sound
	// --- Returns: TRUE if stop event was unregistereded successfully or FALSE otherwise
	// --- Effect : Unregister stop event

If you would like to unregister all events altogether then you will find next 
functions very helpful:

	void UnregisterAllEffects();
	// --- Effect : Unregister all registered start and stop playing events

	void UnregisterAllPlayEvents();
	// --- Effect : Unregister all registered start playing events

	void UnregisterAllStopEvents();
	// --- Effect : Unregister all registered stop playing events


At any moment you can request whether particular event is registered as stop or play
sound event using next functions:

	BOOL IsRegisteredPlayEvent(UINT nMsgIDtoStartPlaying);
	// --- In  :	nMsgIDtoStartPlaying	-	test message ID to start playing sound
	// --- Returns: TRUE if specified event was registereded as the one to start 
	//				playing sound or FALSE otherwise

	BOOL IsRegisteredStopEvent(UINT nMsgIDtoStopPlaying);
	// --- In  :	nMsgIDtoStopPlaying	-	test message ID to stop playing sound
	// --- Returns: TRUE if specified event was registereded as the one to stop 
	//				playing sound or FALSE otherwise


There is set of functions that you will hardly use but that are very helpful while 
navigating through all registered sound events. They are pretty explanatory 
themselves and here is the list of them:

	POSITION GetFirstPlayEvent()
	// --- Returns: A POSITION value that indicates a starting position for iterating 
	//				the play events; or NULL if there is no registered play event.
	// --- Effect : Starts a play events iteration by returning a POSITION value that 
	//				can be passed to a GetNextPlayEvent call. The iteration sequence is 
	//				not predictable; therefore, the "first play event" has no special 
	//				significance.

	UINT GetNextPlayEvent(POSITION& rNextPosition);
	// --- In  :	rNextPosition	-	Specifies a reference to a POSITION value 
	//									returned by a previous GetNextPlayEvent or 
	//									GetFirstPlayEvent call.
	// --- Returns: message ID that is registered to start playing sound
	// --- Effect : Retrieves the message ID of a play event at rNextPosition, then 
	//				updates rNextPosition to refer to the next play event. This function 
	//				is most useful for iterating through all the play events. 
	//				If the retrieved element is the last play event, then the new value 
	//				of rNextPosition is set to NULL.

	POSITION GetFirstStopEvent()
	// --- Returns: A POSITION value that indicates a starting position for iterating 
	//				the stop events; or NULL if there is no registered stop event.
	// --- Effect : Starts a stop events iteration by returning a POSITION value that 
	//				can be passed to a GetNextStopEvent call. The iteration sequence is 
	//				not predictable; therefore, the "first stop event" has no special 
	//				significance.

	UINT GetNextStopEvent(POSITION& rNextPosition);
	// --- In  :	rNextPosition	-	Specifies a reference to a POSITION value 
	//									returned by a previous GetNextStopEvent or 
	//									GetFirstStopEvent call.
	// --- Returns: message ID that is registered to stop any playing sound
	// --- Effect : Retrieves the message ID of a stop event at rNextPosition, then 
	//				updates rNextPosition to refer to the next stop event. This function 
	//				is most useful for iterating through all the stop events. 
	//				If the retrieved element is the last play event, then the new value 
	//				of rNextPosition is set to NULL.



While programming using COXSoundEffectManager class to full extent you can find next 
function of big help:

	LPSOUNDEFFECT GetSoundEffect(UINT nMsgIDtoStartPlaying);
	// --- In  :	nMsgIDtoStartPlaying	-	registered message ID to start playing 
	//											sound
	// --- Returns: pointer to SOUNDEFFECT structure if successful, or NULL otherwise.
	//				SOUNDEFFECT structure is designed specifically to describe any 
	//				registered sound effect and used internally in the class. 
	//				It is declared as follows:
	//
	//				typedef struct _tagSOUNDEFFECT
	//				{
	//					// pointer to opened COXSound object
	//					COXSound *pSound;
	//					// if TRUE then sound will be played in loop, otherwise it 
	//					// will be played only once
	//					BOOL bLoop;
	//					// if bLoop is TRUE then if it equals -1 then sound will be 
	//					// looped infinitely, otherwise it defines the number of 
	//					// times it will be looped. The value of zero means that 
	//					// sound will be still played once.
	//					int nLoopCount;
	//					// if TRUE then the sound will be started to play even though 
	//					// another sound is played at the same moment, otherwise if 
	//					// any sound was playing at the moment when the associated 
	//					// sound event was fired then the sound won't be played.
	//					BOOL bForceToStart;
	//
	//					. . . . . . . . . . . . . . . . . . . . .
	//
	//				}
	//


	BOOL StopPlaying();
	// --- Returns: TRUE if successful or FALSE otherwise
	// --- Effect : Stops playing any registered sound effect
	BOOL IsPlaying();
	// --- Returns: TRUE if any registered sound effect is being played 
	//				or FALSE otherwise

As you can see you can determine at any moment if any sound effect is playing and
stop playing any sound programmatically without sending any registered stop event.
And at any moment you can request info about any registered sound effect.

As we mentioned above all these are functions rarely used due to the fact that they 
operate on internal COXSoundEffectManager data and only defined as public ones in 
order to give a sophisticated application a way to control sound events and effects
at the very low level.

But next two functions can be of great value. Every people have their own preferences.
One sound can be good for one person while unbearable for the other. Choosing the 
right sound can be a nightmare for a programmer. Another problem can be with sequences
of defined sound events (someone might prefer to hear as less sounds as possible and
someone else could be a meloman, you never know). So we decided to provide the 
functionality of saving in registry all registered play and stop sound events and 
sound effects. Next two functions are defined:

	BOOL SaveState(LPCTSTR lpszProfileName);
	// --- In  :	lpszProfileName	-	name of hive in registry where
	//									all information about COXSoundEffectManager
	//									will be saved. 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Saves COXSoundEffectManager registered start and stop playing 
	//				effects into registry
	BOOL LoadState(LPCTSTR lpszProfileName);
	// --- In  :	lpszProfileName	-	name of hive in registry where
	//									all information about COXSoundEffectManager
	//									was saved. 
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Loads COXSoundEffectManager start and stop playing effects from 
	//				registry file


//////////////////////////////////////////////////////////////////////////////
		IMPORTANT !!!
//////////////////////////////////////////////////////////////////////////////
If you don't need to save state of Sound Effect Manager settings into 
registry then you probably wouldn't like to include source code for 
COXRegistryValFile class into your project (that is a must if you 
call COXSoundEffectManager::SaveState() and/or COXSoundEffectManager::LoadState()).
If this is a case then you have to include in you stdafx.h file next code:
	
	#define OXSE_NO_SAVESTATE

//////////////////////////////////////////////////////////////////////////////



We described all functionality you've got as a programmer using COXSoundEffectManager
class. But from our point of view this class has two weak sides. First of all, it is 
designed to handle only one target window at the moment. That means if you want 
to assign sound effects to five buttons then you have to create five  
COXSoundEffectManager objects which adds a lot of weight to your source code. 
To resolve this problem we created COXSoundEffectOrganizer class which can be used
to register sound effects for multiple windows. Of course internally this class
creates COXSoundEffectManager object for every registered window but hides this fact 
from a programmer while providing the same functionality as COXSoundEffectManager 
does. Plus COXSoundEffectOrganizer is aware about all COXSoundEffectManager objects
it manages and can stop and start playing sound in any of them (that is why 
COXSoundEffectManager constructor is designed to take pointer to 
COXSoundEffectOrganizer object as parameter which must be set to NULL if 
COXSoundEffectManager object is used as stand-alone). 

!!! It is higly recommended to use COXSoundEffectOrganizer instead of 
	COXSoundEffectManager even in situation when you provide sound effects 
	only for one window	!!!

Another problem has to do with customizing of sound effects. Although we 
provide basic functions to save into and load from registry state of any 
COXSoundEffectManager object, there is still a problem to provide an easy way to 
users to customize sound effects so they can do that automatically. In order to 
resolve that problem we designed COXSoundCustomizeDlg class which takes pointer
to COXSoundEffectOrganizer object and provides user interface to add, delete and 
edit any sound events and effects for all windows registered with given 
COXSoundEffectOrganizer object (another reason to use always COXSoundEffectOrganizer
instead of COXSoundEffectManager).

Refer to the documentation on both classes for more details.




/////////////////////////////
COXSoundEffectOrganizer

COXSoundEffectManager class provides all the functionality we need to associate any
sound events with one window object. It's not very convinient when you have to 
provide sound effects for big number of window objects (you have to create 
COXSoundEffectManager object for every such window object). And still you have a
problem with organizing all these COXSoundEffectManager objects. To simplify
the process of using multiple instances of COXSoundEffectManager object and 
manage them in the most effective way we designed COXSoundEffectOrganizer class.

We duplicated most functions that can be found in COXSoundEffectManager class
just by adding an additional parameter - pointer to the window object that was
registered or about to be registered with any sound event. E.g. to register whatever 
sound events and any sound to be played when corresponding event is fired with any 
window object you have to use next functions:

	BOOL RegisterPlayEvent(CWnd* pWnd, UINT nMsgIDtoStartPlaying, 
		LPCTSTR pszFileName, BOOL bLoop=FALSE, int nLoopCount=-1, 
		BOOL bForceToStart=TRUE);
	BOOL RegisterPlayEvent(CWnd* pWnd, UINT nMsgIDtoStartPlaying, 
		UINT nSoundResource, HINSTANCE hResInstance = AfxGetResourceHandle(), 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	BOOL RegisterPlayEvent(CWnd* pWnd, UINT nMsgIDtoStartPlaying, CFile* pOpenedFile, 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	// --- In  :	pWnd					-	pointer to the window for which specified
	//											playing event will be created
	//				nMsgIDtoStartPlaying	-	message ID to start playing sound
	//				pszFileName				-	complete path and filename for a 
	//											WAVE file to open
	//				nSoundResource			-	resourse ID to load WAVE
	//				hResInstance			-	handle to the application that contains
	//											specified resourse
	//				pOpenedFile				-	pointer to CFile object from which to 
	//											read WAVE data
	//				bLoop					-	if TRUE then specified sound will be 
	//											played in loop, otherwise the specified 
	//											sound will be played only once
	//				nLoopCount				-	if bLoop is TRUE then this parameter 
	//											specifies the number of times it will 
	//											be played (note that if you specify 0 the
	//											sound will be played once anyway). If you
	//											set it to -1 the specified sound will be 
	//											played infinitely.
	//				bForceToStart			-	if TRUE then if nMsgIDtoStartPlaying 
	//											message was fired then any sound effect 
	//											that is playing at the moment will be
	//											stoped and the specified sound effect 
	//											will be played, otherwise if 
	//											nMsgIDtoStartPlaying message was fired 
	//											and if any sound effect is playing at 
	//											the moment then the specified sound 
	//											effect won't be played
	//
	// --- Out : 
	// --- Returns: TRUE if sound was registereded successfully to be played if 
	//				nMsgIDtoStartPlaying is fired in pWnd window or FALSE otherwise
	// --- Effect : Register sound effect


So you have to add only one line of code to set sound event for a window object, e.g.:

	m_SoundEffectOrganizer.RegisterPlayEvent(&m_btnAbout,WM_LBUTTONDOWN,
		_T(".\\Sound\\sound.wav")));

As you can see you can use as source of sound an existing sound file or resource
from your application and you can set additional options that define the way sound 
is played: sound can be played only once or loop infinetely or loop for specified number
of times. Also you can specify if sound should be started playing even though another 
can be played at the same moment. COXSoundEffectOrganizer will take into account any
currently playing sound event associated with any registered window object. The same
considerations apply for next counterparts of COXSoundEffectManager functions:

	BOOL UnregisterPlayEvent(CWnd* pWnd, UINT nMsgIDtoStartPlaying);
	// --- In  :	pWnd					-	pointer to the window for which specified
	//											playing event will be unregistered
	// ---			nMsgIDtoStartPlaying	-	registered message ID to start playing 
	//											sound
	// --- Out : 
	// --- Returns: TRUE if sound was unregistereded successfully or FALSE otherwise
	// --- Effect : Unregister sound effect

	BOOL RegisterStopEvent(CWnd* pWnd, UINT nMsgIDtoStopPlaying);
	// --- In  :	pWnd				-	pointer to the window for which specified
	//										stop event will be created
	// ---			nMsgIDtoStopPlaying	-	message ID to stop playing sound
	// --- Out : 
	// --- Returns: TRUE if the specified event was registereded successfully 
	//				or FALSE otherwise
	// --- Effect : Register stop event which whould stop playing any effect 
	//				if have been fired in the specified window
	BOOL UnregisterStopEvent(CWnd* pWnd, UINT nMsgIDtoStopPlaying);
	// --- In  :	pWnd				-	pointer to the window for which specified
	//										stop event will be unregistered
	// ---			nMsgIDtoStopPlaying	-	registered message ID to stop playing 
	//										any sound
	// --- Out : 
	// --- Returns: TRUE if stop event was unregistereded successfully or FALSE otherwise
	// --- Effect : Unregister stop event

	BOOL StopPlaying(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which any playing
	//										event will be stoped. If pWnd is set to NULL 
	//										then any currently playing sound effect in 
	//										any window will be stopped
	// --- Out : 
	// --- Returns: TRUE if successful or FALSE otherwise
	// --- Effect : Stops playing any registered sound effect in specified window.
	//				
	BOOL IsPlaying(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which any playing
	//										event will be searching for. If pWnd is set 
	//										to NULL then any currently playing sound 
	//										effect in any window will be searching for
	// --- Out : 
	// --- Returns: TRUE if in specified window any registered sound effect is being 
	//				played or FALSE otherwise
	// --- Effect : 

	void UnregisterAllEffects(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which all 
	//										registered start and stop playing events 
	//										will be unregistered. If pWnd is set 
	//										to NULL then all registered start and stop 
	//										playing events will be unregistered for all
	//										windows
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered start and stop playing events in the 
	//				specified window

	void UnregisterAllPlayEvents(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which all 
	//										registered start playing events 
	//										will be unregistered. If pWnd is set 
	//										to NULL then all registered start 
	//										playing events will be unregistered for all
	//										windows
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered start playing events in the 
	//				specified window

	void UnregisterAllStopEvents(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which all 
	//										registered stop playing events 
	//										will be unregistered. If pWnd is set 
	//										to NULL then all registered stop 
	//										playing events will be unregistered for all
	//										windows
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered stop playing events in the 
	//				specified window

Refer to the COXSoundEffectManager class documentation for more detailed description of 
above mentioned functions.

There is set of functions that you will hardly use but that are very helpful while 
navigating through all window object registered with some sound events. They are pretty 
explanatory themselves and here is the list of them:

	inline POSITION GetFirstRegisteredWindow() { 
		return m_allSoundEffectManagers.GetStartPosition(); }
	// --- In  :	
	// --- Out : 
	// --- Returns: A POSITION value that indicates a starting position for iterating 
	//				the registered window; or NULL if there is no any registered 
	//				window.
	// --- Effect : Starts an registered windows iteration by returning a POSITION 
	//				value that can be passed to a GetNextRegisteredWindow call. The 
	//				iteration sequence is not predictable; therefore, the "first 
	//				registered window" has no special significance.

	CWnd* GetNextRegisteredWindow(POSITION& rNextPosition);
	// --- In  :	rNextPosition	-	Specifies a reference to a POSITION value 
	//									returned by a previous GetNextRegisteredWindow 
	//									or GetNextRegisteredWindow call.
	// --- Out : 
	// --- Returns: pointer to the window that has been registered with the organizer
	// --- Effect : Retrieves the the window that has been registered with the 
	//				organizer at rNextPosition, then updates rNextPosition to refer 
	//				to the next window. This function is most useful for iterating 
	//				through all the registered windows. 
	//				If the retrieved element is the last window, then the new value 
	//				of rNextPosition is set to NULL.



If you need full control on COXSoundEffectOrganizer class use next function to get
pointer to COXSoundEffectManager object used internally to manage sound effects for
particular window object:

	COXSoundEffectManager* GetManager(CWnd* pWnd);
	// --- In  :	pWnd					-	pointer to the window 
	// --- Out : 
	// --- Returns: pointer to corresponding COXSoundEffectManager object if successful, 
	//				or NULL otherwise
	// --- Effect : 

As we mentioned above these functions are rarely used due to the fact that they 
operate on internal COXSoundEffectOrganizer data and only defined as public ones in 
order to give a sophisticated application a way to control sound events and effects
at the very low level.

But next two functions can be of great value. Every people have their own preferences.
One sound can be good for one person while unbearable for the other. Choosing the 
right sound can be a nightmare for a programmer. Another problem can be with sequences
of defined sound events (someone might prefer to hear as less sounds as possible and
someone else could be a meloman, you never know). So we decided to provide the 
functionality of saving in registry all registered play and stop sound events and 
sound effects. Next two functions are defined:

	BOOL SaveState(CWnd* pWnd, LPCTSTR lpszProfileName);
	// --- In  :	pWnd			-	pointer to the window for which all play and
	//									stop event will be saved
	//				lpszProfileName	-	name of hive in registry where
	//									all information about sound effects associated
	//									with pWnd will be saved. 
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Saves pWnd's registered start and stop playing 
	//				effects into registry
	BOOL LoadState(CWnd* pWnd, LPCTSTR lpszProfileName);
	// --- In  :	pWnd			-	pointer to the window for which all restored 
	//									play and stop event will be applied
	//				lpszProfileName	-	name of hive in registry where
	//									all information about sound effects associated
	//									with pWnd was saved. 
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Load start and stop playing effects from registry and apply them
	//				to specified window

//////////////////////////////////////////////////////////////////////////////
		IMPORTANT !!!
//////////////////////////////////////////////////////////////////////////////
If you don't need to save state of Sound Effect Manager settings into 
registry then you probably wouldn't like to include source code for 
COXRegistryValFile class into your project (that is a must if you 
call COXSoundEffectOrganizer::SaveState() and/or COXSoundEffectOrganizer::LoadState()).
If this is a case then you have to include in you stdafx.h file next code:
	
	#define OXSE_NO_SAVESTATE

//////////////////////////////////////////////////////////////////////////////



COXSoundEffectOrganizer class provides all functionality to associate any sound effect 
with any window. It's matter only of one line of code to register any sound event. Plus
you've got the functionality of saving registered sound events into registry or 
loading from registry and applying previously saved settings. But wouldn't it be nice
to have standard built-in sound effects customization functionality. In order to 
resolve that problem we designed COXSoundCustomizeDlg class which takes pointer
to COXSoundEffectOrganizer object and provides user interface to add, delete and 
edit any sound events and effects for all windows registered with given 
COXSoundEffectOrganizer object. Refer to the documentation on COXSoundCustomizeDlg 
class for more details.



*/

/////////////////////////////////////////////////////////////////////////////

#ifndef _SOUNDEFFECTMANAGER_H
#define _SOUNDEFFECTMANAGER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXHookWnd.h"
#include "OXSound.h"


#ifndef _WIN32_IE
#if _MFC_VER>=0x0420
#define _WIN32_IE 0x0300
#else
#define _WIN32_IE 0x0200
#endif
#endif


// internally used sound event that specifies that none sound is currently played
#define SNDEVENT_NONE				0xffffffff

// Following structure is defined specifically to describe any sound effect. 
//
typedef struct _tagSOUNDEFFECT
{
	COXSound *pSound;
	BOOL bLoop;
	int nLoopCount;
	BOOL bForceToStart;

	// Constructor
	_tagSOUNDEFFECT()
	{
		pSound=NULL;
		bLoop=FALSE;
		nLoopCount=-1;
		bForceToStart=TRUE;
	}

    // assignment operator
    _tagSOUNDEFFECT& operator=( const _tagSOUNDEFFECT& se )
	{
		if(this==&se)
			return *this;
		
		delete pSound;
		pSound=new COXSound;
		bLoop=se.bLoop;
		nLoopCount=se.nLoopCount;
		bForceToStart=se.bForceToStart;
		return *this;
	}

	// Destructor
	~_tagSOUNDEFFECT()
	{
		delete pSound;
	}

	// Serialization. Defined specifically to support save and load sound effect state
	// in COXSoundEffectManager
	void Serialize(CArchive& ar)
	{
	    // Only CObject-derived objects and six data-type
		// primitives are serializable. However, you
		// can cast any data type to a serializable data type,
		// and then you can serialize your data. The serializable
		// data types are

		// BYTE:    8 bits unsigned
		// WORD:    16 bits unsigned
		// LONG:    32 bits unsigned
		// DWORD:   32 bits unsigned
		// float    32 bits
		// double   64 bits, IEEE standard
    
		if (ar.IsStoring())
		{
			ar << (DWORD)bLoop;
			ar << (DWORD)nLoopCount;
			ar << (DWORD)bForceToStart;
		}
	    else
		{
			delete pSound;
			pSound=new COXSound;
			DWORD dwDummy;
			ar >> dwDummy;
			bLoop=(BOOL)dwDummy;
			ar >> dwDummy;
			nLoopCount=(int)dwDummy;
			ar >> dwDummy;
			bForceToStart=(BOOL)dwDummy;
		}
	}

} SOUNDEFFECT, * LPSOUNDEFFECT;
//
//	pSound			-	pointer to opened COXSound object
//	bLoop			-	if TRUE then sound will be played in loop, otherwise it will 
//						be played only once
//	nLoopCount		-	if bLoop is TRUE then if it equals -1 then sound will be 
//						looped infinitely, otherwise it defines the number of times 
//						it will be looped. The value of zero means that sound will be 
//						still played once.
//	bForceToStart	-	if TRUE then the sound will be started to play even though 
//						another sound is played at the same moment, otherwise if any 
//						sound was playing at the moment when the associated sound event 
//						was fired then the sound won't be played.
//
// This structure is specificaly designed to provide customization functionality
//


/////////////////////////////////////////////////////////////////////////////
class OX_CLASS_DECL COXSoundEffectOrganizer;

class OX_CLASS_DECL COXSoundEffectManager : public COXHookWnd 
{
public:
	COXSoundEffectManager(COXSoundEffectOrganizer* pOrganizer=NULL);
	// --- In  :	pOrganizer	-	pointer to COXSoundEffectOrganizer object 
	//								which created this instance of 
	//								COXSoundEffectManager, otherwise NULL
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object

protected:
	DECLARE_DYNAMIC(COXSoundEffectManager);

	// The heart of the class. Handle all needed messages. While deriving
	// your own class make sure to call this function. I wouldn't recommend you 
	// to change anything in it
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);	
	// --- In  :	msg	-	message ID
	//				wp	-	WPARAM
	//				lp	-	LPARAM
	// --- Out : 
	// --- Returns:	result of message handling. Different for different messages.
	// --- Effect :	Sets the number of color shades used in gradient filling. 

// Attributes
protected:
	// array of all playing events
	CMap<UINT, UINT, LPSOUNDEFFECT, LPSOUNDEFFECT> m_allSoundPlayEvents;
	// array of all stop events
	CMap<UINT, UINT, UINT, UINT> m_allSoundStopEvents;

	// currently playing event
	UINT m_nCurrentEvent;
	// the next event in the queue
	UINT m_nNextEvent;

	// pointer to COXSoundOrganizer object which instantiated this instance of
	// the COXSoundEffectManager
	COXSoundEffectOrganizer* m_pOrganizer;

	// number of times current effect has been played
	int m_nLoopCount;

public:
	virtual ~COXSoundEffectManager();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destroys the object

	BOOL Attach(CWnd* pWnd);
	// --- In  :	pWnd		-	pointer to created window to be hooked
	// --- Out : 
	// --- Returns:	TRUE if pWnd was successfully hooked, otherwise FALSE
	// --- Effect : Hooks window
	void Detach();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Unhooks attached window if any was hooked

	BOOL RegisterPlayEvent(UINT nMsgIDtoStartPlaying, LPCTSTR pszFileName, 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	BOOL RegisterPlayEvent(UINT nMsgIDtoStartPlaying, UINT nSoundResource, 
		HINSTANCE hResInstance = AfxGetResourceHandle(), 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	BOOL RegisterPlayEvent(UINT nMsgIDtoStartPlaying, CFile* pOpenedFile, 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	// --- In  :	nMsgIDtoStartPlaying	-	message ID to start playing sound
	//				sFileName				-	complete path and filename for a 
	//											WAVE file to open
	//				nSoundResource			-	resourse ID to load WAVE
	//				hResInstance			-	handle to the application that contains
	//											specified resourse
	//				pOpenedFile				-	pointer to CFile object from which to 
	//											read WAVE data
	//				bLoop					-	if TRUE then specified sound will be 
	//											played in loop, otherwise the specified 
	//											sound will be played only once
	//				nLoopCount				-	if bLoop is TRUE then this parameter 
	//											specifies the number of times it will 
	//											be played (note that if you specify 0 the
	//											sound will be played once anyway). If you
	//											set it to -1 the specified sound will be 
	//											played infinitely.
	//				bForceToStart			-	if TRUE then if nMsgIDtoStartPlaying 
	//											message was fired then any sound effect 
	//											that is playing at the moment will be
	//											stoped and the specified sound effect 
	//											will be played, otherwise if 
	//											nMsgIDtoStartPlaying message was fired 
	//											and if any sound effect is playing at 
	//											the moment then the specified sound 
	//											effect won't be played
	//
	// --- Out : 
	// --- Returns: TRUE if sound was registereded successfully to be played if 
	//				nMsgIDtoStartPlaying is fired or FALSE otherwise
	// --- Effect : Register sound effect
	
	BOOL UnregisterPlayEvent(UINT nMsgIDtoStartPlaying);
	// --- In  :	nMsgIDtoStartPlaying	-	registered message ID to start playing 
	//											sound
	// --- Out : 
	// --- Returns: TRUE if sound was unregistereded successfully or FALSE otherwise
	// --- Effect : Unregister sound effect

	BOOL IsRegisteredPlayEvent(UINT nMsgIDtoStartPlaying);
	// --- In  :	nMsgIDtoStartPlaying	-	test message ID to start playing sound
	// --- Out : 
	// --- Returns: TRUE if specified event was registereded as the one to start 
	//				playing sound or FALSE otherwise
	// --- Effect : 

	LPSOUNDEFFECT GetSoundEffect(UINT nMsgIDtoStartPlaying);
	// --- In  :	nMsgIDtoStartPlaying	-	registered message ID to start playing 
	//											sound
	// --- Out : 
	// --- Returns: pointer to SOUNDEFFECT structure if successful, or NULL otherwise.
	//				SOUNDEFFECT structure is designed specifically to describe any 
	//				registered sound effect and used internally in the class. 
	//				It is declared as follows:
	//
	//				typedef struct _tagSOUNDEFFECT
	//				{
	//					// pointer to opened COXSound object
	//					COXSound *pSound;
	//					// if TRUE then sound will be played in loop, otherwise it 
	//					// will be played only once
	//					BOOL bLoop;
	//					// if bLoop is TRUE then if it equals -1 then sound will be 
	//					// looped infinitely, otherwise it defines the number of 
	//					// times it will be looped. The value of zero means that 
	//					// sound will be still played once.
	//					int nLoopCount;
	//					// if TRUE then the sound will be started to play even though 
	//					// another sound is played at the same moment, otherwise if 
	//					// any sound was playing at the moment when the associated 
	//					// sound event was fired then the sound won't be played.
	//					BOOL bForceToStart;
	//
	//					. . . . . . . . . . . . . . . . . . . . .
	//
	//				}
	//
	// --- Effect : 

	inline POSITION GetFirstPlayEvent() { 
		return m_allSoundPlayEvents.GetStartPosition(); }
	// --- In  :	
	// --- Out : 
	// --- Returns: A POSITION value that indicates a starting position for iterating 
	//				the play events; or NULL if there is no registered play event.
	// --- Effect : Starts a play events iteration by returning a POSITION value that 
	//				can be passed to a GetNextPlayEvent call. The iteration sequence is 
	//				not predictable; therefore, the "first play event" has no special 
	//				significance.

	UINT GetNextPlayEvent(POSITION& rNextPosition);
	// --- In  :	rNextPosition	-	Specifies a reference to a POSITION value 
	//									returned by a previous GetNextPlayEvent or 
	//									GetFirstPlayEvent call.
	// --- Out : 
	// --- Returns: message ID that is registered to start playing sound
	// --- Effect : Retrieves the message ID of a play event at rNextPosition, then 
	//				updates rNextPosition to refer to the next play event. This function 
	//				is most useful for iterating through all the play events. 
	//				If the retrieved element is the last play event, then the new value 
	//				of rNextPosition is set to NULL.


	BOOL RegisterStopEvent(UINT nMsgIDtoStopPlaying);
	// --- In  :	nMsgIDtoStopPlaying	-	message ID to stop playing sound
	// --- Out : 
	// --- Returns: TRUE if the specified event was registereded successfully 
	//				or FALSE otherwise
	// --- Effect : Register stop event which whould stop playing any effect 
	//				if have been fired

	BOOL UnregisterStopEvent(UINT nMsgIDtoStopPlaying);
	// --- In  :	nMsgIDtoStopPlaying	-	registered message ID to stop playing 
	//										any sound
	// --- Out : 
	// --- Returns: TRUE if stop event was unregistereded successfully or FALSE otherwise
	// --- Effect : Unregister stop event

	BOOL IsRegisteredStopEvent(UINT nMsgIDtoStopPlaying);
	// --- In  :	nMsgIDtoStopPlaying	-	test message ID to stop playing sound
	// --- Out : 
	// --- Returns: TRUE if specified event was registereded as the one to stop 
	//				playing sound or FALSE otherwise
	// --- Effect : 

	inline POSITION GetFirstStopEvent() { 
		return m_allSoundStopEvents.GetStartPosition(); }
	// --- In  :	
	// --- Out : 
	// --- Returns: A POSITION value that indicates a starting position for iterating 
	//				the stop events; or NULL if there is no registered stop event.
	// --- Effect : Starts a stop events iteration by returning a POSITION value that 
	//				can be passed to a GetNextStopEvent call. The iteration sequence is 
	//				not predictable; therefore, the "first stop event" has no special 
	//				significance.

	UINT GetNextStopEvent(POSITION& rNextPosition);
	// --- In  :	rNextPosition	-	Specifies a reference to a POSITION value 
	//									returned by a previous GetNextStopEvent or 
	//									GetFirstStopEvent call.
	// --- Out : 
	// --- Returns: message ID that is registered to stop any playing sound
	// --- Effect : Retrieves the message ID of a stop event at rNextPosition, then 
	//				updates rNextPosition to refer to the next stop event. This function 
	//				is most useful for iterating through all the stop events. 
	//				If the retrieved element is the last play event, then the new value 
	//				of rNextPosition is set to NULL.



	BOOL StopPlaying();
	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if successful or FALSE otherwise
	// --- Effect : Stops playing any registered sound effect
	BOOL IsPlaying();
	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if any registered sound effect is being played 
	//				or FALSE otherwise
	// --- Effect : 

	void UnregisterAllEffects();
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered start and stop playing events

	void UnregisterAllPlayEvents();
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered start playing events

	void UnregisterAllStopEvents();
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered stop playing events

	virtual void Serialize(CArchive& ar);
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : serialize all registered start and stop playing events

#ifndef OXSE_NO_SAVESTATE
	// Save to and load from registry all registered start and stop playing events
	// These functions require using COXRegistryValFile class. If you 
	// are not going to use them then you can include in your stdafx.h 
	// file next code:
	//	
	//		#define OXSE_NO_SAVESTATE
	//
	// In this case you don't need to include into your project references 
	// to source code for COXRegistryValFile class.
	//
	BOOL SaveState(LPCTSTR lpszProfileName);
	// --- In  :	lpszProfileName	-	name of hive in registry where
	//									all information about COXSoundEffectManager
	//									will be saved. 
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Saves COXSoundEffectManager registered start and stop playing 
	//				effects into registry
	BOOL LoadState(LPCTSTR lpszProfileName);
	// --- In  :	lpszProfileName	-	name of hive in registry where
	//									all information about COXSoundEffectManager
	//									was saved. 
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Loads COXSoundEffectManager start and stop playing effects from 
	//				registry file
#endif // OXCP_NO_SAVESTATE

protected:
	// creates sound
	COXSound* CreateSound();
	// register sound effect
	BOOL RegisterSoundEffect(UINT nMsgIDtoStartPlaying, COXSound* pSound, 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	// start playing current sound effect
	BOOL StartPlaying();
	// reset all settings
	void Reset();

};



class OX_CLASS_DECL COXSoundEffectOrganizer
{
public:
	COXSoundEffectOrganizer();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object

protected:

// Attributes
protected:
	CMap<CWnd*, CWnd*, COXSoundEffectManager*, COXSoundEffectManager*> 
		m_allSoundEffectManagers;

public:
	virtual ~COXSoundEffectOrganizer();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destroys the object

	BOOL RegisterPlayEvent(CWnd* pWnd, UINT nMsgIDtoStartPlaying, 
		LPCTSTR pszFileName, BOOL bLoop=FALSE, int nLoopCount=-1, 
		BOOL bForceToStart=TRUE);
	BOOL RegisterPlayEvent(CWnd* pWnd, UINT nMsgIDtoStartPlaying, 
		UINT nSoundResource, HINSTANCE hResInstance = AfxGetResourceHandle(), 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	BOOL RegisterPlayEvent(CWnd* pWnd, UINT nMsgIDtoStartPlaying, CFile* pOpenedFile, 
		BOOL bLoop=FALSE, int nLoopCount=-1, BOOL bForceToStart=TRUE);
	// --- In  :	pWnd					-	pointer to the window for which specified
	//											playing event will be created
	//				nMsgIDtoStartPlaying	-	message ID to start playing sound
	//				sFileName				-	complete path and filename for a 
	//											WAVE file to open
	//				nSoundResource			-	resourse ID to load WAVE
	//				hResInstance			-	handle to the application that contains
	//											specified resourse
	//				pOpenedFile				-	pointer to CFile object from which to 
	//											read WAVE data
	//				bLoop					-	if TRUE then specified sound will be 
	//											played in loop, otherwise the specified 
	//											sound will be played only once
	//				nLoopCount				-	if bLoop is TRUE then this parameter 
	//											specifies the number of times it will 
	//											be played (note that if you specify 0 the
	//											sound will be played once anyway). If you
	//											set it to -1 the specified sound will be 
	//											played infinitely.
	//				bForceToStart			-	if TRUE then if nMsgIDtoStartPlaying 
	//											message was fired then any sound effect 
	//											that is playing at the moment will be
	//											stoped and the specified sound effect 
	//											will be played, otherwise if 
	//											nMsgIDtoStartPlaying message was fired 
	//											and if any sound effect is playing at 
	//											the moment then the specified sound 
	//											effect won't be played
	//
	// --- Out : 
	// --- Returns: TRUE if sound was registereded successfully to be played if 
	//				nMsgIDtoStartPlaying is fired in pWnd window or FALSE otherwise
	// --- Effect : Register sound effect

	BOOL UnregisterPlayEvent(CWnd* pWnd, UINT nMsgIDtoStartPlaying);
	// --- In  :	pWnd					-	pointer to the window for which specified
	//											playing event will be unregistered
	// ---			nMsgIDtoStartPlaying	-	registered message ID to start playing 
	//											sound
	// --- Out : 
	// --- Returns: TRUE if sound was unregistereded successfully or FALSE otherwise
	// --- Effect : Unregister sound effect

	COXSoundEffectManager* GetManager(CWnd* pWnd);
	// --- In  :	pWnd					-	pointer to the window 
	// --- Out : 
	// --- Returns: pointer to corresponding COXSoundEffectManager object if successful, 
	//				or NULL otherwise
	// --- Effect : 

	inline POSITION GetFirstRegisteredWindow() { 
		return m_allSoundEffectManagers.GetStartPosition(); }
	// --- In  :	
	// --- Out : 
	// --- Returns: A POSITION value that indicates a starting position for iterating 
	//				the registered window; or NULL if there is no any registered 
	//				window.
	// --- Effect : Starts an registered windows iteration by returning a POSITION 
	//				value that can be passed to a GetNextRegisteredWindow call. The 
	//				iteration sequence is not predictable; therefore, the "first 
	//				registered window" has no special significance.

	CWnd* GetNextRegisteredWindow(POSITION& rNextPosition);
	// --- In  :	rNextPosition	-	Specifies a reference to a POSITION value 
	//									returned by a previous GetNextRegisteredWindow 
	//									or GetNextRegisteredWindow call.
	// --- Out : 
	// --- Returns: pointer to the window that has been registered with the organizer
	// --- Effect : Retrieves the the window that has been registered with the 
	//				organizer at rNextPosition, then updates rNextPosition to refer 
	//				to the next window. This function is most useful for iterating 
	//				through all the registered windows. 
	//				If the retrieved element is the last window, then the new value 
	//				of rNextPosition is set to NULL.

	BOOL RegisterStopEvent(CWnd* pWnd, UINT nMsgIDtoStopPlaying);
	// --- In  :	pWnd				-	pointer to the window for which specified
	//										stop event will be created
	// ---			nMsgIDtoStopPlaying	-	message ID to stop playing sound
	// --- Out : 
	// --- Returns: TRUE if the specified event was registereded successfully 
	//				or FALSE otherwise
	// --- Effect : Register stop event which whould stop playing any effect 
	//				if have been fired in the specified window
	BOOL UnregisterStopEvent(CWnd* pWnd, UINT nMsgIDtoStopPlaying);
	// --- In  :	pWnd				-	pointer to the window for which specified
	//										stop event will be unregistered
	// ---			nMsgIDtoStopPlaying	-	registered message ID to stop playing 
	//										any sound
	// --- Out : 
	// --- Returns: TRUE if stop event was unregistereded successfully or FALSE otherwise
	// --- Effect : Unregister stop event

	BOOL StopPlaying(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which any playing
	//										event will be stoped. If pWnd is set to NULL 
	//										then any currently playing sound effect in 
	//										any window will be stopped
	// --- Out : 
	// --- Returns: TRUE if successful or FALSE otherwise
	// --- Effect : Stops playing any registered sound effect in specified window.
	//				
	BOOL IsPlaying(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which any playing
	//										event will be searching for. If pWnd is set 
	//										to NULL then any currently playing sound 
	//										effect in any window will be searching for
	// --- Out : 
	// --- Returns: TRUE if in specified window any registered sound effect is being 
	//				played or FALSE otherwise
	// --- Effect : 

	void UnregisterAllEffects(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which all 
	//										registered start and stop playing events 
	//										will be unregistered. If pWnd is set 
	//										to NULL then all registered start and stop 
	//										playing events will be unregistered for all
	//										windows
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered start and stop playing events in the 
	//				specified window

	void UnregisterAllPlayEvents(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which all 
	//										registered start playing events 
	//										will be unregistered. If pWnd is set 
	//										to NULL then all registered start 
	//										playing events will be unregistered for all
	//										windows
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered start playing events in the 
	//				specified window

	void UnregisterAllStopEvents(CWnd* pWnd=NULL);
	// --- In  :	pWnd				-	pointer to the window for which all 
	//										registered stop playing events 
	//										will be unregistered. If pWnd is set 
	//										to NULL then all registered stop 
	//										playing events will be unregistered for all
	//										windows
	// --- Out : 
	// --- Returns:
	// --- Effect : Unregister all registered stop playing events in the 
	//				specified window


#ifndef OXSE_NO_SAVESTATE
	// Save to and load from registry all registered start and stop playing events
	// for specified window. These functions require using COXRegistryValFile class. 
	// If you are not going to use them then you can include in your stdafx.h 
	// file next code:
	//	
	//		#define OXSE_NO_SAVESTATE
	//
	// In this case you don't need to include into your project references 
	// to source code for COXRegistryValFile class.
	//
	BOOL SaveState(CWnd* pWnd, LPCTSTR lpszProfileName);
	// --- In  :	pWnd			-	pointer to the window for which all play and
	//									stop event will be saved
	//				lpszProfileName	-	name of hive in registry where
	//									all information about sound effects associated
	//									with pWnd will be saved. 
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Saves pWnd's registered start and stop playing 
	//				effects into registry
	BOOL LoadState(CWnd* pWnd, LPCTSTR lpszProfileName);
	// --- In  :	pWnd			-	pointer to the window for which all restored 
	//									play and stop event will be applied
	//				lpszProfileName	-	name of hive in registry where
	//									all information about sound effects associated
	//									with pWnd was saved. 
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Load start and stop playing effects from registry and apply them
	//				to specified window
#endif // OXCP_NO_SAVESTATE

protected:
	// instantiates COXSoundEffectmanager object
	COXSoundEffectManager* CreateManager(CWnd* pWnd);
};


#endif // _SOUNDEFFECTMANAGER_H

