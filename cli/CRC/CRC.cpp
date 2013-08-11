#include "CRC.h"

/**
 * Constructor...
 */
CRC::CRC()
{
}



/**
 * Getters
 */

const CRCType& CRC::getInitialXOR() const
{
	return this->initialXOR;
}



const CRCType& CRC::getFinalXOR() const
{
	return this->finalXOR;
}

/**
 * Setters
 */

void CRC::setInitialXOR(const CRCType& f)
{
	this->initialXOR = f;
}



void CRC::setFinalXOR(const CRCType& f)
{
	this->finalXOR = f;
}



void CRC::setProgressFunction(
	void(*function)(const CRCProgressType& progressType,
	const IFile::OffsetType& startPos,
	const IFile::OffsetType& curPos,
	const IFile::OffsetType& endPos))
{
	this->progressFunction = function;
}



/**
 * Method that copies the input to the output, outputting
 * computed patch at given position along the way.
 */
void CRC::applyPatch(
	const CRCType& finalChecksum,
	const IFile::OffsetType& finalPos,
	IFile& input,
	IFile& output,
	const bool& overwrite) const
{
	unsigned char* buffer = new unsigned char[input.getBufferSize()];
	CRCType patch = this->computePatch(
		finalChecksum,
		finalPos,
		input,
		overwrite);

	input.seek(0, IFile::FSEEK_BEGINNING);
	output.seek(0, IFile::FSEEK_BEGINNING);
	IFile::OffsetType pos = input.tell();
	this->markProgress(CRCPROG_WRITE_START, 0, pos, input.getFileSize());

	// output first half
	while (pos < finalPos)
	{
		this->markProgress(CRCPROG_WRITE_PROGRESS, 0, pos, input.getFileSize());
		size_t chunkSize = input.getBufferSize();
		if (pos + ((IFile::OffsetType) chunkSize) >= finalPos)
		{
			chunkSize = ((size_t) (finalPos - pos));
		}
		assert(chunkSize >= 0);
		input.read(buffer, chunkSize);
		output.write(buffer, chunkSize);
		pos += chunkSize;
	}

	// output patch
	assert(this->getNumBytes() < input.getBufferSize());
	for (size_t i = 0, j = this->getNumBytes() - 1;
		i < this->getNumBytes();
		i ++, j --)
	{
		// CAUTION: very sensitive part
		buffer[i] = (patch >> (i * 8)) & 0xff;
	}
	output.write(buffer, this->getNumBytes());
	if (overwrite)
	{
		input.seek(this->getNumBytes(), IFile::FSEEK_AHEAD);
		pos += this->getNumBytes();
	}

	// output second half
	while (pos < input.getFileSize())
	{
		this->markProgress(CRCPROG_WRITE_PROGRESS, 0, pos, input.getFileSize());
		size_t chunkSize = input.getBufferSize();
		if (pos + ((IFile::OffsetType) chunkSize) >= input.getFileSize())
		{
			chunkSize = input.getFileSize() - pos;
		}
		assert(chunkSize >= 0);
		input.read(buffer, chunkSize);
		output.write(buffer, chunkSize);
		pos += chunkSize;
	}

	this->markProgress(CRCPROG_WRITE_END, 0, pos, input.getFileSize());
	delete []buffer;
}



/**
 * Computes the checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
const CRCType CRC::computeChecksum(IFile& input) const
{
	CRCType checksum = this->getInitialXOR();
	checksum = this->computePartialChecksum(
		input,
		0,
		input.getFileSize(),
		checksum);
	return checksum ^ this->getFinalXOR();
}



/**
 * Computes partial checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
const CRCType CRC::computePartialChecksum(
	IFile& input,
	const IFile::OffsetType& startPos,
	const IFile::OffsetType& endPos,
	const CRCType& initialChecksum) const
{
	assert(startPos <= endPos);
	if (startPos == endPos)
	{
		return initialChecksum;
	}
	CRCType checksum = initialChecksum;
	unsigned char* buffer = new unsigned char[input.getBufferSize()];
	IFile::OffsetType oldPos = input.tell();
	IFile::OffsetType pos = startPos;
	input.seek(pos, IFile::FSEEK_BEGINNING);
	this->markProgress(CRCPROG_CHECKSUM_START, startPos, startPos, endPos);

	while (pos < endPos)
	{
		this->markProgress(CRCPROG_CHECKSUM_PROGRESS, startPos, pos, endPos);
		size_t chunkSize = input.getBufferSize();
		if (pos + ((IFile::OffsetType) chunkSize) >= endPos)
		{
			chunkSize = ((size_t) (endPos - pos));
		}
		assert(chunkSize >= 0);
		input.read(buffer, chunkSize);
		for (size_t i = 0; i < chunkSize; i ++)
		{
			checksum = this->makeNextChecksum(checksum, buffer[i]);
		}
		pos += chunkSize;
	}

	this->markProgress(CRCPROG_CHECKSUM_END, startPos, endPos, endPos);
	input.seek(oldPos, IFile::FSEEK_BEGINNING);
	delete []buffer;
	return checksum;
}



/**
 * Computes reverse partial checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
const CRCType CRC::computeReversePartialChecksum(
	IFile& input,
	const IFile::OffsetType& startPos,
	const IFile::OffsetType& endPos,
	const CRCType& initialChecksum) const
{
	assert(startPos >= endPos);
	if (startPos == endPos)
	{
		return initialChecksum;
	}
	CRCType checksum = initialChecksum;
	unsigned char* buffer = new unsigned char[input.getBufferSize()];
	IFile::OffsetType oldPos = input.tell();
	IFile::OffsetType pos = startPos;
	IFile::OffsetType revPos = endPos;
	this->markProgress(CRCPROG_CHECKSUM_START, startPos, startPos, endPos);

	while (revPos < startPos)
	{
		this->markProgress(CRCPROG_CHECKSUM_PROGRESS, startPos, pos, endPos);
		size_t chunkSize = input.getBufferSize();
		if (pos - ((IFile::OffsetType) chunkSize) < endPos)
		{
			chunkSize = ((size_t) (pos - endPos));
		}
		assert(chunkSize >= 0);
		pos -= chunkSize;
		++ revPos;
		input.seek(pos, IFile::FSEEK_BEGINNING);
		input.read(buffer, chunkSize);
		for (size_t i = 0, j = chunkSize - 1; i < chunkSize; i ++, j --)
		{
			checksum = this->makePrevChecksum(checksum, buffer[j]);
		}
	}

	this->markProgress(CRCPROG_CHECKSUM_END, startPos, endPos, endPos);
	input.seek(oldPos, IFile::FSEEK_BEGINNING);
	delete []buffer;
	return checksum;
}



void CRC::markProgress(
	const CRCProgressType& progressType,
	const IFile::OffsetType& startPos,
	const IFile::OffsetType& curPos,
	const IFile::OffsetType& endPos) const
{
	if (this->progressFunction == NULL)
	{
		return;
	}
	(*this->progressFunction)(progressType, startPos, curPos, endPos);
}
