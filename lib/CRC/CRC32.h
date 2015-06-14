#ifndef CRC32_H
#define CRC32_H

#include "CRC.h"

class CRC32 : public CRC
{
	public:
		CRC32();

	protected:
		virtual CRCType getPolynomial() const;
		virtual CRCType getPolynomialReverse() const;
		virtual size_t getNumBytes() const;
		virtual size_t getNumBits() const;

		virtual CRCType makeNextChecksum(
			const CRCType &prevChecksum,
			unsigned char c) const;

		virtual CRCType makePrevChecksum(
			const CRCType &nextChecksum,
			unsigned char c) const;

		virtual CRCType computePatch(
			const CRCType &desiredChecksum,
			const File::OffsetType &desiredPosition,
			File &inputFile,
			const bool &overwrite) const;

	private:
		uint32_t lookupTable[256];
		uint32_t invLookupTable[256];
};

#endif
