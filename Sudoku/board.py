import pygame
from cell import Cell
from constants import *
from sudoku_generator import generate_sudoku

difficulty_settings = {"Easy": 30, "Medium": 40, "Hard": 50}
CELLSIZE = 10


class Board:
    # represents sudoku board, has 81 cell objects.
    def __init__(self, width, height, screen, difficulty):
        self.width = width
        self.height = height
        self.screen = screen
        self.difficulty = difficulty
        self.board = self.initialize_board()
        self.selected_row = 0
        self.selected_col = 0
        self.original_board = generate_sudoku(9, difficulty)
        self.cells = self.initialize_cells()
        self.original_cells = [row[:] for row in self.cells]

    def initialize_board(self):
        # initializes the board
        return [["0" for _ in range(9)] for _ in range(9)]

    def initialize_cells(self):
        # initializes the cell with original board
        return [[Cell(self.original_board[i][j], i, j, self.screen) for j in range(9)] for i in range(9)]

    def draw(self):
        # draws the board
        self.screen.fill(background_color)
        self.draw_grid()
        for row in self.cells:
            for cell in row:
                cell.draw()

    def draw_grid(self):
        # draws the grid for board
        for i in range(1, num_rows):
            pygame.draw.line(self.screen, grid_color, (0, i * square_size), (width, i * square_size), line_width)
        for i in range(1, num_cols):
            pygame.draw.line(self.screen, grid_color, (i * square_size, 0), (square_size * i, height), line_width)
        for i in range(0, num_cols, 3):
            pygame.draw.line(self.screen, grid_color, (0, i * square_size), (width, i * square_size), line_width * 2)
            for j in range(0, num_rows, 3):
                pygame.draw.line(self.screen, grid_color, (j * square_size, 0), (square_size * j, height),
                                 line_width * 2)

    def select(self, row, col):
        # marks the cell at (row, col) in the board as the current selected cell.
        # Carter Swarm
        self.selected_row, self.selected_col = row + 1, col + 1
        pygame.draw.rect(self.screen, red, (col * square_size, row * square_size, square_size, square_size), 2)

    def click(self, x, y):
        # if a tuple of (x,y) coordinates is within board,
        # Carter Swarm
        if 1 <= x <= 9:
            row = (y // square_size) + 1
            col = (x // square_size) + 1
            return row, col
        return None

    def clear(self):
        # clears the value cell, can only remove cell values and sketched value done by player
        # Carter Swarm
        if self.original_board[self.selected_row][self.selected_col] == 0:
            cell = self.cells[self.selected_row][self.selected_col]
            cell.set_cell_value(0)

    def sketch(self, value):
        # sets the sketched value of the selected cell equal to user entered value
        self.cells[self.selected_row][self.selected_col].draw()
        # will be displayed at the top left corner of the cell using draw() function
        # Carter Swarm

    def place_number(self, value):
        # sets value of current selected cell equal to user entered value
        # called when user presses the Enter Key
        if self.original_board[self.selected_row][self.selected_col] == 0:
            cell = self.cells[self.selected_row][self.selected_col]
            cell.set_cell_value(value)
            # Carter Swarm

    def reset_to_original(self):
        # resets all cells to board to original values(0 is cleared, otherwise corresponding digit)
        self.board = self.original_board
        self.cells = self.initialize_cells()

    def is_full(self):
        for row in self.cells:
            for cell in row:
                if cell.value == "0":
                    return False
        return True

    # car
    # checks if board is full, returns true if all cells are filled, if not False

    def find_empty(self):
        # finds an empty cell and returns its row and col as a tuple(x,y)
        for i, row in enumerate(self.cells):
            for j, cell in enumerate(row):
                if cell.value == "0":
                    return i, j
                # if finds no empthy cells or if somthing else is wrong
                # Carter Swarm
        return None

    def check_board(self):
        # checks if board is solved correctly, returns true if all cells are filled, if not False
        for i in range(9):
            for j in range(9):
                if not self.is_valid(i, j, self.cells[i][j].value):
                    return False
        return True

    # Carter Swarm

    def is_valid(self, row, col, num):
        return self.valid_in_row(row, num) and self.valid_in_col(col, num) and self.valid_in_box(row - row % 3,
                                                                                                 col - col % 3, num)

    # checks if placing number in square is valid (ryan)

    def valid_in_row(self, row, num):
        for col in range(9):
            if col != self.selected_col and self.cells[row][col].value == num:
                return False
        return True

    # checks if placing number in row is valid

    def valid_in_col(self, col, num):
        for row in range(9):
            if row != self.selected_row and self.cells[row][col].value == num:
                return False
        return True

    # checks if placing number in the column is valid

    def valid_in_box(self, row_start, col_start, num):
        for row in range(row_start, row_start + 3):
            for col in range(col_start, col_start + 3):
                if row != self.selected_row and col != self.selected_col and self.cells[row][col].value == num:
                    return False
        return True
    # checks if placing is number in the box is valid

