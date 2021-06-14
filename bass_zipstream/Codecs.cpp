#include "Codecs.h"

#include "../7z/CPP/7zip/Archive/7z/7zRegister.cpp"
#include "../7z/CPP/7zip/Archive/Iso/IsoRegister.cpp"
#include "../7z/CPP/7zip/Archive/Rar/RarRegister.cpp"
#include "../7z/CPP/7zip/Archive/Tar/TarRegister.cpp"
#include "../7z/CPP/7zip/Archive/Zip/ZipRegister.cpp"

#include "../7z/CPP/7zip/Compress/BcjRegister.cpp"
#include "../7z/CPP/7zip/Compress/Bcj2Register.cpp"
#include "../7z/CPP/7zip/Compress/BZip2Register.cpp"
#include "../7z/CPP/7zip/Compress/CopyRegister.cpp"

#if _WIN64
#include "../7z/CPP/7zip/Compress/Deflate64Register.cpp"
#elif _WIN32
#include "../7z/CPP/7zip/Compress/DeflateRegister.cpp"
#endif

#include "../7z/CPP/7zip/Compress/LzmaRegister.cpp"
#include "../7z/CPP/7zip/Compress/Lzma2Register.cpp"
#include "../7z/CPP/7zip/Compress/PpmdRegister.cpp"
#include "../7z/CPP/7zip/Compress/RarCodecsRegister.cpp"