/**
 * @file utils.h
 * a header for utils.cpp
 * @author Vester
 * @version 0.1
 * @bug no bug is known yet.
 */


#ifndef KAR_UTILS_H_
#define KAR_UTILS_H_
#include <fstream>
#include <stdint.h>
#include <vector>

namespace kar
{
class Chunk;

/**
 * reads 2 bytes in big endian, from a file stream.
 * @param file the file to be read.
 * @return 16-bit value from the file.
 */
uint16_t ReadBigEndianWord(std::istream& file);




/**
 * reads 4 bytes in big endian, from a file stream.
 * @param file the file to be read.
 * @return 32-bit value from the file.
 */
uint32_t ReadBigEndianDWord(std::istream &file);




/**
 * reads a Variable Length Quantity value. up to 4 bytes can be read with this function.
 * @param file the file to be read.
 * @return 32-bit value from a VLQ.
 */
unsigned long ReadVLQ(std::istream &file);




/**
 * reads a Variable Length Quantity from a memory buffer.
 * @param data a 'char' buffer.
 * @param index current index to be read. the function will advance (increment) it after the VLQ data.
 * @return 32-bit value from a VLQ.
 */
unsigned long ReadVLQ(const char* data, unsigned long& index);




/**
 * reads 2 bytes in big endian, from a memory buffer.
 * @param data a 'char' buffer.
 * @param index current index to be read. the function will increment it by 2 bytes.
 * @return 16-bit value from the memory buffer.
 */
uint16_t ReadBigEndianWord(const char* data, unsigned long& index);




/**
 * reads 4 bytes in big endian, from a memory buffer.
 * @param data a 'char' buffer.
 * @param index current index to be read. the function will increment it by 4 bytes.
 * @return 32-bit value from the memory buffer.
 */
uint32_t ReadBigEndianDWord(const char* data, unsigned long& index);




/**
 * reads a Variable Length Quantity from a memory buffer, but does not advance the 'index'.
 * @param data a 'char' buffer.
 * @param size the size of the buffer.
 * @param index current index to be read from the buffer.
 * @return 32-bit VLQ value from the memory buffer
 */
unsigned long CheckVLQ(const char* data, unsigned long size, unsigned long index);




/**
 * reads a MIDI event, but does not advance the 'index'.
 * @param data a 'char' buffer.
 * @param size the size of the buffer.
 * @param index current index to be read from the buffer.
 * @return running_status running status byte. check MIDI specification for a description of "running status".
 */
uint8_t CheckEvent(const char* data, unsigned long size, unsigned long& index, uint8_t running_status);




/**
 * seeks to the beginning of a track. specified by 'track_num'.
 * @param file to be read.
 * @param header a kar::chunk header read previously from the file.
 * @param track_num a zero-base offset of the track.
 * @return 'true' if an error occured. 'false' if success.
 */
bool SeekToTrack(std::istream& file, const kar::Chunk& header, unsigned long track_num);



/**
 * seeks to the beginning of a track. specified by 'track_num'.
 * @param mem address of an midi file image in memory.
 * @param size size of of the midi file image in memory.
 * @param header a kar::chunk header read previously from the file.
 * @param track_num a zero-base offset of the track.
 * @return returns the address of the beginning of the track. returns NULL if fail.
 */
uint8_t* SeekToTrack(uint8_t* mem, unsigned long size, const kar::Chunk& header, unsigned long track_num);



/**
 * returns a value in aligned size. like 1024, 4096, 8192, and so on.. this prevents
 * corruption of data by making sure each allocation does not overlap.
 * an input of 1 to 1023 will return 1024, 1024 to 8191 will return 8192 etc..
 * @param in the input size.
 * @return aligned size.
 */
unsigned long AlignedAllocSize(unsigned long size);




/**
 * check for overlap of buffer memory addresses. (useful to check for memory allocation failure).
 * @param addr1 memory address of buffer1.
 * @param size1 size of buffer1.
 * @param addr2 memory address of buffer2.
 * @param size2 size of buffer2.
 * @return true if memory buffer overlaps, false if not.
 */
bool MemoryAddressOverlap(const void* addr1, unsigned long size1, const void* addr2, unsigned long size2);



/**
 * gets the number of tracks as well as its sizes.
 * @param file the midi file to be read.
 * @param header the header of the file.
 * @return sizes of each track. calling vector::size() returns the number of tracks.
 */
std::vector<unsigned long> CheckTrackSizes(std::istream &file, const kar::Chunk& header);

/**
 * returns the file size in bytes.
 * @param file file to be read.
 * @return the file size in bytes
 */
unsigned long GetFileSize(std::istream& file);

/**
 * reads the file into memory
 * @param file file to be read
 * @param mem starting memory address where the file will be written
 * @return size of the image in memory, 0 if failure
 */
unsigned long ReadFileToMemory(std::istream &file, uint8_t* mem);
}
#endif
