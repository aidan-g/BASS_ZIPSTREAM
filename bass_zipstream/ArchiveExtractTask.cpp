#include "ArchiveExtractTask.h"
#include "Common.h"

ArchiveExtractTask::ArchiveExtractTask(CMyComPtr<IInArchive> archive, CMyComPtr<IArchiveExtractCallback> callback) {
	this->Archive = archive;
	this->Callback = callback;
	this->Completed = false;
}

bool ArchiveExtractTask::Start(const UInt32* indices, UInt32 count) {
	this->Indices.Clear();
	this->Completed = false;
	for (unsigned a = 0; a < count; a++) {
		this->Indices.Add(indices[a]);
	}
	return this->Thread.Create(ExtractThread, this) == 0;
}

HRESULT ArchiveExtractTask::Run() {
	UInt32* indices = (UInt32*)malloc(sizeof(UInt32) * this->Indices.Size());
	if (!indices) {
		//TODO: Warn.
		return S_FALSE;
	}
	for (unsigned a = 0; a < this->Indices.Size(); a++) {
		indices[a] = this->Indices[a];
	}
	HRESULT result = this->Archive->Extract(indices, this->Indices.Size(), false, this->Callback);
	free(indices);
	this->Completed = true;
	return result;
}

bool ArchiveExtractTask::IsRunning(UInt32 index, UInt64& available, bool& completed) {
	for (unsigned a = 0; a < this->Indices.Size(); a++) {
		if (this->Indices[a] == index) {
			completed = this->Completed;
			ArchiveExtractCallback* callback = (ArchiveExtractCallback*)(IArchiveExtractCallback*)this->Callback;
			IOutStream* stream;
			if (callback->GetOutStream(&stream, index) && stream->Seek(0, SEEK_CUR, &available) == S_OK) {
				return true;
			}
			break;
		}
	}
	return false;
}

void ArchiveExtractTask::Wait() {
	this->Thread.Wait();
}

static THREAD_FUNC_DECL ExtractThread(void* param)
{
	ArchiveExtractTask* task = (ArchiveExtractTask*)param;
	task->Run();
	return 0;
}