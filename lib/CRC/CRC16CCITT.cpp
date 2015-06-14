#include "CRC16CCITT.h"
#include "../debug.h"

CRCType CRC16CCITT::getPolynomial() const
{
	return (1 << 0 | 1 << 5 | 1 << 12);
}

CRCType CRC16CCITT::getPolynomialReverse() const
{
	return (1 << (15-0) | 1 << (15-5) | 1 << (15-12));
}

size_t CRC16CCITT::getNumBytes() const
{
	return 2;
}

size_t CRC16CCITT::getNumBits() const
{
	return 16;
}

CRCType CRC16CCITT::makeNextChecksum(
	const CRCType &prevChecksum,
	unsigned char c) const
{
	uint8_t tmp = ((prevChecksum >> 8) ^ c) & 0xff;
	uint16_t ret = (prevChecksum << 8) ^ this->lookupTable[tmp];
	ret &= 0xffff;
	return ret;
}

CRCType CRC16CCITT::makePrevChecksum(
	const CRCType &nextChecksum,
	unsigned char c) const
{
	uint16_t ret =
		(c << 8)
		^ this->invLookupTable[(uint8_t) (nextChecksum & 0xff)]
		^ (nextChecksum << 8)
		^ (nextChecksum >> 8);

	ret &= 0xffff;
	return ret;
}

CRC16CCITT::CRC16CCITT() : CRC()
{
	this->setInitialXOR(0);
	this->setFinalXOR(0);
	for (uint16_t n = 0; n <= 0xff; n ++)
	{
		uint16_t crc1 = 0;
		uint32_t crc2 = 0;
		for (uint8_t k = 0; k < 8; k ++)
		{
			if ((crc1 ^ (n << (8 + k))) & 0x8000)
				crc1 = (crc1 << 1) ^ this->getPolynomial();
			else
				crc1 <<= 1;

			if ((crc2 ^ (n >> k)) & 1)
				crc2 = ((crc2 ^ this->getPolynomial()) >> 1) | 0x100000;
			else
				crc2 >>= 1;
		}
		crc2 &= 0xffff;
		this->lookupTable[n] = crc1;
		this->invLookupTable[n] = static_cast<uint16_t>(crc2);
	}
}

CRCType CRC16CCITT::computePatch(
	const CRCType &desiredChecksum,
	const File::OffsetType &desiredPosition,
	File &inputFile,
	const bool &overwrite) const
{
	uint16_t checksum1 = this->computePartialChecksum(
		inputFile,
		0,
		desiredPosition,
		this->getInitialXOR());

	uint16_t checksum2 = this->computeReversePartialChecksum(
		inputFile,
		inputFile.getFileSize(),
		desiredPosition + ((File::OffsetType) (overwrite ? 2 : 0)),
		(uint16_t) (desiredChecksum ^ this->getFinalXOR()));

	uint16_t patch = checksum2;
	for (size_t i = 0, j = 1; i < 2; i ++, j --)
	{
		//differrent order to CRC32
		patch = this->makePrevChecksum(patch, (checksum1 >> (i << 3)) & 0xff);
	}

	return (patch << 8 | patch >> 8) & 0xffff;
}
