import pygame, sys
import os
import random
from src.zombie import Zombie, collide
from src.plant import PeaShooter, SunFlower
from src.bullet import Bullet

pygame.init()
pygame.font.init()

FPS = 60 # Set FPS of PvZ to 60FPS
WINDOWWIDTH = 640 # width of screen is 640pxl, each cell is 64pxl wide
WINDOWHEIGHT = 448 # height of screen is 448pxl, each cell is 64pxl high
ROW = 7 # No. of rows on the screen
COL = 10 # No. of columns on the screen
COUNT_CHECK = 300 # Checks if enough time have passed for the Sunflowers to generate additional coins

WINDOW = (WINDOWWIDTH, WINDOWHEIGHT)
SCREEN = pygame.display.set_mode(WINDOW)
pygame.display.set_caption('Plants VS Zombies')
lost_font = pygame.font.SysFont("comicsans", 60)
BG = pygame.transform.scale(pygame.image.load(os.path.join("Assets", "grassland.png")), WINDOW)
# top left corner
cellPosX = [0, 64, 128, 192, 256, 320, 384, 448, 512, 576] # Stores the position the cells X-Pos (from the top left corner)
cellPosY = [0, 64, 128, 192, 256, 320, 384] # Stores the position the cells Y-Pos (from the top left corner)
# Cell occupancy status:
occupancy = [False] * (ROW*COL) # Check if the cell currently have a plant occupying it
coin = 0 # Initialise coin, which shows available coins to buy plants

coin_font = pygame.font.SysFont("comicsans", 30)
health_font = pygame.font.SysFont("comicsans", 30)
lost_font = pygame.font.SysFont("comicsans", 70)

"""
The drawWindow method helps draw the different zombie's, plants and bullets onto the screen
"""
def drawWindow(zombieList, plantList, bulletList, lost):
    global coin
    SCREEN.blit(BG, (0, 0))

    for z in zombieList:
        z.draw(SCREEN)
    
    for p in plantList:
        p.draw(SCREEN)
    
    for b in bulletList:
        b.draw(SCREEN)

    if lost:
        lost_label = lost_font.render("Game Over!", 1, (255, 255, 255))
        SCREEN.blit(lost_label, (WINDOWWIDTH/2 - lost_label.get_width()/2, 210))
    else:
        coin_label = coin_font.render(f"Coin: {coin}", 1, (255, 255, 255))
        SCREEN.blit(coin_label, (WINDOWWIDTH - 100, 10))

    pygame.display.update()

"""
The main loop of the game, where most of the logics and control are implemented
"""
def main():
    global coin
    run = True
    fpsClock = pygame.time.Clock()
    counter = 1
    
    zombieList = [] # keep track of all the zombies spawned
    plantList = [] # keep track of all the plant spawned
    bulletList = [] # keep track of all the bullets fired

    lost = False
    lost_count = 0

    while run:
        fpsClock.tick(FPS)
        
        # Spawn Zombies:
        if counter%200 == 0:
            randomY = random.randrange(0, len(cellPosY))
            z = Zombie(cellPosX[-1], cellPosY[randomY])
            zombieList.append(z)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            # Generate a PeaShooter plant
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1: # 1 for left button
                x, y = pygame.mouse.get_pos()
                indX = x//64
                indY = y//64
                if coin - 25 >= 0 and not occupancy[(indY*COL)+indX]:
                    occupancy[(indY*COL)+indX] = True
                    newPlant = PeaShooter(cellPosX[indX], cellPosY[indY])
                    plantList.append(newPlant)
                    coin -= 25
            # Generate a SunFlower plant
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 3: # 3 for right button
                x, y = pygame.mouse.get_pos()
                indX = x//64
                indY = y//64
                if coin - 50 >= 0 and not occupancy[(indY*COL)+indX]:
                    occupancy[(indY*COL)+indX] = True
                    newPlant = SunFlower(cellPosX[indX], cellPosY[indY])
                    plantList.append(newPlant)
                    coin -= 50
        
        for p in plantList[:]:
            # If the plant is dead, remove said plant from the available plant list
            # and free up its occupancy in the occupancy list
            if p.health <= 0:
                tempX = p.xpos//64
                tempY = p.ypos//64
                occupancy[(tempY*COL)+tempX] = False
                plantList.remove(p)
                continue
            if type(p).__name__ == 'SunFlower' and counter == COUNT_CHECK:
                coin += 10
                continue
            if type(p).__name__ == 'PeaShooter' and p.cooldown_counter == p.cooldown:
                bullet = Bullet(p.xpos, p.ypos)
                bulletList.append(bullet)
                p.cooldown_counter = 0
            elif type(p).__name__ == 'PeaShooter':
                p.cooldown_counter += 1
            

        for z in zombieList[:]:
            checked = False
            if z.xpos < 0:
                lost = True
                break
            if z.health <= 0:
                zombieList.remove(z)
                coin += 10
                continue
            # Check if the zombies have reached a plant
            # If it has, it'll attack the plant whenever its attack cd is done
            # Otherwise, it'll carry on moving forward at its own zombie pace
            for p in plantList[:]:
                if collide(z, p):
                    if z.attack_cd == 50:
                        if p.health > 0:
                            p.health -= z.attack
                            z.attack_cd = 0
                            checked = True
                            break
                        else:
                            tempX = p.xpos//64
                            tempY = p.ypos//64
                            occupancy[(tempY*COL)+tempX] = False
                            plantList.remove(p)
                            checked = True
                            break
                    else:
                        z.attack_cd += 1
                        checked = True
                        break
                elif z.ypos == p.ypos and not collide(z, p):
                    if counter%50 == 0 and z.xpos >= 0:
                        z.xpos += z.speed*z.direction
                        z.attack_cd += 1
                        checked = True
                    break
                elif z.ypos == p.ypos:
                    if counter%50 == 0 and z.xpos >= 0:
                        z.xpos += z.speed*z.direction
                        z.attack_cd += 1
                        checked = True
                    break
                    
            if not checked and counter%10 == 0 and z.xpos >= 0:
                z.xpos += z.speed*z.direction
                z.attack_cd += 1
        
        for b in bulletList[:]:
            checked = False
            if b.xpos > cellPosX[-1]+64:
                bulletList.remove(b)
                continue

            # Check if any of the bullet have hit the Zombies
            # If it has, it'll remove the bullet from the bullet list
            # and deduct health off the hit zombie
            # If the bullet goes beyone the border of the screen, remove it as well
            for z in zombieList[:]:
                if collide(b, z):
                    bulletList.remove(b)
                    z.health -= b.damage
                    checked = True
                    break
                elif b.ypos == z.ypos:
                    b.xpos += b.speed*b.direction
                    checked = True
                    break
            
            if not checked:
                b.xpos += b.speed*b.direction

        pygame.display.update()
        
        if counter%20 == 0:
            coin += 1

        if counter == COUNT_CHECK:
            counter = 1
        else:
            counter += 1

        if lost:
            if lost_count > FPS*3:
                run = False
            else:
                lost_count += 1
                drawWindow([], [], [], lost)
                continue
        else:
            drawWindow(zombieList, plantList, bulletList, lost)

"""
This is where the player will enter when they launch the game.
The user will select the difficulty of the game.
"""
def launch_menu():
    global coin
    difficulty_font = pygame.font.SysFont("comicsans",40)
    rect_font = pygame.font.SysFont("comicsans",30)
    run = True

    while run:
        easyButton = pygame.Rect(80, 200, 100, 100)
        normalButton = pygame.Rect(280, 200, 100, 100)
        hardButton = pygame.Rect(460, 200, 100, 100)

        SCREEN.blit(BG, (0, 0))
        difficulty_label = difficulty_font.render("Choose your difficulty!", 1, (255, 255, 255))
        SCREEN.blit(difficulty_label, (WINDOWWIDTH/2 - difficulty_label.get_width()/2, 100))
        
        pygame.draw.rect(SCREEN, (255, 0, 0), easyButton)
        pygame.draw.rect(SCREEN, (255, 255, 0), normalButton)
        pygame.draw.rect(SCREEN, (0, 0, 255), hardButton)

        easy_label = rect_font.render("Easy!", 1, (0, 0, 0))
        SCREEN.blit(easy_label, (80 + easy_label.get_width()/2, 240))

        normal_label = rect_font.render("Normal!", 1, (0, 0, 0))
        SCREEN.blit(normal_label, (254 + normal_label.get_width()/2, 240))

        hard_label = rect_font.render("Hard!", 1, (0, 0, 0))
        SCREEN.blit(hard_label, (455 + hard_label.get_width()/2, 240))

        pygame.display.update()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
                break
            if event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1 and easyButton.collidepoint(event.pos):
                    coin = 150
                    main()
                elif event.button == 1 and normalButton.collidepoint(event.pos):
                    coin = 100
                    main()
                elif event.button == 1 and hardButton.collidepoint(event.pos):
                    coin = 50
                    main()
    
    pygame.quit()

if __name__ == "__main__":
    launch_menu()
    