import pygame
from random import randint

class Zombie():
    def __init__(self, xpos, ypos):
        self.zombieImage = pygame.image.load('./Assets/zombie.png')
        self.zombieImage = pygame.transform.scale(self.zombieImage, (64, 64))
        self.xpos = xpos
        self.ypos = ypos
        self.rect = self.zombieImage.get_rect()
        self.direction = -1
        self.health = 100
        self.speed = 2
        self.attack = 10
        self.attack_cd = 0
        self.mask = pygame.mask.from_surface(self.zombieImage)

    def draw(self,window):
        window.blit(self.zombieImage, (self.xpos, self.ypos))

    def hit(self, damage):
        self.health -= damage

def collide(obj1, obj2):
    offset_x = obj2.xpos - obj1.xpos
    offset_y = obj2.ypos - obj1.ypos
    return obj1.mask.overlap(obj2.mask, (offset_x, offset_y)) != None