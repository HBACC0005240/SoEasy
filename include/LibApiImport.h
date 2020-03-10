#pragma once
#include "MINT.h"
#include <ProcessSnapshot.h>
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "advapi32.lib")

class LibApiImport
{
};
typedef NTSTATUS(* _ZwOpenProcess)
(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PCLIENT_ID ClientId
);
typedef NTSTATUS(NTAPI *_ZwCreateJobObject)
(
	_Out_ PHANDLE JobHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes
);
typedef NTSTATUS(NTAPI *_ZwAssignProcessToJobObject)
(
	_In_ HANDLE JobHandle,
	_In_ HANDLE ProcessHandle
);

typedef NTSTATUS(NTAPI *_NtTerminateJobObject)
(
	_In_ HANDLE JobHandle,
	_In_ NTSTATUS ExitStatus
);

typedef NTSTATUS(NTAPI *_ZwTerminateJobObject)
(
	_In_ HANDLE JobHandle,
	_In_ NTSTATUS ExitStatus
);
typedef NTSTATUS(NTAPI *_ZwClose)
(
	_In_ HANDLE Handle
);
typedef NTSTATUS(NTAPI *_NtTerminateProcess)
(
	_In_opt_ HANDLE ProcessHandle,
	_In_ NTSTATUS ExitStatus
);

typedef NTSTATUS(NTAPI *_ZwTerminateProcess)
(
	_In_opt_ HANDLE ProcessHandle,
	_In_ NTSTATUS ExitStatus
);

// ntdll

typedef NTSTATUS(NTAPI *_NtQueryInformationEnlistment)(
	_In_ HANDLE EnlistmentHandle,
	_In_ ENLISTMENT_INFORMATION_CLASS EnlistmentInformationClass,
	_Out_writes_bytes_(EnlistmentInformationLength) PVOID EnlistmentInformation,
	_In_ ULONG EnlistmentInformationLength,
	_Out_opt_ PULONG ReturnLength
	);

typedef NTSTATUS(NTAPI *_NtQueryInformationResourceManager)(
	_In_ HANDLE ResourceManagerHandle,
	_In_ RESOURCEMANAGER_INFORMATION_CLASS ResourceManagerInformationClass,
	_Out_writes_bytes_(ResourceManagerInformationLength) PVOID ResourceManagerInformation,
	_In_ ULONG ResourceManagerInformationLength,
	_Out_opt_ PULONG ReturnLength
	);

typedef NTSTATUS(NTAPI *_NtQueryInformationTransaction)(
	_In_ HANDLE TransactionHandle,
	_In_ TRANSACTION_INFORMATION_CLASS TransactionInformationClass,
	_Out_writes_bytes_(TransactionInformationLength) PVOID TransactionInformation,
	_In_ ULONG TransactionInformationLength,
	_Out_opt_ PULONG ReturnLength
	);

typedef NTSTATUS(NTAPI *_NtQueryInformationTransactionManager)(
	_In_ HANDLE TransactionManagerHandle,
	_In_ TRANSACTIONMANAGER_INFORMATION_CLASS TransactionManagerInformationClass,
	_Out_writes_bytes_(TransactionManagerInformationLength) PVOID TransactionManagerInformation,
	_In_ ULONG TransactionManagerInformationLength,
	_Out_opt_ PULONG ReturnLength
	);

typedef NTSTATUS(NTAPI *_NtQueryDefaultLocale)(
	_In_ BOOLEAN UserProfile,
	_Out_ PLCID DefaultLocaleId
	);

typedef NTSTATUS(NTAPI *_NtQueryDefaultUILanguage)(
	_Out_ LANGID* DefaultUILanguageId
	);

//typedef NTSTATUS(NTAPI *_NtTraceControl)(
//	_In_ TRACE_CONTROL_INFORMATION_CLASS TraceInformationClass,
//	_In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
//	_In_ ULONG InputBufferLength,
//	_Out_writes_bytes_opt_(TraceInformationLength) PVOID TraceInformation,
//	_In_ ULONG TraceInformationLength,
//	_Out_ PULONG ReturnLength
//	);

typedef NTSTATUS(NTAPI *_NtQueryOpenSubKeysEx)(
	_In_ POBJECT_ATTRIBUTES TargetKey,
	_In_ ULONG BufferLength,
	_Out_writes_bytes_opt_(BufferLength) PVOID Buffer,
	_Out_ PULONG RequiredSize
	);

typedef NTSTATUS(NTAPI* _RtlDefaultNpAcl)(
	_Out_ PACL* Acl
	);

typedef NTSTATUS(NTAPI* _RtlGetTokenNamedObjectPath)(
	_In_ HANDLE Token,
	_In_opt_ PSID Sid,
	_Out_ PUNICODE_STRING ObjectPath
	);

typedef NTSTATUS(NTAPI* _RtlGetAppContainerNamedObjectPath)(
	_In_opt_ HANDLE Token,
	_In_opt_ PSID AppContainerSid,
	_In_ BOOLEAN RelativePath,
	_Out_ PUNICODE_STRING ObjectPath
	);

typedef NTSTATUS(NTAPI* _RtlGetAppContainerSidType)(
	_In_ PSID AppContainerSid,
	_Out_ PAPPCONTAINER_SID_TYPE AppContainerSidType
	);

typedef NTSTATUS(NTAPI* _RtlGetAppContainerParent)(
	_In_ PSID AppContainerSid,
	_Out_ PSID* AppContainerSidParent
	);

typedef NTSTATUS(NTAPI* _RtlDeriveCapabilitySidsFromName)(
	_Inout_ PUNICODE_STRING UnicodeString,
	_Out_ PSID CapabilityGroupSid,
	_Out_ PSID CapabilitySid
	);

typedef HRESULT(WINAPI* _GetAppContainerRegistryLocation)(
	_In_ REGSAM desiredAccess,
	_Outptr_ PHKEY phAppContainerKey
	);

typedef HRESULT(WINAPI* _GetAppContainerFolderPath)(
	_In_ PCWSTR pszAppContainerSid,
	_Out_ PWSTR* ppszPath
	);

typedef BOOL(WINAPI* _ConvertSecurityDescriptorToStringSecurityDescriptorW)(
	_In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
	_In_ DWORD RequestedStringSDRevision,
	_In_ SECURITY_INFORMATION SecurityInformation,
	_Outptr_ LPWSTR* StringSecurityDescriptor,
	_Out_opt_ PULONG StringSecurityDescriptorLen
	);

typedef ULONG(WINAPI* _PssCaptureSnapshot)(
	_In_ HANDLE ProcessHandle,
	_In_ ULONG CaptureFlags,
	_In_opt_ ULONG ThreadContextFlags,
	_Out_ HANDLE* SnapshotHandle
	);

typedef ULONG(WINAPI* _PssFreeSnapshot)(
	_In_ HANDLE ProcessHandle,
	_In_ HANDLE SnapshotHandle
	);

typedef ULONG(WINAPI* _PssQuerySnapshot)(
	_In_ HANDLE SnapshotHandle,
	_In_ ULONG InformationClass,
	_Out_writes_bytes_(BufferLength) void* Buffer,
	_In_ DWORD BufferLength
	);

typedef LONG(WINAPI* _DnsQuery_W)(
	_In_ PWSTR Name,
	_In_ USHORT Type,
	_In_ ULONG Options,
	_Inout_opt_ PVOID Extra,
	_Outptr_result_maybenull_ PVOID* DnsRecordList,
	_Outptr_opt_result_maybenull_ PVOID* Reserved
	);

typedef LONG(WINAPI* _DnsExtractRecordsFromMessage_W)(
	_In_ struct _DNS_MESSAGE_BUFFER* DnsBuffer,
	_In_ USHORT MessageLength,
	_Out_ PVOID* DnsRecordList
	);

typedef BOOL(WINAPI* _DnsWriteQuestionToBuffer_W)(
	_Inout_ struct _DNS_MESSAGE_BUFFER* DnsBuffer,
	_Inout_ PULONG BufferSize,
	_In_ PWSTR Name,
	_In_ USHORT Type,
	_In_ USHORT Xid,
	_In_ BOOL RecursionDesired
	);

typedef VOID(WINAPI* _DnsFree)(
	_Pre_opt_valid_ _Frees_ptr_opt_ PVOID Data,
	_In_ ULONG FreeType
	);

typedef BOOL(WINAPI* _CreateEnvironmentBlock)(
	_At_((PZZWSTR*)Environment, _Outptr_) PVOID* Environment,
	_In_opt_ HANDLE TokenHandle,
	_In_ BOOL Inherit
	);

typedef BOOL(WINAPI* _DestroyEnvironmentBlock)(
	_In_ PVOID Environment
	);

typedef BOOLEAN(WINAPI* _WinStationQueryInformationW)(
	_In_opt_ HANDLE ServerHandle,
	_In_ ULONG SessionId,
	_In_ ULONG WinStationInformationClass,
	_Out_writes_bytes_(WinStationInformationLength) PVOID pWinStationInformation,
	_In_ ULONG WinStationInformationLength,
	_Out_ PULONG pReturnLength
	);


#define VOID void
#define LIB_API_DECLARE_IMPORT(Name) _##Name Name##_Import(VOID)

LIB_API_DECLARE_IMPORT(ZwCreateJobObject);
LIB_API_DECLARE_IMPORT(ZwAssignProcessToJobObject);
LIB_API_DECLARE_IMPORT(ZwTerminateJobObject);
LIB_API_DECLARE_IMPORT(ZwClose);
LIB_API_DECLARE_IMPORT(ZwOpenProcess);
LIB_API_DECLARE_IMPORT(ZwTerminateProcess);


LIB_API_DECLARE_IMPORT(NtQueryInformationEnlistment);
LIB_API_DECLARE_IMPORT(NtQueryInformationResourceManager);
LIB_API_DECLARE_IMPORT(NtQueryInformationTransaction);
LIB_API_DECLARE_IMPORT(NtQueryInformationTransactionManager);
LIB_API_DECLARE_IMPORT(NtQueryDefaultLocale);
LIB_API_DECLARE_IMPORT(NtQueryDefaultUILanguage);
//LIB_API_DECLARE_IMPORT(NtTraceControl);
LIB_API_DECLARE_IMPORT(NtQueryOpenSubKeysEx);

LIB_API_DECLARE_IMPORT(RtlDefaultNpAcl);
LIB_API_DECLARE_IMPORT(RtlGetTokenNamedObjectPath);
LIB_API_DECLARE_IMPORT(RtlGetAppContainerNamedObjectPath);
LIB_API_DECLARE_IMPORT(RtlGetAppContainerSidType);
LIB_API_DECLARE_IMPORT(RtlGetAppContainerParent);
LIB_API_DECLARE_IMPORT(RtlDeriveCapabilitySidsFromName);

LIB_API_DECLARE_IMPORT(ConvertSecurityDescriptorToStringSecurityDescriptorW);

LIB_API_DECLARE_IMPORT(DnsQuery_W);
LIB_API_DECLARE_IMPORT(DnsExtractRecordsFromMessage_W);
LIB_API_DECLARE_IMPORT(DnsWriteQuestionToBuffer_W);
LIB_API_DECLARE_IMPORT(DnsFree);

LIB_API_DECLARE_IMPORT(PssCaptureSnapshot);
LIB_API_DECLARE_IMPORT(PssQuerySnapshot);
LIB_API_DECLARE_IMPORT(PssFreeSnapshot);

LIB_API_DECLARE_IMPORT(CreateEnvironmentBlock);
LIB_API_DECLARE_IMPORT(DestroyEnvironmentBlock);
LIB_API_DECLARE_IMPORT(GetAppContainerRegistryLocation);
LIB_API_DECLARE_IMPORT(GetAppContainerFolderPath);

LIB_API_DECLARE_IMPORT(WinStationQueryInformationW);
