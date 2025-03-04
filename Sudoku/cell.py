import pygame
from constants import *

CELLSIZE = 10


class Cell:

    def __init__(self, value, row, col, screen):
        # Constructor for the Cell class
        self.value = value
        self.row = row
        self.col = col
        self.screen = screen
        self.selected = False

    def set_cell_value(self, value):
        # Setter for this cell’s value
        self.value = value
        return self.value
        # carson

    def set_selected(self, selected):
        self.selected = selected

    def draw(self):
        # draws the cell, along with value inside it
        num_font = pygame.font.Font(None, 25)
        surfaces = [num_font.render(str(i), 0, number_color) for i in range(1, 10)]

        for i, surface in enumerate(surfaces, start=1):
            rect = surface.get_rect(
                center=(self.col * square_size + square_size // 2, self.row * square_size + square_size // 2))
            if self.selected:
                pygame.draw.rect(self.screen, red,
                                 (self.col * square_size, self.row * square_size, square_size, square_size), 2)

            self.screen.blit(surface, rect) if self.value == i else None

        #