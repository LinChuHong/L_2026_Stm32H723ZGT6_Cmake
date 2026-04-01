import serial
import time
import zlib

PORT = "COM8"
BAUD = 115200
CHUNK = 256
TIMEOUT = 10

ser = serial.Serial(PORT, BAUD, timeout=TIMEOUT)
# Give the device time to settle and flush any startup output.
time.sleep(1.0)
ser.reset_input_buffer()


def bytes_to_text(data):
    return ''.join(chr(ch) if 32 <= ch < 127 else '.' for ch in data)


def read_response():
    seen = bytearray()
    deadline = time.time() + TIMEOUT
    prev_timeout = ser.timeout
    ser.timeout = 0.1
    try:
        while time.time() < deadline:
            b = ser.read(1)
            if not b:
                continue

            if b == b'\x06':
                return True, seen
            if b == b'\x15':
                return False, seen

            seen.extend(b)
    finally:
        ser.timeout = prev_timeout

    return None, seen


def send_file(filename):
    with open(filename, "rb") as f:
        addr = 0

        while True:
            data = f.read(CHUNK)
            if not data:
                break

            packet = bytearray()
            packet += b'\xAA\x55'
            packet += addr.to_bytes(4, 'little')
            packet += len(data).to_bytes(2, 'little')
            packet += data

            crc = zlib.crc32(data) & 0xFFFFFFFF
            packet += crc.to_bytes(4, 'little')

            ser.reset_input_buffer()
            ser.write(packet)
            ser.flush()

            result, prefix = read_response()
            if result is None:
                print(
                    f"Error at {addr}: timeout waiting for ACK"
                    + (f" prefix={prefix.hex().upper()}"
                       f" text={bytes_to_text(prefix)!r}"
                       if prefix else "")
                )
                return

            if result is False:
                print(
                    f"Error at {addr}: got NACK"
                    + (f" prefix={prefix.hex().upper()}"
                       f" text={bytes_to_text(prefix)!r}"
                       if prefix else "")
                )
                return

            if prefix:
                print(
                    f"Sent {len(data)} bytes @ 0x{addr:08X}"
                    f" (ignored unexpected prefix: {prefix.hex().upper()} {bytes_to_text(prefix)!r})"
                )
            else:
                print(f"Sent {len(data)} bytes @ 0x{addr:08X}")

            addr += len(data)

            time.sleep(0.005)

    print("DONE")


if __name__ == "__main__":
    send_file(
        "F:/projects/CSEE/L_2025_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/images/mergeBinFile.bin"
    )
    ser.close()