// ==========================================================================
// 					Class Implementation : COXSound
// ==========================================================================
//

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
//
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OXSound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(COXSound, CObject, 1)

/////////////////////////////////////////////////////////////////////////////
// Definition of static members
CMap<COXSound*, COXSound*, DWORD, DWORD> COXSound::m_allSoundObjects;

// Data members -------------------------------------------------------------
// protected:
	// static CMap<COXSound*, COXSound*, DWORD, DWORD> m_allSoundObjects;
	// --- List of all the COXSound objects thar currently exist
	//     The value (DWORD) is not used at the moment
	//	   This data member is only used by the friend class COXSoundWnd

	// HWND m_hCallbackWnd;
	// --- The window to which notification will be posted

	// UINT m_nRes;
	// --- Resource number

	// CString m_sFilename;
	// --- File name of the source

	// HGLOBAL m_hWave;
	// LPVOID m_lpWave;

	// BOOL m_bIsPlaying;
	// --- Whether the sound is currently playing (TRUE) or not (FALSE)

	// BOOL m_bIsLoaded;
	// --- Whether the sound is currently loaded (TRUE) or not (FALSE)

	// BOOL m_bLooping;
	// --- Whether the sound should loop (TRUE) or not (FALSE)

	// MMRESULT m_mmLastResult;
	// --- The last result code

	// LPVOID m_lpWaveData;
	// WAVEHDR m_waveHdr;
	// LPWAVEHDR m_lpWaveHdr;
	// HWAVEOUT m_hWaveOut;
	// WAVEFORMATEX m_waveFormatEx;
	// --- Wave data

// private:
	
// Member functions ---------------------------------------------------------
// public:


COXSound::COXSound() :
	m_hCallbackWnd(NULL),
	// ... No wave loaded from resource or file...
	m_nRes(0),
	m_sFilename(),
	m_hWave(NULL),
	m_lpWave(NULL),
	m_bIsPlaying(FALSE),
	m_bIsLoaded(FALSE),
	m_bLooping(FALSE),
	// ... Set the last error to MMSYSERR_NOERROR...
	m_mmLastResult(MMSYSERR_NOERROR),
	m_lpWaveData(NULL),
	m_lpWaveHdr(NULL),
	m_hWaveOut(NULL)
{
	// Clear the WAVEHDR and WAVEFORMATEX structure...
	::ZeroMemory(&m_waveHdr, sizeof(m_waveHdr));
	::ZeroMemory(&m_waveFormatEx, sizeof(WAVEFORMATEX));

	// Register in global map
	m_allSoundObjects.SetAt(this, 0);

	// Make sure the helper window exists
	// and is created in this thread
	HWND hHelperWindow = COXSoundWnd::CreateTheSoundWindow();
	ASSERT(hHelperWindow != NULL);
	ASSERT(::GetCurrentThreadId() == ::GetWindowThreadProcessId(hHelperWindow, NULL));
}


void COXSound::	SetCallbackWnd(CWnd* pCallbackWnd)
{
	// ... Set the callback window information
	m_hCallbackWnd=pCallbackWnd->GetSafeHwnd();
}


BOOL COXSound::CanPlay()
{
	// ... Return TRUE if there are any wave playback devices...
	return (0 < waveOutGetNumDevs());
}


BOOL COXSound::Open(LPCTSTR pszFilename)
{
	// Open a sound from a passed WAVE filename...

	// .... Assume failure
	BOOL bRet = FALSE;
	CFile cFile;

	m_nRes = 0;
	m_sFilename = pszFilename;

	if(cFile.Open(pszFilename, CFile::modeRead))
	{
		bRet=Open(&cFile);
		cFile.Close();

		m_SoundSourceInfo.Reset();
		m_SoundSourceInfo.source=SNDSRC_FILE;
		TCHAR szFullPath[_MAX_PATH];
		AfxFullPath(szFullPath,pszFilename);
		m_SoundSourceInfo.sFileName=szFullPath;

		return bRet;
	}

	// .. If the file cannot be opened, return FALSE.
	return bRet;
}
	

BOOL COXSound::Open(UINT nSoundResource, HINSTANCE hResInstance)
{
	// Open a sound from a resource...

	HGLOBAL		hMem = NULL;
	HRSRC		hResInfo = NULL;
	LPVOID		lpRes = NULL;

	// Find the resource...
	hResInfo=FindResource(hResInstance,MAKEINTRESOURCE(nSoundResource),_T("WAVE"));
	if(!hResInfo)
	{
		return FALSE;
	}

	// Load the resource...
	hMem=LoadResource(hResInstance, hResInfo);
	if(!hMem)
	{
		return FALSE;
	}

	// Save resource ID...
	m_nRes=nSoundResource;
	m_sFilename.Empty();

	// Stop any current sound from playing...
	Stop();

	// Free previous memory...
	FreeMem();

	// Determine the size of the resource...
	DWORD dwSize=SizeofResource(hResInstance, hResInfo);

	lpRes=LockResource(hMem);

	m_hWave=::GlobalAlloc(GHND, dwSize);
	m_lpWave=::GlobalLock(m_hWave);

	// Copy to the global buffer for this class...
	::CopyMemory(m_lpWave,lpRes,dwSize);
	
	m_SoundSourceInfo.Reset();
	m_SoundSourceInfo.source=SNDSRC_INTRESOURCE;
	m_SoundSourceInfo.hInstance=hResInstance;
	m_SoundSourceInfo.nResourceID=nSoundResource;

	return CanPlayLoadedData();
}


BOOL COXSound::Open(CFile* pOpenedFile)
{
	ASSERT_VALID(pOpenedFile);

	// Determine the length of the buffer to read...
	DWORD dwFileSize = (DWORD) pOpenedFile->GetLength();
	DWORD dwPos = (DWORD) pOpenedFile->GetPosition();

	DWORD dwSize = dwFileSize - dwPos;

	if(dwSize == 0)
	{
		return FALSE;
	}

	// Stop any current sound from playing...
	Stop();

	// Free previous memory...
	FreeMem();

	m_hWave=::GlobalAlloc(GHND, dwSize);
	m_lpWave=::GlobalLock(m_hWave);

	pOpenedFile->Read(m_lpWave,dwSize);

	m_SoundSourceInfo.Reset();
	m_SoundSourceInfo.source=SNDSRC_CFILE;

	return CanPlayLoadedData();
}


BOOL COXSound::Play(BOOL bLoop, BOOL bAsync)
{
	BOOL bRet = FALSE;

	ASSERT(m_lpWave != NULL);

	Stop();

	if(bAsync)
	{
		// Play the sound asynchronously...
		bRet=PlayWithCallback();

		if(bRet)
		{
			m_bIsPlaying=TRUE;
			m_bLooping=bLoop;
		}
	}
	else
	{
		// Play synchronously and return after the call is complete...
		bRet=PlaySound((LPCTSTR)m_lpWave,NULL,SND_MEMORY);
	}

	return bRet;
}


void COXSound::Stop()
{
	// Stop the current sound from playing...
	if(!m_bIsPlaying)
	{
		return;
	}

	if (m_hWaveOut)
	{
		// changes were made on 10th of July: commented the next line 
		waveOutPause(m_hWaveOut);
		// and uncommented the next ones
		waveOutReset(m_hWaveOut);
		CloseWaveOutDevice();
	}
	
	// Stop playing any sound...
	PlaySound(NULL, NULL, 0);

	m_bIsPlaying = FALSE;
}


BOOL COXSound::IsWaveLoaded() const
{
	// Return the status of the wave - is it loaded?
	return m_bIsLoaded;
}


BOOL COXSound::IsPlaying() const
{
	// Return a value of TRUE if a sound is playing...
	return m_bIsPlaying;
}


BOOL COXSound::GetWaveFormat(WAVEFORMATEX* waveFormatEx)
{
	ASSERT(waveFormatEx != NULL);

	// Returns the current WAVEFORMATEX information...
	::CopyMemory(waveFormatEx, &m_waveFormatEx, sizeof(WAVEFORMATEX));

	return m_bIsLoaded;
}


MMRESULT COXSound::GetLastMMError() const
{
	return m_mmLastResult;
}


void COXSound::GetErrorText(MMRESULT hResult, CString& sDesc)
{
	waveOutGetErrorText(hResult,sDesc.GetBuffer(MAXERRORLENGTH),MAXERRORLENGTH);
	sDesc.ReleaseBuffer();
}


BOOL COXSound::GetCurrentPosition(MMTIME* pMMTime) const
{
	if(!m_hWaveOut)
	{
		return FALSE;
	}

	if(!IsPlaying())
	{
		return FALSE;
	}

	ASSERT(pMMTime != NULL);

	// Return time in milliseconds
	pMMTime->wType=TIME_MS;

	MMRESULT hResult;
	hResult=waveOutGetPosition(m_hWaveOut,pMMTime,sizeof(MMTIME));

	return (hResult==MMSYSERR_NOERROR);
}


void COXSound::HandleCallback()
{
	// Free up the buffer...
	FreeBuffer();

	// If looping, play it again...
	if(m_bLooping)
	{
		// Notification that we are looping again...
		if(m_hCallbackWnd != NULL)
		{
			::SendMessage(m_hCallbackWnd,WM_OX_SOUNDPLAYLOOPING,NULL,(LPARAM)this);
		}

		if(m_bIsPlaying)
		{
			PlayWithCallback();
		}
	}

	if(m_hCallbackWnd!=NULL && (!m_bLooping || !m_bIsPlaying))
	{
		CloseWaveOutDevice();

		m_bIsPlaying = FALSE;

		// If not looping, notify the callback window we have completed playback...
		::SendMessage(m_hCallbackWnd,WM_OX_SOUNDPLAYBACKCOMPLETE,NULL,(LPARAM)this);
	}
}


COXSound::~COXSound()
{
	Stop();
	CloseWaveOutDevice();
	FreeMem();

	// Unregister from global map
	VERIFY(m_allSoundObjects.RemoveKey(this));
}


// protected:
void COXSound::FreeMem()
	//	--- In:			
	//	--- Out:
	//	--- Returns:	
	//	---	Effect:		Frees up global memory from a loaded WAVE
{
	// Free up the global memory block associated with the wave data...
	if (m_hWave)
	{
		::GlobalUnlock(m_hWave);
		::GlobalFree(m_hWave);
		m_hWave = NULL;
	}
}


void COXSound::FreeBuffer()
	//	--- In:			
	//	--- Out:		
	//	--- Returns:	
	//	---	Effect:		Frees a prepared waveOut buffer
{
	if(m_hWaveOut)
	{
		waveOutUnprepareHeader(m_hWaveOut, m_lpWaveHdr, sizeof(WAVEHDR));
	}

	// Reset the waveOut device...
	waveOutReset(m_hWaveOut);
}


void CALLBACK COXSound::waveOutProc(HWAVEOUT /* hWaveOut */, UINT uiMsg, 
									DWORD_PTR /* dwInstance */, DWORD_PTR dwParam1, 
									DWORD_PTR /* dwParam2 */)
	//	--- In:			hWaveOut : Handle of the WaveOut device
	//					uiMsg : callback message (from Windows)
	//					dwParam1 : Points to WAVEHDR structure
	//					dwParam2 : User-defined
	//	--- Out:
	//	--- Returns:	
	//	---	Effect:		Callback notification from the waveOut driver
{
	// Callback function for the waveOutOpen() / waveOutWrite() calls...

#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	if(uiMsg==MM_WOM_DONE)
	{
		LPWAVEHDR lpWaveHdr = (LPWAVEHDR)dwParam1;
		COXSound* pMe = (COXSound*)lpWaveHdr->dwUser;
		HWND hCallbackWnd = NULL;


		if (pMe == NULL)
		{
			return;
		}
		ASSERT_VALID(pMe);

		hCallbackWnd = pMe->m_hCallbackWnd;

		if (!pMe->m_bIsPlaying)
		{
			return;
		}

		// Post a message to the helper window
		::PostMessage(COXSoundWnd::GetTheSoundWindow(), 
			WM_OX_INTERNAL_SOUNDCALLBACK, (DWORD_PTR)pMe, NULL);
	}	
}


BOOL COXSound::PrepareWaveHeader()
	//	--- In:
	//	--- Out:
	//	--- Returns:	BOOL : TRUE if successful
	//	---	Effect:		Prepares WAVE data and header information for playback.
{
	// Internal protected call used to prepare the wave out device...

	MMRESULT mmResult = MMSYSERR_NOERROR;

	if (!m_lpWave)
	{
		return FALSE;
	}

	CloseWaveOutDevice();

	BYTE*			pbWaveData = NULL;
	LPWAVEFORMATEX	pWaveHeader = NULL;
	DWORD			dwBufferBytes = 0;

	::ZeroMemory(&m_waveFormatEx, sizeof(WAVEFORMATEX));
	m_waveFormatEx.cbSize = sizeof(WAVEFORMATEX);

	pWaveHeader = &m_waveFormatEx;

	// Parse the WAVE data...
	if(!ParseWaveData(m_lpWave,&pWaveHeader,&pbWaveData,&dwBufferBytes))
	{
		return FALSE;
	}

	if (!pbWaveData)
	{
		return FALSE;
	}

	// Save the WAVEFORMATEX header information...
	::CopyMemory(&m_waveFormatEx, pWaveHeader, sizeof(WAVEFORMATEX));

	// Open the wave output device for playback...
	mmResult = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, pWaveHeader, (DWORD_PTR) &waveOutProc, (DWORD_PTR) this, CALLBACK_FUNCTION);
	m_mmLastResult = mmResult;

	if (mmResult != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	// Prepare the wave header to be written to the wave output device...
	m_lpWaveHdr=(LPWAVEHDR) GlobalAllocPtr(GMEM_MOVEABLE|GMEM_SHARE,sizeof(WAVEHDR));
	if (!m_lpWaveHdr)
	{
		return FALSE;
	}

	m_lpWaveData = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE, dwBufferBytes + 1);
	if (!m_lpWaveData)
	{
		return FALSE;
	}

	// Copy the data to the global buffer...
	::CopyMemory(m_lpWaveData, pbWaveData, dwBufferBytes);

	// Set up the WAVEHDR structure...
	m_lpWaveHdr->lpData = (LPSTR) m_lpWaveData;
	m_lpWaveHdr->dwBufferLength = dwBufferBytes;
	m_lpWaveHdr->dwUser = (DWORD_PTR) this;
	m_lpWaveHdr->dwFlags = 0;
	m_lpWaveHdr->dwLoops = 0;

	mmResult = waveOutPrepareHeader(m_hWaveOut, m_lpWaveHdr, sizeof(WAVEHDR));
	m_mmLastResult = mmResult;
	if (mmResult != MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	mmResult = waveOutWrite(m_hWaveOut, m_lpWaveHdr, sizeof(WAVEHDR));
	m_mmLastResult = mmResult;
	if (mmResult != MMSYSERR_NOERROR)
	{
		waveOutUnprepareHeader(m_hWaveOut, m_lpWaveHdr, sizeof(WAVEHDR));
		CloseWaveOutDevice();
		return FALSE;
	}

	m_bIsPlaying = TRUE;
	m_bIsLoaded = TRUE;

	return TRUE;
}


BOOL COXSound::PlayWithCallback()
	//	--- In:
	//	--- Out:
	//	--- Returns:	BOOL : TRUE if the function is successful
	//	---	Effect:		Plays a loaded WAVE asynchronously with callback notification
{
	// If playing asynchronously, there MUST be a callback
	// window to handle processing when the Wave completes.

	ASSERT((m_hCallbackWnd != NULL) && ::IsWindow(m_hCallbackWnd));

	if(!::SendMessage(m_hCallbackWnd,WM_OX_SOUNDABOUTTOPLAY,NULL,(LPARAM)this))
	{
		if(m_hWaveOut)
		{
			waveOutPause(m_hWaveOut);
			waveOutReset(m_hWaveOut);
		}

		CloseWaveOutDevice();

		if(!PrepareWaveHeader())
		{
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}


BOOL COXSound::ParseWaveData(void* pvRes, WAVEFORMATEX** ppWaveHeader, 
							 BYTE** ppbWaveData, DWORD* pcbWaveSize)
	//	--- In:			pvRes : Pointer to the raw WAVE data
	//	--- Out:		ppWaveHeader : WAVEFORMATEX structure
	//					ppbWaveData : WAVE sample data for playback
	//					pcbWaveSize : Size of the WAVE sample data
	//	--- Returns:	BOOL : TRUE if the function is successful
	//	---	Effect:		Parses a loaded WAVE into its header and data components
{
	DWORD*	pdw = NULL;
	DWORD*	pdwEnd = NULL;
	DWORD	dwRiff = 0;
	DWORD	dwType = 0;
	DWORD	dwLength = 0;

	ASSERT(pvRes != NULL);

	if(ppWaveHeader != NULL)
	{
		*ppWaveHeader = NULL;
	}

	if(ppbWaveData != NULL)
	{
		*ppbWaveData = NULL;
	}

	if (pcbWaveSize != NULL)
	{
		*pcbWaveSize = 0;
	}

	pdw = (DWORD*)pvRes;
	dwRiff = *pdw++;
	dwLength = *pdw++;
	dwType = *pdw++;

	// Not a RIFF-encoded resource...
	if (dwRiff != mmioFOURCC('R', 'I', 'F', 'F'))
	{
		return FALSE;
	}

	// This is not a WAVE...
	if (dwType != mmioFOURCC('W', 'A', 'V', 'E'))
	{
		return FALSE;
	}

	pdwEnd = (DWORD*) ((BYTE*) pdw + dwLength - 4);

	while (pdw < pdwEnd)
	{
		dwType = *pdw++;
		dwLength = *pdw++;

		switch(dwType)
		{
		case mmioFOURCC('f', 'm', 't', ' '):
			if (ppWaveHeader && !*ppWaveHeader)
				{
				// Is this a WAVE?
				if (dwLength < sizeof(WAVEFORMAT))
					return FALSE;

				*ppWaveHeader = (WAVEFORMATEX*) pdw;

				if ((!ppbWaveData || *ppbWaveData) &&
					(!pcbWaveSize || *pcbWaveSize))
					{
					return TRUE;
					}
				}
			break;

		case mmioFOURCC('d', 'a', 't', 'a'):
			if ((ppbWaveData && !*ppbWaveData) ||
				(pcbWaveSize && !*pcbWaveSize))
				{
				if (ppbWaveData != NULL)
					*ppbWaveData = (LPBYTE) pdw;

				if (pcbWaveSize != NULL)
					*pcbWaveSize = dwLength;

				if ((ppWaveHeader == NULL) || (*ppWaveHeader != NULL))
					return TRUE;
				}
			break;
		}
		pdw = (DWORD*) ((BYTE*) pdw + ((dwLength + 1) & ~1));
	}
	
	return FALSE;
}


void COXSound::CloseWaveOutDevice()
	//	--- In:
	//	--- Out:
	//	--- Returns:	
	//	---	Effect:		Closes the wave output device and frees associated memory.
{
	if (m_hWaveOut != NULL)
	{
		waveOutClose(m_hWaveOut);
		m_hWaveOut = NULL;
	}

	if (m_lpWaveHdr != NULL)
	{
		GlobalFreePtr(m_lpWaveHdr);
		m_lpWaveHdr = NULL;
	}

	if (m_lpWaveData != NULL)
	{
		GlobalFreePtr(m_lpWaveData);
		m_lpWaveData = NULL;
	}
}


LPVOID COXSound::GlobalAllocPtr(UINT uiFlags, DWORD dwBytes)
	//	--- In:			uiFlags : Flags for GlobalAlloc()
	//					dwBytes : Size of requested buffer
	//	--- Out:		
	//	--- Returns:	LPVOID : Pointer to memory buffer
	//	---	Effect:		Utility function which allocates and locks a block of global memory
{
	HGLOBAL		hMem = NULL;
	LPVOID		pPtr = NULL;

	hMem = ::GlobalAlloc(uiFlags, dwBytes);

	if (hMem == NULL)
	{
		return NULL;
	}

	pPtr = ::GlobalLock(hMem);
	if (pPtr == NULL)
	{
		::GlobalUnlock(hMem);
		return NULL;
	}

	return pPtr;
}


void COXSound::GlobalFreePtr(LPVOID pPtr)
	//	--- In:			pPtr : Pointer to a global memory buffer
	//	--- Out:		
	//	--- Returns:	
	//	---	Effect:		Utility function which unlocks and frees a block of global memory
{
	HGLOBAL	hMem = NULL;
	ASSERT(pPtr != NULL);

	hMem = ::GlobalHandle(pPtr);

	if (hMem != NULL)
	{
		::GlobalUnlock(hMem);
		::GlobalFree(hMem);
	}
}


void COXSound::Serialize(CArchive& ar)
{
	m_SoundSourceInfo.Serialize(ar);
	if(!ar.IsStoring())
	{
		switch(m_SoundSourceInfo.source)
		{
		case SNDSRC_FILE:
			Open(m_SoundSourceInfo.sFileName);
			break;

		case SNDSRC_INTRESOURCE:
			Open(m_SoundSourceInfo.nResourceID,m_SoundSourceInfo.hInstance);
			break;
		}
    }
}


BOOL COXSound::CanPlayLoadedData()
{
	if(!CanPlay())
	{
		return FALSE;
	}

	if(m_lpWave==NULL)
	{
		return FALSE;
	}

	// check if loaded resource is really a WAVE data
	BYTE*			pbWaveData=NULL;
	DWORD			dwBufferBytes=0;
	WAVEFORMATEX	waveFormatEx={ sizeof(WAVEFORMATEX) };
	LPWAVEFORMATEX	pWaveHeader=&waveFormatEx;
	if(!ParseWaveData(m_lpWave,&pWaveHeader,&pbWaveData,&dwBufferBytes))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


