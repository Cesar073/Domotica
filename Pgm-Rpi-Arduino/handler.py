import serial
import time

dev = serial.Serial("/dev/ttyAMA0", 9600)
cadena = "CGVT005T005"
#dev.write(cadena.encode('ascii').rstrip())
# print(f"cadena:{str(cadena.encode('ascii').rstrip())}")

counter = 0
while True:
    dev.write(cadena.encode('ascii'))
    datos = dev.readline()
    counter += 1
    if counter == 1000:
        counter = 0
    print(datos.decode('utf-8') + " - " + str(counter))
    time.sleep(0.5)
#dev.close()