import serial
import time
import threading

dev = serial.Serial("/dev/ttyUSB0", 9600)

#dev.write(cadena.encode('ascii').rstrip())
# print(f"cadena:{str(cadena.encode('ascii').rstrip())}")

def bucle_de_escritura():
    while True:
        opcion = input("""
        1 - Consultar datos generales
        2 - Ver sensor luz
        3 - Ver sensor temp
        elija:
        """)
        if opcion == "1":
            cadena = "CGVFF050F050"
            dev.write(cadena.encode('ascii'))
        elif opcion == "2":
            cadena = "CGVTF050F050"
            dev.write(cadena.encode('ascii'))
        elif opcion == "3":
            cadena = "CGVFT050F050"
            dev.write(cadena.encode('ascii'))
        else:
            print("Error de opción")


def bucle_de_lectura():
    while True:
        datos = dev.readline()
        print(datos.decode('utf-8'))
        time.sleep(0.1)
#dev.close()


t1 = threading.Thread(name="hilo_1", target=bucle_de_escritura).start()
t2 = threading.Thread(name="hilo_2", target=bucle_de_lectura).start()