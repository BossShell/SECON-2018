from threading import *
import time
from ArduinoSerial import *

class MainThread(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        # Setup code
        Arduino = ArduinoSerial('/dev/ttyACM0')
        time.sleep(2)
        i = 0
        
        while i < 10:
            #distance = Arduino.analogRead(0)
            value = Arduino.digitalRead(53)
            
            #if value[0] == 1:
                #Arduino.digitalWrite(22, 1)
            #else:
                #Arduino.digitalWrite(22, 0)
            
            #print('US1: %04d' % distance[0] + '  LS1: %04d' % value[0])
            print('LS1: %04d' % value[0])
            time.sleep(0.05)
            i += 1
