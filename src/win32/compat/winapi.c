/*
 * Windows APIs that are different for each system.
 *   We use pointers to the entry points so that a
 *   single binary will run on all Windows systems.
 *
 *     Kern Sibbald MMIII
 */

#include "burp.h"

// Init with win9x, but maybe set to NT in InitWinAPI.
DWORD g_platform_id = VER_PLATFORM_WIN32_WINDOWS;
DWORD g_MinorVersion = 0;
DWORD g_MajorVersion = 0;

// API Pointers

t_OpenProcessToken      p_OpenProcessToken=NULL;
t_AdjustTokenPrivileges p_AdjustTokenPrivileges=NULL;
t_LookupPrivilegeValue  p_LookupPrivilegeValue=NULL;

t_SetProcessShutdownParameters p_SetProcessShutdownParameters=NULL;

t_CreateFileA           p_CreateFileA=NULL;
t_CreateFileW           p_CreateFileW=NULL;
t_CreateDirectoryA      p_CreateDirectoryA;
t_CreateDirectoryW      p_CreateDirectoryW;

t_OpenEncryptedFileRawA  p_OpenEncryptedFileRawA=NULL;
t_OpenEncryptedFileRawW  p_OpenEncryptedFileRawW=NULL;

t_wunlink               p_wunlink=NULL;
t_wmkdir                p_wmkdir=NULL;

t_GetFileAttributesA    p_GetFileAttributesA=NULL;
t_GetFileAttributesW    p_GetFileAttributesW=NULL;

t_GetFileAttributesExA  p_GetFileAttributesExA=NULL;
t_GetFileAttributesExW  p_GetFileAttributesExW=NULL;

t_SetFileAttributesA    p_SetFileAttributesA=NULL;
t_SetFileAttributesW    p_SetFileAttributesW=NULL;
t_BackupRead            p_BackupRead=NULL;
t_BackupWrite           p_BackupWrite=NULL;
t_WideCharToMultiByte   p_WideCharToMultiByte=NULL;
t_MultiByteToWideChar   p_MultiByteToWideChar=NULL;

t_AttachConsole         p_AttachConsole=NULL;

t_FindFirstFileA        p_FindFirstFileA=NULL;
t_FindFirstFileW        p_FindFirstFileW=NULL;

t_FindNextFileA         p_FindNextFileA=NULL;
t_FindNextFileW         p_FindNextFileW=NULL;

t_SetCurrentDirectoryA  p_SetCurrentDirectoryA=NULL;
t_SetCurrentDirectoryW  p_SetCurrentDirectoryW=NULL;

t_GetCurrentDirectoryA  p_GetCurrentDirectoryA=NULL;
t_GetCurrentDirectoryW  p_GetCurrentDirectoryW=NULL;

t_GetVolumePathNameW    p_GetVolumePathNameW=NULL;
t_GetVolumeNameForVolumeMountPointW p_GetVolumeNameForVolumeMountPointW=NULL;

t_SHGetFolderPath       p_SHGetFolderPath=NULL;

t_CreateProcessA        p_CreateProcessA=NULL;
t_CreateProcessW        p_CreateProcessW=NULL;

void InitWinAPIWrapper() 
{
	OSVERSIONINFO osversioninfo={ sizeof(OSVERSIONINFO) };

	// Get the current OS version.
	if (!GetVersionEx(&osversioninfo))
		g_platform_id=0;
	else
	{
		g_platform_id=osversioninfo.dwPlatformId;
		g_MinorVersion=osversioninfo.dwMinorVersion;
		g_MajorVersion=osversioninfo.dwMajorVersion;
	}

	HMODULE hLib=LoadLibraryA("KERNEL32.DLL");
	if(hLib)
	{
		// Create process calls.
		p_CreateProcessA=(t_CreateProcessA)
			GetProcAddress(hLib, "CreateProcessA");
		p_CreateProcessW=(t_CreateProcessW)
			GetProcAddress(hLib, "CreateProcessW");

		// Create file calls.
		p_CreateFileA=(t_CreateFileA)
			GetProcAddress(hLib, "CreateFileA");
		p_CreateDirectoryA=(t_CreateDirectoryA)
			GetProcAddress(hLib, "CreateDirectoryA");

		// Attribute calls.
		p_GetFileAttributesA=(t_GetFileAttributesA)
			GetProcAddress(hLib, "GetFileAttributesA");
		p_GetFileAttributesExA=(t_GetFileAttributesExA)
			GetProcAddress(hLib, "GetFileAttributesExA");
		p_SetFileAttributesA=(t_SetFileAttributesA)
			GetProcAddress(hLib, "SetFileAttributesA");

		// Process calls.
		p_SetProcessShutdownParameters=(t_SetProcessShutdownParameters)
			GetProcAddress(hLib, "SetProcessShutdownParameters");

		// char conversion calls.
		p_WideCharToMultiByte=(t_WideCharToMultiByte)
			GetProcAddress(hLib, "WideCharToMultiByte");
		p_MultiByteToWideChar=(t_MultiByteToWideChar)
			GetProcAddress(hLib, "MultiByteToWideChar");

		// Find files.
		p_FindFirstFileA=(t_FindFirstFileA)
			GetProcAddress(hLib, "FindFirstFileA"); 
		p_FindNextFileA=(t_FindNextFileA)
			GetProcAddress(hLib, "FindNextFileA");

		// Get and set directory.
		p_GetCurrentDirectoryA=(t_GetCurrentDirectoryA)
			GetProcAddress(hLib, "GetCurrentDirectoryA");
		p_SetCurrentDirectoryA=(t_SetCurrentDirectoryA)
			GetProcAddress(hLib, "SetCurrentDirectoryA");

		if(g_platform_id!=VER_PLATFORM_WIN32_WINDOWS)
		{
			p_CreateFileW=(t_CreateFileW)
				GetProcAddress(hLib, "CreateFileW");
			p_CreateDirectoryW=(t_CreateDirectoryW)
				GetProcAddress(hLib, "CreateDirectoryW");

			// backup calls.
			p_BackupRead=(t_BackupRead)
				GetProcAddress(hLib, "BackupRead");
			p_BackupWrite=(t_BackupWrite)
				GetProcAddress(hLib, "BackupWrite");

			p_GetFileAttributesW=(t_GetFileAttributesW)
				GetProcAddress(hLib, "GetFileAttributesW");
			p_GetFileAttributesExW=(t_GetFileAttributesExW)
				GetProcAddress(hLib, "GetFileAttributesExW");
			p_SetFileAttributesW=(t_SetFileAttributesW)
				GetProcAddress(hLib, "SetFileAttributesW");
			p_FindFirstFileW=(t_FindFirstFileW)
				GetProcAddress(hLib, "FindFirstFileW");
			p_FindNextFileW=(t_FindNextFileW)
				GetProcAddress(hLib, "FindNextFileW");
			p_GetCurrentDirectoryW=(t_GetCurrentDirectoryW)
				GetProcAddress(hLib, "GetCurrentDirectoryW");
			p_SetCurrentDirectoryW=(t_SetCurrentDirectoryW)
				GetProcAddress(hLib, "SetCurrentDirectoryW");

			// Some special stuff we need for VSS.
			// But static linkage doesn't work on Win 9x.
			p_GetVolumePathNameW=(t_GetVolumePathNameW)
				GetProcAddress(hLib, "GetVolumePathNameW");
			p_GetVolumeNameForVolumeMountPointW
			  =(t_GetVolumeNameForVolumeMountPointW)
			 	 GetProcAddress(hLib,
				  "GetVolumeNameForVolumeMountPointW");

			p_AttachConsole=(t_AttachConsole)
				GetProcAddress(hLib, "AttachConsole");
		}
	}

	if(g_platform_id!=VER_PLATFORM_WIN32_WINDOWS)
	{
		hLib=LoadLibraryA("MSVCRT.DLL");
		if(hLib)
		{
			p_wunlink=(t_wunlink)
				GetProcAddress(hLib, "_wunlink");
			p_wmkdir=(t_wmkdir)
				GetProcAddress(hLib, "_wmkdir");
		}

		hLib=LoadLibraryA("ADVAPI32.DLL");
		if(hLib)
		{
			p_OpenProcessToken=(t_OpenProcessToken)
				GetProcAddress(hLib, "OpenProcessToken");
			p_AdjustTokenPrivileges=(t_AdjustTokenPrivileges)
				GetProcAddress(hLib, "AdjustTokenPrivileges");
			p_LookupPrivilegeValue=(t_LookupPrivilegeValue)
				GetProcAddress(hLib, "LookupPrivilegeValueA");

			p_OpenEncryptedFileRawA=(t_OpenEncryptedFileRawA)
				GetProcAddress(hLib, "OpenEncryptedFileRawA");
			p_OpenEncryptedFileRawW=(t_OpenEncryptedFileRawW)
				GetProcAddress(hLib, "OpenEncryptedFileRawW");
		}
	}

	hLib=LoadLibraryA("SHELL32.DLL");
	if(hLib)
		p_SHGetFolderPath=(t_SHGetFolderPath)
			GetProcAddress(hLib, "SHGetFolderPathA");
	else
	{
		// If SHELL32 isn't found try SHFOLDER for older systems.
		if((hLib=LoadLibraryA("SHFOLDER.DLL")))
			p_SHGetFolderPath=(t_SHGetFolderPath)
				GetProcAddress(hLib, "SHGetFolderPathA");
	}
	atexit(Win32ConvCleanupCache);
}
