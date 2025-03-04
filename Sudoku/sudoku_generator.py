"""
This was adapted from a GeeksforGeeks article "Program for Sudoku Generator" by Aarti_Rathi and Ankur Trisal
https://www.geeksforgeeks.org/program-sudoku-generator/

"""
import math,random

class SudokuGenerator:

  def __init__(self, row_length, removed_cells):

    self.row_length = row_length #given in Generate_sudoku, standard is 9
    self.board = [[0 for i in range(self.row_length)] for i in range(self.row_length)] #creates a 9x9 board, nested list
    self.box_length = int(math.sqrt(self.row_length)) #square root of the length
    self.removed_cells = removed_cells #This value is changed based on difficulty of the Sudoku
    #cars

  def get_board(self):
    return self.board

  def print_board(self):
      for row in range(self.row_length):
          for col in range(self.row_length):
              print(self.board[row][col], end=" ")
          print()
    #car
  def valid_in_row(self, row, num):
    #checks to see if it is valid in a row, by seeing if any == num
    #if no, True, if yes, False
    for cell in range(0, self.row_length):
      if self.board[row][cell] == num:
        return False
    return True
    #car

  def valid_in_col(self, col, num):
    #checks if it is valid in a col, same as the other one but with some things switched
    #around in order to check vertically
    for cell in range(self.row_length):
      if self.board[cell][col] == num:
        return False
    return True
    #car

  def valid_in_box(self, row_start, col_start, num):
    #if it is valid in the 3x3 box, then it is True, else False.
    #When called must use the (row//self.box_length) * self.box_length format for the values.
    for row_cells in range(row_start, row_start+3):
      for col_cells in range(col_start, col_start + 3):
        if self.board[row_cells][col_cells] == num:
          return False
    return True
    #car

  def is_valid(self, row, col, num):
    #if its valid in all three subsequent functions, returns true
    return (self.valid_in_row(row, num) and self.valid_in_col(col, num) and
            self.valid_in_box(row - row % self.box_length, col - col % self.box_length, num))
    #car

  def fill_box(self, row_start, col_start):
    for row in range(self.box_length):
        for col in range(self.box_length):
            while True:
                num = random.randrange(1, 10)
                if self.valid_in_box(row_start, col_start, num):
                    break
            self.board[row_start + row][col_start + col] = num

    #car

  def fill_diagonal(self):
    #fills the boxes in the diagonal sudoku
    #using starting points for them into fill_box
    self.fill_box(0,0)
    self.fill_box(3,3)
    self.fill_box(6,6)
    #car

  def fill_remaining(self, row, col):
    #given to us, requires other methods
    if (col >= self.row_length and row < self.row_length - 1):
        row += 1
        col = 0
    if row >= self.row_length and col >= self.row_length:
        return True
    if row < self.box_length:
        if col < self.box_length:
            col = self.box_length
    elif row < self.row_length - self.box_length:
        if col == int(row // self.box_length * self.box_length):
            col += self.box_length
    else:
        if col == self.row_length - self.box_length:
            row += 1
            col = 0
            if row >= self.row_length:
                return True

    for num in range(1, self.row_length + 1):
        if self.is_valid(row, col, num):
            self.board[row][col] = num
            if self.fill_remaining(row, col + 1):
                return True
            self.board[row][col] = 0
    return False

  def fill_values(self):
    self.fill_diagonal()
    self.fill_remaining(0, self.box_length)
  #given to us, calls other funcs

  def remove_cells(self):
      #removes amount of cells which is specified in the Generate_sudoku func
      #removes amount till amount is zero and the while condition is met
      #checks to see if value == 0 if not it removes them, helps not duplicate removing
    while self.removed_cells > 0:
        row_cells_to_be_removed = random.randint(0, self.row_length - 1)
        col_cells_to_be_removed = random.randint(0, self.row_length - 1)
        if self.board[row_cells_to_be_removed][col_cells_to_be_removed] != 0:
            self.board[row_cells_to_be_removed][col_cells_to_be_removed] = 0
            self.removed_cells -= 1
          #car



def generate_sudoku(size, removed):
  sudoku = SudokuGenerator(size, removed)
  sudoku.fill_values()
  board = sudoku.get_board()
  sudoku.remove_cells()
  board = sudoku.get_board()
  return board
  #this is outside of the sudokugenerator class.
  #given size and removed, generates  and returns sizebysize sudoku board
  #board has cleared 'removed' number of cells, calls constructor and appropriate methods from SudokuGenerator class.
  #OK THIS ENTIRE class is done, im done for this part and now all thats left is to implement the pygame stuff. Im not that great at it (carson) so if anyone can take some cracks at it that'd be great. when generate sudoku is called, it gives a completed random list.
#I can try to take a crack at it in impli,ting the Pygame stuff in the Board and Cell classes. also need the main as well

#this is the document to our report

#https://docs.google.com/document/d/1W6TIohiULLwkD1dEoaj6iQGRsW1WrcCDK31I5wQktME/edit?usp=sharing