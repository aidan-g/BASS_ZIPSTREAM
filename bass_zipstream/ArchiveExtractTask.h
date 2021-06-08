#include "ArchiveExtractCallback.h"

#include "..\7z\CPP\Windows\Thread.h"

static THREAD_FUNC_DECL ExtractThread(void* param);

class ArchiveExtractTask :
	public CMyUnknownImp,
	public IUnknown {

private:
	CMyComPtr<IInArchive> Archive;
	CMyComPtr<IArchiveExtractCallback> Callback;
	CRecordVector<UInt32> Indices;
	NWindows::CThread Thread;
	bool Completed;

public:
	MY_UNKNOWN_IMP;

	ArchiveExtractTask(CMyComPtr<IInArchive> archive, CMyComPtr<IArchiveExtractCallback> callback);

	bool Start(const UInt32* indices, UInt32 count);

	bool IsRunning(UInt32 index, UInt64& available);

	HRESULT Run();

	void Wait();
};

