import time
import serial
from pprint import pprint

def Checksum(frame):
	checksum = 0
	checksum = sum(frame)
	checksum = (checksum & 0xFF)
	#print "chk "+hex(checksum),
	return  checksum
	

def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)

def main():
	ser = serial.Serial("/dev/ttyUSB1",115200, timeout=0.1)
	
	motorA = 0
	motorB = 0	

	while 1:

		for x in xrange(0,254):
			motorA = x
			motorB = x

			frame = bytearray()
			frame.append(0xAC)
			frame.append(0x02) 
			frame.append(motorA)
			frame.append(motorB)
			frame.append(Checksum(frame))
			
			frame_string = "> "
			for i in xrange(0,len(frame)):
				frame_string +=  " " +hex(frame[i])
			
			print frame_string		
			ser.write(frame)

			print ser.readline()


if __name__ == "__main__":
	main()
