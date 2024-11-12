# For Martino
import pygame as pg
from math import *
import random
pg.init()


# destination
# boost


# TURN_PHYSIC = True
TURN_PHYSIC = False
# Fixed/Facing/Motion
# ORENTATION = "Fixed"
ORENTATION = "Facing"

HAS_DEST = True
DEST_DIST = 1024 #/8192
DEST_RADIUS = 25
DEST_MASS = 15

# keys
KEY_PAUSE = pg.K_SPACE
KEY_ACC = pg.K_w
KEY_DEC = pg.K_s
KEY_LEFT = pg.K_a
KEY_RIGHT = pg.K_d
KEY_ACC2 = pg.K_UP
KEY_DEC2 = pg.K_DOWN
KEY_LEFT2 = pg.K_LEFT
KEY_RIGHT2 = pg.K_RIGHT
KEY_RESTART = pg.K_F5

SAFE_DIST = 200

V_MAX = 500

GRID_SIZE = 128

CHUNKSIZE = 1024
PLANET_CNT = 5
MASS_MIN = 3
MASS_AVG = 12
MASS_AB = 3
# r=RADIUS_RATIO*mass^(1/3)
RADIUS_RATIO = 5
BLACKHOLE_CHANCE=0.01
# BLACKHOLE_CHANCE=0
BLACKHOLE_R_RATIO = 0.2
BLACKHOLE_M_RATIO = 5
BLACKHOLE_DEATH_RATIO = 4
# a = MG/r^2
G = 2000

ROCKET_POS = 0.7
ROCKET_HEIGHT = 0.025
ROCKET_WIDTH = 0.005
ROCKET_COLOR = (179,179,179)

ACCELARATION = 100

# with turn physic
ANGLAR_ACC = 1
#without
ANGLAR_VEL = 1

INIT_V = [0,0]

# how many unit to search for planet
GRAVITY_RANGE = 1024

# planet: (x, y, m, r, is_black_hole)
size = [640,480]
FRAMERATE= 40

TIMEPERFRAME = 1/FRAMERATE

# RANDOM_SEED = False
RANDOM_SEED =True
# SEED = 37272358
SEED = 4
SEED = -32577269
SEED = 1598438236

testing = False

def binomial(p, n):
    c=0
    for it3 in range(n):
        if random.random()<p:
            c+=1
    return c

def map1(x, y):
    if x==0 and y==0:
        return 0
    md = abs(x)+abs(y)
    base = (md-1)*2*md + 1
    if y>=0:
        return base+md-x
    return base+3*md+x


screen = pg.display.set_mode(size,pg.RESIZABLE)
pg.display.set_caption("Game")


def to_screen(x, y):
    # covert position relative to ship to screen pos
    # ship coord: unit point up
    w = screen.get_width()
    h = screen.get_height()
    return w/2 + x, h*ROCKET_POS - y

def gen(x, y):
    planet=[]
    # print("generate chunk",x,y)
    for it2 in range(binomial(0.5,2*PLANET_CNT)):
        px = CHUNKSIZE*(x+random.random())
        py = CHUNKSIZE*(y+random.random())
        if dist((0,0),(px,py))<SAFE_DIST:
            continue
        pm = MASS_MIN+2*(MASS_AVG-MASS_MIN)*random.betavariate(MASS_AB,MASS_AB)
        pr = pm**(1/3) * RADIUS_RATIO
        pb = False
        pc = "#ffff00"
        if random.random()<BLACKHOLE_CHANCE:
            pr *= BLACKHOLE_R_RATIO
            pm *= BLACKHOLE_M_RATIO
            pb = True
        planet.append((px, py, pm, pr, pb, pc))
    return planet
    

class Space:
    def __init__(self, seed):
        self.seed = seed
        random.seed(self.seed)
        self.chunks={(0,0):gen(0,0)}
    def range(self, l, r, t, b):
        for x in range(l,r):
            for y in range(b,t):
                if (x,y) not in self.chunks:
                    random.seed(self.seed+map1(x,y))
                    self.chunks[(x,y)] = gen(x,y)
                for p in self.chunks[(x,y)]:
                    yield p
    

class Matrix22:
    def __init__(self,r1c1,r1c2,r2c1,r2c2):
        self.a11=r1c1
        self.a12=r1c2
        self.a21=r2c1
        self.a22=r2c2
    
    def mul(self, b1, b2):
        # right-mul by a column vector
        return (self.a11*b1 + self.a12*b2, self.a21*b1 + self.a22*b2)

class Game:
    def __init__(self, s):
        self.screen = s
        self.game = pg.Surface(screen.get_size())
        self.show_info = False
        self.show_line = False
        self.show_menu = False
        self.reset()

    def reset(self):
        if RANDOM_SEED:
            self.seed = random.randint(-2**31, 2**31-1)
        else:
            self.seed = SEED
        print("seed:", self.seed)
        self.space = Space(self.seed)
        self.pos = [0, 0]
        self.v = INIT_V[:]
        self.theta = 0#pi/4
        self.omega = 0
        self.t = 0
        pg.mouse.set_visible(False)
        self.state = 0 # 0:normal, 1:pause, 2:win, 3:fall to star, 4: black hole
        if HAS_DEST:
            random.seed(self.seed)
            theta1 = random.random()*2*pi
            self.dest = (cos(theta1)*DEST_DIST, sin(theta1)*DEST_DIST)

    def resize(self, w, h):
        self.game = pg.Surface((w,h))

    def pause(self):
        if self.state==0:
            self.state = 1
            print("pos: ", self.pos)
            print("v: ", self.v)
            print("theta: ", self.theta)
            pg.mouse.set_visible(True)
        elif self.state == 1:
            self.state = 0
            pg.mouse.set_visible(False)
    
    def acc(self):
        # accelerate
        if self.state != 0:return
        self.v[0] += ACCELARATION*cos(self.theta)*TIMEPERFRAME
        self.v[1] += ACCELARATION*sin(self.theta)*TIMEPERFRAME
    
    def dec(self):
        # decelerate
        if self.state != 0:return
        self.v[0] -= ACCELARATION*cos(self.theta)*TIMEPERFRAME
        self.v[1] -= ACCELARATION*sin(self.theta)*TIMEPERFRAME
    
    def turn(self, clockwise=False):
        if self.state != 0:return
        if clockwise:
            mul=-1
        else:
            mul=1
        if TURN_PHYSIC:
            self.omega += ANGLAR_ACC*TIMEPERFRAME*mul
        else:
            self.theta += ANGLAR_VEL*TIMEPERFRAME*mul

    def move(self):
        if self.state != 0:return
        self.t += 1
        vabs = sqrt(self.v[0]**2+self.v[1]**2)
        if vabs>V_MAX:
            # print(vabs)
            self.v[0] *= V_MAX/vabs
            self.v[1] *= V_MAX/vabs
        self.pos[0]+=self.v[0]*TIMEPERFRAME/2
        self.pos[1]+=self.v[1]*TIMEPERFRAME/2
        self.fall()
        self.pos[0]+=self.v[0]*TIMEPERFRAME/2
        self.pos[1]+=self.v[1]*TIMEPERFRAME/2
        if TURN_PHYSIC:
            self.theta += self.omega*TIMEPERFRAME

    def fall(self):
        if self.state != 0:return
        l=int((self.pos[0]-GRAVITY_RANGE)/CHUNKSIZE)
        r=int((self.pos[0]+GRAVITY_RANGE)/CHUNKSIZE)
        t=int((self.pos[1]+GRAVITY_RANGE)/CHUNKSIZE)
        b=int((self.pos[1]-GRAVITY_RANGE)/CHUNKSIZE)
        if HAS_DEST:
            dx = self.dest[0] - self.pos[0]
            dy = self.dest[1] - self.pos[1]
            di = dist(self.pos, self.dest)
            if di < DEST_RADIUS:
                self.state=2
                self.over(1)
                return
            self.v[0]+=dx*G*DEST_MASS/di**3
            self.v[1]+=dy*G*DEST_MASS/di**3
        for p in self.space.range(l,r,t,b):
            dx = p[0]-self.pos[0]
            dy = p[1]-self.pos[1]
            di = sqrt(dx**2+dy**2)
            if not testing:
                if p[4]:
                    if (di<p[3]*BLACKHOLE_DEATH_RATIO):
                        self.state = 4
                        break
                else:
                    if (di<p[3]):
                        self.state = 3
                        break
            self.v[0]+=dx*G*p[2]/di**3
            self.v[1]+=dy*G*p[2]/di**3
        if self.state not in (0,1):
            self.over(1)
    
    def show(self):
        if self.state not in (0,1):
            self.over()
            return
        else:
            self.draw_game()
        if self.show_menu:
            self.draw_menu()
        pg.display.flip()

    def draw_game(self):
        w = self.screen.get_width()
        h = self.screen.get_height()
        corners = [(-w/2, h*ROCKET_POS), (w/2, h*ROCKET_POS),
        (-w/2, h*(ROCKET_POS-1)), (w/2, h*(ROCKET_POS-1))]
        if ORENTATION == "Facing":
            dix,diy = (cos(self.theta), sin(self.theta))
        elif ORENTATION =="Motion":
            speed = sqrt(self.v[0]**2+self.v[1]**2)
            dix,diy = self.v[0]/speed, self.v[1]/speed
        else:
            dix,diy = 0, 1
        # dir_v is unit vector that it's facing
        l = 1e9
        b = 1e9
        r = -1e9
        t = -1e9
        # from inner coord
        tm1 = Matrix22(diy, -dix, dix, diy)
        # to inner coord
        tm2 = Matrix22(diy, dix, -dix, diy)
        for i in range(4):
            tx, ty = tm2.mul(*corners[i])
            tx += self.pos[0]
            ty += self.pos[1]
            r = max(r, tx)
            l = min(l, tx)
            t = max(t, ty)
            b = min(b, ty)
        l = int(l/CHUNKSIZE)
        b = int(b/CHUNKSIZE)
        r = int(r/CHUNKSIZE)
        t = int(t/CHUNKSIZE)
        r += 2
        t += 2
        l -= 1
        b -= 1
        self.screen.fill("#ffffff")
        for px in range(l*CHUNKSIZE, r*CHUNKSIZE, CHUNKSIZE):
            for py in range(b*CHUNKSIZE, t*CHUNKSIZE, CHUNKSIZE):
                px1, py1 = to_screen(*tm1.mul(px-self.pos[0],py-self.pos[1]))
                px2, py2 = to_screen(*tm1.mul(px-self.pos[0],py+CHUNKSIZE-self.pos[1]))
                px3, py3 = to_screen(*tm1.mul(px+CHUNKSIZE-self.pos[0],py+CHUNKSIZE-self.pos[1]))
                px4, py4 = to_screen(*tm1.mul(px+CHUNKSIZE-self.pos[0],py-self.pos[1]))
                # print(px1,py1, px2,py2)
                pg.draw.polygon(self.screen, (0, 0, 0), ((px1, py1), (px2, py2), (px3, py3), (px4, py4)))
        for px in range(l*CHUNKSIZE, r*CHUNKSIZE, GRID_SIZE):
            for py in range(b*CHUNKSIZE, t*CHUNKSIZE, GRID_SIZE):
                px1, py1 = to_screen(*tm1.mul(px-self.pos[0],py-self.pos[1]))
                px2, py2 = to_screen(*tm1.mul(px-self.pos[0],py+GRID_SIZE-self.pos[1]))
                px3, py3 = to_screen(*tm1.mul(px+GRID_SIZE-self.pos[0],py+GRID_SIZE-self.pos[1]))
                px4, py4 = to_screen(*tm1.mul(px+GRID_SIZE-self.pos[0],py-self.pos[1]))
                # print(px1,py1, px2,py2)
                pg.draw.polygon(self.screen, "#7f7f7f", ((px1, py1), (px2, py2), (px3, py3), (px4, py4)), 1)
        # Destination
        if HAS_DEST:
            desx,desy = to_screen(*tm1.mul(self.dest[0]-self.pos[0],self.dest[1]-self.pos[1]))
            pg.draw.circle(screen, "#0000ff", (desx,desy), DEST_RADIUS)
        # Stars
        for px,py,pm,pr,pb,pc in self.space.range(l,r,t,b):
            px,py = to_screen(*tm1.mul(px-self.pos[0],py-self.pos[1]))
            if pb:
                pg.draw.circle(screen, "#e34402", (px,py), pr, 1)
            else:
                pg.draw.circle(screen, pc, (px,py), pr)
        tm = Matrix22(sin(self.theta),cos(self.theta), -cos(self.theta), sin(self.theta))
        # Dest indicator on the edge
        if HAS_DEST:
            ptx, pty = tm1.mul(self.dest[0]-self.pos[0],self.dest[1]-self.pos[1])
            mul = 1e9
            if (ptx>0):
                mul = min(mul, w/2/ptx)
            if (ptx<0):
                mul = min(mul, -w/2/ptx)
            if (pty>0):
                mul = min(mul, h*ROCKET_POS/pty)
            if (pty<0):
                mul = min(mul, h*(ROCKET_POS-1)/pty)
            if (mul<1):
                pg.draw.circle(self.screen, "#0000ff", to_screen(ptx*mul,pty*mul), 9)
            # line to dest
            if self.show_line:
                pg.draw.line(self.screen, "#ff0000", to_screen(0,0), (desx,desy))
        triangle = ((-h*ROCKET_WIDTH, -h*ROCKET_HEIGHT/3), (h*ROCKET_WIDTH, -h*ROCKET_HEIGHT/3),
        (0,h*ROCKET_HEIGHT*2/3))
        triangle = list(map(lambda a:to_screen(*tm1.mul(*tm.mul(*a))), triangle))
        pg.draw.polygon(self.screen, ROCKET_COLOR, triangle)
        # Velocity
        if self.show_line:
            px, py = self.v
            mul = sqrt(px**2+py**2)**0.5
            if dist((0,0), self.v):
                px /= dist((0,0), self.v)
                py /= dist((0,0), self.v)
            px *= mul
            py *= mul
            px,py = to_screen(*tm1.mul(px,py))
            pg.draw.line(self.screen, "#3fff00", to_screen(0,0), (px, py), 2)
        if self.show_info:
            self.display_info()
    
    def over(self, plog = False):
        if testing:
            self.state = 0
            return
        if plog:
            print(["Normal", "Paused", "Win", "Lose1", "Lose2"][self.state])
            print(self.pos, self.v, self.theta, self.omega)
        pg.mouse.set_visible(True)
        if self.state == 2:
            self.win()
        elif self.state == 3:
            self.lose("Oops, you burned to a crisp")
        elif self.state == 4:
            self.lose("Oops, you got spaghettified")
        self.draw_menu()

    def display_info(self):
        s = self.screen
        w = s.get_width()
        my_font = pg.font.SysFont("Calibri",15)
        f1 = my_font.render(f"Pos : x={self.pos[0]}, y={self.pos[1]}", False, "#ffffff")
        f2 = my_font.render(f"Theta : {self.theta}", False, "#ffffff")
        f3 = my_font.render(f"Speed : {dist((0,0),self.v)}", False, "#ffffff")
        f4 = my_font.render(f"Distance : {dist(self.dest, self.pos)}", False, "#ffffff")
        f5 = my_font.render(f"Seed : {self.seed}", False, "#ffffff")
        h = 0
        s.blit(f1, (w-f1.get_width(), h))
        h += f1.get_height()
        s.blit(f2, (w-f2.get_width(), h))
        h += f2.get_height()
        s.blit(f3, (w-f3.get_width(), h))
        h += f3.get_height()
        s.blit(f4, (w-f4.get_width(), h))
        h += f4.get_height()
        s.blit(f5, (w-f5.get_width(), h))
        h += f5.get_height()

    def lose(self, msg):
        s = self.screen
        w = s.get_width()
        h = s.get_height()
        s.fill("#ffffff")
        my_font1 = pg.font.SysFont("Calibri",50)
        my_font2 = pg.font.SysFont("Calibri",15)
        f1=my_font1.render(msg, False, "#000000")
        f2=my_font2.render("Press F5 to restart", False, "#000000")
        s.blit(f1,(w/2-f1.get_width()/2,h/2-f1.get_height()/2))
        s.blit(f2,(w/2-f2.get_width()/2,h/2+f1.get_height()/2))

    def win(self):
        if self.state == 1:return
        if testing:
            self.state = 0
            return
        s = self.screen
        w = s.get_width()
        h = s.get_height()
        s.fill("#ffffff")
        my_font1 = pg.font.SysFont("Calibri",50)
        my_font2 = pg.font.SysFont("Calibri",15)
        f1=my_font1.render("YAY, you win", False, "#000000")
        f2=my_font2.render(f"Your score is {self.t}", False, "#000000")
        f3=my_font2.render("Press F5 to restart", False, "#000000")
        s.blit(f1,(w/2-f1.get_width()/2,h/2-f1.get_height()/2))
        s.blit(f2,(w/2-f2.get_width()/2,h/2+f1.get_height()/2))
        s.blit(f3,(w/2-f3.get_width()/2,h/2+f1.get_height()/2+f2.get_height()))

    def toggle_menu(self):
        if self.show_menu:
            self.show_menu = False
        else:
            self.show_menu =True

    def draw_menu(self):
        self.screen.fill((127,127,127,127))


# screen.fill("#ffffff")
pg.display.flip()

g=Game(screen)
clock = pg.time.Clock()
running = True
f3 = False
while running:
    #sim stuff
    if  g.state == 0:
        g.move()
    #display
    pg.display.flip()
    clock.tick(FRAMERATE)
    keys = pg.key.get_pressed()
    if keys[KEY_ACC] or keys[KEY_ACC2]:
        g.acc()
    if keys[KEY_DEC] or keys[KEY_DEC2]:
        g.dec()
    if keys[KEY_LEFT] or keys[KEY_LEFT2]:
        g.turn()
    if keys[KEY_RIGHT] or keys[KEY_RIGHT2]:
        g.turn(1)
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
            pg.quit()
            break
        elif event.type==pg.KEYDOWN:
            if event.key == KEY_RESTART:
                g.reset()
            elif event.key == KEY_PAUSE:
                g.pause()
            elif event.key == pg.K_F4:
                if keys[pg.K_F3]:
                    testing = not testing
                    f3 = False
            elif event.key == pg.K_F3:
                f3 = True
            elif event.key == pg.K_b:
                if keys[pg.K_F3]:
                    f3 = False
                    g.show_line = not g.show_line
            elif event.key == pg.K_ESCAPE:
                g.toggle_menu()
        elif event.type == pg.KEYUP:
            if event.key == pg.K_F3:
                if f3:
                    f3 = False
                    g.show_info = not g.show_info
        elif event.type == pg.VIDEORESIZE:
            screen = pg.display.set_mode((event.w, event.h),
                                              pg.RESIZABLE)
            W=screen.get_width()//2
            H=screen.get_height()//2
            g.resize(W, H)
    if running:
        g.show()
    
