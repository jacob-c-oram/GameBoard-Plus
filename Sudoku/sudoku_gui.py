import pygame
import sys
from board import Board
from constants import *

# initializes pygame and sets up the screen
pygame.init()
screen = pygame.display.set_mode((width, window_height))
exit_button_rect = pygame.Rect(0, 0, 0, 0)
restart_button_rect = pygame.Rect(0, 0, 0, 0)


def draw_buttons():
    # Draw the buttons
    screen.blit(restart_surface, restart_button_rect)
    screen.blit(reset_surface, reset_button_rect)
    screen.blit(exit_surface, exit_button_rect)


# displays the main menu
def display_game_screen(screen):
    # sets up the fonts
    easy_font = pygame.font.Font(None, 80)
    button_font = pygame.font.Font(None, 45)

    # loads in the background image
    menu_image = pygame.image.load("SUDOKUBACKGROUND.jpg")
    menu_image = pygame.transform.scale(menu_image, (1000, 675))
    screen.blit(menu_image, (0, 0))

    # displays the main title of the game
    main_title = easy_font.render("Welcome to Sudoku", 10, white)
    main_title_rect = main_title.get_rect(center=(width // 2, height // 2 - 50))
    screen.blit(main_title, main_title_rect)

    # the text for the dif game modes
    game_font = pygame.font.Font(None, 50)
    game_mode = game_font.render("Select Game Mode:", 10, white)
    game_rect = game_mode.get_rect(center=(width - 300, height - 160))
    screen.blit(game_mode, game_rect)

    # text for the difficulties
    easy_title = button_font.render("Easy", 10, (255, 255, 255))
    medium_title = button_font.render("Medium", 10, (255, 255, 255))
    hard_title = button_font.render("Hard", 10, (255, 255, 255))

    # creates buttons for each of the difficulties
    easy_button = pygame.Surface(
        (easy_title.get_size()[0] + 15, easy_title.get_size()[1] + 15))
    easy_button.fill(orange)
    easy_button.blit(easy_title, (10, 10))
    medium_button = pygame.Surface(
        (medium_title.get_size()[0] + 15, medium_title.get_size()[1] + 15))
    medium_button.fill(orange)
    medium_button.blit(medium_title, (10, 10))
    hard_button = pygame.Surface(
        (hard_title.get_size()[0] + 15, hard_title.get_size()[1] + 15))
    hard_button.fill(orange)
    hard_button.blit(hard_title, (10, 10))

    # setting the positions for the buttons
    easy_rect = easy_button.get_rect(center=(width // 2 - 180,
                                             height // 2 + 200))
    medium_rect = medium_button.get_rect(center=(width // 2, height // 2 + 200))
    hard_rect = hard_button.get_rect(center=(width // 2 + 180,
                                             height // 2 + 200))

    #  displays the buttons/
    screen.blit(easy_button, easy_rect)
    screen.blit(medium_button, medium_rect)
    screen.blit(hard_button, hard_rect)

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                if easy_rect.collidepoint(event.pos):
                    return 1
                elif medium_rect.collidepoint(event.pos):
                    return 40
                elif hard_rect.collidepoint(event.pos):
                    return 50

        pygame.display.update()


def reset_board(board):
    # Reset the board to its original state
    board.reset_to_original()


def display_game_over_screen(screen, restart_button_rect=None, exit_button_rect=None):
    # displays the game over screen
    restart_button_rect, exit_button_rect
    game_over_font = pygame.font.Font(None, 40)
    restart_game = False
    background_image = pygame.image.load("SUDOKUBACKGROUND.jpg")
    background_image = pygame.transform.scale(background_image,
                                              (width + 150, height + 150))

    while not restart_game:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if restart_button_rect.collidepoint(event.pos):
                    restart_game = True
                elif exit_button_rect.collidepoint(event.pos):
                    pygame.quit()
                    sys.exit()

        screen.blit(background_image, (0, 0))

        if winner != 0:
            text = 'Game Won!'
        else:
            text = "Game Over :("

        game_over_surface = game_over_font.render(text, 10, background_color)
        game_over_rect = game_over_surface.get_rect(center=(width // 2,
                                                            height // 2 - 100))
        screen.blit(game_over_surface, game_over_rect)

        restart_surface = game_over_font.render('Restart', 10, background_color)
        restart_button_rect = restart_surface.get_rect(center=(width // 2,
                                                               height // 2 + 100))
        screen.blit(restart_surface, restart_button_rect)

        menu_surface = game_over_font.render('Exit', 10, background_color)
        exit_button_rect = menu_surface.get_rect(center=(width // 2,
                                                         height // 2 + 150))
        screen.blit(menu_surface, exit_button_rect)

        pygame.display.update()

    return restart_game


if __name__ == '__main__':
    # initialize pygame and set up the screen
    screen = pygame.display.set_mode((width, window_height))

    # initialize button rectangles
    exit_button_rect = pygame.Rect(0, 0, 0, 0)
    restart_button_rect = pygame.Rect(0, 0, 0, 0)

    # variables for game state
    board = None
    winner = False
    game_over = False

    # get user-selected difficulty level from the main menu
    difficulty = display_game_screen(screen)

    # set up font for game over and buttons
    game_over_font = pygame.font.Font(None, 50)

    # create the buttons
    restart_title = game_over_font.render("Restart", 10, white)
    restart_surface = pygame.Surface(
        (restart_title.get_size()[0] + 20, restart_title.get_size()[1] + 20))
    restart_button_rect = restart_surface.get_rect(center=(width // 2 - 200,
                                                           height // 2 + 325))
    restart_surface.fill(orange)
    restart_surface.blit(restart_title, (10, 10))

    reset_title = game_over_font.render("Reset", 10, white)
    reset_surface = pygame.Surface(
        (reset_title.get_size()[0] + 20, reset_title.get_size()[1] + 20))
    reset_button_rect = reset_surface.get_rect(center=(width // 2,
                                                       height // 2 + 325))
    reset_surface.fill(orange)
    reset_surface.blit(reset_title, (10, 10))

    exit_title = game_over_font.render("Exit", 10, white)
    exit_surface = pygame.Surface(
        (exit_title.get_size()[0] + 20, exit_title.get_size()[1] + 20))
    exit_button_rect = exit_surface.get_rect(center=(width // 2 + 200,
                                                     height // 2 + 325))
    exit_surface.fill(orange)
    exit_surface.blit(exit_title, (10, 10))

    # main game loop
    while True:
        # clears the screen
        screen.fill(white)

        # create a new game board
        board = Board(width, height, screen, difficulty)
        board.draw()

        # Draw the buttons
        draw_buttons()

        pygame.display.flip()

        num = -1
        clicked_row, clicked_col = -1, -1

        # game loop for handling the inputs during game
        while not game_over:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    sys.exit()

                if event.type == pygame.MOUSEBUTTONDOWN and not game_over:
                    if reset_button_rect.collidepoint(event.pos):
                        # reset the board to its original state
                        board.reset_to_original()
                        clicked_row, clicked_col = -1, -1  # Reset clicked cells
                        board.draw()
                        draw_buttons()
                    elif exit_button_rect.collidepoint(event.pos):
                        # exit the game
                        pygame.quit()
                        sys.exit()
                    elif restart_button_rect.collidepoint(event.pos):
                        # restarts the game with the new difficulty level
                        difficulty = display_game_screen(screen)
                        screen.fill(white)
                        board = Board(width, height, screen, difficulty)
                        clicked_row, clicked_col = -1, -1  # Reset clicked cells
                        board.draw()
                        draw_buttons()
                    else:
                        # checks for clicks on the game board
                        clicked_row = int(event.pos[1] / square_size)
                        clicked_col = int(event.pos[0] / square_size)
                        if 0 <= clicked_row < height and 0 <= clicked_col < width:
                            if board.selected_row is not None and board.selected_col is not None:
                                board.cells[board.selected_row][board.selected_col].set_selected(False)
                            board.selected_row = clicked_row
                            board.selected_col = clicked_col
                            board.cells[clicked_row][clicked_col].set_selected(True)
                            board.draw()
                            draw_buttons()

                if event.type == pygame.KEYDOWN:
                    if event.key in [pygame.K_1, pygame.K_2, pygame.K_3, pygame.K_4, pygame.K_5, pygame.K_6, pygame.K_7,
                                     pygame.K_8, pygame.K_9]:
                        num = int(event.unicode)
                        if 0 <= clicked_row < height and 0 <= clicked_col < width:
                            if board.original_board[clicked_row][clicked_col] == 0:
                                board.cells[clicked_row][clicked_col].set_cell_value(num)
                                board.draw()
                                draw_buttons()

                    if event.key == pygame.K_RETURN and num > 0:
                        # places the number on the board
                        board.place_number(num)

                        if board.is_full():
                            # check if the board is full and determines winner or loser
                            if board.check_board():
                                winner = 1
                            else:
                                winner = 1
                                game_over = True

                if game_over:
                    # shows the game over screen and ask for user input
                    pygame.display.update()
                    pygame.time.delay(1000)
                    restart_game = display_game_over_screen(screen)

                    if restart_game:
                        # restart the game and initialize a new board
                        difficulty = display_game_screen(screen)
                        screen.fill(white)
                        board = Board(width, height, screen, difficulty)
                        board.draw()
                        draw_buttons()
                        game_over = False
                        winner = False
            pygame.display.update()