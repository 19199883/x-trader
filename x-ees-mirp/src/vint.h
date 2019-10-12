#pragma once

#include <stdint.h>
#include <iostream>

class vint
{
public:
	inline uint32_t ZigZagEncode32(int32_t n) 
	{
	  // Note:  the right-shift must be arithmetic
	  // Note:  left shift must be unsigned because of overflow
	  return (static_cast<uint32_t>(n) << 1) ^ static_cast<uint32_t>(n >> 31);
	}

	inline int32_t ZigZagDecode32(uint32_t n) 
	{
	  // Note:  Using unsigned types prevent undefined behavior
	  return static_cast<int32_t>((n >> 1) ^ (~(n & 1) + 1));
	}

	inline uint64_t ZigZagEncode64(int64_t n) 
	{
	  // Note:  the right-shift must be arithmetic
	  // Note:  left shift must be unsigned because of overflow
	  return (static_cast<uint64_t>(n) << 1) ^ static_cast<uint64_t>(n >> 63);
	}

	inline int64_t ZigZagDecode64(uint64_t n) 
	{
	  // Note:  Using unsigned types prevent undefined behavior
	  return static_cast<int64_t>((n >> 1) ^ (~(n & 1) + 1));
	}

	/*
	 * For an unsigned varint, I believe the following will decode it for you, assuming 
	 * you've got the varint data in a buffer pointed to by data. This example function
	 * returns the number of bytes decoded in the reference argument int decoded_bytes.
	 */
	static uint64_t decode_unsigned_varint(const uint8_t *const data, uint8_t &decoded_bytes )
	{
		uint8_t i = 0;
		uint64_t decoded_value = 0;
		uint8_t shift_amount = 0;

		do 
		{
			decoded_value |= (uint64_t)(data[i] & 0x7F) << shift_amount;     
			shift_amount += 7;
		} while ( (data[i++] & 0x80) != 0 );

		decoded_bytes = i;
		return decoded_value;
	}

	/*
	 * To decode a signed varint, you can use this second function that calls the 
	 * first: 
	 */
	static int64_t decode_signed_varint(const uint8_t *const data, uint8_t &decoded_bytes)
	{
		uint64_t unsigned_value = decode_unsigned_varint(data, decoded_bytes);
		return ZigZagDecode64(unsigned_value );
	}
	
	/*
	 * Encode an unsigned 64-bit varint.  Returns number of encoded bytes.
	* buffer' must have room for up to 10 bytes.
	*/
	uint8_t encode_unsigned_varint(uint8_t *const buffer, uint64_t value)
	{
		uint8_t encoded = 0;

		do
		{
			uint8_t next_byte = value & 0x7F;
			value >>= 7;

			if (value)
				next_byte |= 0x80;

			buffer[encoded++] = next_byte;

		} while (value);


		return encoded;
	}

	/*
	 * Encode a signed 64-bit varint.  Works by first zig-zag transforming
	 * signed value into an unsigned value, and then reusing the unsigned
	 * encoder.  'buffer' must have room for up to 10 bytes.
	 */
	uint8_t encode_signed_varint(uint8_t *const buffer, int64_t value)
	{
		uint64_t uvalue;

		uvale = ZigZagEncode64(value);
		return encode_unsigned_varint( buffer, uvalue );
	}
};

