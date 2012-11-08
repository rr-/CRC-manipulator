#include "CRC.h"

/**
 * Constructor...
 */
CRC::CRC() {
}

/**
 * Getters
 */

const CRCType& CRC::getInitialXOR() const {
	return this->initialXOR;
}

const CRCType& CRC::getFinalXOR() const {
	return this->finalXOR;
}

/**
 * Setters
 */

void CRC::setInitialXOR(const CRCType& f) {
	this->initialXOR = f;
}

void CRC::setFinalXOR(const CRCType& f) {
	this->finalXOR = f;
}

void CRC::setProgressFunction(void(*function)(const CRCProgressType& progressType, const IFile::OffsetType& startPosition, const IFile::OffsetType& currentPosition, const IFile::OffsetType& endPosition)) {
	this->progressFunction = function;
}

/**
 * Method that copies the input to the output, outputting
 * computed patch at given position along the way.
 */
void CRC::applyPatch(const CRCType& desiredChecksum, const IFile::OffsetType& desiredPosition, IFile& inputFile, IFile& outputFile, const bool& overwrite) const {
	unsigned char* buffer = new unsigned char[inputFile.getBufferSize()];
	CRCType patch = this->computePatch(desiredChecksum, desiredPosition, inputFile, overwrite);
	inputFile.seek(0, IFile::FSEEK_BEGINNING);
	outputFile.seek(0, IFile::FSEEK_BEGINNING);
	IFile::OffsetType position = inputFile.tell();
	this->markProgress(CRCPROG_PATCH_APPLY_START, 0, position, inputFile.getFileSize());

	// output first half
	while (position < desiredPosition) {
		this->markProgress(CRCPROG_PATCH_APPLY_PROGRESS, 0, position, inputFile.getFileSize());
		size_t chunkSize = inputFile.getBufferSize();
		if (position + static_cast<IFile::OffsetType>(chunkSize) >= desiredPosition) {
			chunkSize = static_cast<size_t>(desiredPosition - position);
		}
		assert(chunkSize >= 0);
		inputFile.read(buffer, chunkSize);
		outputFile.write(buffer, chunkSize);
		position += chunkSize;
	}

	// output patch
	assert(this->getNumBytes() < inputFile.getBufferSize());
	for (size_t i = 0, j = this->getNumBytes() - 1; i < this->getNumBytes(); i ++, j --)
		buffer[i] =(patch >>(i * 8)) & 0xff; // CAUTION: very sensitive part
	outputFile.write(buffer, this->getNumBytes());
	if (overwrite) {
		inputFile.seek(this->getNumBytes(), IFile::FSEEK_AHEAD);
		position += this->getNumBytes();
	}

	// output second half
	while (position < inputFile.getFileSize()) {
		this->markProgress(CRCPROG_PATCH_APPLY_PROGRESS, 0, position, inputFile.getFileSize());
		size_t chunkSize = inputFile.getBufferSize();
		if (position + static_cast<IFile::OffsetType>(chunkSize) >= inputFile.getFileSize()) {
			chunkSize = inputFile.getFileSize() - position;
		}
		assert(chunkSize >= 0);
		inputFile.read(buffer, chunkSize);
		outputFile.write(buffer, chunkSize);
		position += chunkSize;
	}

	this->markProgress(CRCPROG_PATCH_APPLY_END, 0, position, inputFile.getFileSize());
	delete []buffer;
}

/**
 * Computes the checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
const CRCType CRC::computeChecksum(IFile& inputFile) const {
	CRCType checksum = this->getInitialXOR();
	checksum = this->computePartialChecksum(inputFile, 0, inputFile.getFileSize(), checksum);
	return checksum ^ this->getFinalXOR();
}

/**
 * Computes partial checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
const CRCType CRC::computePartialChecksum(IFile& inputFile, const IFile::OffsetType& startPosition, const IFile::OffsetType& endPosition, const CRCType& initialChecksum) const {
	assert(startPosition <= endPosition);
	if (startPosition == endPosition) {
		return initialChecksum;
	}
	CRCType checksum = initialChecksum;
	unsigned char* buffer = new unsigned char[inputFile.getBufferSize()];
	IFile::OffsetType oldPosition = inputFile.tell();
	IFile::OffsetType position = startPosition;
	inputFile.seek(position, IFile::FSEEK_BEGINNING);
	this->markProgress(CRCPROG_PARTIAL_CHECKSUM_START, startPosition, startPosition, endPosition);

	while (position < endPosition) {
		this->markProgress(CRCPROG_PARTIAL_CHECKSUM_PROGRESS, startPosition, position, endPosition);
		size_t chunkSize = inputFile.getBufferSize();
		if (position + static_cast<IFile::OffsetType>(chunkSize) >= endPosition) {
			chunkSize = static_cast<size_t>(endPosition - position);
		}
		assert(chunkSize >= 0);
		inputFile.read(buffer, chunkSize);
		for (size_t i = 0; i < chunkSize; i ++)
			checksum = this->makeNextChecksum(checksum, buffer[i]);
		position += chunkSize;
	}

	this->markProgress(CRCPROG_PARTIAL_CHECKSUM_END, startPosition, endPosition, endPosition);
	inputFile.seek(oldPosition, IFile::FSEEK_BEGINNING);
	delete []buffer;
	return checksum;
}

/**
 * Computes reverse partial checksum of given file.
 * NOTICE: Leaves internal file pointer position intact.
 */
const CRCType CRC::computeReversePartialChecksum(IFile& inputFile, const IFile::OffsetType& startPosition, const IFile::OffsetType& endPosition, const CRCType& initialChecksum) const {
	assert(startPosition >= endPosition);
	if (startPosition == endPosition) {
		return initialChecksum;
	}
	CRCType checksum = initialChecksum;
	unsigned char* buffer = new unsigned char[inputFile.getBufferSize()];
	IFile::OffsetType oldPosition = inputFile.tell();
	IFile::OffsetType position = startPosition;
	IFile::OffsetType reversePosition = endPosition;
	this->markProgress(CRCPROG_PARTIAL_CHECKSUM_START, startPosition, startPosition, endPosition);

	while (reversePosition < startPosition) {
		this->markProgress(CRCPROG_PARTIAL_CHECKSUM_PROGRESS, startPosition, position, endPosition);
		size_t chunkSize = inputFile.getBufferSize();
		if (position - static_cast<IFile::OffsetType>(chunkSize) < endPosition) {
			chunkSize = static_cast<size_t>(position - endPosition);
		}
		assert(chunkSize >= 0);
		position -= chunkSize;
		++ reversePosition;
		inputFile.seek(position, IFile::FSEEK_BEGINNING);
		inputFile.read(buffer, chunkSize);
		for (size_t i = 0, j = chunkSize - 1; i < chunkSize; i ++, j --) {
			checksum = this->makePrevChecksum(checksum, buffer[j]);
		}
	}

	this->markProgress(CRCPROG_PARTIAL_CHECKSUM_END, startPosition, endPosition, endPosition);
	inputFile.seek(oldPosition, IFile::FSEEK_BEGINNING);
	delete []buffer;
	return checksum;
}

void CRC::markProgress(const CRCProgressType& progressType, const IFile::OffsetType& startPosition, const IFile::OffsetType& currentPosition, const IFile::OffsetType& endPosition) const {
	if (this->progressFunction == NULL) {
		return;
	}
	(*this->progressFunction)(progressType, startPosition, currentPosition, endPosition);
}
