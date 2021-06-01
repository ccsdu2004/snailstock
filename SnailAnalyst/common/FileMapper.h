#ifndef FILE_MAPPER_H
#define FILE_MAPPER_H
#include <QByteArray>
#include <windows.h> 
  
class FileMapper
{  
public:
    FileMapper()
	{	
	}
	
	~FileMapper()
	{
		if(pvBuffer)
			UnmapViewOfFile(pvBuffer);
		if(INVALID_HANDLE_VALUE != fileMapper)
			CloseHandle(fileMapper);
		if(INVALID_HANDLE_VALUE != file)
			CloseHandle(file);
	}
	
	bool load(const QString& filename)
	{
		file = CreateFileA(filename.toLocal8Bit().data(),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);    
        if(INVALID_HANDLE_VALUE == file)
			return false;
  
        DWORD size_low,size_high;  
        size_low = GetFileSize(file,&size_high);
		fileSize = size_low;
  
        fileMapper = CreateFileMapping(file,NULL,PAGE_READWRITE,size_high,size_low,NULL);  
		
        if(fileMapper == INVALID_HANDLE_VALUE)
			return false;
   
        pvBuffer = MapViewOfFile(fileMapper,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
		return pvBuffer != nullptr;
    }
 
    QByteArray data()const
	{
	    return QByteArray((char*)pvBuffer,fileSize);
	}
private:
    HANDLE file = INVALID_HANDLE_VALUE;
	HANDLE fileMapper = INVALID_HANDLE_VALUE;
	unsigned long fileSize = 0;
	void* pvBuffer = nullptr;
};

#endif