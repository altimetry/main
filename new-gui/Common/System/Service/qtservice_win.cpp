/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QLibrary>
#include <QMutex>
#include <QSemaphore>
#include <QProcess>
#include <QSettings>
#include <QTextStream>
#include <qt_windows.h>
#include <QWaitCondition>
#include <QAbstractEventDispatcher>
#include <QVector>
#include <QThread>
#if QT_VERSION >= 0x050000
#  include <QAbstractNativeEventFilter>
#endif
#include <stdio.h>
#if defined(QTSERVICE_DEBUG)
#include <QDebug>
#endif

#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "../include/ntsecapi.h"	//for Log On As a Service Privilege

#include "common/QtStringUtils.h"

#include "qtservice.h"
#include "qtservice_p.h"

#include "qtservice_win.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Copyright 1996 - 1997 Microsoft Corporation
//
//Abstract:
//
//    This module illustrates how to use the Windows NT LSA security API
//    to manage account privileges on the local or a remote machine.
//
//    When targeting a domain controller for privilege update operations,
//    be sure to target the primary domain controller for the domain.
//    The privilege settings are replicated by the primary domain controller
//    to each backup domain controller as appropriate.  The NetGetDCName()
//    Lan Manager API call can be used to get the primary domain controller
//    computer name from a domain name.
//
//    For a list of privileges, consult winnt.h, and search for
//    SE_ASSIGNPRIMARYTOKEN_NAME.
//
//    For a list of logon rights, which can also be assigned using this
//    sample code, consult ntsecapi.h, and search for SE_BATCH_LOGON_NAME
//
//    You can use domain\account as argv[1]. For instance, mydomain\scott will
//    grant the privilege to the mydomain domain account scott.
//
//    The optional target machine is specified as argv[2], otherwise, the
//    account database is updated on the local machine.
//
//    The LSA APIs used by this sample are Unicode only.
//
//    Use LsaRemoveAccountRights() to remove account rights.
//
//Author:
//
//    Scott Field (sfield)    17-Apr-96
//        Minor cleanup
//
//    Scott Field (sfield)    12-Jul-95


// If you have the ddk, include ntstatus.h.
//
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS  ((NTSTATUS)0x00000000L)
#endif

//void
//DisplayWinError(
//    LPSTR szAPI,                // pointer to function name (ANSI)
//    DWORD WinError              // DWORD WinError
//    );
//
void DisplayWinError( LPSTR szAPI, DWORD WinError )
{
	LPSTR MessageBuffer;
	DWORD dwBufferLength;

	//
	// TODO: Get this fprintf out of here!
	//
	fprintf(stderr,"%s error %d!\n", szAPI, WinError);

	if(dwBufferLength=FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		WinError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR) &MessageBuffer,
		0,
		nullptr
	))
	{
		DWORD dwBytesWritten; // unused

							  //
							  // Output message string on stderr.
							  //
		WriteFile(
			GetStdHandle(STD_ERROR_HANDLE),
			MessageBuffer,
			dwBufferLength,
			&dwBytesWritten,
			nullptr
		);

		//
		// Free the buffer allocated by the system.
		//
		LocalFree(MessageBuffer);
	}
}

//This function attempts to obtain a SID representing the supplied
//account on the supplied system.
//
//If the function succeeds, the return value is TRUE. A buffer is
//allocated which contains the SID representing the supplied account.
//This buffer should be freed when it is no longer needed by calling
//HeapFree(GetProcessHeap(), 0, buffer)
//
//If the function fails, the return value is FALSE. Call GetLastError()
//to obtain extended error information.
//
//BOOL
//GetAccountSid(
//    LPTSTR SystemName,          // where to lookup account
//    LPTSTR AccountName,         // account of interest
//    PSID *Sid                   // resultant buffer containing SID
//    );
BOOL GetAccountSid( LPTSTR SystemName, LPCTSTR AccountName, PSID *Sid )
{
	LPTSTR ReferencedDomain=nullptr;
	DWORD cbSid=128;    // initial allocation attempt
	DWORD cchReferencedDomain=16; // initial allocation size
	SID_NAME_USE peUse;
	BOOL bSuccess=FALSE; // assume this function will fail

	__try {

		//
		// initial memory allocations
		//
		*Sid = (PSID)HeapAlloc(GetProcessHeap(), 0, cbSid);

		if(*Sid == nullptr) __leave;

		ReferencedDomain = (LPTSTR)HeapAlloc(
			GetProcessHeap(),
			0,
			cchReferencedDomain * sizeof(TCHAR)
		);

		if(ReferencedDomain == nullptr) __leave;

		//
		// Obtain the SID of the specified account on the specified system.
		//
		while(!LookupAccountName(
			SystemName,         // machine to lookup account on
			AccountName,        // account to lookup
			*Sid,               // SID of interest
			&cbSid,             // size of SID
			ReferencedDomain,   // domain account was found on
			&cchReferencedDomain,
			&peUse
		)) {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				//
				// reallocate memory
				//
				*Sid = (PSID)HeapReAlloc(
					GetProcessHeap(),
					0,
					*Sid,
					cbSid
				);
				if(*Sid == nullptr) __leave;

				ReferencedDomain = (LPTSTR)HeapReAlloc(
					GetProcessHeap(),
					0,
					ReferencedDomain,
					cchReferencedDomain * sizeof(TCHAR)
				);
				if(ReferencedDomain == nullptr) __leave;
			}
			else __leave;
		}

		//
		// Indicate success.
		//
		bSuccess = TRUE;

	} // try
	__finally {

		//
		// Cleanup and indicate failure, if appropriate.
		//

		HeapFree(GetProcessHeap(), 0, ReferencedDomain);

		if(!bSuccess) {
			if(*Sid != nullptr) {
				HeapFree(GetProcessHeap(), 0, *Sid);
				*Sid = nullptr;
			}
		}

	} // finally

	return bSuccess;
}

//void
//InitLsaString(
//    PLSA_UNICODE_STRING LsaString, // destination
//    LPWSTR String                  // source (Unicode)
//    );
//
void InitLsaString( PLSA_UNICODE_STRING LsaString, LPWSTR String )
{
	DWORD StringLength;

	if(String == nullptr) {
		LsaString->Buffer = nullptr;
		LsaString->Length = 0;
		LsaString->MaximumLength = 0;
		return;
	}

	StringLength = lstrlenW(String);
	LsaString->Buffer = String;
	LsaString->Length = (USHORT) StringLength * sizeof(WCHAR);
	LsaString->MaximumLength=(USHORT)(StringLength+1) * sizeof(WCHAR);
}


//NTSTATUS
//SetPrivilegeOnAccount(
//    LSA_HANDLE PolicyHandle,    // open policy handle
//    PSID AccountSid,            // SID to grant privilege to
//    LPWSTR PrivilegeName,       // privilege to grant (Unicode)
//    BOOL bEnable                // enable or disable
//    );
//
NTSTATUS SetPrivilegeOnAccount( LSA_HANDLE PolicyHandle, PSID AccountSid, LPWSTR PrivilegeName, BOOL bEnable )
{
	LSA_UNICODE_STRING PrivilegeString;

	//
	// Create a LSA_UNICODE_STRING for the privilege name.
	//
	InitLsaString(&PrivilegeString, PrivilegeName);

	//
	// grant or revoke the privilege, accordingly
	//
	if(bEnable) {
		return LsaAddAccountRights(
			PolicyHandle,       // open policy handle
			AccountSid,         // target SID
			&PrivilegeString,   // privileges
			1                   // privilege count
		);
	}
	else {
		return LsaRemoveAccountRights(
			PolicyHandle,       // open policy handle
			AccountSid,         // target SID
			FALSE,              // do not disable all rights
			&PrivilegeString,   // privileges
			1                   // privilege count
		);
	}
}

//NTSTATUS
//OpenPolicy(
//    LPWSTR ServerName,          // machine to open policy on (Unicode)
//    DWORD DesiredAccess,        // desired access to policy
//    PLSA_HANDLE PolicyHandle    // resultant policy handle
//    );
NTSTATUS OpenPolicy( LPWSTR ServerName, DWORD DesiredAccess, PLSA_HANDLE PolicyHandle )
{
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	LSA_UNICODE_STRING ServerString;
	PLSA_UNICODE_STRING Server;

	//
	// Always initialize the object attributes to all zeros.
	//
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	if (ServerName != nullptr) {
		//
		// Make a LSA_UNICODE_STRING out of the LPWSTR passed in
		//
		InitLsaString(&ServerString, ServerName);
		Server = &ServerString;
	} else {
		Server = nullptr;
	}

	//
	// Attempt to open the policy.
	//
	return LsaOpenPolicy(
		Server,
		&ObjectAttributes,
		DesiredAccess,
		PolicyHandle
	);
}

//void
//DisplayNtStatus(
//    LPSTR szAPI,                // pointer to function name (ANSI)
//    NTSTATUS Status             // NTSTATUS error value
//    );
//
void DisplayNtStatus( LPSTR szAPI, NTSTATUS Status )
{
	//
	// Convert the NTSTATUS to Winerror. Then call DisplayWinError().
	//
	DisplayWinError(szAPI, LsaNtStatusToWinError(Status));
}


bool HasLogOnAsServicePrivilege(  const std::string &pwcAccount, const std::string &pwcMachine )	//= "localhost" 
{
	LSA_HANDLE PolicyHandle;
	PSID AccountSid;

	WCHAR wComputerName[256] = L"";	// static machine name buffer
	WCHAR AccountName[256];			// static account name buffer

	//NTSTATUS LsaEnumerateAccountRights(
	//	_In_   LSA_HANDLE PolicyHandle,
	//	_In_   PSID AccountSid,
	//	_Out_  PLSA_UNICODE_STRING *UserRights,
	//	_Out_  PULONG CountOfRights
	//);

	// Pick up account name
	// Assumes source is ANSI. Resultant string is ANSI or Unicode
	//
	wsprintf( AccountName, TEXT("%hS"), pwcAccount.c_str() );

	// Pick up machine name, if appropriate
	// assumes source is ANSI. Resultant string is Unicode.
	//
	wsprintfW( wComputerName, L"%hS", pwcMachine.c_str() );

	//
	// Open the policy on the target machine.
	//
	NTSTATUS Status = OpenPolicy(
		wComputerName,      // local machine
		POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES,
		&PolicyHandle       // resultant policy handle
	);

	if(Status != STATUS_SUCCESS) 
	{
		DisplayNtStatus("OpenPolicy", Status);
		return false;
	}

	bool has_right = false;
	//
	// Obtain the SID of the user/group.
	// Note that we could target a specific machine, but we don't.
	// Specifying nullptr for target machine searches for the SID in the
	// following order: well-known, Built-in and local, primary domain,
	// trusted domains.
	//
	if ( GetAccountSid(
		nullptr,		// default lookup logic
		AccountName,	// account to obtain SID
		&AccountSid		// buffer to allocate to contain resultant SID
	) ) 
	{
		LSA_UNICODE_STRING *UserRights = nullptr;
		ULONG CountOfRights = 0;

		NTSTATUS status = LsaEnumerateAccountRights( PolicyHandle, AccountSid, &UserRights, &CountOfRights );
		for ( ULONG i = 0; i < CountOfRights; ++i )
		{
			LSA_UNICODE_STRING *UserRight = &UserRights[ i ];
			if ( std::wstring( UserRight->Buffer, UserRight->Length ) == std::wstring( L"SeServiceLogonRight" ) )
				has_right = true;
		}
		LsaFreeMemory( UserRights );
	}

	return has_right;
}



bool GrantLogOnAsServicePrivilege( const std::string &pwcAccount, const std::string &pwcMachine )	//= "localhost" 
{
	LSA_HANDLE PolicyHandle;

	WCHAR wComputerName[256] = L"";	// static machine name buffer
	WCHAR AccountName[256];			// static account name buffer
	PSID pSid;

	NTSTATUS Status;
	bool result = false;            // assume error

	// Pick up account name
	// Assumes source is ANSI. Resultant string is ANSI or Unicode
	//
	wsprintf( AccountName, TEXT("%hS"), pwcAccount.c_str() );

	// Pick up machine name, if appropriate
	// assumes source is ANSI. Resultant string is Unicode.
	//
	wsprintfW( wComputerName, L"%hS", pwcMachine.c_str() );

	//
	// Open the policy on the target machine.
	//
	Status = OpenPolicy(
		wComputerName,      // local machine
		POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES,
		&PolicyHandle       // resultant policy handle
	);

	if(Status != STATUS_SUCCESS) {
		DisplayNtStatus("OpenPolicy", Status);
		return false;
	}

	//
	// Obtain the SID of the user/group.
	// Note that we could target a specific machine, but we don't.
	// Specifying nullptr for target machine searches for the SID in the
	// following order: well-known, Built-in and local, primary domain,
	// trusted domains.
	//
	if(GetAccountSid(
		nullptr,       // default lookup logic
		AccountName,// account to obtain SID
		&pSid       // buffer to allocate to contain resultant SID
	)) {
		//
		// We only grant the privilege if we succeeded in obtaining the
		// SID. We can actually add SIDs which cannot be looked up, but
		// looking up the SID is a good sanity check which is suitable for
		// most cases.

		//
		// Grant the SeServiceLogonRight to users represented by pSid.
		//
		Status = SetPrivilegeOnAccount(
			PolicyHandle,           // policy handle
			pSid,                   // SID to grant privilege
			L"SeServiceLogonRight", // Unicode privilege
			TRUE                    // enable the privilege
		);

		if(Status == STATUS_SUCCESS)
		{
			result = true;
			wprintf ( L"'SeServiceLogonRight' added for '%s'\n",      AccountName);
		}
		else
			DisplayNtStatus("SetPrivilegeOnAccount", Status);
	}
	else {
		//
		// Error obtaining SID.
		//
		DisplayWinError("GetAccountSid", GetLastError());
	}

	//
	// Close the policy handle.
	//
	LsaClose(PolicyHandle);

	//
	// Free memory allocated for SID.
	//
	if(pSid != nullptr) HeapFree(GetProcessHeap(), 0, pSid);

	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef SERVICE_STATUS_HANDLE(WINAPI*PRegisterServiceCtrlHandler)(const wchar_t*,LPHANDLER_FUNCTION);
static PRegisterServiceCtrlHandler pRegisterServiceCtrlHandler = 0;
typedef BOOL(WINAPI*PSetServiceStatus)(SERVICE_STATUS_HANDLE,LPSERVICE_STATUS);
static PSetServiceStatus pSetServiceStatus = 0;
typedef BOOL(WINAPI*PChangeServiceConfig2)(SC_HANDLE,DWORD,LPVOID);
static PChangeServiceConfig2 pChangeServiceConfig2 = 0;
typedef BOOL(WINAPI*PCloseServiceHandle)(SC_HANDLE);
static PCloseServiceHandle pCloseServiceHandle = 0;
typedef SC_HANDLE(WINAPI*PCreateService)(SC_HANDLE,LPCTSTR,LPCTSTR,DWORD,DWORD,DWORD,DWORD,LPCTSTR,LPCTSTR,LPDWORD,LPCTSTR,LPCTSTR,LPCTSTR);
static PCreateService pCreateService = 0;
typedef SC_HANDLE(WINAPI*POpenSCManager)(LPCTSTR,LPCTSTR,DWORD);
static POpenSCManager pOpenSCManager = 0;
typedef BOOL(WINAPI*PDeleteService)(SC_HANDLE);
static PDeleteService pDeleteService = 0;
typedef SC_HANDLE(WINAPI*POpenService)(SC_HANDLE,LPCTSTR,DWORD);
static POpenService pOpenService = 0;
typedef BOOL(WINAPI*PQueryServiceStatus)(SC_HANDLE,LPSERVICE_STATUS);
static PQueryServiceStatus pQueryServiceStatus = 0;
typedef BOOL(WINAPI*PStartServiceCtrlDispatcher)(CONST SERVICE_TABLE_ENTRY*);
static PStartServiceCtrlDispatcher pStartServiceCtrlDispatcher = 0;
typedef BOOL(WINAPI*PStartService)(SC_HANDLE,DWORD,const wchar_t**);
static PStartService pStartService = 0;
typedef BOOL(WINAPI*PControlService)(SC_HANDLE,DWORD,LPSERVICE_STATUS);
static PControlService pControlService = 0;
typedef HANDLE(WINAPI*PDeregisterEventSource)(HANDLE);
static PDeregisterEventSource pDeregisterEventSource = 0;
typedef BOOL(WINAPI*PReportEvent)(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCTSTR*,LPVOID);
static PReportEvent pReportEvent = 0;
typedef HANDLE(WINAPI*PRegisterEventSource)(LPCTSTR,LPCTSTR);
static PRegisterEventSource pRegisterEventSource = 0;
typedef DWORD(WINAPI*PRegisterServiceProcess)(DWORD,DWORD);
static PRegisterServiceProcess pRegisterServiceProcess = 0;
typedef BOOL(WINAPI*PQueryServiceConfig)(SC_HANDLE,LPQUERY_SERVICE_CONFIG,DWORD,LPDWORD);
static PQueryServiceConfig pQueryServiceConfig = 0;
typedef BOOL(WINAPI*PQueryServiceConfig2)(SC_HANDLE,DWORD,LPBYTE,DWORD,LPDWORD);
static PQueryServiceConfig2 pQueryServiceConfig2 = 0;


#define RESOLVE(name) p##name = (P##name)lib.resolve(#name);
#define RESOLVEA(name) p##name = (P##name)lib.resolve(#name"A");
#define RESOLVEW(name) p##name = (P##name)lib.resolve(#name"W");

static bool winServiceInit()
{
    if (!pOpenSCManager) {
        QLibrary lib("advapi32");

        // only resolve unicode versions
        RESOLVEW(RegisterServiceCtrlHandler);
        RESOLVE(SetServiceStatus);
        RESOLVEW(ChangeServiceConfig2);
        RESOLVE(CloseServiceHandle);
        RESOLVEW(CreateService);
        RESOLVEW(OpenSCManager);
        RESOLVE(DeleteService);
        RESOLVEW(OpenService);
        RESOLVE(QueryServiceStatus);
        RESOLVEW(StartServiceCtrlDispatcher);
        RESOLVEW(StartService); // need only Ansi version
        RESOLVE(ControlService);
        RESOLVE(DeregisterEventSource);
        RESOLVEW(ReportEvent);
        RESOLVEW(RegisterEventSource);
        RESOLVEW(QueryServiceConfig);
        RESOLVEW(QueryServiceConfig2);
    }
    return pOpenSCManager != 0;
}

bool QtServiceController::isInstalled() const
{
    Q_D(const QtServiceController);
    bool result = false;
    if (!winServiceInit())
        return result;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t*)d->serviceName.utf16(),
                                          SERVICE_QUERY_CONFIG);

        if (hService) {
            result = true;
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

bool QtServiceController::isRunning() const
{
    Q_D(const QtServiceController);
    bool result = false;
    if (!winServiceInit())
        return result;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(),
                                          SERVICE_QUERY_STATUS);
        if (hService) {
            SERVICE_STATUS info;
            int res = pQueryServiceStatus(hService, &info);
            if (res)
                result = info.dwCurrentState != SERVICE_STOPPED;
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}


QString QtServiceController::serviceFilePath() const
{
    Q_D(const QtServiceController);
    QString result;
    if (!winServiceInit())
        return result;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(),
                                          SERVICE_QUERY_CONFIG);
        if (hService) {
            DWORD sizeNeeded = 0;
            char data[8 * 1024];
            if (pQueryServiceConfig(hService, (LPQUERY_SERVICE_CONFIG)data, 8 * 1024, &sizeNeeded)) {
                LPQUERY_SERVICE_CONFIG config = (LPQUERY_SERVICE_CONFIG)data;
                result = QString::fromUtf16((const ushort*)config->lpBinaryPathName);
            }
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

QString QtServiceController::serviceDescription() const
{
    Q_D(const QtServiceController);
    QString result;
    if (!winServiceInit())
        return result;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(),
             SERVICE_QUERY_CONFIG);
        if (hService) {
            DWORD dwBytesNeeded;
            char data[8 * 1024];
            if (pQueryServiceConfig2(
                    hService,
                    SERVICE_CONFIG_DESCRIPTION,
                    (unsigned char *)data,
                    8096,
                    &dwBytesNeeded)) {
                LPSERVICE_DESCRIPTION desc = (LPSERVICE_DESCRIPTION)data;
                if (desc->lpDescription)
                    result = QString::fromUtf16((const ushort*)desc->lpDescription);
            }
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

QtServiceController::StartupType QtServiceController::startupType() const
{
    Q_D(const QtServiceController);
    StartupType result = ManualStartup;
    if (!winServiceInit())
        return result;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager(0, 0, 0);
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(),
                                          SERVICE_QUERY_CONFIG);
        if (hService) {
            DWORD sizeNeeded = 0;
            char data[8 * 1024];
            if (pQueryServiceConfig(hService, (QUERY_SERVICE_CONFIG *)data, 8 * 1024, &sizeNeeded)) {
                QUERY_SERVICE_CONFIG *config = (QUERY_SERVICE_CONFIG *)data;
                result = config->dwStartType == SERVICE_DEMAND_START ? ManualStartup : AutoStartup;
            }
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

bool QtServiceController::uninstall()
{
    Q_D(QtServiceController);
    bool result = false;
    if (!winServiceInit())
        return result;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(), DELETE);
        if (hService) {
            if (pDeleteService(hService))
                result = true;
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

bool QtServiceController::start(const QStringList &args)
{
    Q_D(QtServiceController);
    bool result = false;
    if (!winServiceInit())
        return result;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
    if (hSCM) {
        // Try to open the service
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(), SERVICE_START);
        if (hService) {
            QVector<const wchar_t *> argv(args.size());
            for (int i = 0; i < args.size(); ++i)
                argv[i] = (const wchar_t*)args.at(i).utf16();

            if (pStartService(hService, args.size(), argv.data()))
                result = true;
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

bool QtServiceController::stop()
{
    Q_D(QtServiceController);
    bool result = false;
    if (!winServiceInit())
        return result;

    SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
    if (hSCM) {
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(), SERVICE_STOP|SERVICE_QUERY_STATUS);
        if (hService) {
            SERVICE_STATUS status;
            if (pControlService(hService, SERVICE_CONTROL_STOP, &status)) {
                bool stopped = status.dwCurrentState == SERVICE_STOPPED;
                int i = 0;
                while(!stopped && i < 10) {
                    Sleep(200);
                    if (!pQueryServiceStatus(hService, &status))
                        break;
                    stopped = status.dwCurrentState == SERVICE_STOPPED;
                    ++i;
                }
                result = stopped;
            } else {
                qErrnoWarning(GetLastError(), "stopping");
            }
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

bool QtServiceController::pause()
{
    Q_D(QtServiceController);
    bool result = false;
    if (!winServiceInit())
        return result;

    SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
    if (hSCM) {
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(),
                             SERVICE_PAUSE_CONTINUE);
        if (hService) {
            SERVICE_STATUS status;
            if (pControlService(hService, SERVICE_CONTROL_PAUSE, &status))
                result = true;
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

bool QtServiceController::resume()
{
    Q_D(QtServiceController);
    bool result = false;
    if (!winServiceInit())
        return result;

    SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
    if (hSCM) {
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(),
                             SERVICE_PAUSE_CONTINUE);
        if (hService) {
            SERVICE_STATUS status;
            if (pControlService(hService, SERVICE_CONTROL_CONTINUE, &status))
                result = true;
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

bool QtServiceController::sendCommand(int code)
{
   Q_D(QtServiceController);
   bool result = false;
   if (!winServiceInit())
        return result;

    if (code < 0 || code > 127 || !isRunning())
        return result;

    SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_CONNECT);
    if (hSCM) {
        SC_HANDLE hService = pOpenService(hSCM, (wchar_t *)d->serviceName.utf16(),
                                          SERVICE_USER_DEFINED_CONTROL);
        if (hService) {
            SERVICE_STATUS status;
            if (pControlService(hService, 128 + code, &status))
                result = true;
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }
    return result;
}

#if defined(QTSERVICE_DEBUG)
#  if QT_VERSION >= 0x050000
extern void qtServiceLogDebug(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#  else
extern void qtServiceLogDebug(QtMsgType type, const char* msg);
#  endif
#endif

void QtServiceBase::logMessage(const QString &message, MessageType type,
                           int id, uint category, const QByteArray &data)
{
#if defined(QTSERVICE_DEBUG)
    QByteArray dbgMsg("[LOGGED ");
    switch (type) {
    case Error: dbgMsg += "Error] " ; break;
    case Warning: dbgMsg += "Warning] "; break;
    case Success: dbgMsg += "Success] "; break;
    case Information: //fall through
    default: dbgMsg += "Information] "; break;
    }
#  if QT_VERSION >= 0x050000
    qtServiceLogDebug((QtMsgType)-1, QMessageLogContext(), QLatin1String(dbgMsg) + message);
#  else
    qtServiceLogDebug((QtMsgType)-1, (dbgMsg + message.toAscii()).constData());
#  endif
#endif

    Q_D(QtServiceBase);
    if (!winServiceInit())
        return;
    WORD wType;
    switch (type) {
    case Error: wType = EVENTLOG_ERROR_TYPE; break;
    case Warning: wType = EVENTLOG_WARNING_TYPE; break;
    case Information: wType = EVENTLOG_INFORMATION_TYPE; break;
    default: wType = EVENTLOG_SUCCESS; break;
    }
    HANDLE h = pRegisterEventSource(0, (wchar_t *)d->controller.serviceName().utf16());
    if (h) {
        const wchar_t *msg = (wchar_t*)message.utf16();
        const char *bindata = data.size() ? data.constData() : 0;
        pReportEvent(h, wType, category, id, 0, 1, data.size(),(const wchar_t **)&msg,
                     const_cast<char *>(bindata));
        pDeregisterEventSource(h);
    }
}


//brat: class QtServiceControllerHandler


class QtServiceSysPrivate
{
public:
    enum {
        QTSERVICE_STARTUP = 256
    };
    QtServiceSysPrivate();

    void setStatus( DWORD dwState );
    void setServiceFlags(QtServiceBase::ServiceFlags flags);
    DWORD serviceFlags(QtServiceBase::ServiceFlags flags) const;
    inline bool available() const;
    static void WINAPI serviceMain( DWORD dwArgc, wchar_t** lpszArgv );
    static void WINAPI handler( DWORD dwOpcode );

    SERVICE_STATUS status;
    SERVICE_STATUS_HANDLE serviceStatus;
    QStringList serviceArgs;

    static QtServiceSysPrivate *instance;
#if QT_VERSION < 0x050000
    static QCoreApplication::EventFilter nextFilter;
#endif

    QWaitCondition condition;
    QMutex mutex;
    QSemaphore startSemaphore;
    QSemaphore startSemaphore2;

    QtServiceControllerHandler *controllerHandler;

    void handleCustomEvent(QEvent *e);
};

QtServiceControllerHandler::QtServiceControllerHandler(QtServiceSysPrivate *sys)
    : QObject(), d_sys(sys)
{

}

void QtServiceControllerHandler::customEvent(QEvent *e)
{
    d_sys->handleCustomEvent(e);
}


QtServiceSysPrivate *QtServiceSysPrivate::instance = 0;
#if QT_VERSION < 0x050000
QCoreApplication::EventFilter QtServiceSysPrivate::nextFilter = 0;
#endif

QtServiceSysPrivate::QtServiceSysPrivate()
{
    instance = this;
}

inline bool QtServiceSysPrivate::available() const
{
    return 0 != pOpenSCManager;
}

void WINAPI QtServiceSysPrivate::serviceMain(DWORD dwArgc, wchar_t** lpszArgv)
{
    if (!instance || !QtServiceBase::instance())
        return;

    // Windows spins off a random thread to call this function on
    // startup, so here we just signal to the QApplication event loop
    // in the main thread to go ahead with start()'ing the service.

    for (DWORD i = 0; i < dwArgc; i++)
        instance->serviceArgs.append(QString::fromUtf16((unsigned short*)lpszArgv[i]));

    instance->startSemaphore.release(); // let the qapp creation start
    instance->startSemaphore2.acquire(); // wait until its done
    // Register the control request handler
    instance->serviceStatus = pRegisterServiceCtrlHandler((TCHAR*)QtServiceBase::instance()->serviceName().utf16(), handler);

    if (!instance->serviceStatus) // cannot happen - something is utterly wrong
        return;

    handler(QTSERVICE_STARTUP); // Signal startup to the application -
                                // causes QtServiceBase::start() to be called in the main thread

    // The MSDN doc says that this thread should just exit - the service is
    // running in the main thread (here, via callbacks in the handler thread).
}


// The handler() is called from the thread that called
// StartServiceCtrlDispatcher, i.e. our HandlerThread, and
// not from the main thread that runs the event loop, so we
// have to post an event to ourselves, and use a QWaitCondition
// and a QMutex to synchronize.
void QtServiceSysPrivate::handleCustomEvent(QEvent *e)
{
    int code = e->type() - QEvent::User;

    switch(code) {
    case QTSERVICE_STARTUP: // Startup
        QtServiceBase::instance()->start();
        break;
    case SERVICE_CONTROL_STOP:
        QtServiceBase::instance()->stop();
        QCoreApplication::instance()->quit();
        break;
    case SERVICE_CONTROL_PAUSE:
        QtServiceBase::instance()->pause();
        break;
    case SERVICE_CONTROL_CONTINUE:
        QtServiceBase::instance()->resume();
        break;
    default:
	if (code >= 128 && code <= 255)
	    QtServiceBase::instance()->processCommand(code - 128);
        break;
    }

    mutex.lock();
    condition.wakeAll();
    mutex.unlock();
}

void WINAPI QtServiceSysPrivate::handler( DWORD code )
{
    if (!instance)
        return;

    instance->mutex.lock();
    switch (code) {
    case QTSERVICE_STARTUP: // QtService startup (called from WinMain when started)
        instance->setStatus(SERVICE_START_PENDING);
        QCoreApplication::postEvent(instance->controllerHandler, new QEvent(QEvent::Type(QEvent::User + code)));
        instance->condition.wait(&instance->mutex);
        instance->setStatus(SERVICE_RUNNING);
        break;
    case SERVICE_CONTROL_STOP: // 1
        instance->setStatus(SERVICE_STOP_PENDING);
        QCoreApplication::postEvent(instance->controllerHandler, new QEvent(QEvent::Type(QEvent::User + code)));
        instance->condition.wait(&instance->mutex);
        // status will be reported as stopped by start() when qapp::exec returns
        break;

    case SERVICE_CONTROL_PAUSE: // 2
        instance->setStatus(SERVICE_PAUSE_PENDING);
        QCoreApplication::postEvent(instance->controllerHandler, new QEvent(QEvent::Type(QEvent::User + code)));
        instance->condition.wait(&instance->mutex);
        instance->setStatus(SERVICE_PAUSED);
        break;

    case SERVICE_CONTROL_CONTINUE: // 3
        instance->setStatus(SERVICE_CONTINUE_PENDING);
        QCoreApplication::postEvent(instance->controllerHandler, new QEvent(QEvent::Type(QEvent::User + code)));
        instance->condition.wait(&instance->mutex);
        instance->setStatus(SERVICE_RUNNING);
        break;

    case SERVICE_CONTROL_INTERROGATE: // 4
        break;

    case SERVICE_CONTROL_SHUTDOWN: // 5
        // Don't waste time with reporting stop pending, just do it
        QCoreApplication::postEvent(instance->controllerHandler, new QEvent(QEvent::Type(QEvent::User + SERVICE_CONTROL_STOP)));
        instance->condition.wait(&instance->mutex);
        // status will be reported as stopped by start() when qapp::exec returns
        break;

    default:
        if ( code >= 128 && code <= 255 ) {
            QCoreApplication::postEvent(instance->controllerHandler, new QEvent(QEvent::Type(QEvent::User + code)));
            instance->condition.wait(&instance->mutex);
        }
        break;
    }

    instance->mutex.unlock();

    // Report current status
    if (instance->available() && instance->status.dwCurrentState != SERVICE_STOPPED)
        pSetServiceStatus(instance->serviceStatus, &instance->status);
}

void QtServiceSysPrivate::setStatus(DWORD state)
{
    if (!available())
	return;
    status.dwCurrentState = state;
    pSetServiceStatus(serviceStatus, &status);
}

void QtServiceSysPrivate::setServiceFlags(QtServiceBase::ServiceFlags flags)
{
    if (!available())
        return;
    status.dwControlsAccepted = serviceFlags(flags);
    pSetServiceStatus(serviceStatus, &status);
}

DWORD QtServiceSysPrivate::serviceFlags(QtServiceBase::ServiceFlags flags) const
{
    DWORD control = 0;
    if (flags & QtServiceBase::CanBeSuspended)
        control |= SERVICE_ACCEPT_PAUSE_CONTINUE;
    if (!(flags & QtServiceBase::CannotBeStopped))
        control |= SERVICE_ACCEPT_STOP;
    if (flags & QtServiceBase::NeedsStopOnShutdown)
        control |= SERVICE_ACCEPT_SHUTDOWN;

    return control;
}

//#include "qtservice_win.moc"


class HandlerThread : public QThread
{
public:
    HandlerThread()
        : success(true), console(false), QThread()
        {}

    bool calledOk() { return success; }
    bool runningAsConsole() { return console; }

protected:
    bool success, console;
    void run()
        {
            SERVICE_TABLE_ENTRYW st [2];
            st[0].lpServiceName = (wchar_t*)QtServiceBase::instance()->serviceName().utf16();
            st[0].lpServiceProc = QtServiceSysPrivate::serviceMain;
            st[1].lpServiceName = 0;
            st[1].lpServiceProc = 0;

            success = (pStartServiceCtrlDispatcher(st) != 0); // should block

            if (!success) {
                if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
                    // Means we're started from console, not from service mgr
                    // start() will ask the mgr to start another instance of us as a service instead
                    console = true;
                }
                else {
                    QtServiceBase::instance()->logMessage(QString("The Service failed to start [%1]").arg(qt_error_string(GetLastError())), QtServiceBase::Error);
                }
                QtServiceSysPrivate::instance->startSemaphore.release();  // let start() continue, since serviceMain won't be doing it
            }
        }
};

/*
  Ignore WM_ENDSESSION system events, since they make the Qt kernel quit
*/

#if QT_VERSION >= 0x050000

class QtServiceAppEventFilter : public QAbstractNativeEventFilter
{
public:
    QtServiceAppEventFilter() {}
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
};

bool QtServiceAppEventFilter::nativeEventFilter(const QByteArray &, void *message, long *result)
{
    MSG *winMessage = (MSG*)message;
    if (winMessage->message == WM_ENDSESSION && (winMessage->lParam & ENDSESSION_LOGOFF)) {
        *result = TRUE;
        return true;
    }
    return false;
}

Q_GLOBAL_STATIC(QtServiceAppEventFilter, qtServiceAppEventFilter)

#else

bool myEventFilter(void* message, long* result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (!msg || (msg->message != WM_ENDSESSION) || !(msg->lParam & ENDSESSION_LOGOFF))
        return QtServiceSysPrivate::nextFilter ? QtServiceSysPrivate::nextFilter(message, result) : false;

    if (QtServiceSysPrivate::nextFilter)
        QtServiceSysPrivate::nextFilter(message, result);
    if (result)
        *result = TRUE;
    return true;
}

#endif

/* There are three ways we can be started:

   - By a service controller (e.g. the Services control panel), with
   no (service-specific) arguments. ServiceBase::exec() will then call
   start() below, and the service will start.

   - From the console, but with no (service-specific) arguments. This
   means we should ask a controller to start the service (i.e. another
   instance of this executable), and then just terminate. We discover
   this case (as different from the above) by the fact that
   StartServiceCtrlDispatcher will return an error, instead of blocking.

   - From the console, with -e(xec) argument. ServiceBase::exec() will
   then call ServiceBasePrivate::exec(), which calls
   ServiceBasePrivate::run(), which runs the application as a normal
   program.
*/

bool QtServiceBasePrivate::start()
{
    sysInit();
    if (!winServiceInit())
        return false;

    // Since StartServiceCtrlDispatcher() blocks waiting for service
    // control events, we need to call it in another thread, so that
    // the main thread can run the QApplication event loop.
    HandlerThread* ht = new HandlerThread();
    ht->start();

    QtServiceSysPrivate* sys = QtServiceSysPrivate::instance;

    // Wait until service args have been received by serviceMain.
    // If Windows doesn't call serviceMain (or
    // StartServiceControlDispatcher doesn't return an error) within
    // a timeout of 20 secs, something is very wrong; give up
    if (!sys->startSemaphore.tryAcquire(1, 20000))
        return false;

    if (!ht->calledOk()) {
        if (ht->runningAsConsole())
            return controller.start(args.mid(1));
        else
            return false;
    }

    int argc = sys->serviceArgs.size();
    QVector<char *> argv(argc);
    QList<QByteArray> argvData;
    for (int i = 0; i < argc; ++i)
        argvData.append(sys->serviceArgs.at(i).toLocal8Bit());
    for (int i = 0; i < argc; ++i)
        argv[i] = argvData[i].data();

    q_ptr->createApplication(argc, argv.data());
    QCoreApplication *app = QCoreApplication::instance();
    if (!app)
        return false;

#if QT_VERSION >= 0x050000
    QAbstractEventDispatcher::instance()->installNativeEventFilter(qtServiceAppEventFilter());
#else
    QtServiceSysPrivate::nextFilter = app->setEventFilter(myEventFilter);
#endif

    sys->controllerHandler = new QtServiceControllerHandler(sys);

    sys->startSemaphore2.release(); // let serviceMain continue (and end)

    sys->status.dwWin32ExitCode = q_ptr->executeApplication();
    sys->setStatus(SERVICE_STOPPED);

    if (ht->isRunning())
        ht->wait(1000);         // let the handler thread finish
    delete sys->controllerHandler;
    sys->controllerHandler = 0;
    if (ht->isFinished())
        delete ht;
    delete app;
    sysCleanup();
    return true;
}

bool QtServiceBasePrivate::install(const QString &account, const QString &password)
{
    bool result = false;
    if (!winServiceInit())
        return result;

    // Open the Service Control Manager
    SC_HANDLE hSCM = pOpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if (hSCM) {
        QString acc = account;
        DWORD dwStartType = startupType == QtServiceController::AutoStartup ? SERVICE_AUTO_START : SERVICE_DEMAND_START;
        DWORD dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        wchar_t *act = 0;
        wchar_t *pwd = 0;
        if (!acc.isEmpty()) {
            // The act string must contain a string of the format "Domain\UserName",
            // so if only a username was specified without a domain, default to the local machine domain.
            if (!acc.contains(QChar('\\'))) {
                acc.prepend(QLatin1String(".\\"));
            }
            if (!acc.endsWith(QLatin1String("\\LocalSystem")))
                act = (wchar_t*)acc.utf16();
        }
        if (!password.isEmpty() && act) {
            pwd = (wchar_t*)password.utf16();
        }

        // Only set INTERACTIVE if act is LocalSystem. (and act should be 0 if it is LocalSystem).
        if (!act) dwServiceType |= SERVICE_INTERACTIVE_PROCESS;

        // Create the service
        SC_HANDLE hService = pCreateService(hSCM, (wchar_t *)controller.serviceName().utf16(),
                                            (wchar_t *)controller.serviceName().utf16(),
                                            SERVICE_ALL_ACCESS,
                                            dwServiceType, // QObject::inherits ( const char * className ) for no inter active ????
                                            dwStartType, SERVICE_ERROR_NORMAL, (wchar_t *)filePath().utf16(),
                                            0, 0, 0,
                                            act, pwd);
        if (hService) {
            result = true;
            if (!serviceDescription.isEmpty()) {
                SERVICE_DESCRIPTION sdesc;
                sdesc.lpDescription = (wchar_t *)serviceDescription.utf16();
                pChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sdesc);
            }
            pCloseServiceHandle(hService);
        }
        pCloseServiceHandle(hSCM);
    }

	return result;
}

QString QtServiceBasePrivate::filePath() const
{
    wchar_t path[_MAX_PATH];
    ::GetModuleFileNameW( 0, path, sizeof(path) );
    return QString::fromUtf16((unsigned short*)path);
}

bool QtServiceBasePrivate::sysInit()
{
    sysd = new QtServiceSysPrivate();

    sysd->serviceStatus			    = 0;
    sysd->status.dwServiceType		    = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;
    sysd->status.dwCurrentState		    = SERVICE_STOPPED;
    sysd->status.dwControlsAccepted         = sysd->serviceFlags(serviceFlags);
    sysd->status.dwWin32ExitCode	    = NO_ERROR;
    sysd->status.dwServiceSpecificExitCode  = 0;
    sysd->status.dwCheckPoint		    = 0;
    sysd->status.dwWaitHint		    = 0;

    return true;
}

void QtServiceBasePrivate::sysSetPath()
{

}

void QtServiceBasePrivate::sysCleanup()
{
    if (sysd) {
        delete sysd;
        sysd = 0;
    }
}

void QtServiceBase::setServiceFlags(QtServiceBase::ServiceFlags flags)
{
    if (d_ptr->serviceFlags == flags)
        return;
    d_ptr->serviceFlags = flags;
    if (d_ptr->sysd)
        d_ptr->sysd->setServiceFlags(flags);
}




///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_qtservice_win.cpp"
