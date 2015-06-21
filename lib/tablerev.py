#!/usr/bin/python
#coding: utf-8
import os, sys

class Polynomial:
	def __init__(self, *args, **kwargs):
		self.maxDegree = kwargs.get('maxDegree', None)
		if self.maxDegree is None:
			return None
		self.value = kwargs.get('value', None)
		if self.value is None:
			coefficients = kwargs.get('coefficients', None)
			if coefficients is None:
				return None
			self.value = 0
			for coeff in coefficients:
				self.value |= 1 << coeff

	def toInteger(self):
		return self.value

	def reverse(self):
		revValue = self.value
		tmpValue = self.value >> 1
		size = self.maxDegree - 1;
		while tmpValue:
			revValue <<= 1;
			revValue |= tmpValue & 1;
			size -= 1
			tmpValue >>= 1
		revValue <<= size
		revValue &= (1 << self.maxDegree) - 1;
		return Polynomial(value=revValue, maxDegree=self.maxDegree)

class CRC():
	def __init__(self):
		pass

class CRC16IBM(CRC):
	def __init__(self):
		self.initialXOR = 0
		self.finalXOR = 0
		self.polynomial = Polynomial(coefficients=[0, 2, 15], maxDegree=16)
		self.revPolynomial = self.polynomial.reverse()

		self.lookupTable = {}
		for n in range(256):
			crc = 0
			for k in range(8):
				if(crc ^ (n >> k)) & 1:
					crc = (crc >> 1) ^ self.revPolynomial.toInteger()
				else:
					crc >>= 1
			self.lookupTable[n] = crc
		self.invLookupTable = [0 for n in range(256)]

	def makeNextChecksum(self, prevChecksum, c):
		tmp = (prevChecksum ^ c) & 0xff
		ret = (prevChecksum >> 8) ^ self.lookupTable[tmp]
		ret &= 0xffff
		return ret

	def makePrevChecksum(self, nextChecksum, c):
		tmp = (nextChecksum >> 8) & 0xff
		ret = c ^ self.invLookupTable[tmp] ^ (nextChecksum << 8)
		ret &= 0xffff
		return ret

	def reverseIndex(self, i):
		return i >> 8

class CRC16CCITT(CRC):
	def __init__(self):
		self.initialXOR = 0
		self.finalXOR = 0
		self.polynomial = Polynomial(coefficients=[0, 5, 12], maxDegree=16)
		self.revPolynomial = self.polynomial.reverse()

		self.lookupTable = {}
		for n in range(256):
			crc = 0
			for k in range(8):
				if(crc ^ (n << (8 + k))) & 0x8000:
					crc = (crc << 1) ^ self.polynomial.toInteger()
				else:
					crc <<= 1
			self.lookupTable[n] = crc
		self.invLookupTable = [0 for n in range(256)]

	def makeNextChecksum(self, prevChecksum, c):
		tmp = ((prevChecksum >> 8) ^ c) & 0xff
		ret = (prevChecksum << 8) ^ self.lookupTable[tmp]
		ret &= 0xffff
		return ret

	def makePrevChecksum(self, nextChecksum, c):
		tmp = nextChecksum & 0xff
		ret = (c << 8) ^ self.invLookupTable[tmp] ^ (nextChecksum << 8) ^ (nextChecksum >> 8)
		ret &= 0xffff
		return ret

	def reverseIndex(self, i):
		return i

class CRC32POSIX(CRC):
	def __init__(self):
		self.initialXOR = 0
		self.finalXOR = 0
		self.polynomial = Polynomial(coefficients=[0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26], maxDegree=32)
		self.revPolynomial = self.polynomial.reverse()

		self.lookupTable = {}
		for n in range(256):
			t = n << 24
			for k in range(8):
				if t & 0x80000000:
					t = (t << 1) ^ self.polynomial.toInteger()
				else:
					t = (t << 1)
			self.lookupTable[n] = t
		self.invLookupTable = [0 for n in range(256)]

	def makeNextChecksum(self, prevChecksum, c):
		tmp = ((prevChecksum >> 24) ^ c) & 0xff
		ret = (prevChecksum << 8) ^ self.lookupTable[tmp]
		ret &= 0xffffffff
		return ret

	def makePrevChecksum(self, nextChecksum, c):
		tmp = nextChecksum & 0xff
		ret = (c << 24) ^ self.invLookupTable[tmp] ^ (nextChecksum << 24) ^ (nextChecksum >> 8)
		ret &= 0xffffffff
		return ret

	def reverseIndex(self, i):
		return i

def print_hash(hash, crc):
	sys.stdout.write(('0x{0:0' + str(crc.polynomial.maxDegree >> 2) + 'x}').format(hash))

crc = CRC32POSIX()

def patchTables(crc):
	chosen = 0xffffffff & ((1 << crc.polynomial.maxDegree) - 1)
	tmp2 = [0 for n in range(256)]
	crc.invLookupTable = [chosen for n in range(256)]
	for n in range(256):
		val1 = crc.makeNextChecksum(chosen, n)
		val2 = crc.makePrevChecksum(val1, n)
		index = crc.reverseIndex(val1)
		tmp2[index & 0xff] = val2
	crc.invLookupTable = tmp2

patchTables(crc)

print('Checking table correctness...')
#choose two random bits
for bit1 in range(crc.polynomial.maxDegree):
	for bit2 in range(crc.polynomial.maxDegree):
		chosen = (1 << bit1) | (1 << bit2)
		for n in range(256):
			val1 = crc.makeNextChecksum(chosen, n)
			val2 = crc.makePrevChecksum(val1, n)
			if val2 != chosen:
				raise RuntimeError('{0:08x} != {1:08x}'.format(val2, chosen))
print('Passed')

for n in range(256):
	print_hash(crc.invLookupTable[n], crc)
	sys.stdout.write(',')
	if n % 8 == 7:
		print('')
