#ifndef KAR_FLAGS_H_
#define KAR_FLAGS_H_

enum
{
     BIT_1       = 1,
     BIT_2       = 2,
     BIT_3       = 4,
     BIT_4       = 8,
     BIT_5       = 16,
     BIT_6       = 32,
     BIT_7       = 64,
     BIT_8       = 128,
     BIT_9       = 256,
     BIT_10      = 512,
     BIT_11      = 1024,
     BIT_12      = 2048,
     BIT_13      = 4096,
     BIT_14      = 8192,
     BIT_15      = 16384,
     BIT_16      = 32768,
     BIT_17      = 65536,
     BIT_18      = 131072,
     BIT_19      = 262144,
     BIT_20      = 524288,
     BIT_21      = 1048576,
     BIT_22      = 2097152,
     BIT_23      = 4194304,
     BIT_24      = 8388608,
     BIT_25      = 16777216,
     BIT_26      = 33554432,
     BIT_27      = 67108864,
     BIT_28      = 134217728,
     BIT_29      = 268435456,
     BIT_30      = 536870912,
     BIT_31      = 1073741824
    // BIT_32      = 2147483648
};

namespace FTRACK
{
    enum
    {
        no_meta = BIT_28,
        no_midi = BIT_29,
        mute = BIT_30
    };
};

/**
 * Enum, String Event Type
 */
namespace ESET
{

enum
{
    type = 1,
    title,
    artist,
    sequencer,
    language,
    info,
    warning,
    paragraph,
    line,
    version,
    ended,
    started,
    maker,
    writer,
    copyright
};

}

/**
 * Enum Ksys Events
 */
namespace EKSYS
{
enum
{
    tempo_change,
    song_ended,
    song_started
};
}
#endif
