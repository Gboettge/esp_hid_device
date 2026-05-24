#import evdev
from evdev import InputDevice, categorize, ecodes


gamepad = InputDevice('/dev/input/event10')

aBtn = 34
bBtn = 36
xBtn = 35
yBtn = 23

up = 46
down = 32
left = 48
right = 30

start = 24
select = 49

lTrig = 37
rTrig = 50

print(gamepad)

for event in gamepad.read_loop():
    if event.type == ecodes.EV_KEY:
        if event.value == 1:
            if event.code == yBtn:
                print("Y")
            elif event.code == bBtn:
                print("B")
            elif event.code == aBtn:
                print("A")
            elif event.code == xBtn:
                print("X")

            elif event.code == up:
                print("up")
            elif event.code == down:
                print("down")
            elif event.code == left:
                print("left")
            elif event.code == right:
                print("right")

            elif event.code == start:
                print("start")
            elif event.code == select:
                print("select")

            elif event.code == lTrig:
                print("left bumper")
            elif event.code == rTrig:
                print("right bumper")