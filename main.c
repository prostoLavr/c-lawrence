#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

char getch_immediate() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}


char DEFAULT_INTERPRETER_NAME[] = "main";
char DEFAULT_FOLDER_NAME[] = ".dynamic";


int prepare(char *name) {
    if (mkdir(DEFAULT_FOLDER_NAME, 0777) == -1) {
        perror("mkdir");
    }
    
    char dir_name[
      strlen(DEFAULT_FOLDER_NAME) 
      + 1
      + (
        name
          ? strlen(name)
          : strlen(DEFAULT_INTERPRETER_NAME)
      )
    ];
    strcpy(dir_name, DEFAULT_FOLDER_NAME);
    strcat(dir_name, "/");
    strcat(dir_name, name ? name : DEFAULT_INTERPRETER_NAME);
    
    if (mkdir(dir_name, 0777) == -1) {
        perror("mkdir");
        return 1;
    }
    return 0;
}

char *print_help() {
  printf(
    "help\n"
    "This is help.\n"
    "There is nothing yet :(\n"
  );
}
#define BACKSPACE 127

char *read_input_line() {
  int allocated = 2;
  char *input = malloc(allocated);
  input[0] = '\0';
  printf(">>> ");
  while (true) {
    int input_len = strlen(input);
    char input_char = getch_immediate();
    if (input_len == 0 && input_char == '\n') {
      puts("");
      return NULL;
    }
    if (input_len == 0 && input_char == '\t') {
      print_help();
      return NULL;
    }
    if (input_char == '\n' && input[input_len - 1] == '\n') {
      puts("\n");
      return input;
    }
    if (input_len >= allocated - 1) {
      allocated *= 2;
      input = realloc(input, input_len);
    }
    if (input_len > 0 && input_char == BACKSPACE) {
      char *last_next_line_ptr = strrchr(input, '\n');
      if (last_next_line_ptr == NULL) {
        last_next_line_ptr = input - 1;
      }
      int last_next_line_pos = last_next_line_ptr - input;
      int last_line_input = input_len - last_next_line_pos - 1;
      if (last_line_input > 0) {
        input_len--;
        input[input_len] = '\0';
        printf("\b \b");
        fflush(stdout);
      }
    } else if (input_char != BACKSPACE) {
      putchar(input_char);
      strncat(input, &input_char, 1);
    }
    if (input_char == '\n') {
      printf("... ");
    }
  }
}

char *print_hello(char *name) {
  if (name && name[0] != '\0') {
      printf("Lawrence's C interpreter v0.0.1 (Named: `%s`)\n", name);
  } else {
      printf("Lawrence's C interpreter v0.0.1\n");
  }
}


void interpreter(char *name) {
  print_hello(name);  
  while (true) {
    char *input = read_input_line();
    if (input != NULL) {
      printf("Total input bellow:\n%s", input);
      // Total input here
      free(input);
    }
  }
}

int main(char *name) {
    interpreter(NULL);
}
