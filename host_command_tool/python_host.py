#!/usr/bin/python
from serial import *
from base64 import *
from cmd import Cmd
from struct import *

DEBUG = True
CMD_INIT = '\x7F'
CMD_GET = 0x00
CMD_GETVERS = 0x01
CMD_GETID = 0x02
CMD_READMEM = 0x11
CMD_GO = 0x21
CMD_WRITEMEM = 0x31
CMD_ERASE = 0x43
CMD_WRITE_PROTECT = 0x63
CMD_WRITE_UNPROTECT = 0x73
CMD_READ_PROTECT = 0x82
CMD_READ_UNPROTECT = 0x92
ACK = 0x79
NAK = 0x1F

def CMD_FULL(CMD):
	comp = ~CMD&0xFF
	return bytearray([CMD, comp])

def byteToInt8(byte):
	return unpack("B", byte)[0] # index because result of unpack is a tuple

def isACK(reply):
	return ACK == byteToInt8(reply[0])

def isNAK(reply):
	return NAK == byteToInt8(reply[0])

def inRange(addr, type):
	if type == CMD_READMEM: # readMem
		return True #(addr >= 0x08000000 and addr < 0x08040000) or (addr >= 0x1fffd800 and addr < 0x1ffff800) or (addr >= 0x1ffff800 and addr < 0x1ffff810) or (addr >= 0x20001400 and addr < 0x2000a000)
	elif type == CMD_GO:
		return True # (addr >= 0x08000000 and addr < 0x08040000) or (addr >= 0x20001400 and addr < 0x2000a000)
	elif type == CMD_WRITEMEM:
		return (addr >= 0x08000000 and addr < 0x08040000) or (addr >= 0x20001400 and addr < 0x2000a000)
	elif type == CMD_ERASE:
		return True
	elif type == CMD_EXT_ERASE:
		return True
	elif type == CMD_WRITE_PROTECT:
		return True
	else:
		return False


def cmdGetStr(x):
	return {
		0: "Bootloader Version",
		1: "Get",
		2: "Get Version and Read P protection status",
		3: "Get ID",
		4: "Read Memory",
		5: "Go",
		6: "Write Memory",
		7: "Erase or Extended Erase",
		8: "Write Protect",
		9: "Write Unprotect",
		10: "Readout Protect",
		11: "Readout Unprotect",
	}[x]

def cmdGetVersStr(x):
	return {
		0: "Bootloader Version",
		1: "Option Byte 1",
		2: "Option Byte 2",
	}[x]

def cmdGetIDStr(x):
	return {
		0: "PID MSB",
		1: "PID LSB",
	}[x]

class myCmd(Cmd):

	DEVICE = '/dev/ttyUSB0'
	BAUD = 38400
	RUNNING = False
	RETRIES = 3
	TIMEOUT = 0.2
	FILE = '../lab1/build/lab1.bin'
	PAGE_SZ = 2048
	reply = bytearray()

	def clearReply(self):
		self.reply = bytearray()

	def waitACK(self):
		for i in range(self.RETRIES):
			self.reply = self.ser.read()
			if len(self.reply) == 0:
				print "%d of %d retries has failed. Trying again." % (i + 1, self.RETRIES)
				continue
			elif isNAK(self.reply):
				print "A NAK was received. Please try again later."
				return 1
			elif isACK(self.reply):
				if DEBUG:
					print "ACK--------------------------------------------------------------------------ACK"
				return 2
			elif i == self.RETRIES - 1:
				print "The request timed out. This could be due to incomplete packets or the board may not be responding."
				return 0
			else:
				print "len = %d" % len(self.reply)
				for i in range(len(self.reply)):
					print "i = %d" % ord(self.reply[i])
		return 0


	def reset(self):
		from time import sleep
		self.RUNNING = False
		self.ser.close()
		print "Resetting"
		for i in range(5):
			time.sleep(0.1)
			print ".",
		print ""
		self.ser = Serial(self.DEVICE, self.BAUD, parity = PARITY_EVEN, timeout = self.TIMEOUT, write_timeout = self.TIMEOUT)
		try:
			sent = self.ser.write(CMD_INIT)
		except SerialTimeoutException:
			print "The write timed out. Please run init again."
		else:
			if sent == 1:
				if self.waitACK() != 2:
					print "An error occured during the system reset. Please run init again."
					self.clearReply()
					return

				print "System reset."
				self.RUNNING = True
				self.clearReply()
			else:
				print "The write failed somehow. Please run init again."

	def do_s(self, args):
		argv = args.split()
		self.ser.write(bytearray.fromhex(argv[0]))

	def do_r(self, args):
		reply = self.ser.read()
		if len(reply) == 0:
			print "no reply"
		elif isNAK(reply):
			print "NAK"
		elif isACK(reply):
			print "ACK"
		else:
			print byteToInt8(reply)

	def do_init(self, args):
		"""Sends the init byte to tell the bootloader to be ready to receive instructions."""
		if self.RUNNING:
			print "Device is already running."
			return
		try:
			self.ser = Serial(self.DEVICE, self.BAUD, parity = PARITY_EVEN, timeout = self.TIMEOUT, write_timeout = self.TIMEOUT)
		except ValueError:
			print "The baud rate you selected, %d, was out of range. Please change it and try again." % self.BAUD
		except SerialException:
			print "The device you specified, %s, could not be found. Please change it and try again." % self.DEVICE
		else:
			try:
				sent = self.ser.write(CMD_INIT)
				print "Sending 0x7F over UART to init the bootloader."
			except SerialTimeoutException:
				print "The write timed out, please try again later."
			else:
				if sent == 1:
					if self.waitACK() != 2:
						print "Error in init. Please try again later."
						return

					print "ACK received. Bootloader is now running and will accept commands."
					self.RUNNING = True
				else:
					print "The write failed somehow. Please try again later."
			finally:
				self.clearReply()


	def do_get(self, args):
		"""Shows the bootloader version number as well as the supported commands."""
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return
		try:
			sent = self.ser.write(CMD_FULL(CMD_GET))
		except SerialTimeoutException:
			print "The write timed out, please try again later."
		else:
			if sent == 2:
				if self.waitACK() != 2:
					print "Error in Get! Please try again later."
					self.clearReply()
					return
				self.reply = self.ser.read() # read nBytes
				nBytes = byteToInt8(self.reply)
				print "%02d:\t0x%02x\tN = %d" % (0, nBytes, nBytes)

				self.reply = self.ser.read(nBytes + 1)
				for i in range(nBytes + 1):
					value = byteToInt8(self.reply[i])
					print "%02d:\t0x%02x\t%s" % (i + 1, value, cmdGetStr(i))

				self.waitACK()
			else:
				print "The write failed somehow. Please try again later."
		finally:
			self.clearReply()


	def do_getVers(self, args):
		"""Shows the bootloader version and read protection status."""
		replySz = 5
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return
		try:
			sent = self.ser.write(CMD_FULL(CMD_GETVERS))
		except SerialTimeoutException:
			print "The write timed out, please try again later."
		else:
			if sent == 2:
				if self.waitACK() != 2:
					print "Error in GetVers! Please try again later."
					self.clearReply()
					return

				self.reply = self.ser.read(replySz - 2)
				nBytes = len(self.reply)
				for i in range(nBytes):
					value = byteToInt8(self.reply[i])
					print "%02d:\t0x%02x\t%s" % (i , value, cmdGetVersStr(i))

				self.waitACK()
			else:
				print "The write failed somehow. Please try again later."
		finally:
			self.clearReply()


	def do_getID(self, args):
		"""Shows the PID of the board"""
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return
		try:
			sent = self.ser.write(CMD_FULL(CMD_GETID))
		except SerialTimeoutException:
			print "The write timed out, please try again later."
		else:
			if sent == 2:
				if self.waitACK() != 2:
					print "Error in GetID! Please try again later."
					self.clearReply()
					return

				self.reply = self.ser.read() # recv nBytes
				nBytes = byteToInt8(self.reply[0])
				print "%02d:\t0x%02x\tN = %d" % (0, nBytes, nBytes)

				self.reply = self.ser.read(nBytes + 1)
				for i in range(nBytes + 1):
					value = byteToInt8(self.reply[i])
					print "%02d:\t0x%02x\t%s" % (i + 1, value, cmdGetIDStr(i))

				self.waitACK()
			else:
				print "The write failed somehow. Please try again later."
		finally:
			self.clearReply()


	def do_readMem(self, args):
		"""Reads the memory at an address.
		\r\t<hex address> <nBytes> - reads nBytes bytes of data from the address given, the address can be a maximum of 4 bytes and nBytes can be from 0-255"""
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return

		argv = args.split()
		if (len(argv) != 2):
			print "Wrong number of args! You can only have 2."
			return
		elif argv[0] == "flash":
			argv[0] = "08000000"
		elif argv[0] == "sram":
			argv[0] = "20001400"
		elif len(argv[0]) > 8:
			print "The address you entered was too long! Try again."
			return

		try:
			addr = bytearray.fromhex(argv[0])
			nBytes = int(argv[1], 10) - 1 # enter 1-256
		except ValueError:
			print "The address you entered was not a valid hex number, or the number of bytes you entered was not a valid decimal number."
		else:

			while len(addr) < 4:
				addr = bytearray([0x0]) + addr
			if nBytes < 0 or nBytes > 255: # read 1-256 bytes, represented as 0x00 - 0xff
				print "The nBytes you entered was out of range."
			elif not inRange(unpack(">I", addr)[0], CMD_READMEM):
				print "The address you entered %08x was out of range, or was not a valid address for this command." % unpack(">I", addr)[0]
			else:
				try:
					sent = self.ser.write(CMD_FULL(CMD_READMEM))
				except SerialTimeoutException:
					print "The write timed out, please try again later."
				else:
					if sent == 2:
						if self.waitACK() != 2:
							print "RDP active, or no reply from the bootloader."
							self.clearReply()
							return

						lrc = 0
						for i in addr:
							lrc ^= i
						addr.append(lrc&0xFF)
						self.ser.write(addr)

						if self.waitACK() != 2:
							print "Address not valid"
							self.clearReply()
							return

						n = bytearray([nBytes, ~nBytes & 0xFF])
						self.ser.write(n)

						if self.waitACK() != 2:
							print "Checksum error"
							self.clearReply()
							return

						self.reply = self.ser.read(nBytes + 1)

						if len(self.reply) != nBytes + 1:
							print "range error!"
							self.reset()
							return

						addr = unpack('>I', addr[:4])
						for i in range(len(self.reply)):
							print "0x%08x = 0x%02x" % (addr[0] + i, byteToInt8(self.reply[i]))

					else:
						print "The write failed somehow. Please try again later."
		finally:
			self.clearReply()


	def do_go(self, args):
		"""Jumps to user code in RAM or flash memory at given address.
		\r\t<hex address> - jumps to code at address, the address can be a maximum of 4 bytes"""
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return

		argv = args.split()
		if (len(argv) != 1):
			print "Wrong number of args! You can only have 1."
			return
		elif argv[0] == "flash":
			argv[0] = "08000000"
		elif argv[0] == "sram":
			argv[0] = "20001400"
		elif len(argv[0]) > 8:
			print "The address you entered was too long! Try again."
			return

		try:
			addr = bytearray.fromhex(argv[0])
		except ValueError:
			print "The address you entered was not a valid hex number."
		else:
			while len(addr) < 4:
				addr = bytearray([0x0]) + addr
			if not inRange(unpack(">I", addr)[0], CMD_GO):
				print "The address you entered %x was out of range, or was not a valid address for this command." % unpack(">I", addr)[0]
			else:
				try:
					sent = self.ser.write(CMD_FULL(CMD_GO))
				except SerialTimeoutException:
					print "The write timed out, please try again later."
				else:
					if sent == 2:
						if self.waitACK() != 2:
							print "RDP active, or no reply from the bootloader."
							self.clearReply()
							return

						lrc = 0
						for i in addr:
							lrc ^= i
						addr.append(lrc&0xFF)
						self.ser.write(addr)

						if self.waitACK() != 2:
							print "Address not valid"
							self.clearReply()
							return

						print "Jumping to user code at 0x%08x." % unpack('>I', addr[:4])
					else:
						print "The write failed somehow. Please try again later."
		finally:
			self.clearReply()


	def do_writeMem(self, args):
		"""Writes to the memory at an address.
		\r\t<hex address> - writes the set file to the address given, the address can be a maximum of 4 bytes"""
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return

		argv = args.split()
		if (len(argv) != 1):
			print "Wrong number of args! You can only have 1."
			return
		elif argv[0] == "flash":
			argv[0] = "08000000"
		elif argv[0] == "sram":
			argv[0] = "20001400"
		elif len(argv[0]) > 8:
			print "The address you entered was too long! Try again."
			return

		try:
			addr = bytearray.fromhex(argv[0])
			bin = open(self.FILE, 'r')
		except ValueError:
			print "The address you entered was not a valid hex number."
		except IOError:
			print "The file you specified could not be opened."
		else:
			while len(addr) < 4:
				addr = bytearray([0x0]) + addr
			addrVal = unpack(">I", addr)[0]
			fileSz = 0

			while bin.read(1):
				fileSz += 1

			if not inRange(addrVal, CMD_WRITEMEM):
				print "The address %08x you entered was out of range." % addrVal
			elif not inRange(addrVal + fileSz, CMD_WRITEMEM):
				print "The file was too big to fit in the memory at address %08x." % addrVal
			else:
				chunks = fileSz // 256

				if fileSz % 256 == 0:
					rounds = chunks
				else:
					rounds = chunks + 1
				# print "chunks = %d, rounds = %d" % (chunks, rounds)
				bin.seek(0)

				for i in range(rounds):
					try:
						sent = self.ser.write(CMD_FULL(CMD_WRITEMEM))
					except SerialTimeoutException:
						print "The write timed out, please try again later."
					else:
						if sent == 2:
							if self.waitACK() != 2:
								print "RDP active, or no reply from the bootloader."
								self.clearReply()
								return

							addr = bytearray(pack(">I", addrVal + i * 256))
							# print "addr = %d%d%d%d, addrVal = %x" % (addr[0], addr[1], addr[2], addr[3], addrVal)
							lrc = 0
							for byt in addr:
								lrc ^= byt
							addr.append(lrc&0xFF)
							self.ser.write(addr)

							if self.waitACK() != 2:
								print "Address not valid"
								self.clearReply()
								return

							nBytes = 255

							if i == chunks:
								nBytes = fileSz - chunks * 256 - 1
							# print "nbytes = %d" % nBytes

							n = bytearray([nBytes & 0xFF])

							lrc = 0 ^ n[0]
							for j in range(nBytes + 1):
								ch = bin.read(1)
								if len(ch) != 1:
									break
								lrc ^= ord(ch)
								n.append(ch)
								print "0x%08x = %02x" % (addrVal + (i * 256) + j, ord(ch))

							n.append(lrc&0xff)
							print "Writing %d bytes in file %s to address at %08x." % (fileSz, self.FILE, addrVal)
							self.ser.write(n)

							if self.waitACK() != 2:
								print "something went wrong"
								self.clearReply()
								return
						else:
							print "The write failed somehow. Please try again later."
		finally:
			self.clearReply()


	def do_eraseMem(self, args):
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return

		argv = args.split()
		if (len(argv) == 1): # erase all
			print "Erasing all memory"
		elif (len(argv) < 1):
			pass
		elif len(argv[0]) > 8:
			print "The address you entered was too long! Try again."
			return

		try:
			sent = self.ser.write(CMD_FULL(CMD_ERASE))
		except SerialTimeoutException:
			print "The write timed out, please try again later."
		else:
			if sent == 2:
				if self.waitACK() == 2:
					pass
				else:
					return
				self.waitACK();
			else:
				print "The write failed somehow. Please try again later."
		self.clearReply()


	def do_writeP(self, args):
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return
		mode = None
		count = 256
		start = 0
		argv = args.split()
		if (len(argv) == 0):
			print "Write protecting all pages."
			mode = 0
		elif argv[0] == "-s": # give num and adds
			try:
				count = int(argv[1], 16)
			except ValueError:
				print "Not an integer was given. Please try again."
			else:
				if count < 1 or count > 256:
					print "You specified %02x pages to protect, which is out of the range of 0x0 - 0xff." % count
					return
				if len(argv) != count + 2:
					print "Wrong number of arguments! You specified %02x pages to protect but gave %02x." % (count, len(argv) - 2)
					return
				mode = 1
		elif argv[0] == "-r": # give a range
			if len(argv) != 3:
				print "Wrong number of arguments! You need to give both a start and a range for the range version of this command."
				return
			try:
				start = int(argv[1], 16)
				count = int(argv[2], 16)
			except ValueError:
				print "Not an integer was given. Please try again."
			else:
				if start < 0 or start > 127 or count <= 0 or start + count > 127:
					print "You specified %02x pages to protect starting from page %02x, which is out of the range of 0x0 - 0x7f." % (count, start)
					return
				mode = 2
		else:
			print "Unrecognized arguments! Please try again."
			return

		try:
			sent = self.ser.write(CMD_FULL(CMD_WRITE_PROTECT))
		except SerialTimeoutException:
			print "The write timed out, please try again later."
		else:
			if sent == 2:
				if self.waitACK() != 2:
					print "Write Protect command unsuccessful. Most likely reason for this is that RDP is active. Disable it with the readU command."
					self.clearReply()
					return

				if mode == 0:
					self.ser.send()
					lrc = 0
					for i in range(256):
						pass
				elif mode == 1:
					pass
				elif mode == 2:
					pass


				if self.waitACK() != 2:
					print "There was a problem removing Write Protection for Flash memory. Please try again."
					self.clearReply()
					return

				print "Write Unprotect command successful. All Flash memory sectors can now be written to."
				self.reset()
			else:
				print "The write failed somehow. Please try again later."
		self.clearReply()


	def do_writeU(self, args):
		"""Used to disable the flash memory write protection, also resets the system on success."""
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return
		try:
			sent = self.ser.write(CMD_FULL(CMD_WRITE_UNPROTECT))
		except SerialTimeoutException:
			print "The write timed out, please try again later."
		else:
			if sent == 2:
				if self.waitACK() != 2:
					print "Write Unprotect command unsuccessful. Most likely reason for this is that RDP is active. Disable it with the readU command."
					self.clearReply()
					return

				if self.waitACK() != 2:
					print "There was a problem removing Write Protection for Flash memory. Please try again."
					self.clearReply()
					return

				print "Write Unprotect command successful. All Flash memory sectors can now be written to."
				self.reset()
			else:
				print "The write failed somehow. Please try again later."
		self.clearReply()


	def do_readP(self, args):
		"""Used to enable the flash memory read protection, also resets the system on success."""
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return
		try:
			sent = self.ser.write(CMD_FULL(CMD_READ_PROTECT))
		except SerialTimeoutException:
			print "The write timed out, please try again later."
		else:
			if sent == 2:
				if self.waitACK() != 2:
					print "Read Protect command unsuccessful. Most likely reason for this is that RDP is active. Disable it with the readU command."
					self.clearReply()
					return

				if self.waitACK() != 2:
					print "There was a problem setting Read Protection for Flash memory. Please try again."
					self.clearReply()
					return

				print "Read Protect command successful. All Flash memory sectors have read protection enabled."
				self.reset()
			else:
				print "The write failed somehow. Please try again later."
		self.clearReply()


	def do_readU(self, args):
		"""Used to disable the flash memory read protection, also erases all the flash memory and resets the system on success."""
		if not self.RUNNING:
			print "Device is not running. Please init it first."
			return
		try:
			sent = self.ser.write(CMD_FULL(CMD_READ_UNPROTECT))
		except SerialTimeoutException:
			print "The write timed out, please try again later."
		else:
			if sent == 2:
				if self.waitACK() != 2:
					print "Read Unprotect command unsuccessful. There seems to be a problem with disabling RDP, which should be temporary. Try again later."
					self.clearReply()
					return

				if self.waitACK() != 2:
					print "There was a problem removing Read Protection for Flash memory. Please try again."
					self.clearReply()
					return

				print "Read Unprotect command successful. All Flash memory sectors have been erased, read protection disabled, and the RDP has been deactivated."
				self.reset()
			else:
				print "The write failed somehow. Please try again later."
		self.clearReply()


	def do_options(self, args):
		"""Changes options like device to connect to and baud rate.
		\r\tlists the current settings
		\r\t-b <baud rate> - changes the baud rate to the specified value
		\r\t-d <path> - changes the device to connect to the specified path
		\r\t-f <file> - changes the file we write to the board
		\r\t-r <retries> - changes the number of times to retry when the board does not respond
		\r\t-t <timeout> - changes the timeout for input/output"""
		argv = args.split()
		if len(argv) == 0:
			print "DEVICE:\t\t%s\nBAUD:\t\t%d\nRETRIES:\t%d\nTIMEOUT:\t%f\nFILE:\t\t%s" % (self.DEVICE, self.BAUD, self.RETRIES, self.TIMEOUT, self.FILE)
		elif len(argv) < 2:
			print "Not enough args! You need at least 2."
		elif argv[0] == '-b':
			try:
				success = int(argv[1])
			except ValueError:
				print "You didn't enter a valid number."
			else:
				print "Setting BAUD to %d." % success
				self.BAUD = success
		elif argv[0] == '-d':
			print "Setting DEVICE to %s." % argv[1]
			self.DEVICE = argv[1]
		elif argv[0] == '-f':
			print "Setting FILE to %s." % argv[1]
			self.FILE = argv[1]
		elif argv[0] == '-r':
			try:
				success = int(argv[1])
			except ValueError:
				print "You didn't enter a valid number."
			else:
				print "Setting RETRIES to %d." % success
				self.RETRIES = success
		elif argv[0] == '-t':
			try:
				success = float(argv[1])
			except ValueError:
				print "You didn't enter a valid number."
			else:
				print "Setting TIMEOUT to %f." % success
				self.TIMEOUT = success
		else:
			print "Unrecognized command. Please try again."
		return

	def do_exit(self, args):
		"""Exits the tool."""
		print "Exiting!"
		if self.RUNNING:
			self.RUNNING = False
			self.ser.close()
		raise SystemExit
		return

if __name__ == '__main__':
	prompt = myCmd()
	prompt.prompt = '> '
	prompt.cmdloop('Starting tool for bootloader interaction.')
