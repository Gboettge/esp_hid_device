#import evdev
from evdev import InputDevice, categorize, ecodes

gamepad = InputDevice('/dev/input/event9')

print(gamepad)


for event in gamepad.read_loop():
    if event.type == ecodes.EV_KEY: #filter to val
        print(categorize(event))