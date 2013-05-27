/*----------------------------------------------------------------------------

	NAME
		Utils.c

	PURPOSE
		Some general-purpose functions for the WinTab demos.

	COPYRIGHT
		Copyright (c) Wacom Company, Ltd. 2010 All Rights Reserved
		All rights reserved.

---------------------------------------------------------------------------- */

#include "Utils.h"

#ifdef WACOM_DEBUG

void WacomTrace( char *lpszFormat, ...);

#define WACOM_ASSERT( x ) assert( x )
#define WACOM_TRACE(...)  WacomTrace(__VA_ARGS__)
#else
#define WACOM_TRACE(...)
#define WACOM_ASSERT( x )

#endif // WACOM_DEBUG

//////////////////////////////////////////////////////////////////////////////
HINSTANCE ghWintab = NULL;

WTINFOA gpWTInfoA = NULL;
WTOPENA gpWTOpenA = NULL;
WTGETA gpWTGetA = NULL;
WTSETA gpWTSetA = NULL;
WTCLOSE gpWTClose = NULL;
WTPACKET gpWTPacket = NULL;
WTENABLE gpWTEnable = NULL;
WTOVERLAP gpWTOverlap = NULL;
WTSAVE gpWTSave = NULL;
WTCONFIG gpWTConfig = NULL;
WTRESTORE gpWTRestore = NULL;
WTEXTSET gpWTExtSet = NULL;
WTEXTGET gpWTExtGet = NULL;
WTQUEUESIZESET gpWTQueueSizeSet = NULL;
WTDATAPEEK gpWTDataPeek = NULL;
WTPACKETSGET gpWTPacketsGet = NULL;

// TODO - add more wintab32 function pointers as needed

char* pszProgramName = NULL;

#define GETPROCADDRESS(type, func) \
	gp##func = (type)GetProcAddress(ghWintab, #func); \
	if (!gp##func){ WACOM_ASSERT(FALSE); UnloadWintab(); return FALSE; }

//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Find wintab32.dll and load it.  
//		Find the exported functions we need from it.
//
//	Returns
//		TRUE on success.
//		FALSE on failure.
//
BOOL LoadWintab( void )
{

	ghWintab = LoadLibraryA(  "Wintab32.dll" );
	
	
	if ( !ghWintab )
	{
		DWORD err = GetLastError();
		WACOM_TRACE("LoadLibrary error: %i\n", err);
		ShowError("Could not load Wintab32.dll");
		return FALSE;
	}
	
	
	// Explicitly find the exported Wintab functions in which we are interested.
	// We are using the ASCII, not unicode versions (where applicable).
	GETPROCADDRESS( WTOPENA, WTOpenA );
	GETPROCADDRESS( WTINFOA, WTInfoA );
	GETPROCADDRESS( WTGETA, WTGetA );
	GETPROCADDRESS( WTSETA, WTSetA );
	GETPROCADDRESS( WTPACKET, WTPacket );
	GETPROCADDRESS( WTCLOSE, WTClose );
	GETPROCADDRESS( WTENABLE, WTEnable );
	GETPROCADDRESS( WTOVERLAP, WTOverlap );
	GETPROCADDRESS( WTSAVE, WTSave );
	GETPROCADDRESS( WTCONFIG, WTConfig );
	GETPROCADDRESS( WTRESTORE, WTRestore );
	GETPROCADDRESS( WTEXTSET, WTExtSet );
	GETPROCADDRESS( WTEXTGET, WTExtGet );
	GETPROCADDRESS( WTQUEUESIZESET, WTQueueSizeSet );
	GETPROCADDRESS( WTDATAPEEK, WTDataPeek );
	GETPROCADDRESS( WTPACKETSGET, WTPacketsGet );


	// TODO - don't forget to NULL out pointers in UnloadWintab().
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Uninitializes use of wintab32.dll
//
// Returns
//		Nothing.
//
void UnloadWintab( void )
{
	WACOM_TRACE( "UnloadWintab()\n" );

	if ( ghWintab )
	{
		FreeLibrary( ghWintab );
		ghWintab = NULL;
	}

	gpWTOpenA			= NULL;
	gpWTClose			= NULL;
	gpWTInfoA			= NULL;
	gpWTPacket			= NULL;
	gpWTEnable			= NULL;
	gpWTOverlap			= NULL;
	gpWTSave			= NULL;
	gpWTConfig			= NULL;
	gpWTGetA				= NULL;
	gpWTSetA				= NULL;
	gpWTRestore			= NULL;
	gpWTExtSet			= NULL;
	gpWTExtGet			= NULL;
	gpWTQueueSizeSet	= NULL;
	gpWTDataPeek		= NULL;
	gpWTPacketsGet		= NULL;
}



//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Display error to user.
//
void ShowError( char *pszErrorMessage )
{
	char gpszProgramName[]="abc";

	WACOM_TRACE( "ShowError()\n" );

	WACOM_ASSERT( pszErrorMessage );

	MessageBoxA( NULL, pszErrorMessage, gpszProgramName, MB_OK | MB_ICONHAND );
}

HWND GetConsoleHwnd(void)
   {
    #define MY_BUFSIZE 1024 // �R���\�[�� �E�B���h�E�̃^�C�g���p�̃o�b�t�@�T�C�Y
    HWND hwndFound;         // �Ăяo�����֕Ԃ����l
    char pszNewWindowTitle[MY_BUFSIZE];
                           // �쐬�����E�B���h�E�̃^�C�g��������܂�
    char pszOldWindowTitle[MY_BUFSIZE]; // ���̃E�B���h�E�^�C�g��������܂�

    // ���݂̃E�B���h�E�^�C�g�����擾���܂�

    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

    // �Ǝ��ɁA�E�B���h�E�̐V�K�^�C�g�����t�H�[�}�b�g���܂�

    wsprintf(pszNewWindowTitle,"%d/%d",
             GetTickCount(),
             GetCurrentProcessId());

    // ���݂̃E�B���h�E�^�C�g����ύX���܂�

    SetConsoleTitle(pszNewWindowTitle);

    // �E�B���h�E�^�C�g���̃A�b�v�f�[�g���m���Ȃ��̂ɂ����܂�

    Sleep(40);

    // �E�B���h�E�̐V�K�^�C�g����T���ɂ����܂�

    hwndFound=FindWindow(NULL, pszNewWindowTitle);

    // ���̃E�B���h�E�^�C�g���֖߂��܂�

    SetConsoleTitle(pszOldWindowTitle);

    return(hwndFound);
   }


#ifdef WACOM_DEBUG

//////////////////////////////////////////////////////////////////////////////

void WacomTrace( char *lpszFormat, ...)
{
	char szTraceMessage[ 128 ];

	int nBytesWritten;

	va_list args;

	WACOM_ASSERT( lpszFormat );

	va_start( args, lpszFormat );

	nBytesWritten = _vsnprintf( szTraceMessage, sizeof( szTraceMessage ) - 1, 
		lpszFormat, args );

	if ( nBytesWritten > 0 )
	{
		OutputDebugStringA( szTraceMessage );
	}

	va_end( args );
}

#endif // WACOM_DEBUG