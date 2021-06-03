#include "ArchiveExtractCallback.h"

#include "..\7z\CPP\Windows\Thread.h"

static THREAD_FUNC_DECL ExtractThread(void* param);

class ArchiveExtractTask {

private:
	CMyComPtr<IInArchive> Archive;
	CMyComPtr<IArchiveExtractCallback> Callback;
	CRecordVector<UInt32> Indices;
	NWindows::CThread Thread;
	bool Completed;

public:

	ArchiveExtractTask(CMyComPtr<IInArchive> archive, CMyComPtr<IArchiveExtractCallback> callback);

	bool Start(const UInt32* indices, UInt32 count);

	bool IsRunning(UInt32 index, UInt64& available, bool& completed);

	HRESULT Run();

	void Wait();
};

