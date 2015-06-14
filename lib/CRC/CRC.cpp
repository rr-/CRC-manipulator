#include "CRC.h"

CRC::CRC()
{
}

CRC::~CRC()
{
}

const CRCType &CRC::getInitialXOR() const
{
	return this->initialXOR;
}

const CRCType &CRC::getFinalXOR() const
{
	return this->finalXOR;
}

void CRC::setInitialXOR(const CRCType &f)
{
	this->initialXOR = f;
}

void CRC::setFinalXOR(const CRCType &f)
{
	this->finalXOR = f;
}

void CRC::setProgressFunction(
	void(*function)(const CRCProgressType &progressType,
	const File::OffsetType &startPos,
	const File::OffsetType &curPos,
	const File::OffsetType &endPos))
{
	this->progressFunction = function;
}

/**
 * Method that copies the input to the output, outputting
 * computed patch at given position along the way.
 */
void CRC::applyPatch(
	const CRCType &finalChecksum,
	const File::OffsetType &finalPos,
	File &input,
	File &output,
	const bool &overwrite) const
{
	unsigned char *buffer = new unsigned char[input.getBufferSize()];
	CRCType patch = this->computePatch(
		finalChecksum,
		finalPos,
		input,
		overwrite);

	input.seek(0, File::FSEEK_BEGINNING);
	//output.seek(0, File::FSEEK_BEGINNING);
	File::OffsetType pos = input.tell();
	this->markProgress(CRCPROG_WRITE_START, 0, pos, input.getFileSize());

	//output first half
	while (pos < finalPos)
	{
		this->markProgress(CRCPROG_WRITE_PROGRESS, 0, pos, input.getFileSize());
		size_t chunkSize = input.getBufferSize();
		if (pos + ((File::OffsetType) chunkSize) >= finalPos)
		{
			chunkSize = ((size_t) (finalPos - pos));
		}
		input.read(buffer, chunkSize);
		output.write(buffer, chunkSize);
		pos += chunkSize;
	}

	//output patch
	assert(this->getNumBytes() < input.getBufferSize());
	for (size_t i = 0, j = this->getNumBytes() - 1;
		i < this->getNumBytes();
		i ++, j --)
	{
		//CAUTION: very sensitive part
		buffer[i] = (patch >> (i << 3)) & 0xff;
	}
	output.write(buffer, this->getNumBytes());
	if (overwrite)
	{
		pos += this->getNumBytes();
		input.seek(pos, File::FSEEK_BEGINNING);
	}

	//output second half
	while (pos < input.getFileSize())
	{
		this->markProgress(CRCPROG_WRITE_PROGRESS, 0, pos, input.getFileSize());
		size_t chunkSize = input.getBufferSize();
		if (pos + ((File::OffsetType) chunkSize) >= input.getFileSize())
		{
			chunkSize = input.getFileSize() - pos;
		}
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
CRCType CRC::computeChecksum(File &input) const
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
CRCType CRC::computePartialChecksum(
	File &input,
	const File::OffsetType &startPos,
	const File::OffsetType &endPos,
	const CRCType &initialChecksum) const
{
	assert(startPos <= endPos);
	if (startPos == endPos)
	{
		return initialChecksum;
	}
	CRCType checksum = initialChecksum;
	unsigned char *buffer = new unsigned char[input.getBufferSize()];
	File::OffsetType oldPos = input.tell();
	File::OffsetType pos = startPos;
	input.seek(pos, File::FSEEK_BEGINNING);
	this->markProgress(CRCPROG_CHECKSUM_START, startPos, startPos, endPos);

	while (pos < endPos)
	{
		this->markProgress(CRCPROG_CHECKSUM_PROGRESS, startPos, pos, endPos);
		size_t chunkSize = input.getBufferSize();
		if (pos + ((File::OffsetType) chunkSize) >= endPos)
		{
			chunkSize = ((size_t) (endPos - pos));
		}
		input.read(buffer, chunkSize);
		for (size_t i = 0; i < chunkSize; i ++)
		{
			checksum = this->makeNextChecksum(checksum, buffer[i]);
		}
		pos += chunkSize;
	}

	this->markProgress(CRCPROG_CHECKSUM_END, startPos, endPos, endPos);
	input.seek(oldPos, File::FSEEK_BEGINNING);
	delete []buffer;
	return checksum;
}

/**
 * Computes reverse partial checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
CRCType CRC::computeReversePartialChecksum(
	File &input,
	const File::OffsetType &startPos,
	const File::OffsetType &endPos,
	const CRCType &initialChecksum) const
{
	assert(startPos >= endPos);
	if (startPos == endPos)
	{
		return initialChecksum;
	}
	CRCType checksum = initialChecksum;
	unsigned char *buffer = new unsigned char[input.getBufferSize()];
	File::OffsetType oldPos = input.tell();
	File::OffsetType pos = startPos;
	this->markProgress(CRCPROG_CHECKSUM_START, startPos, startPos, endPos);

	while (pos > endPos)
	{
		this->markProgress(CRCPROG_CHECKSUM_PROGRESS, startPos, pos, endPos);
		size_t chunkSize = input.getBufferSize();
		if (pos - ((File::OffsetType) chunkSize) < endPos)
		{
			chunkSize = ((size_t) (pos - endPos));
		}
		pos -= chunkSize;
		input.seek(pos, File::FSEEK_BEGINNING);
		input.read(buffer, chunkSize);
		for (size_t i = 0, j = chunkSize - 1; i < chunkSize; i ++, j --)
		{
			checksum = this->makePrevChecksum(checksum, buffer[j]);
		}
	}

	this->markProgress(CRCPROG_CHECKSUM_END, startPos, endPos, endPos);
	input.seek(oldPos, File::FSEEK_BEGINNING);
	delete []buffer;
	return checksum;
}

void CRC::markProgress(
	const CRCProgressType &progressType,
	const File::OffsetType &startPos,
	const File::OffsetType &curPos,
	const File::OffsetType &endPos) const
{
	if (this->progressFunction == NULL)
	{
		return;
	}
	(*this->progressFunction)(progressType, startPos, curPos, endPos);
}
