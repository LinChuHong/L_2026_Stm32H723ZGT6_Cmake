import serial

b = (115200,1000000)

mySerial = serial.Serial("COM5",b[0],timeout=2)



while True:

    if mySerial.in_waiting:
        data = mySerial.read_until()
        print(data.decode().strip())
