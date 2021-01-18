import pygame
import os

class Bullet():
    def __init__(self, xpos, ypos):
        self.speed = 10
        self.xpos = xpos
        self.ypos = ypos
        self.direction = 1
        self.damage = 10
        self.image = pygame.image.load("./Assets/peabullet.png")
        self.mask = pygame.mask.from_surface(self.image)

    def draw(self, window):
        window.blit(self.image, (self.xpos, self.ypos))

def collide(obj1, obj2):
    offset_x = obj2.xpos - obj1.xpos
    offset_y = obj2.ypos - obj1.ypos
    return obj1.mask.overlap(obj2.mask, (offset_x, offset_y)) != None