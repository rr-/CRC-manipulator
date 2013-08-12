#ifndef CRC16CCITT_H
#define CRC16CCITT_H

#include "CRC.h"

class CRC16CCITT : public CRC
{
	private:
		uint16_t lookupTable [256];
		uint16_t invLookupTable [256];
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
	public:
		CRC16CCITT();
};

#endif
