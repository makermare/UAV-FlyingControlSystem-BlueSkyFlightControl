import wiringpi

serial = wiringpi.serialOpen('/dev/ttyUSB0', 9600)
wiringpi.serialPuts(serial, 'Send:')
test = 0xaa
wiringpi.serialPutchar(serial, test)
wiringpi.serialPuts(serial, '\n')
wiringpi.serialClose(serial)