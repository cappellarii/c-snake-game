#include <GL/freeglut.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// dimensões do grid
#define GRID_SIZE 30
#define MAX_SNAKE (GRID_SIZE * GRID_SIZE)
#define FRAME_MS 16
#define WINDOW_SIZE 600

// tempo
#define EASY_TICK_MS 160
#define NORMAL_TICK_MS 100
#define HARD_TICK_MS 65

typedef struct {
    int x;
    int y;
} Point;

// 0 = cabeça e o resto só segue o path
Point snake[MAX_SNAKE];
int snake_len = 3;
int dir_x = 1, dir_y = 0;
bool can_turn = true;
Point food;
bool game_over = false;
bool game_won = false;
bool paused = false;
int score = 0;
int best_score = 0;
int mode = 1;
int elapsed_ms = 0;
bool round_started = false;
bool score_reported = false;


bool select_difficulty(void) {
    puts("Jogo da Cobra - escolha a dificuldade:");
    puts("  1. Fácil");
    puts("  2. Normal");
    puts("  3. Difícil");

    for (;;) {
        int choice = 0;
        bool invalid = false;
        int ch;

        printf("Digite 1, 2 ou 3: ");
        fflush(stdout);

        
        // seleção de dificuldade
        while ((ch = getchar()) != '\n' && ch != EOF) {
            if (isspace((unsigned char)ch)) {
                continue;
            }
            if (choice == 0 && ch >= '1' && ch <= '3') {
                choice = ch - '0';
            } else {
                invalid = true;
            }
        }

        if (ferror(stdin)) {
            fputs("Erro ao ler a dificuldade no terminal.\n", stderr);
            return false;
        }
        if (choice != 0 && !invalid) {
            mode = choice - 1;
            return true;
        }
        if (ch == EOF) {
            fputs("Nenhuma dificuldade selecionada; encerrando.\n", stderr);
            return false;
        }
        puts("Opção inválida. Digite 1, 2 ou 3.");
    }
}


const char *mode_name(void) {
    if (mode == 0) return "Fácil";
    if (mode == 2) return "Difícil";
    return "Normal";
}


void print_scoreboard(const char *result) {
    if (!round_started || score_reported) return;

    printf("\n--- Placar da cobra ---\n");
    printf("Resultado: %s\n", result);
    printf("Dificuldade: %s\n", mode_name());
    printf("Pontos: %d\n", score);
    printf("Recorde da sessão: %d\n", best_score);
    fflush(stdout);
    score_reported = true;
}


int tick_ms(void) {
    if (mode == 0) return EASY_TICK_MS;
    if (mode == 2) return HARD_TICK_MS;
    return NORMAL_TICK_MS;
}


bool spawn_food(void) {
    Point free_cells[MAX_SNAKE];
    int free_count = 0;

    // função de celulas livres pra comida
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            bool occupied = false;
            for (int i = 0; i < snake_len; i++) {
                if (snake[i].x == x && snake[i].y == y) {
                    occupied = true;
                    break;
                }
            }
            if (!occupied) {
                free_cells[free_count++] = (Point){ x, y };
            }
        }
    }
    if (free_count == 0) {
        return false;
    }
// random da comida
    food = free_cells[rand() % free_count];
    return true;
}


void reset_game(void) {
    print_scoreboard("Reiniciado");
    snake_len = 3;
    for (int i = 0; i < snake_len; i++) {
        snake[i].x = (GRID_SIZE / 2) - i;
        snake[i].y = GRID_SIZE / 2;
    }
    dir_x = 1;
    dir_y = 0;
    can_turn = true;
    game_over = false;
    game_won = false;
    paused = false;
    score = 0;
    elapsed_ms = 0;
    round_started = true;
    score_reported = false;
    if (!spawn_food()) {
        game_over = true;
        game_won = true;
        print_scoreboard("Vitória");
    }
}

// células do grid
void draw_cell(int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x + 0.05f, y + 0.05f);
    glVertex2f(x + 0.95f, y + 0.05f);
    glVertex2f(x + 0.95f, y + 0.95f);
    glVertex2f(x + 0.05f, y + 0.95f);
    glEnd();
}

// função texto na tela
void draw_text(float x, float y, const char *text, void *font) {
    glRasterPos2f(x, y);
    while (*text) {
        unsigned char ch = (unsigned char)*text++;
        // converte caracteres UTF-8 para Latin-1.
        if ((ch == 0xc2 || ch == 0xc3) &&
            ((unsigned char)*text & 0xc0) == 0x80) {
            ch = (unsigned char)(((ch & 0x1f) << 6) | ((unsigned char)*text++ & 0x3f));
        }
        glutBitmapCharacter(font, ch);
    }
}

// comida e cobra

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    draw_cell(food.x, food.y, 0.9f, 0.2f, 0.2f);

    for (int i = 0; i < snake_len; i++) {
        if (i == 0) {
            draw_cell(snake[i].x, snake[i].y, 0.3f, 1.0f, 0.3f);
        } else {
            draw_cell(snake[i].x, snake[i].y, 0.1f, 0.7f, 0.2f);
        }
    }

    // coordenadas da janela pra desenhar e uma corzinha

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, WINDOW_SIZE, 0.0, WINDOW_SIZE);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(1.0f, 1.0f, 1.0f);
    draw_text(16.0f, WINDOW_SIZE - 28.0f, "Pontos:", GLUT_BITMAP_9_BY_15);
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "%d    Recorde: %d    Modo: %s", score, best_score, mode_name());
    draw_text(85.0f, WINDOW_SIZE - 28.0f, score_text, GLUT_BITMAP_9_BY_15);

    if (game_over) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.78f);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(WINDOW_SIZE, 0.0f);
        glVertex2f(WINDOW_SIZE, WINDOW_SIZE);
        glVertex2f(0.0f, WINDOW_SIZE);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        draw_text(230.0f, 355.0f, game_won ? "VOCÊ VENCEU!" : "FIM DE JOGO", GLUT_BITMAP_HELVETICA_18);
        snprintf(score_text, sizeof(score_text), "Pontos: %d   Recorde: %d", score, best_score);
        draw_text(205.0f, 320.0f, score_text, GLUT_BITMAP_9_BY_15);
        draw_text(85.0f, 280.0f, "R: reiniciar  |  1: Fácil  2: Normal  3: Difícil", GLUT_BITMAP_9_BY_15);
        draw_text(245.0f, 250.0f, "ESC: sair", GLUT_BITMAP_9_BY_15);
    } else if (paused) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.78f);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(WINDOW_SIZE, 0.0f);
        glVertex2f(WINDOW_SIZE, WINDOW_SIZE);
        glVertex2f(0.0f, WINDOW_SIZE);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        draw_text(250.0f, 320.0f, "PAUSADO", GLUT_BITMAP_HELVETICA_18);
        draw_text(195.0f, 280.0f, "Pressione P para continuar", GLUT_BITMAP_9_BY_15);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}


void update(int value) {
    (void)value;
    if (!game_over && !paused) {


        // renderização mais rápida que o movimento na grade


        elapsed_ms += FRAME_MS;
        while (!game_over && elapsed_ms >= tick_ms()) {
            elapsed_ms -= tick_ms();
            Point next = { snake[0].x + dir_x, snake[0].y + dir_y };

            if (next.x < 0 || next.x >= GRID_SIZE || next.y < 0 || next.y >= GRID_SIZE) {
                game_over = true;
            }

            bool ate = (next.x == food.x && next.y == food.y);


            // celula da cauda é liberada se não comer comida


            int body_limit = snake_len - (ate ? 0 : 1);
            for (int i = 0; i < body_limit; i++) {
                if (snake[i].x == next.x && snake[i].y == next.y) {
                    game_over = true;
                    break;
                }
            }

            // verifica se a cobra bateu na parede ou nop proprio corpo
            // ---->*|
            if (!game_over) {
                if (ate && snake_len < MAX_SNAKE) {
                    snake_len++;
                }

                for (int i = snake_len - 1; i > 0; i--) {
                    snake[i] = snake[i - 1];
                }
                snake[0] = next;

                if (ate) {
                    score++;
                    if (score > best_score) best_score = score;
                    if (!spawn_food()) {
                        game_over = true;
                        game_won = true;
                    }
                }
                can_turn = true;
            }
        }
        if (game_over) {
            print_scoreboard(game_won ? "Vitória" : "Fim de jogo");
        }
    }

    glutPostRedisplay();
    glutTimerFunc(FRAME_MS, update, 0);
}

// teclado
void keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    if (key == 'r' || key == 'R') {
        reset_game();
        glutPostRedisplay();
        return;
    }
    if (key == 27) {
        print_scoreboard("Saiu");
        exit(0);
    }
    if (!game_over && (key == 'p' || key == 'P')) {
        paused = !paused;
        glutPostRedisplay();
        return;
    }
    if (game_over && key >= '1' && key <= '3') {
        mode = key - '1';
        reset_game();
        glutPostRedisplay();
        return;
    }
    if (game_over || paused || !can_turn) {
        return;
    }

    // lei newtoniana: a cobra não pode inverter a direção instantaneamente
    if (key == 'w' && dir_y == 0) {
        dir_x = 0;
        dir_y = 1;
        can_turn = false;
    } else if (key == 's' && dir_y == 0) {
        dir_x = 0;
        dir_y = -1;
        can_turn = false;
    } else if (key == 'a' && dir_x == 0) {
        dir_x = -1;
        dir_y = 0;
        can_turn = false;
    } else if (key == 'd' && dir_x == 0) {
        dir_x = 1;
        dir_y = 0;
        can_turn = false;
    }
}


void close_window(void) {
    print_scoreboard("Saiu");
}

// main loop e inicialização do GLUT
void init_gl(void) {
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, GRID_SIZE, 0.0, GRID_SIZE);
}


int main(int argc, char **argv) {
    if (!select_difficulty()) return EXIT_FAILURE;

    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("Jogo da Cobrinha");

    init_gl();
    reset_game();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutCloseFunc(close_window);
    glutTimerFunc(FRAME_MS, update, 0);

    glutMainLoop();
    return 0;
}
