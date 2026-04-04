import serial
import struct

# baud = 12000000
baud = 12000000

def send_file(filename):
    ser = serial.Serial("COM8", baud, timeout=5)

    with open(filename, "rb") as f:
        address = 0

        while True:
            chunk = f.read(4096)
            if not chunk:
                break

            size = len(chunk)

            # send size + data
            ser.write(struct.pack("<H", size))
            ser.write(chunk)

            # wait for ACK
            ack = ser.read(1)
            if ack != b'\xAA':
                print("ACK error:", ack)
                ser.close()
                return

            address += size

    # ✅ send completion signal (size = 0)
    ser.write(struct.pack("<H", 0))

    # optional: wait for final ACK
    ack = ser.read(1)
    if ack == b'\xCC':
        print("Transfer complete acknowledged by MCU")
    else:
        print("No final ACK or wrong response:", ack)

    ser.close()
    print("DONE")

send_file("F:/projects/CSEE/L_2025_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/images/mergeBinFile.bin")