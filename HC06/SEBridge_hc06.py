#!/usr/bin/python3

import serial
import sys

NAME="SE2BRG"
BAUD = 38400
BAUD_CODE = 6
DEVICE = '/dev/ttyAMA0' 

#Note - baud codes:
#1 set to 1200bps
#2 set to 2400bps
#3 set to 4800bps
#4 set to 9600bps (Default)
#5 set to 19200bps
#6 set to 38400bps
#7 set to 57600bps
#8 set to 115200bps

# Configure line endings to assume v3 firmware on HC06
CRLF = '\r\n'
v3 = True

# Open the serial port at a given baud rate
def open(ser,baud) :
  ser.baudrate = baud
  ser.parity = serial.PARITY_NONE
  ser.stopbits = serial.STOPBITS_ONE
  ser.timeout = 2 # should receive data within 2 secs in all cases
  ser.open()

# Send data, adding CRLF as needed and get response
def send_data(ser, cmd) :
 ser.write( (cmd + CRLF).encode('UTF-8')) 
 if v3:
   response = ser.read_until(CRLF.encode('UTF-8'), 256)
 else:
   #time.sleep(2)
   response = ser.read(256)
 return response

# Try to figure out what rate the HC06 is set to.
# Returns 0 if not found
def scan_baud_rates(ser) :
  baudRates = [300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 74880, 115200, 230400, 250000]
  rate = 0
  for baud in baudRates :
    print('Trying baud rate ', baud)
    open(ser,baud)
    response = send_data(ser,'AT')
    print(response)
    if response == ('OK' + CRLF).encode('UTF-8') :
      rate = baud
      print("HC06 responding at ", baud)
      ser.close();
      break
    ser.close(); 
  else:  # didn't find it
    print('No response at from HC06')
    
  return rate

 
 

ser = serial.Serial()
ser.port = DEVICE

# First, try and find the HC06 listening at some, potentially unknown baud rate

print('Looking for HC06 with v3 firmware')
baud = scan_baud_rates(ser)
if baud == 0 :
  v3 = False
  CRLF='' # old firmware doesn't expect CRLF pairs
  print('Not found - Retrying for old firmware')
  baud = scan_baud_rates(ser)
  
if baud == 0 :
  sys.exit('No HC06 module found, exiting')

    
print('Opening port at ', baud, 'bps')
open(ser,baud)

print(send_data(ser,'AT'))  # show it's listening

# slightly redundant check for V3 firmware
response = send_data(ser,'AT+VERSION?')
v3 = response.find(b'VERSION:3') != -1
if v3:
  print('V3 firmware found');

#Set the baud rate
print("Setting baud rate to ", BAUD);
if v3: 
  response = send_data(ser,'AT+UART=' + str(BAUD) + ',0,0') # 1 stop bit, no parity
else :
  response = send_data(ser,'AT+BAUD' + BAUD_CODE) 
print(response)

ser.close()

#now baudrate set to BAUD, reopen at that
print('Reopening at ' + str(BAUD) + ' bps')
open(ser,BAUD)

if v3:
  print(send_data(ser,'AT+PSWD="1234"'))
  print(send_data(ser,'AT+NAME=' + NAME))
  print(send_data(ser,'AT'))
else:
  print(send_data(ser,'AT+PIN1234'))
  print(send_data(ser,'AT+NAME' + NAME))
  print(send_data(ser,'AT'))

ser.close()

