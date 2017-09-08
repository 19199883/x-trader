/**
 * base64.h
 *
 * RFC 1521 base64 encoding/decoding
 *
 */
#ifndef _BASE64_H_
#define _BASE64_H_

#include <string.h>
#include <string>

#define ERR_BASE64_BUFFER_TOO_SMALL               -0x002A  /* Output buffer too small. */
#define ERR_BASE64_INVALID_CHARACTER              -0x002C  /* Invalid character in input. */

namespace my_cmn
{
// 对外接口，字符串的加解密
std::string MYEncrypt(const std::string &text);
std::string MYDecrypt(const std::string &cypher_text);
}

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode a buffer into base64 format
 *
 * \param dst      destination buffer
 * \param dlen     size of the buffer
 * \param src      source buffer
 * \param slen     amount of data to be encoded
 *
 * \return         0 if successful, or POLARSSL_ERR_BASE64_BUFFER_TOO_SMALL.
 *                 *dlen is always updated to reflect the amount
 *                 of data that has (or would have) been written.
 *
 * \note           Call this function with *dlen = 0 to obtain the
 *                 required buffer size in *dlen
 */
int base64_encode(unsigned char *dst, size_t *dlen,
                  unsigned char *src, size_t slen);

/**
 * \brief          Decode a base64-formatted buffer
 *
 * \param dst      destination buffer (can be NULL for checking size)
 * \param dlen     size of the buffer
 * \param src      source buffer
 * \param slen     amount of data to be decoded
 *
 * \return         0 if successful, POLARSSL_ERR_BASE64_BUFFER_TOO_SMALL, or
 *                 POLARSSL_ERR_BASE64_INVALID_CHARACTER if the input data is
 *                 not correct. *dlen is always updated to reflect the amount
 *                 of data that has (or would have) been written.
 *
 * \note           Call this function with *dst = NULL or *dlen = 0 to obtain
 *                 the required buffer size in *dlen
 */
int base64_decode(unsigned char *dst, size_t *dlen,
                  unsigned char *src, size_t slen);

#ifdef SELF_TEST
/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int base64_self_test(int verbose);
#endif

#ifdef __cplusplus
}
#endif

#endif /* base64.h */
