#include "LibApiImport.h"


PVOID LibApiImportProcedure(
	_Inout_ PVOID *Cache,
	_Inout_ PBOOLEAN CacheValid,
	_Inout_ PULONG Cookie,
	_In_ PWSTR ModuleName,
	_In_ PSTR ProcedureName
)
{
	PVOID module;
	PVOID procedure;

	if (*CacheValid)
		return (PVOID)((ULONG_PTR)*Cache ^ (ULONG_PTR)*Cookie);

	if (!(module = GetModuleHandle(ModuleName)))
		module = LoadLibrary(ModuleName);

	if (!module)
		return NULL;

	procedure = GetProcAddress((HMODULE)module, ProcedureName);

	if (*Cookie == 0) *Cookie = NtGetTickCount();
	*Cache = (PVOID)((ULONG_PTR)procedure ^ (ULONG_PTR)*Cookie);

	//  就是说多核处理器会对运行CPU指令顺序重排优化，MemoryBarrier可以阻止指令重排，调用Thread.MemoryBarrier()之后的代码中内存访问不能再这之前就完成了。
	//  也就是它可以限制指令重排和内存读写的缓存。
	MemoryBarrier();//
	*CacheValid = TRUE;

	return procedure;
}
//参考移植processhacker代码
#define LIB_API_DEFINE_IMPORT(Module, Name) \
_##Name Name##_Import(VOID) \
{ \
    static PVOID cache = NULL; \
    static BOOLEAN cacheValid = FALSE; \
    static ULONG cookie = 0; \
\
    return (_##Name)LibApiImportProcedure(&cache, &cacheValid, &cookie, Module, #Name); \
}

LIB_API_DEFINE_IMPORT(L"ntdll.dll", ZwCreateJobObject);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", ZwAssignProcessToJobObject);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", ZwTerminateJobObject);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", ZwClose);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", ZwOpenProcess);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", ZwTerminateProcess);


LIB_API_DEFINE_IMPORT(L"ntdll.dll", NtQueryInformationEnlistment);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", NtQueryInformationResourceManager);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", NtQueryInformationTransaction);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", NtQueryInformationTransactionManager);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", NtQueryDefaultLocale);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", NtQueryDefaultUILanguage);
//LIB_API_DEFINE_IMPORT(L"ntdll.dll", NtTraceControl);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", NtQueryOpenSubKeysEx);

LIB_API_DEFINE_IMPORT(L"ntdll.dll", RtlDefaultNpAcl);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", RtlGetTokenNamedObjectPath);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", RtlGetAppContainerNamedObjectPath);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", RtlGetAppContainerSidType);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", RtlGetAppContainerParent);
LIB_API_DEFINE_IMPORT(L"ntdll.dll", RtlDeriveCapabilitySidsFromName);

LIB_API_DEFINE_IMPORT(L"advapi32.dll", ConvertSecurityDescriptorToStringSecurityDescriptorW);

LIB_API_DEFINE_IMPORT(L"dnsapi.dll", DnsQuery_W);
LIB_API_DEFINE_IMPORT(L"dnsapi.dll", DnsExtractRecordsFromMessage_W);
LIB_API_DEFINE_IMPORT(L"dnsapi.dll", DnsWriteQuestionToBuffer_W);
LIB_API_DEFINE_IMPORT(L"dnsapi.dll", DnsFree);

LIB_API_DEFINE_IMPORT(L"kernel32.dll", PssCaptureSnapshot);
LIB_API_DEFINE_IMPORT(L"kernel32.dll", PssQuerySnapshot);
LIB_API_DEFINE_IMPORT(L"kernel32.dll", PssFreeSnapshot);

LIB_API_DEFINE_IMPORT(L"userenv.dll", CreateEnvironmentBlock);
LIB_API_DEFINE_IMPORT(L"userenv.dll", DestroyEnvironmentBlock);
LIB_API_DEFINE_IMPORT(L"userenv.dll", GetAppContainerRegistryLocation);
LIB_API_DEFINE_IMPORT(L"userenv.dll", GetAppContainerFolderPath);

LIB_API_DEFINE_IMPORT(L"winsta.dll", WinStationQueryInformationW);
