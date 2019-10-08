#include "fileload.h"

extern "C" void UncompressBackward(void *bottom);


void *LoadFile(FileHandle *handle, const char *name) {

	int entryNum = DVDConvertPathToEntrynum(name);

	DVDHandle dvdhandle;
	if (!DVDFastOpen(entryNum, &dvdhandle)) {
		return 0;
	}

	handle->length = dvdhandle.length;
	handle->filePtr = EGG__Heap__alloc((handle->length+0x1F) & ~0x1F, 0x20, GetArchiveHeap());

	int ret = DVDReadPrio(&dvdhandle, handle->filePtr, (handle->length+0x1F) & ~0x1F, 0, 2);

	DVDClose(&dvdhandle);


	return handle->filePtr;
}

/*void *LoadCompressedFile(FileHandle *handle, const char *name) {

	int entryNum = DVDConvertPathToEntrynum(name);

	DVDHandle dvdhandle;
	if (!DVDFastOpen(entryNum, &dvdhandle)) {
		return 0;
	}

	u32 infoBlock[0x20 / sizeof(u32)] __attribute__ ((aligned(32)));
	DVDReadPrio(&dvdhandle, infoBlock, 0x20, dvdhandle.length - 8, 2);

	// Reverse it!
	infoBlock[1] = (infoBlock[1] >> 24) | ((infoBlock[1] >> 8) & 0xFF00) | ((infoBlock[1] & 0xFF00) << 8) | ((infoBlock[1] & 0xFF) << 24);

	u32 uncompSize = dvdhandle.length + infoBlock[1];
	handle->length = uncompSize;
	handle->filePtr = EGG__Heap__alloc((uncompSize+0x1F) & ~0x1F, 0x20, GetArchiveHeap());

	int ret = DVDReadPrio(&dvdhandle, handle->filePtr, (dvdhandle.length+0x1F) & ~0x1F, 0, 2);

	DVDClose(&dvdhandle);

	UncompressBackward((void*)((u32)handle->filePtr + dvdhandle.length));


	return handle->filePtr;
}*/

bool FreeFile(FileHandle *handle) {
	if (!handle) return false;

	if (handle->filePtr) {
		EGG__Heap__free(handle->filePtr, GetArchiveHeap());
	}

	handle->filePtr = 0;
	handle->length = 0;

	return true;
}




File::File() {
	m_loaded = false;
}

File::~File() {
	close();
}

bool File::open(const char *filename) {
	if (m_loaded)
		close();

	void *ret = LoadFile(&m_handle, filename);
	if (ret != 0)
		m_loaded = true;

	return (ret != 0);
}

/*bool File::openCompressed(const char *filename) {
	if (m_loaded)
		close();

	void *ret = LoadCompressedFile(&m_handle, filename);
	if (ret != 0)
		m_loaded = true;

	return (ret != 0);
}*/

void File::close() {
	if (!m_loaded)
		return;

	m_loaded = false;
	FreeFile(&m_handle);
}

bool File::isOpen() {
	return m_loaded;
}

void *File::ptr() {
	if (m_loaded)
		return m_handle.filePtr;
	else
		return 0;
}

u32 File::length() {
	if (m_loaded)
		return m_handle.length;
	else
		return 0xFFFFFFFF;
}

