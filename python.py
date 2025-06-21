from pylab import *
from pygame.math import Vector2 as vec2


def dda_ray_cast(start_pos, angle, draw_cell, num_cells):
    # Conditions initialmes
    direction = vec2(cos(angle), sin(angle))

    cell = vec2(floor(start_pos.x), floor(start_pos.y))
    offset = start_pos - cell

    step = vec2(
        1 if direction.x > 0 else -1,
        1 if direction.y > 0 else -1
    )

    # Distance de parcours d'une case, en X et en Y
    distBetweenColumns = abs(1/direction.y) if direction.y != 0 else float('inf')
    distBetweenRows = abs(1/direction.x) if direction.x != 0 else float('inf')

    distToX = (1-offset.x if step.x == 1 else offset.x) * distBetweenRows
    distToY = (1-offset.y if step.y == 1 else offset.y) * distBetweenColumns

    for _ in range(num_cells):
        # case la moins lointaine entre +x et +y
        if distToX < distToY:
            cell.x += step.x
            distToX += distBetweenRows
        else:
            cell.y += step.y
            distToY += distBetweenColumns
        draw_cell(*cell)




# --- Visualisation Pygame ---

import pygame as pg
import math

CELL_SIZE = 40
GRID_W, GRID_H = 16, 12
SCREEN_W, SCREEN_H = GRID_W * CELL_SIZE, GRID_H * CELL_SIZE

pg.init()
screen = pg.display.set_mode((SCREEN_W, SCREEN_H))
clock = pg.time.Clock()

def draw_grid():
    # Dessine la grille
    for x in range(GRID_W):
        for y in range(GRID_H):
            rect = pg.Rect(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE)
            pg.draw.rect(screen, (200, 200, 200), rect, 1)

def draw_cell(x, y):
    # Colorie une cellule traversée par le rayon
    rect = pg.Rect(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE)
    pg.draw.rect(screen, (255, 200, 100), rect)
    pg.draw.rect(screen, (200, 120, 0), rect, 2)

def main():
    ray_origin = vec2(3.5, 5.5)  # Position de départ du rayon (au centre d'une cellule)
    ray_angle = math.radians(30) # Angle initial du rayon

    running = True
    while running:
        for event in pg.event.get():
            if event.type == pg.QUIT:
                running = False

        # Calcule l'angle du rayon en fonction de la position de la souris
        mouse_pos = pg.mouse.get_pos()
        mx, my = mouse_pos[0] / CELL_SIZE, mouse_pos[1] / CELL_SIZE
        ray_angle = math.atan2(my - ray_origin.y, mx - ray_origin.x)

        screen.fill((30, 30, 30))
        draw_grid()

        # Dessine le point d'origine du rayon
        pg.draw.circle(screen, (0, 255, 0), (int(ray_origin.x*CELL_SIZE), int(ray_origin.y*CELL_SIZE)), 7)

        visited = []
        def callback(x, y):
            visited.append((x, y))
            draw_cell(x, y)

        # Lance le rayon avec l'algorithme DDA
        start = (ray_origin.x*CELL_SIZE, ray_origin.y*CELL_SIZE)
        dda_ray_cast(ray_origin, ray_angle, callback, 10)

        # Dessine le rayon jusqu'à la dernière cellule visitée
        if visited:
            end = ((visited[-1][0]+0.5)*CELL_SIZE, (visited[-1][1]+0.5)*CELL_SIZE)
            pg.draw.line(screen, (0, 255, 255), start, pg.mouse.get_pos(), 2)

        pg.display.flip()
        clock.tick(60)

    pg.quit()

if __name__ == "__main__":
    main()
