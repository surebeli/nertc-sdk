#include "file.h"

#include <io.h>
#include <windows.h>

#include <limits>

size_t File::Write(const uint8_t* data, size_t length) {
  size_t total_written = 0;
  do {
    DWORD written;
    if (!::WriteFile(file_, data + total_written,
                     static_cast<DWORD>(length - total_written), &written,
                     nullptr)) {
      break;
    }
    total_written += written;
  } while (total_written < length);
  return total_written;
}

size_t File::Read(uint8_t* buffer, size_t length) {
  size_t total_read = 0;
  do {
    DWORD read;
    if (!::ReadFile(file_, buffer + total_read,
                    static_cast<DWORD>(length - total_read), &read, nullptr)) {
      break;
    }
    total_read += read;
  } while (total_read < length);
  return total_read;
}

size_t File::WriteAt(const uint8_t* data, size_t length, size_t offset) {
  size_t total_written = 0;
  do {
    DWORD written;

    LARGE_INTEGER offset_li;
    offset_li.QuadPart = offset + total_written;

    OVERLAPPED overlapped = {0};
    overlapped.Offset = offset_li.LowPart;
    overlapped.OffsetHigh = offset_li.HighPart;

    if (!::WriteFile(file_, data + total_written,
                     static_cast<DWORD>(length - total_written), &written,
                     &overlapped)) {
      break;
    }

    total_written += written;
  } while (total_written < length);
  return total_written;
}

size_t File::ReadAt(uint8_t* buffer, size_t length, size_t offset) {
  size_t total_read = 0;
  do {
    DWORD read;

    LARGE_INTEGER offset_li;
    offset_li.QuadPart = offset + total_read;

    OVERLAPPED overlapped = {0};
    overlapped.Offset = offset_li.LowPart;
    overlapped.OffsetHigh = offset_li.HighPart;

    if (!::ReadFile(file_, buffer + total_read,
                    static_cast<DWORD>(length - total_read), &read,
                    &overlapped)) {
      break;
    }

    total_read += read;
  } while (total_read < length);
  return total_read;
}

bool File::Seek(size_t offset) {
  LARGE_INTEGER distance;
  distance.QuadPart = offset;
  return SetFilePointerEx(file_, distance, nullptr, FILE_BEGIN) != 0;
}

bool File::Close() {
  if (file_ == kInvalidPlatformFileValue)
    return false;
  bool ret = CloseHandle(file_) != 0;
  file_ = kInvalidPlatformFileValue;
  return ret;
}
