import pygame
import os

class Plant():
    def __init__(self, xpos, ypos, cd=100):
        self.xpos = xpos
        self.ypos = ypos
        self.cooldown = cd
        self.plantImage = None
        self.cooldown_counter = 0
        self.health = 100

    def draw(self, window):
        window.blit(self.plantImage, (self.xpos, self.ypos))

class PeaShooter(Plant):
    def __init__(self, xpos, ypos):
        super().__init__(xpos, ypos)
        self.plantImage = pygame.image.load('./Assets/peashooter.png')
        self.plantImage = pygame.transform.scale(self.plantImage, (64, 64))
        self.mask = pygame.mask.from_surface(self.plantImage)
        

class SunFlower(Plant):
    def __init__(self, xpos, ypos):
        super().__init__(xpos, ypos)
        self.plantImage = pygame.image.load('./Assets/sunflower.png')
        self.plantImage = pygame.transform.scale(self.plantImage, (64, 64))
        self.mask = pygame.mask.from_surface(self.plantImage)