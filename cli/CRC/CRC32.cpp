#include "CRC32.h"

/**
 * NOTICE: following code is strongly based on SAR-PR-2006-05
 */

const CRCType CRC32::getPolynomial() const {
	return 0x04C11DB7;
}

const CRCType CRC32::getPolynomialReverse() const {
	return 0xEDB88320;
}

const size_t CRC32::getNumBytes() const {
	return 4;
}

const size_t CRC32::getNumBits() const {
	return 32;
}

const CRCType CRC32::makeNextChecksum(const CRCType& prevChecksum, unsigned char c) const {
	uint8_t tmp =(prevChecksum ^ c) & 0xff;
	uint32_t ret =(prevChecksum >> 8) ^ this->lookupTable[tmp];
	ret &= 0xffffffff;
	return ret;
}

const CRCType CRC32::makePrevChecksum(const CRCType& nextChecksum, unsigned char c) const {
	uint8_t tmp =(nextChecksum >> 24) & 0xff;
	uint32_t ret =(nextChecksum << 8) ^(this->invLookupTable[tmp] ^ c);
	ret &= 0xffffffff;
	return ret;
}

CRC32::CRC32() : CRC() {
	this->setInitialXOR(0xffffffff);
	this->setFinalXOR(0xffffffff);
	for (unsigned short n = 0; n <= 0xff; n ++) {
		uint32_t crc1 = n;
		uint32_t crc2 = n << 24;
		for (unsigned char k = 0; k < 8; k ++) {
			if (crc1 & 1) {
				crc1 =(crc1 >> 1) ^ this->getPolynomialReverse();
			} else {
				crc1 >>= 1;
			}
			if (crc2 & 0x80000000) {
				crc2 =((crc2 ^ this->getPolynomialReverse()) << 1) | 1;
			} else {
				crc2 <<= 1;
			}
		}
		this->lookupTable[n] = crc1;
		this->invLookupTable[n] = crc2;
	}
}

const CRCType CRC32::computePatch(const CRCType& desiredChecksum, const IFile::OffsetType& desiredPosition, IFile& inputFile, const bool& overwrite) const {
	uint32_t checksum1 = this->computePartialChecksum(inputFile, 0, desiredPosition, this->getInitialXOR());
	uint32_t checksum2 = desiredChecksum ^ this->getFinalXOR();
	IFile::OffsetType position = desiredPosition;
	if (overwrite) {
		position += 4;
	}
	checksum2 = this->computeReversePartialChecksum(inputFile, inputFile.getFileSize(), position, checksum2);
	uint32_t patch = checksum2;
	size_t i, j;
	for (i = 0, j = 3; i < 4; i ++, j --) {
		patch = this->makePrevChecksum(patch,(checksum1 >>(j << 3)) & 0xff);
	}
	return patch;
}
