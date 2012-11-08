#ifndef CRC16CCITT_H
#define CRC16CCITT_H

#include "CRC.h"
#include "../debug.h"

class CRC16CCITT : public CRC {
	private:
		uint16_t lookupTable [256];
		uint16_t invLookupTable [256];
	protected:
		virtual const CRCType getPolynomial() const;
		virtual const CRCType getPolynomialReverse() const;
		virtual const size_t getNumBytes() const;
		virtual const size_t getNumBits() const;
		virtual const CRCType makeNextChecksum(const CRCType& prevChecksum, unsigned char c) const;
		virtual const CRCType makePrevChecksum(const CRCType& nextChecksum, unsigned char c) const;
		virtual const CRCType computePatch(const CRCType& desiredChecksum, const IFile::OffsetType& desiredPosition, IFile& inputFile, const bool& overwrite) const;
	public:
		CRC16CCITT();
};

#endif
