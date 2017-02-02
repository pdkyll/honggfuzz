/* Based on BoringSSL's client.c fuzzer */

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

static const uint8_t kCertificateDER[] = {
    0x30, 0x82, 0x05, 0x65, 0x30, 0x82, 0x03, 0x4d, 0x02, 0x09, 0x00, 0xe8,
    0x66, 0xed, 0xc9, 0x66, 0xa7, 0xd1, 0xac, 0x30, 0x0d, 0x06, 0x09, 0x2a,
    0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x76,
    0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41,
    0x55, 0x31, 0x0c, 0x30, 0x0a, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x03,
    0x41, 0x55, 0x53, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x07,
    0x0c, 0x06, 0x53, 0x79, 0x64, 0x6e, 0x65, 0x79, 0x31, 0x21, 0x30, 0x1f,
    0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65, 0x72,
    0x6e, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73, 0x20,
    0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x31, 0x25, 0x30, 0x23, 0x06,
    0x03, 0x55, 0x04, 0x03, 0x0c, 0x1c, 0x61, 0x75, 0x73, 0x74, 0x72, 0x61,
    0x6c, 0x69, 0x61, 0x2e, 0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73, 0x74,
    0x65, 0x6e, 0x74, 0x2e, 0x63, 0x61, 0x63, 0x61, 0x63, 0x61, 0x30, 0x1e,
    0x17, 0x0d, 0x31, 0x36, 0x30, 0x39, 0x33, 0x30, 0x31, 0x39, 0x30, 0x33,
    0x33, 0x39, 0x5a, 0x17, 0x0d, 0x31, 0x37, 0x30, 0x39, 0x32, 0x35, 0x31,
    0x39, 0x30, 0x33, 0x33, 0x39, 0x5a, 0x30, 0x73, 0x31, 0x0b, 0x30, 0x09,
    0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31, 0x0c, 0x30,
    0x0a, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x03, 0x41, 0x55, 0x53, 0x31,
    0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x06, 0x53, 0x79,
    0x64, 0x6e, 0x65, 0x79, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04,
    0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20,
    0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73, 0x20, 0x50, 0x74, 0x79, 0x20,
    0x4c, 0x74, 0x64, 0x31, 0x22, 0x30, 0x20, 0x06, 0x03, 0x55, 0x04, 0x03,
    0x0c, 0x19, 0x77, 0x77, 0x77, 0x2e, 0x61, 0x75, 0x73, 0x74, 0x72, 0x61,
    0x6c, 0x69, 0x61, 0x2e, 0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73, 0x74,
    0x65, 0x6e, 0x74, 0x30, 0x82, 0x02, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a,
    0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82,
    0x02, 0x0f, 0x00, 0x30, 0x82, 0x02, 0x0a, 0x02, 0x82, 0x02, 0x01, 0x00,
    0xca, 0x91, 0xe1, 0x6a, 0xf8, 0xd9, 0xcc, 0x06, 0x07, 0x60, 0x16, 0x21,
    0x60, 0xdc, 0xbb, 0xfc, 0x36, 0x3a, 0xf2, 0x93, 0x3c, 0x73, 0xc2, 0x96,
    0xa1, 0xe8, 0x10, 0xbf, 0x11, 0xb8, 0xc9, 0x1c, 0x37, 0x67, 0x9b, 0x85,
    0x00, 0xa1, 0x6f, 0x66, 0x2f, 0xf5, 0x04, 0x6e, 0x1c, 0x22, 0x83, 0x7a,
    0x6e, 0xab, 0x28, 0x43, 0xfe, 0x32, 0xbd, 0xed, 0xa0, 0x7a, 0x4a, 0x84,
    0x14, 0x85, 0x79, 0x8a, 0x1e, 0xf4, 0x4f, 0x12, 0x6c, 0xe2, 0xfd, 0xde,
    0x47, 0x29, 0xe5, 0xe2, 0xad, 0x0d, 0x87, 0x45, 0xcf, 0x07, 0xdb, 0x8c,
    0x40, 0xa1, 0xee, 0x3f, 0x77, 0xcb, 0xee, 0xcc, 0xe2, 0x17, 0x79, 0xcc,
    0x4b, 0xd5, 0x83, 0x9d, 0xe1, 0x48, 0xce, 0x33, 0xf7, 0xd4, 0x68, 0x46,
    0x14, 0x8c, 0x97, 0x82, 0xde, 0xde, 0x9d, 0x76, 0x88, 0x73, 0xf4, 0xf9,
    0xcf, 0x26, 0x32, 0x75, 0x19, 0x9b, 0x70, 0x81, 0xe5, 0x19, 0xb0, 0x8c,
    0x90, 0x8b, 0x5e, 0x1a, 0xbd, 0xc7, 0x95, 0xfb, 0xf2, 0xea, 0x77, 0x6a,
    0x7a, 0xce, 0x67, 0x51, 0xc6, 0x24, 0x99, 0x95, 0x23, 0xc2, 0x04, 0xb8,
    0x44, 0xd8, 0x70, 0xf3, 0xaf, 0x78, 0xc4, 0x71, 0xb1, 0x76, 0x11, 0x0c,
    0xe7, 0x50, 0x16, 0xc8, 0x33, 0x64, 0xcd, 0xdd, 0x0b, 0xaf, 0xb8, 0x2d,
    0x4b, 0x21, 0xb1, 0x64, 0xa1, 0x22, 0x1b, 0xad, 0x8f, 0x42, 0x97, 0xcc,
    0x64, 0x30, 0xb3, 0x3f, 0xb4, 0xfc, 0x6e, 0x15, 0xa4, 0xf3, 0x96, 0x1f,
    0xd8, 0xe4, 0xac, 0xa1, 0xd5, 0xdb, 0x9e, 0x28, 0xb3, 0xac, 0x17, 0x54,
    0x2a, 0x53, 0xeb, 0x00, 0x62, 0x48, 0xaa, 0x74, 0x7e, 0x84, 0x5a, 0x50,
    0xcc, 0x15, 0x48, 0x96, 0x19, 0xea, 0xc7, 0xdc, 0x2b, 0x3d, 0x5f, 0xff,
    0x03, 0x6a, 0x82, 0x43, 0xf3, 0x9b, 0x2e, 0xf3, 0x43, 0x50, 0x83, 0xcd,
    0xb3, 0x98, 0x50, 0x51, 0x9d, 0x4a, 0x1e, 0x7c, 0x58, 0xfc, 0xe5, 0x7b,
    0xb6, 0xe0, 0x8e, 0xce, 0xe3, 0xcf, 0x53, 0x50, 0x12, 0x85, 0x51, 0x49,
    0x9a, 0x28, 0x59, 0x6b, 0x68, 0xef, 0x94, 0x20, 0xc8, 0x72, 0x5a, 0x7f,
    0x7d, 0xaa, 0x8b, 0x34, 0xf5, 0xc4, 0xc6, 0xed, 0xc2, 0x1c, 0xa9, 0xfa,
    0xcf, 0x51, 0x2e, 0xa2, 0x2b, 0xf0, 0xfd, 0x4d, 0x49, 0xee, 0xb3, 0xa4,
    0xee, 0xd4, 0x7b, 0xec, 0xa2, 0xec, 0xd4, 0xf8, 0xbf, 0x16, 0x79, 0x61,
    0x0c, 0x1c, 0x39, 0xd3, 0x75, 0x1f, 0x2d, 0x16, 0xcd, 0x7c, 0x4a, 0xe5,
    0x01, 0x90, 0x91, 0x26, 0x92, 0xd9, 0x4e, 0x89, 0xc4, 0x58, 0xd8, 0x08,
    0x19, 0xa5, 0x8d, 0x85, 0x29, 0xfd, 0xae, 0xb2, 0xc9, 0x72, 0xca, 0x48,
    0x9b, 0xbe, 0x14, 0x03, 0x03, 0xf1, 0x72, 0xb4, 0x6d, 0x19, 0x87, 0xe9,
    0xc9, 0xc0, 0x9e, 0x95, 0xac, 0x79, 0xcf, 0x24, 0xee, 0xc4, 0x4f, 0x84,
    0x25, 0xc7, 0xc0, 0xae, 0x30, 0x04, 0x3c, 0xa9, 0x52, 0x75, 0x6f, 0x97,
    0x79, 0x98, 0xc0, 0xd7, 0xeb, 0x84, 0x9a, 0x57, 0x99, 0x3c, 0x01, 0x8b,
    0x3b, 0xc0, 0x91, 0xe3, 0xfd, 0xe1, 0x88, 0x88, 0xa0, 0x34, 0xd8, 0x5d,
    0xe8, 0xe6, 0xba, 0xab, 0x76, 0x6f, 0x33, 0x3e, 0x97, 0x04, 0x93, 0xd3,
    0x4c, 0x22, 0xb1, 0xa9, 0x79, 0x66, 0xc5, 0xb6, 0xbb, 0x23, 0x50, 0xbe,
    0x54, 0x1a, 0xab, 0x7a, 0x99, 0x9e, 0x2b, 0x4f, 0xed, 0x5a, 0x7a, 0x5c,
    0xbd, 0xf2, 0x32, 0x34, 0x02, 0x82, 0x4c, 0xb8, 0x11, 0x65, 0x22, 0x77,
    0x42, 0x7f, 0xb1, 0x7b, 0x98, 0x1b, 0x82, 0x8f, 0xe7, 0x1f, 0x7b, 0x07,
    0x21, 0x98, 0x60, 0xb4, 0x7b, 0xe2, 0x9a, 0xcb, 0xe9, 0xb3, 0x49, 0x35,
    0x86, 0xea, 0x8b, 0x21, 0x46, 0xe9, 0x5b, 0x16, 0xe8, 0x2c, 0xe5, 0x9c,
    0xb5, 0x96, 0x45, 0xce, 0x0c, 0x40, 0xbb, 0x97, 0x02, 0x03, 0x01, 0x00,
    0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
    0x01, 0x0b, 0x05, 0x00, 0x03, 0x82, 0x02, 0x01, 0x00, 0x56, 0x8f, 0xf2,
    0xb7, 0x14, 0x35, 0x20, 0xf5, 0x42, 0x8b, 0x51, 0xd4, 0x65, 0x10, 0xf3,
    0x8a, 0x38, 0x5d, 0x3b, 0xd0, 0x48, 0xb9, 0x8e, 0x5a, 0x87, 0x29, 0x67,
    0xd7, 0x7f, 0x48, 0x29, 0x97, 0x7b, 0xad, 0xba, 0x09, 0xe1, 0xf8, 0x19,
    0xde, 0x3f, 0x33, 0x04, 0x35, 0x37, 0x8d, 0x6d, 0xb8, 0x7f, 0x29, 0x2b,
    0xd4, 0xfe, 0x1f, 0x4e, 0xa7, 0x9d, 0xcf, 0x4d, 0x4a, 0x9e, 0x2d, 0x01,
    0x8e, 0x2d, 0x0c, 0xa3, 0x68, 0x37, 0xfc, 0x47, 0xd8, 0xb6, 0x6d, 0x37,
    0xf1, 0xf8, 0xb0, 0x71, 0xc1, 0xb5, 0x29, 0x7e, 0x88, 0x96, 0xa7, 0x93,
    0xae, 0x4c, 0x5d, 0x89, 0x10, 0x5e, 0x6b, 0x04, 0xcf, 0x7f, 0x6d, 0x2e,
    0xad, 0x2f, 0xa7, 0x0f, 0xfc, 0x5d, 0x48, 0x5a, 0xd3, 0x70, 0xbc, 0x86,
    0xeb, 0x75, 0xbe, 0xd1, 0x9f, 0x5d, 0x62, 0xb7, 0xcb, 0x7b, 0x1e, 0x9b,
    0x06, 0x7a, 0xe7, 0xe5, 0x05, 0x68, 0x52, 0x7e, 0x15, 0x27, 0xbf, 0x4c,
    0x65, 0x45, 0xce, 0x18, 0xa9, 0x49, 0xa6, 0x12, 0xbf, 0x8c, 0x33, 0x01,
    0xdc, 0xc6, 0x5c, 0x3e, 0x50, 0x77, 0xaf, 0x8e, 0x97, 0xfd, 0xcc, 0x51,
    0x36, 0x82, 0x46, 0xd8, 0x7e, 0xe6, 0xa8, 0x5a, 0x91, 0x26, 0x12, 0x05,
    0x0b, 0x50, 0xf1, 0x8f, 0x21, 0x3a, 0x85, 0x0e, 0x5f, 0x09, 0xf1, 0x6c,
    0xea, 0xb4, 0x6b, 0x33, 0xbd, 0xb1, 0x24, 0xed, 0x53, 0xbb, 0x3f, 0x88,
    0xd3, 0xfb, 0xba, 0x4a, 0x87, 0x0c, 0xbf, 0x83, 0x92, 0xb3, 0x5c, 0x7d,
    0x85, 0x2e, 0x10, 0x65, 0xab, 0xa7, 0x3d, 0x40, 0xdd, 0x8e, 0xe3, 0xba,
    0xbb, 0x1e, 0x58, 0xe9, 0x1c, 0x76, 0x1d, 0xe9, 0x81, 0xf4, 0x9d, 0x77,
    0x2f, 0x44, 0x00, 0x4d, 0xb9, 0x2c, 0x31, 0x5b, 0x42, 0x6f, 0x10, 0x35,
    0xed, 0x08, 0xc4, 0x51, 0xb3, 0x01, 0x9e, 0xce, 0x27, 0x33, 0xd0, 0x44,
    0x98, 0x95, 0x3c, 0xe2, 0x9d, 0xdb, 0x58, 0xff, 0xc7, 0xc7, 0xea, 0x9b,
    0x3f, 0xe2, 0xa2, 0x37, 0x5e, 0x8d, 0x94, 0xa2, 0xb0, 0x55, 0x68, 0x89,
    0x10, 0xd7, 0xb9, 0x3c, 0xc0, 0xec, 0xe4, 0x36, 0x46, 0x3c, 0x47, 0xb9,
    0x8d, 0x74, 0x17, 0xad, 0xca, 0x07, 0x67, 0x27, 0xea, 0xba, 0xd0, 0xa7,
    0x2c, 0x06, 0x0f, 0x2d, 0xa3, 0x41, 0xf8, 0x28, 0x3f, 0x88, 0x38, 0x25,
    0x61, 0xf8, 0x87, 0x92, 0x63, 0xd8, 0x5c, 0x2b, 0xff, 0x3c, 0xa1, 0xff,
    0xb0, 0x98, 0x21, 0xcd, 0x25, 0x7e, 0x5f, 0xa5, 0x05, 0xf5, 0x90, 0x0d,
    0xf7, 0xeb, 0x1a, 0xff, 0xe2, 0xeb, 0x1b, 0x9a, 0x1f, 0x49, 0xdb, 0x65,
    0xc4, 0x1f, 0x32, 0x41, 0xea, 0x79, 0xb7, 0x85, 0x73, 0x01, 0x39, 0x67,
    0x85, 0x49, 0x57, 0xe0, 0x21, 0xf8, 0x61, 0x26, 0x30, 0x89, 0xd1, 0x03,
    0xef, 0x1d, 0xb9, 0x3e, 0x71, 0x64, 0x31, 0xb3, 0xe9, 0x70, 0x91, 0x78,
    0x46, 0x0a, 0x9a, 0x33, 0xe5, 0x21, 0x42, 0x87, 0x26, 0xef, 0xaf, 0xe5,
    0xc7, 0x6a, 0x2f, 0x3c, 0x66, 0xe6, 0xd8, 0x68, 0xc8, 0x98, 0xd8, 0xae,
    0x68, 0x47, 0x52, 0x23, 0xc1, 0x9f, 0x35, 0xb8, 0xc4, 0xf4, 0x42, 0xa7,
    0xd4, 0xf3, 0x3f, 0xb0, 0xcd, 0x04, 0x7d, 0x40, 0x08, 0x22, 0x12, 0xe9,
    0xe1, 0xbd, 0x4d, 0x02, 0x23, 0x4b, 0x9f, 0x98, 0x08, 0x74, 0x1e, 0xdd,
    0x8d, 0xd4, 0xa2, 0x87, 0x29, 0xd3, 0xb4, 0xe6, 0x65, 0x4c, 0xda, 0x5f,
    0x6d, 0x5a, 0xb3, 0x6f, 0xaa, 0xcf, 0xe5, 0xd6, 0x50, 0x8d, 0xca, 0x60,
    0x07, 0xf1, 0x70, 0xa8, 0x8d, 0x5c, 0x8b, 0x18, 0xc4, 0x0b, 0xfa, 0x76,
    0xb3, 0xca, 0xb7, 0xc2, 0xfe, 0x08, 0x51, 0x1e, 0x07, 0xa3, 0xf4, 0x1c,
    0x4f, 0x48, 0x77, 0xb5, 0x3e, 0x23, 0x4b, 0xbd, 0xf2, 0xa8, 0x2f, 0xc3,
    0xd7, 0xf3, 0x61, 0x1b, 0x9c
};

static const uint8_t kRSAPrivateKeyDER[] = {
    0x30, 0x82, 0x09, 0x2a, 0x02, 0x01, 0x00, 0x02, 0x82, 0x02, 0x01, 0x00,
    0xca, 0x91, 0xe1, 0x6a, 0xf8, 0xd9, 0xcc, 0x06, 0x07, 0x60, 0x16, 0x21,
    0x60, 0xdc, 0xbb, 0xfc, 0x36, 0x3a, 0xf2, 0x93, 0x3c, 0x73, 0xc2, 0x96,
    0xa1, 0xe8, 0x10, 0xbf, 0x11, 0xb8, 0xc9, 0x1c, 0x37, 0x67, 0x9b, 0x85,
    0x00, 0xa1, 0x6f, 0x66, 0x2f, 0xf5, 0x04, 0x6e, 0x1c, 0x22, 0x83, 0x7a,
    0x6e, 0xab, 0x28, 0x43, 0xfe, 0x32, 0xbd, 0xed, 0xa0, 0x7a, 0x4a, 0x84,
    0x14, 0x85, 0x79, 0x8a, 0x1e, 0xf4, 0x4f, 0x12, 0x6c, 0xe2, 0xfd, 0xde,
    0x47, 0x29, 0xe5, 0xe2, 0xad, 0x0d, 0x87, 0x45, 0xcf, 0x07, 0xdb, 0x8c,
    0x40, 0xa1, 0xee, 0x3f, 0x77, 0xcb, 0xee, 0xcc, 0xe2, 0x17, 0x79, 0xcc,
    0x4b, 0xd5, 0x83, 0x9d, 0xe1, 0x48, 0xce, 0x33, 0xf7, 0xd4, 0x68, 0x46,
    0x14, 0x8c, 0x97, 0x82, 0xde, 0xde, 0x9d, 0x76, 0x88, 0x73, 0xf4, 0xf9,
    0xcf, 0x26, 0x32, 0x75, 0x19, 0x9b, 0x70, 0x81, 0xe5, 0x19, 0xb0, 0x8c,
    0x90, 0x8b, 0x5e, 0x1a, 0xbd, 0xc7, 0x95, 0xfb, 0xf2, 0xea, 0x77, 0x6a,
    0x7a, 0xce, 0x67, 0x51, 0xc6, 0x24, 0x99, 0x95, 0x23, 0xc2, 0x04, 0xb8,
    0x44, 0xd8, 0x70, 0xf3, 0xaf, 0x78, 0xc4, 0x71, 0xb1, 0x76, 0x11, 0x0c,
    0xe7, 0x50, 0x16, 0xc8, 0x33, 0x64, 0xcd, 0xdd, 0x0b, 0xaf, 0xb8, 0x2d,
    0x4b, 0x21, 0xb1, 0x64, 0xa1, 0x22, 0x1b, 0xad, 0x8f, 0x42, 0x97, 0xcc,
    0x64, 0x30, 0xb3, 0x3f, 0xb4, 0xfc, 0x6e, 0x15, 0xa4, 0xf3, 0x96, 0x1f,
    0xd8, 0xe4, 0xac, 0xa1, 0xd5, 0xdb, 0x9e, 0x28, 0xb3, 0xac, 0x17, 0x54,
    0x2a, 0x53, 0xeb, 0x00, 0x62, 0x48, 0xaa, 0x74, 0x7e, 0x84, 0x5a, 0x50,
    0xcc, 0x15, 0x48, 0x96, 0x19, 0xea, 0xc7, 0xdc, 0x2b, 0x3d, 0x5f, 0xff,
    0x03, 0x6a, 0x82, 0x43, 0xf3, 0x9b, 0x2e, 0xf3, 0x43, 0x50, 0x83, 0xcd,
    0xb3, 0x98, 0x50, 0x51, 0x9d, 0x4a, 0x1e, 0x7c, 0x58, 0xfc, 0xe5, 0x7b,
    0xb6, 0xe0, 0x8e, 0xce, 0xe3, 0xcf, 0x53, 0x50, 0x12, 0x85, 0x51, 0x49,
    0x9a, 0x28, 0x59, 0x6b, 0x68, 0xef, 0x94, 0x20, 0xc8, 0x72, 0x5a, 0x7f,
    0x7d, 0xaa, 0x8b, 0x34, 0xf5, 0xc4, 0xc6, 0xed, 0xc2, 0x1c, 0xa9, 0xfa,
    0xcf, 0x51, 0x2e, 0xa2, 0x2b, 0xf0, 0xfd, 0x4d, 0x49, 0xee, 0xb3, 0xa4,
    0xee, 0xd4, 0x7b, 0xec, 0xa2, 0xec, 0xd4, 0xf8, 0xbf, 0x16, 0x79, 0x61,
    0x0c, 0x1c, 0x39, 0xd3, 0x75, 0x1f, 0x2d, 0x16, 0xcd, 0x7c, 0x4a, 0xe5,
    0x01, 0x90, 0x91, 0x26, 0x92, 0xd9, 0x4e, 0x89, 0xc4, 0x58, 0xd8, 0x08,
    0x19, 0xa5, 0x8d, 0x85, 0x29, 0xfd, 0xae, 0xb2, 0xc9, 0x72, 0xca, 0x48,
    0x9b, 0xbe, 0x14, 0x03, 0x03, 0xf1, 0x72, 0xb4, 0x6d, 0x19, 0x87, 0xe9,
    0xc9, 0xc0, 0x9e, 0x95, 0xac, 0x79, 0xcf, 0x24, 0xee, 0xc4, 0x4f, 0x84,
    0x25, 0xc7, 0xc0, 0xae, 0x30, 0x04, 0x3c, 0xa9, 0x52, 0x75, 0x6f, 0x97,
    0x79, 0x98, 0xc0, 0xd7, 0xeb, 0x84, 0x9a, 0x57, 0x99, 0x3c, 0x01, 0x8b,
    0x3b, 0xc0, 0x91, 0xe3, 0xfd, 0xe1, 0x88, 0x88, 0xa0, 0x34, 0xd8, 0x5d,
    0xe8, 0xe6, 0xba, 0xab, 0x76, 0x6f, 0x33, 0x3e, 0x97, 0x04, 0x93, 0xd3,
    0x4c, 0x22, 0xb1, 0xa9, 0x79, 0x66, 0xc5, 0xb6, 0xbb, 0x23, 0x50, 0xbe,
    0x54, 0x1a, 0xab, 0x7a, 0x99, 0x9e, 0x2b, 0x4f, 0xed, 0x5a, 0x7a, 0x5c,
    0xbd, 0xf2, 0x32, 0x34, 0x02, 0x82, 0x4c, 0xb8, 0x11, 0x65, 0x22, 0x77,
    0x42, 0x7f, 0xb1, 0x7b, 0x98, 0x1b, 0x82, 0x8f, 0xe7, 0x1f, 0x7b, 0x07,
    0x21, 0x98, 0x60, 0xb4, 0x7b, 0xe2, 0x9a, 0xcb, 0xe9, 0xb3, 0x49, 0x35,
    0x86, 0xea, 0x8b, 0x21, 0x46, 0xe9, 0x5b, 0x16, 0xe8, 0x2c, 0xe5, 0x9c,
    0xb5, 0x96, 0x45, 0xce, 0x0c, 0x40, 0xbb, 0x97, 0x02, 0x03, 0x01, 0x00,
    0x01, 0x02, 0x82, 0x02, 0x01, 0x00, 0x97, 0x36, 0x08, 0x37, 0xca, 0xe5,
    0x01, 0x98, 0x6a, 0x7e, 0xfe, 0x66, 0x12, 0x21, 0x13, 0xae, 0x74, 0x68,
    0xd2, 0x54, 0xb8, 0x26, 0x8d, 0x55, 0xd6, 0x2c, 0x1d, 0xfc, 0x12, 0xe5,
    0x86, 0x32, 0x2a, 0xe5, 0x8b, 0x40, 0xe0, 0x34, 0xa1, 0xac, 0x7d, 0x16,
    0x00, 0x25, 0x95, 0x98, 0xe9, 0xde, 0x45, 0xa8, 0x3c, 0x19, 0x6d, 0x32,
    0x41, 0x76, 0x95, 0x79, 0x54, 0x10, 0x7e, 0x25, 0x18, 0x91, 0xd3, 0x03,
    0x79, 0xd6, 0xfe, 0x32, 0xff, 0x60, 0xa2, 0x6c, 0x93, 0x2e, 0xff, 0x10,
    0xff, 0x2e, 0x4c, 0x19, 0xc8, 0x78, 0x4c, 0x72, 0xd4, 0x1e, 0xca, 0x75,
    0x0f, 0xa0, 0x1f, 0x11, 0x79, 0x18, 0xd8, 0x6e, 0xdc, 0x9d, 0xc8, 0xe2,
    0x89, 0x12, 0x5f, 0xe8, 0x4d, 0xa2, 0x2a, 0x8a, 0xbc, 0x3a, 0xb3, 0xd5,
    0x27, 0x63, 0xa8, 0xfe, 0x2e, 0x1b, 0x1f, 0xd7, 0x9a, 0x88, 0xb1, 0x01,
    0xc8, 0x1d, 0x1e, 0x80, 0x6b, 0xba, 0xb1, 0xd7, 0x66, 0xfa, 0xbd, 0x39,
    0xb4, 0x2a, 0xeb, 0xac, 0xbc, 0x51, 0x5e, 0xb4, 0x9e, 0x89, 0x7b, 0x48,
    0x2c, 0xe1, 0x18, 0x5d, 0x27, 0x1a, 0xca, 0x41, 0x6b, 0x27, 0x6c, 0x8f,
    0xd0, 0xcc, 0x31, 0xb5, 0x39, 0x8b, 0x11, 0x4c, 0x46, 0x85, 0x51, 0x5f,
    0xe9, 0x10, 0x1d, 0x3e, 0x21, 0x54, 0xa6, 0x25, 0xd3, 0x7e, 0x93, 0x8d,
    0x9b, 0x6d, 0x96, 0x68, 0x16, 0x5a, 0x3b, 0x5b, 0xe3, 0x22, 0x26, 0x1e,
    0xdc, 0xaa, 0x09, 0x84, 0xcf, 0x46, 0x9c, 0xa8, 0x2e, 0x79, 0x6b, 0xe3,
    0x51, 0x8f, 0x7e, 0x77, 0x72, 0xbe, 0xa0, 0x0e, 0x1e, 0x79, 0x1d, 0xe4,
    0xb2, 0x60, 0xdd, 0x64, 0x3c, 0xea, 0xdd, 0x3c, 0xf1, 0xff, 0x21, 0xff,
    0x91, 0x0b, 0x37, 0xb7, 0xcb, 0xe5, 0xd7, 0x8d, 0xf5, 0x09, 0x76, 0x5a,
    0x1b, 0xd4, 0xf9, 0x25, 0xac, 0x0d, 0xac, 0x07, 0x81, 0xc3, 0xe6, 0x6c,
    0x96, 0x4b, 0x6a, 0x8a, 0xf1, 0x31, 0xd6, 0x22, 0x0e, 0xf9, 0xb0, 0x5e,
    0xb6, 0x69, 0x4d, 0xe9, 0xf0, 0xf2, 0xee, 0xe1, 0x13, 0x14, 0xa7, 0xff,
    0x43, 0x53, 0x86, 0x15, 0x14, 0x5c, 0x89, 0x41, 0xb3, 0x1e, 0xaa, 0x6f,
    0x96, 0x56, 0x9c, 0xe8, 0x59, 0xa3, 0x51, 0xad, 0x30, 0x59, 0xce, 0x47,
    0xde, 0xdc, 0x6d, 0xd7, 0x0d, 0x9a, 0x29, 0xad, 0xd0, 0x61, 0xf5, 0xbc,
    0x68, 0xb0, 0xa9, 0xc5, 0x23, 0x0d, 0xdf, 0x5e, 0xb4, 0x19, 0xd4, 0xc8,
    0x84, 0x70, 0x05, 0x5b, 0x7c, 0x9b, 0xd5, 0x93, 0x39, 0x8a, 0xbe, 0xe8,
    0x8f, 0x42, 0x0e, 0xdc, 0xdf, 0xb0, 0xb6, 0xdb, 0xc7, 0x85, 0xb0, 0xe3,
    0x0b, 0x34, 0x9f, 0xfc, 0xf9, 0x8e, 0x53, 0x0e, 0x64, 0x1b, 0xb4, 0x41,
    0xa6, 0xe0, 0x06, 0x64, 0x90, 0x82, 0x5d, 0x1c, 0x5a, 0x04, 0x14, 0xd8,
    0x7e, 0x9c, 0x7f, 0x9d, 0x35, 0xc1, 0xeb, 0xb6, 0xf2, 0xfe, 0x96, 0x34,
    0x75, 0xcd, 0xea, 0x39, 0xd3, 0xb7, 0x04, 0x12, 0x64, 0xd9, 0xc2, 0x1f,
    0x17, 0x62, 0x37, 0x87, 0x08, 0x6b, 0xdc, 0x0b, 0xe0, 0x0b, 0x30, 0x55,
    0x37, 0x14, 0x10, 0x3b, 0x60, 0x62, 0x53, 0xd5, 0x47, 0xd0, 0xc4, 0xab,
    0xfc, 0xdc, 0xc2, 0x73, 0xff, 0x61, 0xa9, 0x75, 0xf4, 0x99, 0xa3, 0xcf,
    0xae, 0xc8, 0xd7, 0xe0, 0x20, 0x8a, 0xc9, 0xda, 0xfe, 0x66, 0x00, 0x9b,
    0x1d, 0x01, 0x54, 0x18, 0xbb, 0x6b, 0x41, 0xdb, 0x07, 0x84, 0x75, 0x9f,
    0x33, 0x94, 0x85, 0xf5, 0x4c, 0xa9, 0xdf, 0x6d, 0x9e, 0xde, 0xe7, 0x7a,
    0x45, 0x08, 0x1f, 0xc5, 0x22, 0x9d, 0x8d, 0x62, 0xef, 0x94, 0x46, 0x25,
    0x5f, 0x40, 0x8f, 0x6f, 0x24, 0x65, 0x57, 0xfa, 0x0c, 0x1d, 0x1b, 0xa9,
    0xd1, 0x93, 0x00, 0x40, 0xc3, 0xed, 0xd5, 0x7f, 0x8c, 0x98, 0xb2, 0xd4,
    0xf3, 0x19, 0x02, 0x82, 0x01, 0x01, 0x00, 0xea, 0xae, 0x2b, 0x8b, 0x8f,
    0x87, 0x2b, 0xe4, 0x10, 0x20, 0x50, 0xad, 0x1b, 0x81, 0xdc, 0x4a, 0xfb,
    0x38, 0xdb, 0x8e, 0x87, 0x82, 0x21, 0x7e, 0x18, 0xb9, 0xc8, 0xee, 0x04,
    0x7c, 0xdb, 0xad, 0x3d, 0xa4, 0xb1, 0x27, 0xc5, 0xd4, 0xa6, 0xbe, 0x14,
    0x8c, 0x55, 0x7d, 0x39, 0x03, 0x6c, 0xe9, 0xb0, 0x25, 0xc4, 0x2a, 0xc0,
    0xab, 0x98, 0xe9, 0xf8, 0x6c, 0x6b, 0x75, 0x51, 0xc2, 0xcc, 0x44, 0x53,
    0x92, 0x7a, 0x17, 0xda, 0xad, 0x6d, 0x5a, 0x55, 0x7c, 0xfd, 0x2d, 0x85,
    0xa2, 0xa4, 0xb0, 0xc0, 0x4d, 0xf0, 0xde, 0xca, 0xe1, 0xe1, 0xd4, 0x76,
    0xd6, 0xfe, 0xb0, 0xb6, 0xac, 0x36, 0x6a, 0xf3, 0x0c, 0xe0, 0xa0, 0x9c,
    0xbd, 0xd7, 0x46, 0x82, 0xe7, 0x32, 0x73, 0x58, 0x34, 0x75, 0xeb, 0x83,
    0xc9, 0x3d, 0x8e, 0xf7, 0x76, 0xe1, 0xa9, 0x0e, 0xb9, 0x07, 0x87, 0x0c,
    0x37, 0xeb, 0x22, 0xe3, 0x55, 0x7b, 0xf9, 0x24, 0xf6, 0x0e, 0xe7, 0x4f,
    0x88, 0x83, 0x12, 0xf1, 0xec, 0xa9, 0xf7, 0xf7, 0xf5, 0xf4, 0x53, 0x71,
    0xa2, 0x9a, 0x2d, 0xc4, 0x5b, 0x52, 0x28, 0x47, 0xdf, 0x19, 0x1e, 0x47,
    0x7c, 0x3c, 0x82, 0x37, 0x46, 0xeb, 0xce, 0x8c, 0x3c, 0x41, 0x08, 0x76,
    0x16, 0x46, 0x5b, 0x23, 0x6c, 0x77, 0xd4, 0x6a, 0xae, 0xd6, 0x8f, 0x34,
    0x6a, 0x9f, 0x17, 0xa2, 0xb5, 0xeb, 0x5e, 0x4c, 0x7c, 0x5e, 0xe1, 0x73,
    0xb5, 0x75, 0x39, 0xa9, 0x0b, 0x3f, 0xf6, 0xdc, 0xb3, 0x3d, 0x2e, 0x70,
    0x4b, 0x32, 0x19, 0x38, 0x48, 0xe7, 0x57, 0x47, 0xee, 0xee, 0x99, 0xde,
    0x33, 0x0c, 0xc1, 0xe6, 0xcb, 0x45, 0x51, 0xb3, 0xbe, 0x69, 0x35, 0xe6,
    0xc0, 0x11, 0x03, 0x74, 0xd3, 0x24, 0x76, 0x14, 0x82, 0x31, 0x3e, 0xe3,
    0x86, 0x52, 0x13, 0x26, 0xe0, 0x0e, 0x46, 0x5a, 0x84, 0x98, 0xf3, 0x02,
    0x82, 0x01, 0x01, 0x00, 0xdc, 0xf8, 0xef, 0x30, 0x44, 0xb8, 0x35, 0xee,
    0xe0, 0x97, 0x4e, 0x11, 0x4a, 0xab, 0x1b, 0xfa, 0x33, 0x18, 0x3d, 0xc1,
    0x3a, 0x48, 0x8f, 0x4c, 0x1f, 0x81, 0xf9, 0x16, 0x42, 0x73, 0x8b, 0x37,
    0x01, 0xc1, 0x49, 0xb9, 0xb9, 0x72, 0x90, 0x16, 0x0a, 0x64, 0x0f, 0xd6,
    0x18, 0x5e, 0xe6, 0x4f, 0x2d, 0x96, 0xe5, 0xfe, 0x1b, 0x29, 0xff, 0xef,
    0xc5, 0x91, 0x56, 0x09, 0xfd, 0x78, 0xd9, 0xa1, 0x04, 0xe4, 0xcb, 0xfb,
    0x74, 0xac, 0x43, 0x20, 0x7f, 0xd4, 0x1f, 0xb2, 0x63, 0x6a, 0x22, 0xc0,
    0x56, 0xe3, 0x3c, 0x05, 0xdd, 0x64, 0xcc, 0x13, 0x37, 0xab, 0xaa, 0xea,
    0xbf, 0x89, 0x61, 0x39, 0xc4, 0x5b, 0x4c, 0xbd, 0xae, 0xcb, 0x53, 0xdf,
    0x92, 0x7c, 0x48, 0x57, 0x33, 0x4f, 0x9f, 0xb2, 0xae, 0x35, 0xba, 0x87,
    0x35, 0x88, 0x2f, 0xe3, 0xb0, 0x87, 0xef, 0xf3, 0xc1, 0x8f, 0xc2, 0x9a,
    0x72, 0x9f, 0x11, 0xa9, 0x95, 0x00, 0x14, 0xc8, 0xcc, 0x62, 0x50, 0x7f,
    0xb2, 0x23, 0xf8, 0x8b, 0x41, 0xb2, 0x31, 0x70, 0xd2, 0x75, 0x4d, 0x54,
    0xf4, 0x88, 0x28, 0x47, 0x68, 0x3f, 0x01, 0x12, 0x8d, 0x58, 0xab, 0x9c,
    0x5f, 0xd1, 0xe8, 0xf1, 0x8f, 0xb9, 0x15, 0x8c, 0xfb, 0xac, 0x77, 0xd0,
    0xe5, 0x03, 0xf4, 0x9a, 0x4e, 0x26, 0x7c, 0xb3, 0xca, 0x99, 0x10, 0x5f,
    0x6f, 0x21, 0x51, 0x4e, 0xff, 0x74, 0x30, 0x13, 0x1a, 0x3e, 0x28, 0xb1,
    0xe0, 0x55, 0xfc, 0xab, 0xb6, 0xa5, 0xc5, 0x97, 0xb2, 0xa4, 0x9d, 0x0c,
    0x3b, 0xaa, 0xf6, 0xd9, 0x66, 0xea, 0x5f, 0xa1, 0x37, 0x23, 0xb0, 0x3d,
    0x03, 0x94, 0xaf, 0x8c, 0xc3, 0x03, 0xd4, 0xc9, 0x1e, 0x30, 0x63, 0x59,
    0x57, 0x82, 0x65, 0xf2, 0x1e, 0xb2, 0x37, 0xd3, 0xbf, 0xbe, 0xa6, 0xed,
    0x6b, 0x9f, 0x7d, 0x64, 0xc0, 0x74, 0xfb, 0xcd, 0x02, 0x82, 0x01, 0x01,
    0x00, 0x98, 0x99, 0x6e, 0x99, 0x4e, 0x58, 0x0f, 0xf3, 0x39, 0x85, 0x39,
    0xd0, 0x86, 0x7d, 0x77, 0xb8, 0x8e, 0x09, 0x17, 0xc3, 0x63, 0x5b, 0xfb,
    0xd9, 0x59, 0xcc, 0x9c, 0xda, 0x20, 0xb2, 0xeb, 0xc9, 0x87, 0xb6, 0xea,
    0xb7, 0x39, 0x0d, 0xa0, 0xeb, 0x22, 0xc3, 0x69, 0xe7, 0x86, 0x46, 0x32,
    0xf0, 0xf5, 0xe8, 0x68, 0xd9, 0x7f, 0xf5, 0x54, 0xf6, 0x76, 0xe2, 0x51,
    0x31, 0xb5, 0x5b, 0x9c, 0xa5, 0xa5, 0x4e, 0x2e, 0xf4, 0x09, 0xef, 0x11,
    0x97, 0x56, 0xd5, 0x72, 0x6f, 0xc2, 0x60, 0xd3, 0x04, 0x57, 0xd7, 0x96,
    0x93, 0xd8, 0x8a, 0xee, 0xe4, 0xcf, 0xed, 0xd7, 0x29, 0x23, 0x6f, 0x71,
    0xe7, 0x33, 0x6a, 0x21, 0x3a, 0x6f, 0x11, 0x86, 0xc5, 0x43, 0xe3, 0x80,
    0x4b, 0xbe, 0x84, 0x46, 0x55, 0x41, 0x99, 0x7e, 0xdc, 0xd3, 0x0f, 0x4b,
    0x87, 0x39, 0x9a, 0x99, 0x49, 0x78, 0x69, 0x78, 0x0d, 0x74, 0x93, 0xa6,
    0x8c, 0x88, 0x3b, 0x33, 0xcf, 0xb7, 0x48, 0xc6, 0x2a, 0x70, 0x83, 0x7a,
    0xb6, 0x52, 0x57, 0x6d, 0x6b, 0x41, 0x0e, 0x01, 0x81, 0x57, 0x18, 0x26,
    0xa1, 0x28, 0xb2, 0xea, 0x4b, 0x65, 0x22, 0x64, 0xda, 0x2b, 0x85, 0x83,
    0x5a, 0x08, 0x98, 0x39, 0x95, 0x7d, 0xeb, 0xd8, 0x0d, 0xf5, 0x47, 0xd7,
    0xd7, 0x99, 0x13, 0x5d, 0x53, 0x3b, 0x3b, 0x45, 0x7e, 0x02, 0x00, 0x97,
    0x2e, 0xf7, 0x3f, 0x3c, 0x17, 0x0f, 0xbd, 0x63, 0x9f, 0x7d, 0xcb, 0x61,
    0xe9, 0x6c, 0xf3, 0x64, 0x0a, 0x29, 0x5c, 0xcc, 0x13, 0xd8, 0x24, 0x97,
    0xc1, 0x8a, 0x75, 0xd4, 0x52, 0xdb, 0x48, 0x88, 0xb8, 0x21, 0x11, 0xf6,
    0x5e, 0x3d, 0x29, 0xc8, 0x92, 0x13, 0x1c, 0xbb, 0x33, 0x6b, 0x28, 0xcc,
    0xa1, 0xb7, 0x8e, 0x3c, 0xe5, 0x6e, 0xdf, 0x6d, 0xc9, 0x24, 0x53, 0x37,
    0x15, 0x0a, 0x51, 0x04, 0x7f, 0x02, 0x82, 0x01, 0x00, 0x19, 0xad, 0xd4,
    0x1e, 0x07, 0xde, 0x60, 0x66, 0x22, 0x33, 0x73, 0x1f, 0x0f, 0x4e, 0x53,
    0x32, 0x00, 0x3e, 0x10, 0xef, 0x23, 0x96, 0xcb, 0x10, 0x4d, 0x99, 0x0d,
    0x19, 0x49, 0x1f, 0xa4, 0x4e, 0x00, 0x26, 0x36, 0x2b, 0x1f, 0x21, 0xf1,
    0x1c, 0x9e, 0x98, 0x82, 0x3e, 0x9f, 0x16, 0x68, 0x2d, 0x4b, 0x5e, 0xfd,
    0xdb, 0x49, 0xcc, 0xd7, 0xb1, 0x45, 0x84, 0x5e, 0x3b, 0x4a, 0xf9, 0x80,
    0x50, 0xf6, 0x00, 0xa0, 0xb3, 0xd8, 0x1b, 0x2c, 0xb1, 0xda, 0x29, 0x0c,
    0x85, 0xee, 0x87, 0xa7, 0x02, 0x33, 0x16, 0xb5, 0x22, 0xf9, 0x57, 0x7d,
    0x5f, 0xbe, 0x58, 0x74, 0xc4, 0x52, 0xfd, 0xe4, 0x0e, 0x92, 0x83, 0x09,
    0xa8, 0x01, 0x68, 0x1f, 0x97, 0x1b, 0xae, 0xd5, 0xb9, 0x4c, 0x7d, 0x34,
    0x51, 0xcf, 0x6c, 0xef, 0x00, 0x47, 0x78, 0x4c, 0x18, 0x69, 0xab, 0x62,
    0x77, 0x31, 0x47, 0x43, 0x96, 0x2f, 0x3b, 0xcd, 0x11, 0xa7, 0xff, 0x1a,
    0x6b, 0x3b, 0x55, 0x31, 0x85, 0xa5, 0x6e, 0x08, 0xb4, 0x26, 0x2e, 0x4f,
    0x10, 0x8d, 0x64, 0x94, 0x58, 0x0b, 0x12, 0xc7, 0x9b, 0x84, 0xc1, 0xcd,
    0x9e, 0x1a, 0x4e, 0xf3, 0xa2, 0x78, 0x80, 0x2c, 0x51, 0xe6, 0x21, 0xdc,
    0x1f, 0x46, 0x20, 0x04, 0xcc, 0x81, 0xd0, 0xb3, 0xdc, 0x3a, 0xb8, 0xbe,
    0x45, 0x26, 0x86, 0x23, 0x20, 0xf4, 0x09, 0x2b, 0x29, 0xc3, 0x05, 0xfd,
    0x90, 0x3a, 0xbc, 0x7e, 0x7e, 0x2d, 0x53, 0xd0, 0xc9, 0x23, 0xb0, 0xa0,
    0x05, 0xc0, 0xfc, 0xe0, 0x43, 0x4b, 0xab, 0xd2, 0x83, 0xf1, 0x31, 0xcb,
    0x6b, 0x30, 0xbd, 0x34, 0xfd, 0xe2, 0x2c, 0xa5, 0x39, 0xf6, 0x6f, 0xbb,
    0x8a, 0xa3, 0xb7, 0x7b, 0xd4, 0x26, 0x17, 0x41, 0x41, 0xa1, 0xcf, 0x94,
    0x2d, 0x47, 0x5b, 0x20, 0x8a, 0xf6, 0xb6, 0xd2, 0x1b, 0xa3, 0x41, 0xf5,
    0x01, 0x02, 0x82, 0x01, 0x01, 0x00, 0x83, 0xca, 0xca, 0xea, 0xa4, 0xdf,
    0xc1, 0x0b, 0xe9, 0xcf, 0xca, 0x88, 0xbe, 0x3b, 0xe8, 0x9b, 0x3c, 0xa3,
    0xd1, 0x73, 0x15, 0x13, 0x47, 0xfb, 0xbe, 0x64, 0xbf, 0x91, 0x6d, 0x26,
    0xf6, 0xfc, 0x05, 0x77, 0x0b, 0x1f, 0x95, 0x14, 0x74, 0x02, 0x75, 0xe5,
    0x1d, 0xf3, 0x31, 0x24, 0xd1, 0x3b, 0x8e, 0x97, 0xdb, 0xe8, 0xaa, 0xa0,
    0xc8, 0xe4, 0x72, 0xda, 0x6a, 0xf6, 0x42, 0x95, 0xcf, 0x55, 0x4f, 0x80,
    0x3d, 0xb5, 0xea, 0x7b, 0x45, 0x15, 0x08, 0xde, 0xda, 0xd4, 0xe6, 0x1e,
    0xdb, 0x4f, 0x3d, 0x20, 0x73, 0xa6, 0x88, 0x4e, 0x63, 0x42, 0x79, 0xd3,
    0xbb, 0xea, 0xac, 0x75, 0x70, 0xbd, 0x4b, 0xc5, 0x97, 0x9e, 0xd9, 0x33,
    0x12, 0x1a, 0xe1, 0xab, 0xba, 0xd7, 0xa9, 0x48, 0xfd, 0xc9, 0x85, 0x41,
    0x6b, 0x36, 0x4f, 0xbc, 0x64, 0xf7, 0xc3, 0x6d, 0x94, 0x57, 0x22, 0x1d,
    0x65, 0x36, 0x70, 0xe6, 0x48, 0x47, 0xf9, 0xdb, 0x94, 0x06, 0x91, 0xec,
    0x03, 0x0a, 0x16, 0xe9, 0xbd, 0x94, 0x2a, 0x1f, 0x78, 0xd5, 0x70, 0x64,
    0x40, 0xb3, 0x7f, 0xa6, 0xc2, 0x89, 0x9c, 0xed, 0x76, 0xe2, 0x16, 0x29,
    0x64, 0x41, 0xd6, 0x37, 0x11, 0x59, 0x23, 0x64, 0x84, 0x85, 0x6e, 0x88,
    0x0e, 0x4e, 0xd5, 0xc2, 0x5a, 0x70, 0x11, 0x66, 0x22, 0x6a, 0x2e, 0x46,
    0x10, 0x28, 0x01, 0xd7, 0xe8, 0xfb, 0xb4, 0xdb, 0xf2, 0xfd, 0x35, 0xef,
    0x85, 0x42, 0xd2, 0xa2, 0x30, 0xa3, 0x38, 0xc9, 0x66, 0x9b, 0x9d, 0x71,
    0x6c, 0xa5, 0xac, 0x6e, 0x50, 0x7c, 0x0d, 0xb5, 0x7f, 0x50, 0x9f, 0x0d,
    0x28, 0x65, 0x69, 0x9b, 0x97, 0x07, 0x97, 0xf4, 0x47, 0x49, 0xd8, 0x78,
    0xc4, 0x57, 0xff, 0xae, 0x32, 0xb1, 0xcd, 0x7b, 0xb5, 0x99, 0x6c, 0x4a,
    0xc0, 0x40, 0x6d, 0x0b, 0x64, 0x78, 0x37, 0x1b, 0x47, 0x83
};

static const uint8_t kRSACACertDER[] = {
    0x30, 0x82, 0x04, 0x11, 0x30, 0x82, 0x02, 0xf9, 0xa0, 0x03, 0x02, 0x01,
    0x02, 0x02, 0x09, 0x00, 0xac, 0x5d, 0x53, 0x88, 0x67, 0x97, 0x39, 0x87,
    0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
    0x0b, 0x05, 0x00, 0x30, 0x81, 0x9e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
    0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31, 0x0b, 0x30, 0x09, 0x06,
    0x03, 0x55, 0x04, 0x08, 0x0c, 0x02, 0x4e, 0x53, 0x31, 0x21, 0x30, 0x1f,
    0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65, 0x72,
    0x6e, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73, 0x20,
    0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x31, 0x0b, 0x30, 0x09, 0x06,
    0x03, 0x55, 0x04, 0x0b, 0x0c, 0x02, 0x41, 0x41, 0x31, 0x19, 0x30, 0x17,
    0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x10, 0x77, 0x77, 0x77, 0x2e, 0x6e,
    0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73, 0x74, 0x65, 0x6e, 0x74, 0x33, 0x31,
    0x37, 0x30, 0x35, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
    0x09, 0x01, 0x16, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73, 0x74,
    0x65, 0x6e, 0x74, 0x33, 0x40, 0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73,
    0x74, 0x65, 0x6e, 0x74, 0x2e, 0x63, 0x6f, 0x6d, 0x2e, 0x6e, 0x6f, 0x6e,
    0x65, 0x78, 0x69, 0x73, 0x74, 0x65, 0x6e, 0x74, 0x30, 0x1e, 0x17, 0x0d,
    0x31, 0x36, 0x31, 0x31, 0x31, 0x38, 0x32, 0x32, 0x34, 0x34, 0x34, 0x39,
    0x5a, 0x17, 0x0d, 0x31, 0x39, 0x31, 0x31, 0x31, 0x38, 0x32, 0x32, 0x34,
    0x34, 0x34, 0x39, 0x5a, 0x30, 0x81, 0x9e, 0x31, 0x0b, 0x30, 0x09, 0x06,
    0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31, 0x0b, 0x30, 0x09,
    0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x02, 0x4e, 0x53, 0x31, 0x21, 0x30,
    0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65,
    0x72, 0x6e, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73,
    0x20, 0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x31, 0x0b, 0x30, 0x09,
    0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x02, 0x41, 0x41, 0x31, 0x19, 0x30,
    0x17, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x10, 0x77, 0x77, 0x77, 0x2e,
    0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73, 0x74, 0x65, 0x6e, 0x74, 0x33,
    0x31, 0x37, 0x30, 0x35, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
    0x01, 0x09, 0x01, 0x16, 0x28, 0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73,
    0x74, 0x65, 0x6e, 0x74, 0x33, 0x40, 0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69,
    0x73, 0x74, 0x65, 0x6e, 0x74, 0x2e, 0x63, 0x6f, 0x6d, 0x2e, 0x6e, 0x6f,
    0x6e, 0x65, 0x78, 0x69, 0x73, 0x74, 0x65, 0x6e, 0x74, 0x30, 0x82, 0x01,
    0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
    0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01,
    0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xd6, 0x49, 0x39, 0xb9, 0xfd, 0x02,
    0xe4, 0xf7, 0x0b, 0x98, 0x14, 0x51, 0xbc, 0xc2, 0xb0, 0x0b, 0xd6, 0x09,
    0xf0, 0x12, 0xd5, 0xe4, 0x6a, 0x80, 0x10, 0x49, 0x54, 0x71, 0xa7, 0x75,
    0x3f, 0x4b, 0x18, 0xb4, 0xb8, 0xef, 0xe1, 0xb4, 0x03, 0x55, 0xa4, 0xcf,
    0x09, 0x24, 0x03, 0x0a, 0xf2, 0x34, 0xa5, 0xbc, 0xaa, 0x8e, 0x72, 0x39,
    0x9c, 0xfe, 0x7a, 0x16, 0x44, 0x18, 0x7a, 0xbb, 0x56, 0x4f, 0x7a, 0xef,
    0x93, 0x7f, 0x91, 0x98, 0x53, 0xe2, 0xd3, 0xc9, 0x5e, 0x20, 0x4a, 0x91,
    0xf5, 0x8b, 0x51, 0x9d, 0x36, 0x15, 0x20, 0x7b, 0x0b, 0xd1, 0x41, 0xac,
    0x0e, 0xbd, 0x14, 0x09, 0x69, 0xf3, 0xb8, 0x9f, 0x9f, 0x65, 0x2b, 0x26,
    0xae, 0xb9, 0x6b, 0x09, 0x45, 0x7c, 0x55, 0xe0, 0xb5, 0x87, 0x60, 0x14,
    0xd2, 0xaf, 0x93, 0x2b, 0x31, 0x90, 0x7b, 0xe9, 0x7c, 0x2a, 0x04, 0xd7,
    0xb6, 0x72, 0x27, 0xd2, 0xf9, 0x88, 0x27, 0xb7, 0x66, 0xc9, 0xd2, 0x58,
    0xff, 0x65, 0x68, 0x0a, 0x7a, 0x31, 0xc9, 0x99, 0x0d, 0xd1, 0x0c, 0x42,
    0x46, 0xbd, 0xd0, 0x8e, 0xfa, 0xfa, 0x90, 0x50, 0xee, 0x47, 0x8b, 0xf0,
    0xc4, 0x5f, 0x9d, 0xf4, 0x58, 0x7b, 0x53, 0x0d, 0xfb, 0x9d, 0x60, 0xde,
    0x0e, 0x54, 0x9b, 0xef, 0xac, 0xcf, 0xce, 0x50, 0xda, 0xfe, 0xf6, 0x95,
    0xd0, 0x70, 0xf2, 0xeb, 0xb6, 0x90, 0x15, 0xc9, 0xe6, 0xf0, 0x3d, 0xb0,
    0x14, 0x03, 0xc3, 0xa1, 0xc6, 0x9a, 0x5d, 0x5d, 0x70, 0x8f, 0xfe, 0xd2,
    0x8c, 0xac, 0x8d, 0xa7, 0x0d, 0x1e, 0x8f, 0x40, 0xf8, 0xff, 0x8c, 0x06,
    0x23, 0xe0, 0xc5, 0x28, 0x67, 0x54, 0xd2, 0xf3, 0xa0, 0x25, 0x74, 0xd0,
    0xe5, 0x5f, 0x44, 0x11, 0x42, 0xc5, 0x33, 0x4e, 0x58, 0xe6, 0xf2, 0x64,
    0x5f, 0xf4, 0x41, 0x07, 0xaa, 0x06, 0x42, 0xc8, 0x49, 0xff, 0x02, 0x03,
    0x01, 0x00, 0x01, 0xa3, 0x50, 0x30, 0x4e, 0x30, 0x1d, 0x06, 0x03, 0x55,
    0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xd7, 0xde, 0xbb, 0xad, 0x37, 0xb6,
    0x45, 0x6d, 0x71, 0x0d, 0xd4, 0xb3, 0x59, 0x7c, 0x7f, 0xcd, 0xbb, 0xc1,
    0xa5, 0x50, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30,
    0x16, 0x80, 0x14, 0xd7, 0xde, 0xbb, 0xad, 0x37, 0xb6, 0x45, 0x6d, 0x71,
    0x0d, 0xd4, 0xb3, 0x59, 0x7c, 0x7f, 0xcd, 0xbb, 0xc1, 0xa5, 0x50, 0x30,
    0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01,
    0xff, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
    0x01, 0x0b, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x34, 0x8c, 0xa8,
    0x0d, 0xf8, 0xb8, 0x9e, 0xa7, 0xd5, 0xb5, 0x8c, 0xf5, 0x5d, 0x7e, 0x4e,
    0x37, 0x8a, 0xb0, 0xed, 0x8d, 0x22, 0xa0, 0xe4, 0x36, 0x87, 0xaa, 0x91,
    0xc6, 0xcd, 0xae, 0x99, 0x39, 0xd6, 0xc8, 0xa2, 0x61, 0x4f, 0x18, 0xc7,
    0x16, 0xd8, 0x3f, 0x48, 0x71, 0xe7, 0x14, 0x5b, 0x99, 0x04, 0xe5, 0x04,
    0x17, 0x41, 0x98, 0x68, 0x24, 0x8b, 0xab, 0x03, 0x91, 0xcd, 0xfa, 0x54,
    0xc9, 0xeb, 0x97, 0x25, 0x69, 0x34, 0xcd, 0x30, 0x6c, 0xb9, 0x86, 0xea,
    0x5c, 0xf6, 0x0f, 0xcf, 0x02, 0x8d, 0xe4, 0xf6, 0xeb, 0x0a, 0xf3, 0x0d,
    0x96, 0x27, 0x30, 0x16, 0x2c, 0x33, 0x9c, 0x16, 0x3d, 0xad, 0x1c, 0x9f,
    0xc0, 0x76, 0xb3, 0xcb, 0x1e, 0xe3, 0x4b, 0x69, 0x42, 0x22, 0x82, 0x35,
    0x16, 0x7d, 0x90, 0xd3, 0x36, 0x63, 0xba, 0x0d, 0xd8, 0x26, 0xf8, 0xe4,
    0xf3, 0x4d, 0x2f, 0x2d, 0xcf, 0x2b, 0xc6, 0xa5, 0x08, 0xc5, 0xb5, 0xa2,
    0x02, 0x94, 0xc8, 0x11, 0xd4, 0x93, 0x1b, 0x2e, 0xae, 0x38, 0x80, 0x4b,
    0x47, 0x21, 0x24, 0x17, 0x36, 0x1c, 0xd3, 0x6b, 0xfe, 0x52, 0x33, 0xff,
    0x19, 0x5c, 0xa5, 0x24, 0x79, 0x10, 0x26, 0xc7, 0x79, 0x6e, 0xc5, 0xb6,
    0x02, 0x58, 0xdc, 0x00, 0x55, 0x0b, 0xf3, 0xb3, 0x63, 0x61, 0x82, 0xf2,
    0xd4, 0xe3, 0xf2, 0x5f, 0x39, 0xc4, 0x02, 0x58, 0x4d, 0x42, 0x09, 0x01,
    0xa1, 0x56, 0x30, 0x9f, 0x9e, 0x79, 0x21, 0xc6, 0xfe, 0x47, 0x31, 0x95,
    0xbb, 0x31, 0x4c, 0x52, 0xdb, 0x1f, 0xe7, 0xfa, 0xbf, 0x71, 0x5d, 0x5b,
    0xd4, 0x37, 0xbc, 0xbd, 0x7b, 0x85, 0x08, 0x65, 0x06, 0x59, 0xf6, 0xb2,
    0x09, 0x82, 0x0d, 0xce, 0x5a, 0xae, 0xf5, 0x1d, 0xb0, 0x2e, 0x15, 0xd5,
    0x8c, 0xbb, 0xca, 0x4b, 0x4c, 0x27, 0x01, 0x12, 0x8b, 0xe9, 0xfd, 0x61,
    0xed
};

static const uint8_t kECCACertDER[] = {
    0x30, 0x82, 0x02, 0x6e, 0x30, 0x82, 0x02, 0x15, 0xa0, 0x03, 0x02, 0x01,
    0x02, 0x02, 0x09, 0x00, 0xf7, 0x16, 0x78, 0x5d, 0xe9, 0x0d, 0xc6, 0x8e,
    0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02,
    0x30, 0x81, 0x93, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06,
    0x13, 0x02, 0x41, 0x55, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04,
    0x08, 0x0c, 0x06, 0x53, 0x79, 0x64, 0x6e, 0x65, 0x79, 0x31, 0x21, 0x30,
    0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65,
    0x72, 0x6e, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73,
    0x20, 0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x31, 0x0c, 0x30, 0x0a,
    0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x03, 0x73, 0x61, 0x64, 0x31, 0x14,
    0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0b, 0x6e, 0x6f, 0x6e,
    0x65, 0x78, 0x69, 0x73, 0x74, 0x2e, 0x34, 0x34, 0x31, 0x2c, 0x30, 0x2a,
    0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16,
    0x1d, 0x6e, 0x6f, 0x6e, 0x40, 0x65, 0x78, 0x69, 0x73, 0x74, 0x65, 0x6e,
    0x74, 0x2e, 0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73, 0x74, 0x65, 0x6e,
    0x74, 0x34, 0x33, 0x34, 0x33, 0x32, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x36,
    0x31, 0x32, 0x30, 0x33, 0x30, 0x34, 0x30, 0x36, 0x35, 0x38, 0x5a, 0x17,
    0x0d, 0x31, 0x39, 0x31, 0x32, 0x30, 0x33, 0x30, 0x34, 0x30, 0x36, 0x35,
    0x38, 0x5a, 0x30, 0x81, 0x93, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
    0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03,
    0x55, 0x04, 0x08, 0x0c, 0x06, 0x53, 0x79, 0x64, 0x6e, 0x65, 0x79, 0x31,
    0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e,
    0x74, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69,
    0x74, 0x73, 0x20, 0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x31, 0x0c,
    0x30, 0x0a, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x03, 0x73, 0x61, 0x64,
    0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0b, 0x6e,
    0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73, 0x74, 0x2e, 0x34, 0x34, 0x31, 0x2c,
    0x30, 0x2a, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09,
    0x01, 0x16, 0x1d, 0x6e, 0x6f, 0x6e, 0x40, 0x65, 0x78, 0x69, 0x73, 0x74,
    0x65, 0x6e, 0x74, 0x2e, 0x6e, 0x6f, 0x6e, 0x65, 0x78, 0x69, 0x73, 0x74,
    0x65, 0x6e, 0x74, 0x34, 0x33, 0x34, 0x33, 0x32, 0x30, 0x59, 0x30, 0x13,
    0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
    0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x68,
    0x63, 0xa8, 0xd1, 0x25, 0xc8, 0x1e, 0x07, 0x24, 0x62, 0x37, 0xd4, 0x03,
    0xac, 0xa8, 0xa7, 0x6e, 0x90, 0x02, 0x99, 0xcf, 0x29, 0xeb, 0x4c, 0x40,
    0x59, 0xb6, 0x7e, 0x3f, 0x12, 0xe5, 0x1f, 0x52, 0xca, 0x0b, 0x10, 0x7d,
    0xd5, 0xaa, 0x51, 0x8d, 0xd3, 0x50, 0x94, 0x7f, 0x9e, 0x2f, 0x16, 0x79,
    0x37, 0x5b, 0x47, 0x9b, 0xbe, 0xf4, 0xaf, 0xe2, 0x9a, 0x9b, 0x15, 0x31,
    0x0c, 0xc6, 0x61, 0xa3, 0x50, 0x30, 0x4e, 0x30, 0x1d, 0x06, 0x03, 0x55,
    0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x49, 0xdb, 0xfd, 0x0c, 0x73, 0x69,
    0x83, 0x7f, 0xc1, 0xc1, 0xc2, 0x3f, 0xce, 0x4e, 0x82, 0xe6, 0xd3, 0xdc,
    0x3c, 0x6e, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30,
    0x16, 0x80, 0x14, 0x49, 0xdb, 0xfd, 0x0c, 0x73, 0x69, 0x83, 0x7f, 0xc1,
    0xc1, 0xc2, 0x3f, 0xce, 0x4e, 0x82, 0xe6, 0xd3, 0xdc, 0x3c, 0x6e, 0x30,
    0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01,
    0xff, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03,
    0x02, 0x03, 0x47, 0x00, 0x30, 0x44, 0x02, 0x20, 0x0e, 0x49, 0x30, 0xc0,
    0x3f, 0x41, 0xcd, 0xc7, 0x87, 0xe7, 0x0f, 0xfd, 0x2a, 0xcd, 0xd8, 0x9c,
    0xf8, 0x1c, 0x8f, 0xbc, 0x94, 0xab, 0x29, 0xcd, 0xaf, 0x7e, 0xf1, 0x94,
    0x28, 0x25, 0x1d, 0xc1, 0x02, 0x20, 0x29, 0x93, 0xf3, 0x91, 0x44, 0xe3,
    0xac, 0xfe, 0xc5, 0x86, 0xdd, 0x27, 0x36, 0x49, 0xd6, 0x61, 0x72, 0x47,
    0x1b, 0x55, 0x46, 0x29, 0x50, 0x2a, 0x16, 0xf3, 0x60, 0x2e, 0x3f, 0x96,
    0x8d, 0x9a
};

static SSL_CTX *ctx;
static SSL *client;

extern void RESET_RAND(void);

unsigned int psk_callback(SSL * ssl, const char *hint, char *identuty,
                          unsigned int max_identity_len, unsigned char *psk,
                          unsigned int max_psk_len)
{
    return max_psk_len;
}

static void Init()
{
    SSL_library_init();
    OpenSSL_add_ssl_algorithms();
    RESET_RAND();

    ctx = SSL_CTX_new(TLS_method());
    const uint8_t *bufp = kRSAPrivateKeyDER;
    RSA *privkey = d2i_RSAPrivateKey(NULL, &bufp, sizeof(kRSAPrivateKeyDER));
    assert(privkey != NULL);
    EVP_PKEY *pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pkey, privkey);
    int ret = SSL_CTX_use_PrivateKey(ctx, pkey);
    assert(ret == 1);
    EVP_PKEY_free(pkey);

    bufp = kCertificateDER;
    X509 *cert = d2i_X509(NULL, &bufp, sizeof(kCertificateDER));
    assert(cert != NULL);
    ret = SSL_CTX_use_certificate(ctx, cert);
    assert(ret == 1);
    X509_free(cert);
    ret = SSL_CTX_set_cipher_list(ctx, "ALL:eNULL:@SECLEVEL=0");
    assert(ret == 1);

    X509_STORE *store = X509_STORE_new();
    assert(store != NULL);

    bufp = kRSACACertDER;
    cert = d2i_X509(NULL, &bufp, sizeof(kRSACACertDER));
    assert(cert != NULL);
    ret = SSL_CTX_add_client_CA(ctx, cert);
    assert(ret == 1);
    ret = X509_STORE_add_cert(store, cert);
    assert(ret == 1);
    X509_free(cert);

    bufp = kECCACertDER;
    cert = d2i_X509(NULL, &bufp, sizeof(kECCACertDER));
    assert(cert != NULL);
    ret = SSL_CTX_add_client_CA(ctx, cert);
    assert(ret == 1);
    ret = X509_STORE_add_cert(store, cert);
    assert(ret == 1);
    X509_free(cert);

    SSL_CTX_set_cert_store(ctx, store);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_verify_depth(ctx, 10);

    SSL_CTX_set_psk_client_callback(ctx, psk_callback);
    ret = SSL_CTX_use_psk_identity_hint(ctx, "ABCDEFUZZ");
    assert(ret == 1);
}

int LLVMFuzzerTestOneInput(uint8_t * buf, size_t len)
{

    if (ctx == NULL)
        Init();

    RESET_RAND();

    SSL *client = SSL_new(ctx);
    SSL_set_tlsext_host_name(client, "localhost");

    BIO *in = BIO_new(BIO_s_mem());
    BIO_write(in, buf, len);

    BIO *out = BIO_new(BIO_s_fd());
    BIO_set_fd(out, 1, BIO_NOCLOSE);

    SSL_set_bio(client, in, out);

    if (SSL_connect(client) == 1) {
        X509 *peer;
        if ((peer = SSL_get_peer_certificate(client)) != NULL) {
            SSL_get_verify_result(client);
            X509_free(peer);
        }
        // Keep reading application data until error or EOF.
        uint8_t tmp[1024 * 1024];
        for (;;) {
            ssize_t r = SSL_read(client, tmp, sizeof(tmp));
            if (r <= 0) {
                break;
            }
            if (SSL_write(client, tmp, r) <= 0) {
                break;
            }
#ifndef OPENSSL_NO_HEARTBEATS
            SSL_heartbeat(client);
#endif
            SSL_renegotiate(client);
        }
    }

    SSL_free(client);

    return 0;
}

#ifdef __cplusplus
}
#endif
