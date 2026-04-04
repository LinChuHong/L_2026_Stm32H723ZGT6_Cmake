import serial
import struct

baud = 12000000
def send_file(filename):
    ser = serial.Serial("COM8", baud, timeout=5)
    with open(filename, "rb") as f:
        address = 0
        while True:
            chunk = f.read(4096)
            if not chunk:
                break


            # packet = address(4) + size(2) + data
            # packet = struct.pack("<IH", address, size) + chunk
            # ser.write(packet)
            size = len(chunk)
            ser.write(struct.pack("<H",size))
            ser.write(chunk)
       
            # wait for ACK
            ack = ser.read(1)
            if ack != b'\xAA':
                print(ack)
                break

            address += size
    ser.close()
    print("DONE")

send_file("F:/projects/CSEE/L_2025_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/images/mergeBinFile.bin")

# ser = serial.Serial("COM11", 12000000, timeout=5)

# ser.close()