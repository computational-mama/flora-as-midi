import rtmidi
import serial
import time

# Serial port configuration
SERIAL_PORT = "/dev/tty.usbmodem1101"  # Update with your Flora's serial port
BAUD_RATE = 115200

# Initialize MIDI output
midi_out = rtmidi.MidiOut()
midi_out.open_virtual_port("Flora-MIDI")  # Create a virtual MIDI port
print("Virtual MIDI Port 'Flora-MIDI' is ready.")

# Initialize serial communication
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE)
    print(f"Connected to Flora on {SERIAL_PORT}")
except Exception as e:
    print(f"Error: {e}")
    exit()

# Main loop: read from serial and send MIDI messages
try:
    while True:
        if ser.in_waiting >= 3:  # MIDI messages are 3 bytes
            midi_message = [ord(ser.read()), ord(ser.read()), ord(ser.read())]
            print(f"Sending MIDI: {midi_message}")

            # Check if the message is a Control Change (CC) message
            if (midi_message[0] & 0xF0) == 0xB0:  # 0xB0 is the status byte for CC messages
                controller_number = midi_message[1]  # Second byte is the controller number
                controller_value = midi_message[2]   # Third byte is the controller value
                print(f"MIDI CC Controller Number: {controller_number}, Value: {controller_value}")

            midi_out.send_message(midi_message)
        time.sleep(0.01)  # Small delay to prevent CPU overload
except KeyboardInterrupt:
    print("Stopping MIDI bridge...")
finally:
    ser.close()
    midi_out.close_port()
