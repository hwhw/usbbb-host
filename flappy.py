import usbbb
import random

bb = usbbb.BB()

emptycol = [0,0,0,0,0,0,0,0,0,1]
world = [emptycol]*10
threshold = 2000

def collision(world, bird):
    print bird
    if(bird <= 0):
        return True
    if(world[4][10-int(bird)] == 2):
        return True
    return False

def paintworld(loop, bird):
    global world
    for x in range(10):
        for y in range(10):
            if(x == 1 and y == 8):
                bb.set_led10(x, y, 255, 0, 0)
            elif(x == 4 and (9-y) == int(bird)):
                bb.set_led10(x, y, 255, 2*(loop-10)*(loop-10), 0)
            else:
                v = world[x][y]
                if(v == 0):
                    bb.set_led10(x, y, 100, 0, 255)
                elif(v == 1):
                    bb.set_led10(x, y, 0, 100, 0)
                elif(v == 2):
                    bb.set_led10(x, y, 255, 255, 0)


def play():
    global world
    nextw = 10
    loop = 0
    acc = -9.8 * 0.001
    speed = 0
    bird = 4.0
    while True:
        bb.wait_measure()
        if(bb.get_sensor_field(1, 8) > threshold):
            speed = 0.20
        else:
            speed = speed + acc
        bird = bird + speed/2
        if(bird <= 0):
            bird = 0
        if(bird >= 10):
            bird = 9.9
            speed = 0

        if(collision(world, int(bird+0.5))):
            return

        loop = loop + 1
        if(loop == 20):
            loop = 0
            nextw = nextw - 1
            if(nextw == 0):
                nextw = random.randint(4,7)
                h = random.randint(4,9)
                world = world[1:] + [[2]*(h-4) + [0]*4 + [2]*(10-h)]
            else:
                world = world[1:] + [emptycol]

        paintworld(loop, bird)
        bb.transmit(0)

def start(anim):
    global world
    world = [emptycol]*10
    for frame in anim:
        paintworld(0, -1)
        for p in frame:
            bb.set_led10(p[0],p[1],255,255,255)
        bb.transmit(0)
        for x in range(80):
            bb.wait_measure()
    paintworld(0, 4.0)
    bb.transmit(0)
    while(bb.get_sensor_field(1, 8) <= threshold):
        bb.wait_measure()
    play()

while True:
    start([
        [[4,2],[5,2],[6,2],[6,3],[5,4],[6,4],[6,5],[4,6],[5,6],[6,6]],
        [[4,2],[5,2],[6,2],[6,3],[4,4],[5,4],[6,4],[4,5],[4,6],[5,6],[6,6]],
        [[5,2],[5,3],[5,4],[5,5],[5,6]]
    ])
