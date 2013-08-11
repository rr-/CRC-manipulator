#ifndef CRC32_H
#define CRC32_H

#include "CRC.h"

class CRC32 : public CRC {
	private:
		uint32_t lookupTable [256];
		uint32_t invLookupTable [256];
	protected:
		virtual const CRCType getPolynomial() const;
		virtual const CRCType getPolynomialReverse() const;
		virtual const size_t getNumBytes() const;
		virtual const size_t getNumBits() const;

		virtual const CRCType makeNextChecksum(
			const CRCType& prevChecksum,
			unsigned char c) const;

		virtual const CRCType makePrevChecksum(
			const CRCType& nextChecksum,
			unsigned char c) const;

		virtual const CRCType computePatch(
			const CRCType& desiredChecksum,
			const File::OffsetType& desiredPosition,
			File& inputFile,
			const bool& overwrite) const;
	public:
		CRC32();
};

#endif
