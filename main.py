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
DEST_SAFE_DIST = 50

LIGHT = 500

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
INIT_SIZE = [640,480]
FRAMERATE= 40

TIMEPERFRAME = 1/FRAMERATE

# RANDOM_SEED = False
RANDOM_SEED =True
# SEED = 37272358
SEED = 4
SEED = -32577269
SEED = 1598438236
# very easy seed, just press w
SEED = -501839539

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

def gen(x, y, clear_area=None):
    planet=[]
    if clear_area is None:
        clear_area = {}
    # print("generate chunk",x,y)
    for it2 in range(binomial(0.5,2*PLANET_CNT)):
        px = CHUNKSIZE*(x+random.random())
        py = CHUNKSIZE*(y+random.random())
        for p in clear_area:
            if dist(p, (px,py))<clear_area[p]:
                break
        else:
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

def mag(a):
    # return length of vector
    l=0
    for i in a:
        l+=i**2
    return sqrt(l)

def translate(v):
    # take in a vector v
    # return A and A^-1
    # AX = position in a coordinate system where x point in v
    # in other word, Av = (|v|,0), |A| = 1
    x,y = v
    x /= mag(v)
    y/= mag(v)
    Ainv = Matrix22(x,-y,y,x)
    A = Matrix22(x,y,-y,x)
    return A,Ainv

def gen_text(w, h, title, msgs=None):
    if msgs is None:
        msgs = []
    s = pg.Surface((w, h))
    s.fill("#ffffff")
    my_font1 = pg.font.SysFont("Calibri",50)
    my_font2 = pg.font.SysFont("Calibri",15)
    ft=my_font1.render(title, False, "#000000")
    s.blit(ft,(w/2-ft.get_width()/2,h/2-ft.get_height()/2))
    h = h/2+ft.get_height()/2
    for msg in msgs:
        f2=my_font2.render(msg, False, "#000000")
        s.blit(f2,(w/2-f2.get_width()/2,h))
        h += f2.get_height()
    return s

class Main:
    def __init__(self):
        self.screen = pg.display.set_mode(INIT_SIZE, pg.RESIZABLE)
        pg.display.set_caption("Game")
        self.game = Game(*INIT_SIZE)
        self.win_screen = None
        self.lose_screen1 = None
        self.lose_screen2 = None
        self.resize(*INIT_SIZE)
        # 0:normal, 1:pause, 2:win, 3:fall to star, 4: black hole
        self.state = 0
    
    def mainloop(self):
        clock = pg.time.Clock()
        running = True
        f3 = False
        g = self.game
        while running:
            #sim stuff
            if  self.state == 0:
                self.state = g.move()
                if self.state == 2:
                    self.win_screen = gen_text(*self.screen.get_size(), "YAY, you win",
                                               ("Press F5 to restart", 
                                                f"Your score is {self.game.t}"))
            #display
            if self.state in (0,1):
                self.screen.blit(g.screen, (0,0))
            elif self.state == 2:
                self.screen.blit(self.win_screen, (0,0))
            elif self.state == 3:
                self.screen.blit(self.lose_screen1, (0,0))
            elif self.state == 4:
                self.screen.blit(self.lose_screen2, (0,0))
            pg.display.flip()
            clock.tick(FRAMERATE)
            keys = pg.key.get_pressed()
            for event in pg.event.get():
                if event.type == pg.QUIT:
                    running = False
                    pg.quit()
                    break
                elif event.type==pg.KEYDOWN:
                    if event.key == KEY_RESTART:
                        self.state = 0
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
                    self.resize(event.w, event.h)
                    screen = pg.display.set_mode((event.w, event.h),
                                                    pg.RESIZABLE)
                    g.resize(*screen.get_size())
            if running:
                g.show()

    def resize(self, w, h):
        self.screen = pg.display.set_mode((w,h), pg.RESIZABLE)
        self.game.resize(w,h)
        self.win_screen = gen_text(w, h, "YAY, you win",
                                   ("Press F5 to restart", f"Your score is {self.game.t}"))
        self.lose_screen1 = gen_text(w, h, "Oops, you burned to a crisp",
                                     ("Press F5 to restart",))
        self.lose_screen2 = gen_text(w, h, "Oops, you got spaghettified",
                                     ("Press F5 to restart",))


class Space:
    def __init__(self, seed, safe=None):
        if safe is None:
            safe = {}
        self.seed = seed
        random.seed(self.seed)
        self.safe = safe
        self.chunks={(0,0):gen(0,0, self.safe)}
    def range(self, l, r, t, b):
        for x in range(l,r):
            for y in range(b,t):
                if (x,y) not in self.chunks:
                    random.seed(self.seed+map1(x,y))
                    self.chunks[(x,y)] = gen(x,y,self.safe)
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
    def __init__(self, w, h):
        self.screen = pg.Surface((w,h))
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
        safe = {(0,0):SAFE_DIST}
        if HAS_DEST:
            random.seed(self.seed)
            theta1 = random.random()*2*pi
            self.dest = (cos(theta1)*DEST_DIST, sin(theta1)*DEST_DIST)
            safe[self.dest] = DEST_SAFE_DIST
        self.space = Space(self.seed, safe)
        self.pos = [0, 0]
        self.v = INIT_V[:]
        self.theta = 0#pi/4
        self.omega = 0
        self.t = 0
        pg.mouse.set_visible(False)
        self.state = 0 

    def resize(self, w, h):
        self.screen = pg.Surface((w,h))

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
        self.check_keys()
        if self.state != 0:return
        self.t += 1
        vabs = sqrt(self.v[0]**2+self.v[1]**2)
        if vabs>LIGHT:
            # print(vabs)
            self.v[0] *= LIGHT/vabs
            self.v[1] *= LIGHT/vabs
        self.pos[0]+=self.v[0]*TIMEPERFRAME/2
        self.pos[1]+=self.v[1]*TIMEPERFRAME/2
        self.fall()
        self.pos[0]+=self.v[0]*TIMEPERFRAME/2
        self.pos[1]+=self.v[1]*TIMEPERFRAME/2
        if TURN_PHYSIC:
            self.theta += self.omega*TIMEPERFRAME
        return self.state

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
        else:
            self.draw_game()
        if self.show_menu:
            self.draw_menu()

    def draw_game(self):
        s = self.screen
        w = s.get_width()
        h = s.get_height()
        # translate matrix for facing direction
        Td, Td1 = translate((cos(self.theta), sin(self.theta)))
        # velocity
        speed = mag(self.v)
        if speed==0:
            Tv, Tv1 = translate((1,0))
        else:
            Tv, Tv1 = translate((self.v[0]/speed, self.v[1]/speed))
        # up
        Tu, Tu1 = translate((0, 1))
        if ORENTATION == "Facing":
            T, T1 = Td, Td1
        elif ORENTATION =="Motion":
            T, T1 = Tv, Tv1
        else:
            T, T1 = Tu, Tu1
        l = 1e9
        b = 1e9
        r = -1e9
        t = -1e9
        corners = [self.from_screen(0,0), self.from_screen(w,0),
                   self.from_screen(w,h), self.from_screen(0,h)]
        for i in range(4):
            tx, ty = T1.mul(*corners[i])
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
        s.fill("#ffffff")
        for px in range(l*CHUNKSIZE, r*CHUNKSIZE, CHUNKSIZE):
            for py in range(b*CHUNKSIZE, t*CHUNKSIZE, CHUNKSIZE):
                px1, py1 = self.to_screen(*T.mul(px-self.pos[0],py-self.pos[1]))
                px2, py2 = self.to_screen(*T.mul(px-self.pos[0],py+CHUNKSIZE-self.pos[1]))
                px3, py3 = self.to_screen(*T.mul(px+CHUNKSIZE-self.pos[0],py+CHUNKSIZE-self.pos[1]))
                px4, py4 = self.to_screen(*T.mul(px+CHUNKSIZE-self.pos[0],py-self.pos[1]))
                # print(px1,py1, px2,py2)
                pg.draw.polygon(s, (0, 0, 0), ((px1, py1), (px2, py2), (px3, py3), (px4, py4)))
        for px in range(l*CHUNKSIZE, r*CHUNKSIZE, GRID_SIZE):
            for py in range(b*CHUNKSIZE, t*CHUNKSIZE, GRID_SIZE):
                px1, py1 = self.to_screen(*T.mul(px-self.pos[0],py-self.pos[1]))
                px2, py2 = self.to_screen(*T.mul(px-self.pos[0],py+GRID_SIZE-self.pos[1]))
                px3, py3 = self.to_screen(*T.mul(px+GRID_SIZE-self.pos[0],py+GRID_SIZE-self.pos[1]))
                px4, py4 = self.to_screen(*T.mul(px+GRID_SIZE-self.pos[0],py-self.pos[1]))
                # print(px1,py1, px2,py2)
                pg.draw.polygon(s, "#7f7f7f", ((px1, py1), (px2, py2), (px3, py3), (px4, py4)), 1)
        # Destination
        if HAS_DEST:
            desx,desy = self.to_screen(*T.mul(self.dest[0]-self.pos[0],self.dest[1]-self.pos[1]))
            pg.draw.circle(s, "#0000ff", (desx,desy), DEST_RADIUS)
        # Stars
        for px,py,pm,pr,pb,pc in self.space.range(l,r,t,b):
            px,py = self.to_screen(*T.mul(px-self.pos[0],py-self.pos[1]))
            if pb:
                pg.draw.circle(s, "#e34402", (px,py), pr, 1)
            else:
                pg.draw.circle(s, pc, (px,py), pr)
        # Dest indicator on the edge
        if HAS_DEST:
            ptx, pty = T.mul(self.dest[0]-self.pos[0],self.dest[1]-self.pos[1])
            mul = 1e9
            if (ptx>0):
                mul = min(mul, h*ROCKET_POS/ptx)
            if (ptx<0):
                mul = min(mul, h*(ROCKET_POS-1)/ptx)
            if (pty>0):
                mul = min(mul, w/2/pty)
            if (pty<0):
                mul = min(mul, -w/2/pty)
            if (mul<1):
                pg.draw.circle(s, "#0000ff", self.to_screen(ptx*mul,pty*mul), 9)
            # line to dest
            if self.show_line:
                pg.draw.line(s, "#ff0000", self.to_screen(0,0), (desx,desy))
        triangle = ((-h*ROCKET_HEIGHT/3, -h*ROCKET_WIDTH), (-h*ROCKET_HEIGHT/3, h*ROCKET_WIDTH),
        (h*ROCKET_HEIGHT*2/3, 0))
        triangle = list(map(lambda a:self.to_screen(*T.mul(*Td1.mul(*a))), triangle))
        pg.draw.polygon(s, ROCKET_COLOR, triangle)
        # Velocity
        if self.show_line:
            px, py = self.v
            mul = speed**0.5
            if speed:
                px /= speed
                py /= speed
            px *= mul
            py *= mul
            px,py = self.to_screen(*T.mul(px,py))
            pg.draw.line(s, "#3fff00", self.to_screen(0,0), (px, py), 2)
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

    def toggle_menu(self):
        if self.show_menu:
            self.show_menu = False
        else:
            self.show_menu =True

    def draw_menu(self):
        cover = pg.Surface(self.screen.get_size())
        cover.fill((127,127,127))
        cover.set_alpha(127)
        self.screen.blit(cover, (0,0))

    def to_screen(self, x, y):
        # covert position relative to ship to screen pos
        # input cord system: x = distance in front of ship, y= left of ship
        w = self.screen.get_width()
        h = self.screen.get_height()
        return w/2 - y, h*ROCKET_POS - x

    def from_screen(self, x, y):
        # covert position on screen to pos
        # input cord system: x = distance in front of ship, y= left of ship
        w = self.screen.get_width()
        h = self.screen.get_height()
        return h*ROCKET_POS - y, w/2 -x

    def check_keys(self):
        keys = pg.key.get_pressed()
        if keys[KEY_ACC] or keys[KEY_ACC2]:
            self.acc()
        if keys[KEY_DEC] or keys[KEY_DEC2]:
            self.dec()
        if keys[KEY_LEFT] or keys[KEY_LEFT2]:
            self.turn()
        if keys[KEY_RIGHT] or keys[KEY_RIGHT2]:
            self.turn(1)



if __name__ == "__main__":
    m = Main()
    m.mainloop()
