#!/usr/bin/env python
import pygame
import time
import serial
from pygame.locals import *


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
	ser = serial.Serial("/dev/ttyACM0",115200, timeout=0.1)
	pygame.init()
	joysticks = []
	Axes_value = [0,0,0,0,0,0] 

	for i in range(0, pygame.joystick.get_count()):
		joysticks.append(pygame.joystick.Joystick(i))
		joysticks[-1].init()
		print "Detected joystick '",joysticks[-1].get_name(),"'"
	

	while 1:
		for event in pygame.event.get():
			if event.type == QUIT:
				print "Received event 'Quit', exiting."
				return
			elif event.type == JOYAXISMOTION:
				axis =  event.axis
				value = event.value
				scaled_value = int(translate(value, -1, 1, 0, 254))				
				Axes_value[axis] = scaled_value					
		
		frame = bytearray()
		frame.append(0xAC)
		frame.append(0x02)
		frame.append(Axes_value[2])
		frame.append(Axes_value[5])
		frame.append(Checksum(frame))

		print hex(frame[0])+" "+hex(frame[1])+" "+str(frame[2])+" "+str(frame[3])+" "+hex(frame[4])
		ser.write(frame)
		time.sleep(0.01)
		print ser.readline()



if __name__ == "__main__":
	main()
