#include <stdlib.h>
#include <string.h>
#include "gmqcc.h"

const char *util_instr_str[VINSTR_END] = {
    "DONE",       "MUL_F",      "MUL_V",      "MUL_FV",
    "MUL_VF",     "DIV_F",      "ADD_F",      "ADD_V",
    "SUB_F",      "SUB_V",      "EQ_F",       "EQ_V",
    "EQ_S",       "EQ_E",       "EQ_FNC",     "NE_F",
    "NE_V",       "NE_S",       "NE_E",       "NE_FNC",
    "LE",         "GE",         "LT",         "GT",
    "LOAD_F",     "LOAD_V",     "LOAD_S",     "LOAD_ENT",
    "LOAD_FLD",   "LOAD_FNC",   "ADDRESS",    "STORE_F",
    "STORE_V",    "STORE_S",    "STORE_ENT",  "STORE_FLD",
    "STORE_FNC",  "STOREP_F",   "STOREP_V",   "STOREP_S",
    "STOREP_ENT", "STOREP_FLD", "STOREP_FNC", "RETURN",
    "NOT_F",      "NOT_V",      "NOT_S",      "NOT_ENT",
    "NOT_FNC",    "IF",         "IFNOT",      "CALL0",
    "CALL1",      "CALL2",      "CALL3",      "CALL4",
    "CALL5",      "CALL6",      "CALL7",      "CALL8",
    "STATE",      "GOTO",       "AND",        "OR",
    "BITAND",     "BITOR"
};

/*
 * only required if big endian .. otherwise no need to swap
 * data.
 */
#if PLATFORM_BYTE_ORDER == GMQCC_BYTE_ORDER_BIG || PLATFORM_BYTE_ORDER == -1
    static GMQCC_INLINE void util_swap16(uint16_t *d, size_t l) {
        while (l--) {
            d[l] = (d[l] << 8) | (d[l] >> 8);
        }
    }

    static GMQCC_INLINE void util_swap32(uint32_t *d, size_t l) {
        while (l--) {
            uint32_t v;
            v = ((d[l] << 8) & 0xFF00FF00) | ((d[l] >> 8) & 0x00FF00FF);
            d[l] = (v << 16) | (v >> 16);
        }
    }

    /* Some strange system doesn't like constants that big, AND doesn't recognize an ULL suffix
     * so let's go the safe way
     */
    static GMQCC_INLINE void util_swap64(uint32_t *d, size_t l) {
        while (l--) {
            uint64_t v;
            v = ((d[l] << 8) & 0xFF00FF00FF00FF00) | ((d[l] >> 8) & 0x00FF00FF00FF00FF);
            v = ((v << 16) & 0xFFFF0000FFFF0000) | ((v >> 16) & 0x0000FFFF0000FFFF);
            d[l] = (v << 32) | (v >> 32);
        }
    }
#endif

void util_endianswap(void *_data, size_t count, unsigned int typesize) {
#   if PLATFORM_BYTE_ORDER == -1 /* runtime check */
    if (*((char*)&typesize))
        return;
#else

#   if PLATFORM_BYTE_ORDER == GMQCC_BYTE_ORDER_LITTLE
        /* prevent unused warnings */
        (void) _data;
        (void) count;
        (void) typesize;
        return;
#   else
        switch (typesize) {
            case 1: return;
            case 2:
                util_swap16((uint16_t*)_data, count);
                return;
            case 4:
                util_swap32((uint32_t*)_data, count);
                return;
            case 8:
                util_swap64((uint32_t*)_data, count);
                return;

            default:
                con_err ("util_endianswap: I don't know how to swap a %u byte structure!\n", typesize);
                exit(EXIT_FAILURE); /* please blow the fuck up! */
        }
#   endif
#endif
}

void util_swap_header(prog_header_t &code_header) {
    util_endianswap(&code_header.version,              1, sizeof(code_header.version));
    util_endianswap(&code_header.crc16,                1, sizeof(code_header.crc16));
    util_endianswap(&code_header.statements.offset,    1, sizeof(code_header.statements.offset));
    util_endianswap(&code_header.statements.length,    1, sizeof(code_header.statements.length));
    util_endianswap(&code_header.defs.offset,          1, sizeof(code_header.defs.offset));
    util_endianswap(&code_header.defs.length,          1, sizeof(code_header.defs.length));
    util_endianswap(&code_header.fields.offset,        1, sizeof(code_header.fields.offset));
    util_endianswap(&code_header.fields.length,        1, sizeof(code_header.fields.length));
    util_endianswap(&code_header.functions.offset,     1, sizeof(code_header.functions.offset));
    util_endianswap(&code_header.functions.length,     1, sizeof(code_header.functions.length));
    util_endianswap(&code_header.strings.offset,       1, sizeof(code_header.strings.offset));
    util_endianswap(&code_header.strings.length,       1, sizeof(code_header.strings.length));
    util_endianswap(&code_header.globals.offset,       1, sizeof(code_header.globals.offset));
    util_endianswap(&code_header.globals.length,       1, sizeof(code_header.globals.length));
    util_endianswap(&code_header.entfield,             1, sizeof(code_header.entfield));
}

void util_swap_statements(std::vector<prog_section_statement_t> &statements) {
    for (auto &it : statements) {
        util_endianswap(&it.opcode,  1, sizeof(it.opcode));
        util_endianswap(&it.o1,      1, sizeof(it.o1));
        util_endianswap(&it.o2,      1, sizeof(it.o2));
        util_endianswap(&it.o3,      1, sizeof(it.o3));
    }
}

void util_swap_defs_fields(std::vector<prog_section_both_t> &section) {
    for (auto &it : section) {
        util_endianswap(&it.type,   1, sizeof(it.type));
        util_endianswap(&it.offset, 1, sizeof(it.offset));
        util_endianswap(&it.name,   1, sizeof(it.name));
    }
}

void util_swap_functions(std::vector<prog_section_function_t> &functions) {
    for (auto &it : functions) {
        util_endianswap(&it.entry,        1, sizeof(it.entry));
        util_endianswap(&it.firstlocal,   1, sizeof(it.firstlocal));
        util_endianswap(&it.locals,       1, sizeof(it.locals));
        util_endianswap(&it.profile,      1, sizeof(it.profile));
        util_endianswap(&it.name,         1, sizeof(it.name));
        util_endianswap(&it.file,         1, sizeof(it.file));
        util_endianswap(&it.nargs,        1, sizeof(it.nargs));
        /* Don't swap argsize[] - it's just a byte array, which Quake uses only as such. */
    }
}

void util_swap_globals(std::vector<int32_t> &globals) {
    util_endianswap(&globals[0], globals.size(), sizeof(int32_t));
}

/*
* Based On:
*   Slicing-by-8 algorithms by Michael E.
*       Kounavis and Frank L. Berry from Intel Corp.
*       http://www.intel.com/technology/comms/perfnet/download/CRC_generators.pdf
*
*   This code was made to be slightly less confusing with macros, which
*   I suppose is somewhat ironic.
*
*   The code had to be changed for non reflected on the output register
*   since that's the method Quake uses.
*
*   The code also had to be changed for CRC16, which is slightly harder
*   since the CRC32 method in the original Intel paper used a different
*   bit order convention.
*
* Notes about the table:
*   - It's exactly 4K in size
*   - 64 elements fit in a cache line
*   - can do 8 iterations unrolled 8 times for free
*   - The first 256 elements of the table are standard CRC16 table
*
* Table can be generated with the following utility:
*/
#if 0
#include <stdio.h>
#include <stdint.h>
int main(void) {
    for (unsigned i = 0; i < 0x100; ++i) {
        uint16_t x = i << 8;
        for (int j = 0; j < 8; ++j)
            x = (x << 1) ^ ((x & 0x8000) ? 0x1021 : 0);
        tab[0][i] = x;
    }
    for (unsigned i = 0; i < 0x100; ++i) {
        uint16_t c = tab[0][i];
        for (unsigned j = 1; j < 8; ++j) {
            c = tab[0][c >> 8] ^ (c << 8);
            tab[j][i] = c;
        }
    }
    printf("static const uint16_t util_crc16_table[8][256] = {");
    for (int i = 0; i < 8; ++i) {
        printf("{\n");
        for (int j = 0; j < 0x100; ++j) {
            printf((j & 7) ? " " : "    ");
            printf((j != 0x100-1) ? "0x%04X," : "0x%04X", tab[i][j]);
            if ((j & 7) == 7)
                printf("\n");
        }
        printf((i != 7) ? "}," : "}");
    }
    printf("};\n");
    return 0;
}
#endif
/*
 * Non-Reflective version is present as well as a reference.
 *
 * TODO:
 *  combine the crc16 into u32s and mask off low high for byte order
 *  to make the arrays smaller.
 */

static const uint16_t util_crc16_table[8][256] = {{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
},{
    0x0000, 0x3331, 0x6662, 0x5553, 0xCCC4, 0xFFF5, 0xAAA6, 0x9997,
    0x89A9, 0xBA98, 0xEFCB, 0xDCFA, 0x456D, 0x765C, 0x230F, 0x103E,
    0x0373, 0x3042, 0x6511, 0x5620, 0xCFB7, 0xFC86, 0xA9D5, 0x9AE4,
    0x8ADA, 0xB9EB, 0xECB8, 0xDF89, 0x461E, 0x752F, 0x207C, 0x134D,
    0x06E6, 0x35D7, 0x6084, 0x53B5, 0xCA22, 0xF913, 0xAC40, 0x9F71,
    0x8F4F, 0xBC7E, 0xE92D, 0xDA1C, 0x438B, 0x70BA, 0x25E9, 0x16D8,
    0x0595, 0x36A4, 0x63F7, 0x50C6, 0xC951, 0xFA60, 0xAF33, 0x9C02,
    0x8C3C, 0xBF0D, 0xEA5E, 0xD96F, 0x40F8, 0x73C9, 0x269A, 0x15AB,
    0x0DCC, 0x3EFD, 0x6BAE, 0x589F, 0xC108, 0xF239, 0xA76A, 0x945B,
    0x8465, 0xB754, 0xE207, 0xD136, 0x48A1, 0x7B90, 0x2EC3, 0x1DF2,
    0x0EBF, 0x3D8E, 0x68DD, 0x5BEC, 0xC27B, 0xF14A, 0xA419, 0x9728,
    0x8716, 0xB427, 0xE174, 0xD245, 0x4BD2, 0x78E3, 0x2DB0, 0x1E81,
    0x0B2A, 0x381B, 0x6D48, 0x5E79, 0xC7EE, 0xF4DF, 0xA18C, 0x92BD,
    0x8283, 0xB1B2, 0xE4E1, 0xD7D0, 0x4E47, 0x7D76, 0x2825, 0x1B14,
    0x0859, 0x3B68, 0x6E3B, 0x5D0A, 0xC49D, 0xF7AC, 0xA2FF, 0x91CE,
    0x81F0, 0xB2C1, 0xE792, 0xD4A3, 0x4D34, 0x7E05, 0x2B56, 0x1867,
    0x1B98, 0x28A9, 0x7DFA, 0x4ECB, 0xD75C, 0xE46D, 0xB13E, 0x820F,
    0x9231, 0xA100, 0xF453, 0xC762, 0x5EF5, 0x6DC4, 0x3897, 0x0BA6,
    0x18EB, 0x2BDA, 0x7E89, 0x4DB8, 0xD42F, 0xE71E, 0xB24D, 0x817C,
    0x9142, 0xA273, 0xF720, 0xC411, 0x5D86, 0x6EB7, 0x3BE4, 0x08D5,
    0x1D7E, 0x2E4F, 0x7B1C, 0x482D, 0xD1BA, 0xE28B, 0xB7D8, 0x84E9,
    0x94D7, 0xA7E6, 0xF2B5, 0xC184, 0x5813, 0x6B22, 0x3E71, 0x0D40,
    0x1E0D, 0x2D3C, 0x786F, 0x4B5E, 0xD2C9, 0xE1F8, 0xB4AB, 0x879A,
    0x97A4, 0xA495, 0xF1C6, 0xC2F7, 0x5B60, 0x6851, 0x3D02, 0x0E33,
    0x1654, 0x2565, 0x7036, 0x4307, 0xDA90, 0xE9A1, 0xBCF2, 0x8FC3,
    0x9FFD, 0xACCC, 0xF99F, 0xCAAE, 0x5339, 0x6008, 0x355B, 0x066A,
    0x1527, 0x2616, 0x7345, 0x4074, 0xD9E3, 0xEAD2, 0xBF81, 0x8CB0,
    0x9C8E, 0xAFBF, 0xFAEC, 0xC9DD, 0x504A, 0x637B, 0x3628, 0x0519,
    0x10B2, 0x2383, 0x76D0, 0x45E1, 0xDC76, 0xEF47, 0xBA14, 0x8925,
    0x991B, 0xAA2A, 0xFF79, 0xCC48, 0x55DF, 0x66EE, 0x33BD, 0x008C,
    0x13C1, 0x20F0, 0x75A3, 0x4692, 0xDF05, 0xEC34, 0xB967, 0x8A56,
    0x9A68, 0xA959, 0xFC0A, 0xCF3B, 0x56AC, 0x659D, 0x30CE, 0x03FF
},{
    0x0000, 0x3730, 0x6E60, 0x5950, 0xDCC0, 0xEBF0, 0xB2A0, 0x8590,
    0xA9A1, 0x9E91, 0xC7C1, 0xF0F1, 0x7561, 0x4251, 0x1B01, 0x2C31,
    0x4363, 0x7453, 0x2D03, 0x1A33, 0x9FA3, 0xA893, 0xF1C3, 0xC6F3,
    0xEAC2, 0xDDF2, 0x84A2, 0xB392, 0x3602, 0x0132, 0x5862, 0x6F52,
    0x86C6, 0xB1F6, 0xE8A6, 0xDF96, 0x5A06, 0x6D36, 0x3466, 0x0356,
    0x2F67, 0x1857, 0x4107, 0x7637, 0xF3A7, 0xC497, 0x9DC7, 0xAAF7,
    0xC5A5, 0xF295, 0xABC5, 0x9CF5, 0x1965, 0x2E55, 0x7705, 0x4035,
    0x6C04, 0x5B34, 0x0264, 0x3554, 0xB0C4, 0x87F4, 0xDEA4, 0xE994,
    0x1DAD, 0x2A9D, 0x73CD, 0x44FD, 0xC16D, 0xF65D, 0xAF0D, 0x983D,
    0xB40C, 0x833C, 0xDA6C, 0xED5C, 0x68CC, 0x5FFC, 0x06AC, 0x319C,
    0x5ECE, 0x69FE, 0x30AE, 0x079E, 0x820E, 0xB53E, 0xEC6E, 0xDB5E,
    0xF76F, 0xC05F, 0x990F, 0xAE3F, 0x2BAF, 0x1C9F, 0x45CF, 0x72FF,
    0x9B6B, 0xAC5B, 0xF50B, 0xC23B, 0x47AB, 0x709B, 0x29CB, 0x1EFB,
    0x32CA, 0x05FA, 0x5CAA, 0x6B9A, 0xEE0A, 0xD93A, 0x806A, 0xB75A,
    0xD808, 0xEF38, 0xB668, 0x8158, 0x04C8, 0x33F8, 0x6AA8, 0x5D98,
    0x71A9, 0x4699, 0x1FC9, 0x28F9, 0xAD69, 0x9A59, 0xC309, 0xF439,
    0x3B5A, 0x0C6A, 0x553A, 0x620A, 0xE79A, 0xD0AA, 0x89FA, 0xBECA,
    0x92FB, 0xA5CB, 0xFC9B, 0xCBAB, 0x4E3B, 0x790B, 0x205B, 0x176B,
    0x7839, 0x4F09, 0x1659, 0x2169, 0xA4F9, 0x93C9, 0xCA99, 0xFDA9,
    0xD198, 0xE6A8, 0xBFF8, 0x88C8, 0x0D58, 0x3A68, 0x6338, 0x5408,
    0xBD9C, 0x8AAC, 0xD3FC, 0xE4CC, 0x615C, 0x566C, 0x0F3C, 0x380C,
    0x143D, 0x230D, 0x7A5D, 0x4D6D, 0xC8FD, 0xFFCD, 0xA69D, 0x91AD,
    0xFEFF, 0xC9CF, 0x909F, 0xA7AF, 0x223F, 0x150F, 0x4C5F, 0x7B6F,
    0x575E, 0x606E, 0x393E, 0x0E0E, 0x8B9E, 0xBCAE, 0xE5FE, 0xD2CE,
    0x26F7, 0x11C7, 0x4897, 0x7FA7, 0xFA37, 0xCD07, 0x9457, 0xA367,
    0x8F56, 0xB866, 0xE136, 0xD606, 0x5396, 0x64A6, 0x3DF6, 0x0AC6,
    0x6594, 0x52A4, 0x0BF4, 0x3CC4, 0xB954, 0x8E64, 0xD734, 0xE004,
    0xCC35, 0xFB05, 0xA255, 0x9565, 0x10F5, 0x27C5, 0x7E95, 0x49A5,
    0xA031, 0x9701, 0xCE51, 0xF961, 0x7CF1, 0x4BC1, 0x1291, 0x25A1,
    0x0990, 0x3EA0, 0x67F0, 0x50C0, 0xD550, 0xE260, 0xBB30, 0x8C00,
    0xE352, 0xD462, 0x8D32, 0xBA02, 0x3F92, 0x08A2, 0x51F2, 0x66C2,
    0x4AF3, 0x7DC3, 0x2493, 0x13A3, 0x9633, 0xA103, 0xF853, 0xCF63
},{
    0x0000, 0x76B4, 0xED68, 0x9BDC, 0xCAF1, 0xBC45, 0x2799, 0x512D,
    0x85C3, 0xF377, 0x68AB, 0x1E1F, 0x4F32, 0x3986, 0xA25A, 0xD4EE,
    0x1BA7, 0x6D13, 0xF6CF, 0x807B, 0xD156, 0xA7E2, 0x3C3E, 0x4A8A,
    0x9E64, 0xE8D0, 0x730C, 0x05B8, 0x5495, 0x2221, 0xB9FD, 0xCF49,
    0x374E, 0x41FA, 0xDA26, 0xAC92, 0xFDBF, 0x8B0B, 0x10D7, 0x6663,
    0xB28D, 0xC439, 0x5FE5, 0x2951, 0x787C, 0x0EC8, 0x9514, 0xE3A0,
    0x2CE9, 0x5A5D, 0xC181, 0xB735, 0xE618, 0x90AC, 0x0B70, 0x7DC4,
    0xA92A, 0xDF9E, 0x4442, 0x32F6, 0x63DB, 0x156F, 0x8EB3, 0xF807,
    0x6E9C, 0x1828, 0x83F4, 0xF540, 0xA46D, 0xD2D9, 0x4905, 0x3FB1,
    0xEB5F, 0x9DEB, 0x0637, 0x7083, 0x21AE, 0x571A, 0xCCC6, 0xBA72,
    0x753B, 0x038F, 0x9853, 0xEEE7, 0xBFCA, 0xC97E, 0x52A2, 0x2416,
    0xF0F8, 0x864C, 0x1D90, 0x6B24, 0x3A09, 0x4CBD, 0xD761, 0xA1D5,
    0x59D2, 0x2F66, 0xB4BA, 0xC20E, 0x9323, 0xE597, 0x7E4B, 0x08FF,
    0xDC11, 0xAAA5, 0x3179, 0x47CD, 0x16E0, 0x6054, 0xFB88, 0x8D3C,
    0x4275, 0x34C1, 0xAF1D, 0xD9A9, 0x8884, 0xFE30, 0x65EC, 0x1358,
    0xC7B6, 0xB102, 0x2ADE, 0x5C6A, 0x0D47, 0x7BF3, 0xE02F, 0x969B,
    0xDD38, 0xAB8C, 0x3050, 0x46E4, 0x17C9, 0x617D, 0xFAA1, 0x8C15,
    0x58FB, 0x2E4F, 0xB593, 0xC327, 0x920A, 0xE4BE, 0x7F62, 0x09D6,
    0xC69F, 0xB02B, 0x2BF7, 0x5D43, 0x0C6E, 0x7ADA, 0xE106, 0x97B2,
    0x435C, 0x35E8, 0xAE34, 0xD880, 0x89AD, 0xFF19, 0x64C5, 0x1271,
    0xEA76, 0x9CC2, 0x071E, 0x71AA, 0x2087, 0x5633, 0xCDEF, 0xBB5B,
    0x6FB5, 0x1901, 0x82DD, 0xF469, 0xA544, 0xD3F0, 0x482C, 0x3E98,
    0xF1D1, 0x8765, 0x1CB9, 0x6A0D, 0x3B20, 0x4D94, 0xD648, 0xA0FC,
    0x7412, 0x02A6, 0x997A, 0xEFCE, 0xBEE3, 0xC857, 0x538B, 0x253F,
    0xB3A4, 0xC510, 0x5ECC, 0x2878, 0x7955, 0x0FE1, 0x943D, 0xE289,
    0x3667, 0x40D3, 0xDB0F, 0xADBB, 0xFC96, 0x8A22, 0x11FE, 0x674A,
    0xA803, 0xDEB7, 0x456B, 0x33DF, 0x62F2, 0x1446, 0x8F9A, 0xF92E,
    0x2DC0, 0x5B74, 0xC0A8, 0xB61C, 0xE731, 0x9185, 0x0A59, 0x7CED,
    0x84EA, 0xF25E, 0x6982, 0x1F36, 0x4E1B, 0x38AF, 0xA373, 0xD5C7,
    0x0129, 0x779D, 0xEC41, 0x9AF5, 0xCBD8, 0xBD6C, 0x26B0, 0x5004,
    0x9F4D, 0xE9F9, 0x7225, 0x0491, 0x55BC, 0x2308, 0xB8D4, 0xCE60,
    0x1A8E, 0x6C3A, 0xF7E6, 0x8152, 0xD07F, 0xA6CB, 0x3D17, 0x4BA3
},{
    0x0000, 0xAA51, 0x4483, 0xEED2, 0x8906, 0x2357, 0xCD85, 0x67D4,
    0x022D, 0xA87C, 0x46AE, 0xECFF, 0x8B2B, 0x217A, 0xCFA8, 0x65F9,
    0x045A, 0xAE0B, 0x40D9, 0xEA88, 0x8D5C, 0x270D, 0xC9DF, 0x638E,
    0x0677, 0xAC26, 0x42F4, 0xE8A5, 0x8F71, 0x2520, 0xCBF2, 0x61A3,
    0x08B4, 0xA2E5, 0x4C37, 0xE666, 0x81B2, 0x2BE3, 0xC531, 0x6F60,
    0x0A99, 0xA0C8, 0x4E1A, 0xE44B, 0x839F, 0x29CE, 0xC71C, 0x6D4D,
    0x0CEE, 0xA6BF, 0x486D, 0xE23C, 0x85E8, 0x2FB9, 0xC16B, 0x6B3A,
    0x0EC3, 0xA492, 0x4A40, 0xE011, 0x87C5, 0x2D94, 0xC346, 0x6917,
    0x1168, 0xBB39, 0x55EB, 0xFFBA, 0x986E, 0x323F, 0xDCED, 0x76BC,
    0x1345, 0xB914, 0x57C6, 0xFD97, 0x9A43, 0x3012, 0xDEC0, 0x7491,
    0x1532, 0xBF63, 0x51B1, 0xFBE0, 0x9C34, 0x3665, 0xD8B7, 0x72E6,
    0x171F, 0xBD4E, 0x539C, 0xF9CD, 0x9E19, 0x3448, 0xDA9A, 0x70CB,
    0x19DC, 0xB38D, 0x5D5F, 0xF70E, 0x90DA, 0x3A8B, 0xD459, 0x7E08,
    0x1BF1, 0xB1A0, 0x5F72, 0xF523, 0x92F7, 0x38A6, 0xD674, 0x7C25,
    0x1D86, 0xB7D7, 0x5905, 0xF354, 0x9480, 0x3ED1, 0xD003, 0x7A52,
    0x1FAB, 0xB5FA, 0x5B28, 0xF179, 0x96AD, 0x3CFC, 0xD22E, 0x787F,
    0x22D0, 0x8881, 0x6653, 0xCC02, 0xABD6, 0x0187, 0xEF55, 0x4504,
    0x20FD, 0x8AAC, 0x647E, 0xCE2F, 0xA9FB, 0x03AA, 0xED78, 0x4729,
    0x268A, 0x8CDB, 0x6209, 0xC858, 0xAF8C, 0x05DD, 0xEB0F, 0x415E,
    0x24A7, 0x8EF6, 0x6024, 0xCA75, 0xADA1, 0x07F0, 0xE922, 0x4373,
    0x2A64, 0x8035, 0x6EE7, 0xC4B6, 0xA362, 0x0933, 0xE7E1, 0x4DB0,
    0x2849, 0x8218, 0x6CCA, 0xC69B, 0xA14F, 0x0B1E, 0xE5CC, 0x4F9D,
    0x2E3E, 0x846F, 0x6ABD, 0xC0EC, 0xA738, 0x0D69, 0xE3BB, 0x49EA,
    0x2C13, 0x8642, 0x6890, 0xC2C1, 0xA515, 0x0F44, 0xE196, 0x4BC7,
    0x33B8, 0x99E9, 0x773B, 0xDD6A, 0xBABE, 0x10EF, 0xFE3D, 0x546C,
    0x3195, 0x9BC4, 0x7516, 0xDF47, 0xB893, 0x12C2, 0xFC10, 0x5641,
    0x37E2, 0x9DB3, 0x7361, 0xD930, 0xBEE4, 0x14B5, 0xFA67, 0x5036,
    0x35CF, 0x9F9E, 0x714C, 0xDB1D, 0xBCC9, 0x1698, 0xF84A, 0x521B,
    0x3B0C, 0x915D, 0x7F8F, 0xD5DE, 0xB20A, 0x185B, 0xF689, 0x5CD8,
    0x3921, 0x9370, 0x7DA2, 0xD7F3, 0xB027, 0x1A76, 0xF4A4, 0x5EF5,
    0x3F56, 0x9507, 0x7BD5, 0xD184, 0xB650, 0x1C01, 0xF2D3, 0x5882,
    0x3D7B, 0x972A, 0x79F8, 0xD3A9, 0xB47D, 0x1E2C, 0xF0FE, 0x5AAF
},{
    0x0000, 0x45A0, 0x8B40, 0xCEE0, 0x06A1, 0x4301, 0x8DE1, 0xC841,
    0x0D42, 0x48E2, 0x8602, 0xC3A2, 0x0BE3, 0x4E43, 0x80A3, 0xC503,
    0x1A84, 0x5F24, 0x91C4, 0xD464, 0x1C25, 0x5985, 0x9765, 0xD2C5,
    0x17C6, 0x5266, 0x9C86, 0xD926, 0x1167, 0x54C7, 0x9A27, 0xDF87,
    0x3508, 0x70A8, 0xBE48, 0xFBE8, 0x33A9, 0x7609, 0xB8E9, 0xFD49,
    0x384A, 0x7DEA, 0xB30A, 0xF6AA, 0x3EEB, 0x7B4B, 0xB5AB, 0xF00B,
    0x2F8C, 0x6A2C, 0xA4CC, 0xE16C, 0x292D, 0x6C8D, 0xA26D, 0xE7CD,
    0x22CE, 0x676E, 0xA98E, 0xEC2E, 0x246F, 0x61CF, 0xAF2F, 0xEA8F,
    0x6A10, 0x2FB0, 0xE150, 0xA4F0, 0x6CB1, 0x2911, 0xE7F1, 0xA251,
    0x6752, 0x22F2, 0xEC12, 0xA9B2, 0x61F3, 0x2453, 0xEAB3, 0xAF13,
    0x7094, 0x3534, 0xFBD4, 0xBE74, 0x7635, 0x3395, 0xFD75, 0xB8D5,
    0x7DD6, 0x3876, 0xF696, 0xB336, 0x7B77, 0x3ED7, 0xF037, 0xB597,
    0x5F18, 0x1AB8, 0xD458, 0x91F8, 0x59B9, 0x1C19, 0xD2F9, 0x9759,
    0x525A, 0x17FA, 0xD91A, 0x9CBA, 0x54FB, 0x115B, 0xDFBB, 0x9A1B,
    0x459C, 0x003C, 0xCEDC, 0x8B7C, 0x433D, 0x069D, 0xC87D, 0x8DDD,
    0x48DE, 0x0D7E, 0xC39E, 0x863E, 0x4E7F, 0x0BDF, 0xC53F, 0x809F,
    0xD420, 0x9180, 0x5F60, 0x1AC0, 0xD281, 0x9721, 0x59C1, 0x1C61,
    0xD962, 0x9CC2, 0x5222, 0x1782, 0xDFC3, 0x9A63, 0x5483, 0x1123,
    0xCEA4, 0x8B04, 0x45E4, 0x0044, 0xC805, 0x8DA5, 0x4345, 0x06E5,
    0xC3E6, 0x8646, 0x48A6, 0x0D06, 0xC547, 0x80E7, 0x4E07, 0x0BA7,
    0xE128, 0xA488, 0x6A68, 0x2FC8, 0xE789, 0xA229, 0x6CC9, 0x2969,
    0xEC6A, 0xA9CA, 0x672A, 0x228A, 0xEACB, 0xAF6B, 0x618B, 0x242B,
    0xFBAC, 0xBE0C, 0x70EC, 0x354C, 0xFD0D, 0xB8AD, 0x764D, 0x33ED,
    0xF6EE, 0xB34E, 0x7DAE, 0x380E, 0xF04F, 0xB5EF, 0x7B0F, 0x3EAF,
    0xBE30, 0xFB90, 0x3570, 0x70D0, 0xB891, 0xFD31, 0x33D1, 0x7671,
    0xB372, 0xF6D2, 0x3832, 0x7D92, 0xB5D3, 0xF073, 0x3E93, 0x7B33,
    0xA4B4, 0xE114, 0x2FF4, 0x6A54, 0xA215, 0xE7B5, 0x2955, 0x6CF5,
    0xA9F6, 0xEC56, 0x22B6, 0x6716, 0xAF57, 0xEAF7, 0x2417, 0x61B7,
    0x8B38, 0xCE98, 0x0078, 0x45D8, 0x8D99, 0xC839, 0x06D9, 0x4379,
    0x867A, 0xC3DA, 0x0D3A, 0x489A, 0x80DB, 0xC57B, 0x0B9B, 0x4E3B,
    0x91BC, 0xD41C, 0x1AFC, 0x5F5C, 0x971D, 0xD2BD, 0x1C5D, 0x59FD,
    0x9CFE, 0xD95E, 0x17BE, 0x521E, 0x9A5F, 0xDFFF, 0x111F, 0x54BF
},{
    0x0000, 0xB861, 0x60E3, 0xD882, 0xC1C6, 0x79A7, 0xA125, 0x1944,
    0x93AD, 0x2BCC, 0xF34E, 0x4B2F, 0x526B, 0xEA0A, 0x3288, 0x8AE9,
    0x377B, 0x8F1A, 0x5798, 0xEFF9, 0xF6BD, 0x4EDC, 0x965E, 0x2E3F,
    0xA4D6, 0x1CB7, 0xC435, 0x7C54, 0x6510, 0xDD71, 0x05F3, 0xBD92,
    0x6EF6, 0xD697, 0x0E15, 0xB674, 0xAF30, 0x1751, 0xCFD3, 0x77B2,
    0xFD5B, 0x453A, 0x9DB8, 0x25D9, 0x3C9D, 0x84FC, 0x5C7E, 0xE41F,
    0x598D, 0xE1EC, 0x396E, 0x810F, 0x984B, 0x202A, 0xF8A8, 0x40C9,
    0xCA20, 0x7241, 0xAAC3, 0x12A2, 0x0BE6, 0xB387, 0x6B05, 0xD364,
    0xDDEC, 0x658D, 0xBD0F, 0x056E, 0x1C2A, 0xA44B, 0x7CC9, 0xC4A8,
    0x4E41, 0xF620, 0x2EA2, 0x96C3, 0x8F87, 0x37E6, 0xEF64, 0x5705,
    0xEA97, 0x52F6, 0x8A74, 0x3215, 0x2B51, 0x9330, 0x4BB2, 0xF3D3,
    0x793A, 0xC15B, 0x19D9, 0xA1B8, 0xB8FC, 0x009D, 0xD81F, 0x607E,
    0xB31A, 0x0B7B, 0xD3F9, 0x6B98, 0x72DC, 0xCABD, 0x123F, 0xAA5E,
    0x20B7, 0x98D6, 0x4054, 0xF835, 0xE171, 0x5910, 0x8192, 0x39F3,
    0x8461, 0x3C00, 0xE482, 0x5CE3, 0x45A7, 0xFDC6, 0x2544, 0x9D25,
    0x17CC, 0xAFAD, 0x772F, 0xCF4E, 0xD60A, 0x6E6B, 0xB6E9, 0x0E88,
    0xABF9, 0x1398, 0xCB1A, 0x737B, 0x6A3F, 0xD25E, 0x0ADC, 0xB2BD,
    0x3854, 0x8035, 0x58B7, 0xE0D6, 0xF992, 0x41F3, 0x9971, 0x2110,
    0x9C82, 0x24E3, 0xFC61, 0x4400, 0x5D44, 0xE525, 0x3DA7, 0x85C6,
    0x0F2F, 0xB74E, 0x6FCC, 0xD7AD, 0xCEE9, 0x7688, 0xAE0A, 0x166B,
    0xC50F, 0x7D6E, 0xA5EC, 0x1D8D, 0x04C9, 0xBCA8, 0x642A, 0xDC4B,
    0x56A2, 0xEEC3, 0x3641, 0x8E20, 0x9764, 0x2F05, 0xF787, 0x4FE6,
    0xF274, 0x4A15, 0x9297, 0x2AF6, 0x33B2, 0x8BD3, 0x5351, 0xEB30,
    0x61D9, 0xD9B8, 0x013A, 0xB95B, 0xA01F, 0x187E, 0xC0FC, 0x789D,
    0x7615, 0xCE74, 0x16F6, 0xAE97, 0xB7D3, 0x0FB2, 0xD730, 0x6F51,
    0xE5B8, 0x5DD9, 0x855B, 0x3D3A, 0x247E, 0x9C1F, 0x449D, 0xFCFC,
    0x416E, 0xF90F, 0x218D, 0x99EC, 0x80A8, 0x38C9, 0xE04B, 0x582A,
    0xD2C3, 0x6AA2, 0xB220, 0x0A41, 0x1305, 0xAB64, 0x73E6, 0xCB87,
    0x18E3, 0xA082, 0x7800, 0xC061, 0xD925, 0x6144, 0xB9C6, 0x01A7,
    0x8B4E, 0x332F, 0xEBAD, 0x53CC, 0x4A88, 0xF2E9, 0x2A6B, 0x920A,
    0x2F98, 0x97F9, 0x4F7B, 0xF71A, 0xEE5E, 0x563F, 0x8EBD, 0x36DC,
    0xBC35, 0x0454, 0xDCD6, 0x64B7, 0x7DF3, 0xC592, 0x1D10, 0xA571
},{
    0x0000, 0x47D3, 0x8FA6, 0xC875, 0x0F6D, 0x48BE, 0x80CB, 0xC718,
    0x1EDA, 0x5909, 0x917C, 0xD6AF, 0x11B7, 0x5664, 0x9E11, 0xD9C2,
    0x3DB4, 0x7A67, 0xB212, 0xF5C1, 0x32D9, 0x750A, 0xBD7F, 0xFAAC,
    0x236E, 0x64BD, 0xACC8, 0xEB1B, 0x2C03, 0x6BD0, 0xA3A5, 0xE476,
    0x7B68, 0x3CBB, 0xF4CE, 0xB31D, 0x7405, 0x33D6, 0xFBA3, 0xBC70,
    0x65B2, 0x2261, 0xEA14, 0xADC7, 0x6ADF, 0x2D0C, 0xE579, 0xA2AA,
    0x46DC, 0x010F, 0xC97A, 0x8EA9, 0x49B1, 0x0E62, 0xC617, 0x81C4,
    0x5806, 0x1FD5, 0xD7A0, 0x9073, 0x576B, 0x10B8, 0xD8CD, 0x9F1E,
    0xF6D0, 0xB103, 0x7976, 0x3EA5, 0xF9BD, 0xBE6E, 0x761B, 0x31C8,
    0xE80A, 0xAFD9, 0x67AC, 0x207F, 0xE767, 0xA0B4, 0x68C1, 0x2F12,
    0xCB64, 0x8CB7, 0x44C2, 0x0311, 0xC409, 0x83DA, 0x4BAF, 0x0C7C,
    0xD5BE, 0x926D, 0x5A18, 0x1DCB, 0xDAD3, 0x9D00, 0x5575, 0x12A6,
    0x8DB8, 0xCA6B, 0x021E, 0x45CD, 0x82D5, 0xC506, 0x0D73, 0x4AA0,
    0x9362, 0xD4B1, 0x1CC4, 0x5B17, 0x9C0F, 0xDBDC, 0x13A9, 0x547A,
    0xB00C, 0xF7DF, 0x3FAA, 0x7879, 0xBF61, 0xF8B2, 0x30C7, 0x7714,
    0xAED6, 0xE905, 0x2170, 0x66A3, 0xA1BB, 0xE668, 0x2E1D, 0x69CE,
    0xFD81, 0xBA52, 0x7227, 0x35F4, 0xF2EC, 0xB53F, 0x7D4A, 0x3A99,
    0xE35B, 0xA488, 0x6CFD, 0x2B2E, 0xEC36, 0xABE5, 0x6390, 0x2443,
    0xC035, 0x87E6, 0x4F93, 0x0840, 0xCF58, 0x888B, 0x40FE, 0x072D,
    0xDEEF, 0x993C, 0x5149, 0x169A, 0xD182, 0x9651, 0x5E24, 0x19F7,
    0x86E9, 0xC13A, 0x094F, 0x4E9C, 0x8984, 0xCE57, 0x0622, 0x41F1,
    0x9833, 0xDFE0, 0x1795, 0x5046, 0x975E, 0xD08D, 0x18F8, 0x5F2B,
    0xBB5D, 0xFC8E, 0x34FB, 0x7328, 0xB430, 0xF3E3, 0x3B96, 0x7C45,
    0xA587, 0xE254, 0x2A21, 0x6DF2, 0xAAEA, 0xED39, 0x254C, 0x629F,
    0x0B51, 0x4C82, 0x84F7, 0xC324, 0x043C, 0x43EF, 0x8B9A, 0xCC49,
    0x158B, 0x5258, 0x9A2D, 0xDDFE, 0x1AE6, 0x5D35, 0x9540, 0xD293,
    0x36E5, 0x7136, 0xB943, 0xFE90, 0x3988, 0x7E5B, 0xB62E, 0xF1FD,
    0x283F, 0x6FEC, 0xA799, 0xE04A, 0x2752, 0x6081, 0xA8F4, 0xEF27,
    0x7039, 0x37EA, 0xFF9F, 0xB84C, 0x7F54, 0x3887, 0xF0F2, 0xB721,
    0x6EE3, 0x2930, 0xE145, 0xA696, 0x618E, 0x265D, 0xEE28, 0xA9FB,
    0x4D8D, 0x0A5E, 0xC22B, 0x85F8, 0x42E0, 0x0533, 0xCD46, 0x8A95,
    0x5357, 0x1484, 0xDCF1, 0x9B22, 0x5C3A, 0x1BE9, 0xD39C, 0x944F
}};

/* Non - Reflected */
uint16_t util_crc16(uint16_t current, const char *GMQCC_RESTRICT k, size_t len) {
    uint16_t h = current;

    /* don't load twice */
    const uint8_t *GMQCC_RESTRICT data = (const uint8_t *GMQCC_RESTRICT)k;
    size_t n;

    /* deal with the first bytes as bytes until we reach an 8 byte boundary */
    while (len & 7) {
        h = (uint16_t)(h << 8) ^ (*util_crc16_table)[(h >> 8) ^ *data++];
        --len;
    }

    #define SELECT_BULK(X, MOD) util_crc16_table[(X)][data[7-(X)] ^ (MOD)]
    #define SELECT_DATA(X)      util_crc16_table[(X)][data[7-(X)]]

    for (n = len / 8; n; --n) {
        h = SELECT_BULK(7, (h >> 8))   ^
            SELECT_BULK(6, (h & 0xFF)) ^
            SELECT_DATA(5) ^
            SELECT_DATA(4) ^
            SELECT_DATA(3) ^
            SELECT_DATA(2) ^
            SELECT_DATA(1) ^
            SELECT_DATA(0);
        data += 8;
        len  -= 8;
    }

    #undef SELECT_BULK
    #undef SELECT_DATA

    /* deal with the rest with the byte method */
    for (n = len & 7; n; --n)
        h = (uint16_t)(h << 8) ^ (*util_crc16_table)[(h >> 8) ^ *data++];

    return h;
}

/*
 * modifier is the match to make and the transposition from it, while add is the upper-value that determines the
 * transposition from uppercase to lower case.
 */
static size_t util_strtransform(const char *in, char *out, size_t outsz, const char *mod, int add) {
    size_t sz = 1;
    for (; *in && sz < outsz; ++in, ++out, ++sz) {
        *out = (*in == mod[0])
                    ? mod[1]
                    : (util_isalpha(*in) && ((add > 0) ? util_isupper(*in) : !util_isupper(*in)))
                        ? *in + add
                        : *in;
    }
    *out = 0;
    return sz-1;
}

size_t util_strtocmd(const char *in, char *out, size_t outsz) {
    return util_strtransform(in, out, outsz, "-_", 'A'-'a');
}
size_t util_strtononcmd(const char *in, char *out, size_t outsz) {
    return util_strtransform(in, out, outsz, "_-", 'a'-'A');
}
size_t util_optimizationtostr(const char *in, char *out, size_t outsz) {
    return util_strtransform(in, out, outsz, "_ ", 'a'-'A');
}

static int util_vasprintf(char **dat, const char *fmt, va_list args) {
    int     ret;
    int     len;
    char   *tmp = nullptr;
    char    buf[128];
    va_list cpy;

    va_copy(cpy, args);
    len = vsnprintf(buf, sizeof(buf), fmt, cpy);
    va_end (cpy);

    if (len < 0)
        return len;

    if (len < (int)sizeof(buf)) {
        *dat = util_strdup(buf);
        return len;
    }

    tmp = (char*)mem_a(len + 1);
    if ((ret = vsnprintf(tmp, len + 1, fmt, args)) != len) {
        mem_d(tmp);
        *dat = nullptr;
        return -1;
    }

    *dat = tmp;
    return len;
}

int util_snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list  arg;
    int ret;
    va_start(arg, fmt);
    ret = vsnprintf(str, size, fmt, arg);
    va_end(arg);
    return ret;
}

int util_asprintf(char **ret, const char *fmt, ...) {
    va_list  args;
    int read;
    va_start(args, fmt);
    read = util_vasprintf(ret, fmt, args);
    va_end(args);
    return read;
}

int util_sscanf(const char *str, const char *format, ...) {
    va_list  args;
    int read;
    va_start(args, format);
    read = vsscanf(str, format, args);
    va_end(args);
    return read;
}

char *util_strncpy(char *dest, const char *src, size_t n) {
    return strncpy(dest, src, n);
}

char *util_strncat(char *dest, const char *src, size_t n) {
    return strncat(dest, src, n);
}

char *util_strcat(char *dest, const char *src) {
    return strcat(dest, src);
}

const char *util_strerror(int err) {
    return strerror(err);
}

const struct tm *util_localtime(const time_t *timer) {
    return localtime(timer);
}

const char *util_ctime(const time_t *timer) {
    return ctime(timer);
}

int util_getline(char **lineptr, size_t *n, FILE *stream) {
    int   chr;
    int   ret;
    char *pos;

    if (!lineptr || !n || !stream)
        return -1;
    if (!*lineptr) {
        if (!(*lineptr = (char*)mem_a((*n=64))))
            return -1;
    }

    chr = *n;
    pos = *lineptr;

    for (;;) {
        int c = getc(stream);

        if (chr < 2) {
            *n += (*n > 16) ? *n : 64;
            chr = *n + *lineptr - pos;
            if (!(*lineptr = (char*)mem_r(*lineptr,*n)))
                return -1;
            pos = *n - chr + *lineptr;
        }

        if (ferror(stream))
            return -1;
        if (c == EOF) {
            if (pos == *lineptr)
                return -1;
            else
                break;
        }

        *pos++ = c;
        chr--;
        if (c == '\n')
            break;
    }
    *pos = '\0';
    return (ret = pos - *lineptr);
}

#ifndef _WIN32
#include <unistd.h>
bool util_isatty(FILE *file) {
    if (file == stdout) return !!isatty(STDOUT_FILENO);
    if (file == stderr) return !!isatty(STDERR_FILENO);
    return false;
}
#else
bool util_isatty(FILE *file) {
    return false;
}
#endif

/*
 * A small noncryptographic PRNG based on:
 * http://burtleburtle.net/bob/rand/smallprng.html
 */
static uint32_t util_rand_state[4] = {
    0xF1EA5EED, 0x00000000,
    0x00000000, 0x00000000
};

#define util_rand_rot(X, Y) (((X)<<(Y))|((X)>>(32-(Y))))

uint32_t util_rand() {
    uint32_t last;

    last               = util_rand_state[0] - util_rand_rot(util_rand_state[1], 27);
    util_rand_state[0] = util_rand_state[1] ^ util_rand_rot(util_rand_state[2], 17);
    util_rand_state[1] = util_rand_state[2] + util_rand_state[3];
    util_rand_state[2] = util_rand_state[3] + last;
    util_rand_state[3] = util_rand_state[0] + last;

    return util_rand_state[3];
}

#undef util_rand_rot

void util_seed(uint32_t value) {
    size_t i;

    util_rand_state[0] = 0xF1EA5EED;
    util_rand_state[1] = value;
    util_rand_state[2] = value;
    util_rand_state[3] = value;

    for (i = 0; i < 20; ++i)
        (void)util_rand();
}

size_t hash(const char *string) {
    size_t hash = 0;
    for(; *string; ++string) {
        hash += *string;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

