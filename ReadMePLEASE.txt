Read this comment for the life of me when I work on this again.
The only thing is, the offset to be used is the one that is actually in the header. For some reason the comment's written offset is wrong




///////////////////////////////////////////////////////////////////////////////
//
// Format PACK_VER001 Description
//
// If the following files were added to a pack :
// 
//   .\base.cfg                (size 100 bytes)
//   .\graphics\uglykid.jpg    (size 100 bytes)
//   .\sounds\evil\zwaga.wav   (size 200 bytes)
//    
// It would look like this :
// 
// ----
// 0000 PackInfo header
//   0000 Pack Identifier PACK_ID defined below (U32)
//   0004 Format Version PACK_VER001 defined below (U32)
//   0008 Number of directory info entries '2' (U32)
//   0012 Starting offset of directory entries '511' (U32)
//   0016 Number of file info entries '3' (U32)
//   0020 Starting offset of file entries '453' (U32)
//   0024 Unused data for future expansion (32 bytes)
// 0056 File Data for 'base.cfg' (100 bytes)
// 0156 File Data for 'uglykid.jpg' (100 bytes)
// 0256 File Data for 'zwaga.wav' (200 bytes)
// 0456 File Info for 'base.cfg'
//   0456 Directory index '0' (U32)
//   0460 Filename size '8' (U8)
//   0461 Filename 'base.cfg' (8 bytes)
//   0469 Data offset from start of file '0' (U32)
//   0473 Size of data '100' (U32)
// 0477 File Info for 'uglykid.jpg'
//   0477 Directory index '1' (U32)
//   0481 Filename size '11' (U8)
//   0482 Filename 'uglykid.jpg' (11 bytes)
//   0493 Data offset from start of file '100' (U32)
//   0497 Size of data '100' (U32)
// 0501 File Info for 'zwaga.wav'
//   0501 Directory index '2' (U32)
//   0505 Filename size '9' (U8)
//   0506 Filename 'zwaga.wav' (9 bytes)
//   0515 Data offset from start of file '200' (U32)
//   0519 Size of data '200' (U32)
// 0523 Dir Info for 'graphics'
//   0523 Directory name size '8' (U8)
//   0524 Directory name 'graphics' (8 bytes)
// 0532 Dir Info for 'sounds\evil'
//   0532 Directory name size '11' (U8)
//   0533 Directory name 'sounds\evil' (11 bytes)
// ----
//
// For future expansion the unused data in the header could be used to record 
// offsets to new blocks of data within the file.
//
// Notes on file names :
//  - there is no terminating null character and the max size is 256 bytes
//  - directories are NOT saved in filenames (no '\' should appear in the name)
//  - there should be absolutely NO duplicate file names within a pack file, even
//    if they appear in different sub-directories.  This pack format is designed
//    to work with a multi-source file system that is based entirely on the 
//    name of the file - the source of the file is not known.
//
// Notes on directories :
//  - there is no terminating null character and the max size is 256 bytes
//  - directories are strictly sub-dirs from where the pack is extracted.
//  - index 0 means the root dir to which the pack is being extracted
//  - index 1 is the first directory stored in the pack, and so on
//  - the directories are stored without '\' unless between two sub-dirs.
//    eg. 'subdir' but NOT '\subdir'
//    eg. 'subdir\subdir2' but NOT 'subdir\subdir2\'
//
