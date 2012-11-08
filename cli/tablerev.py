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

	def toInteger (self):
		return self.value

	def reverse (self):
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
		for n in xrange (0, 256):
			crc = 0
			for k in xrange(0, 8):
				if (crc ^ (n >> k)) & 1:
					crc = (crc >> 1) ^ self.revPolynomial.toInteger ()
				else:
					crc >>= 1
			self.lookupTable[n] = crc
		self.invLookupTable = [0 for n in range (0, 256)]

	def makeNextChecksum (self, prevChecksum, c):
		tmp = (prevChecksum ^ c) & 0xff
		ret = (prevChecksum >> 8) ^ self.lookupTable[tmp]
		ret &= 0xffff
		return ret

	def makePrevChecksum (self, nextChecksum, c):
		tmp = (nextChecksum >> 8) & 0xff
		ret = c ^ self.invLookupTable[tmp] ^ (nextChecksum << 8)
		ret &= 0xffff
		return ret



class CRC16CCITT(CRC):
	def __init__(self):
		self.initialXOR = 0
		self.finalXOR = 0
		self.polynomial = Polynomial(coefficients=[0, 5, 12], maxDegree=16)
		self.revPolynomial = self.polynomial.reverse()

		self.lookupTable = {}
		for n in xrange (0, 256):
			crc = 0
			for k in xrange(0, 8):
				if (crc ^ (n << (8 + k))) & 0x8000:
					crc = (crc << 1) ^ self.polynomial.toInteger ()
				else:
					crc <<= 1
			self.lookupTable[n] = crc
		self.invLookupTable = [0 for n in range (0, 256)]

	def makeNextChecksum (self, prevChecksum, c):
		tmp = ((prevChecksum >> 8) ^ c) & 0xff
		ret = (prevChecksum << 8) ^ self.lookupTable[tmp]
		ret &= 0xffff
		return ret

	def makePrevChecksum (self, nextChecksum, c):
		tmp = nextChecksum & 0xff
		ret = (c << 8) ^ self.invLookupTable[tmp] ^ (nextChecksum << 8) ^ (nextChecksum >> 8)
		ret &= 0xffff
		return ret



crc = CRC16IBM()

def patchTables(where):
	chosen= 0x2346
	tmp2 = [0 for n in xrange(0, 256)]
	where.invLookupTable = [chosen for n in xrange(0, 256)]
	for n in xrange(0, 256):
		val1 = where.makeNextChecksum (chosen, n)
		val2 = where.makePrevChecksum (val1, n)
		index = val1 >>8#reversed index function
		tmp2[index & 0xff] = val2
	where.invLookupTable = tmp2

	chosen = 0xA945
	success = True
	for n in xrange(0, 256):
		val1 = where.makeNextChecksum (chosen, n)
		val2 = where.makePrevChecksum (val1, n)
		if val2 != chosen:
			success = False
	print 'Success:', success

patchTables(crc)

print "Reverse lookup table test:\n"
for chosen in xrange(0, 0x10000):
	sys.stderr.write('{0:04x} '.format(chosen))
	for n in range(0, 256):
		val1 = crc.makeNextChecksum (chosen, n)
		val2 = crc.makePrevChecksum (val1, n)
		if val2 != chosen:
			raise Exception('{0:04x} != {1:04x}'.format(val2, chosen))
		#print 'f({0:04x},{1:02x})={2:4x}; f^-1({2:04x},{1:02x})={3:04x}  '.format(chosen, n, val1, val2),
		#if n % 4 == 3:
		#	print

for n in xrange(0, 256):
	print '0x{0:04x},'.format(crc.invLookupTable[n]),
	if n % 16 == 15:
		print