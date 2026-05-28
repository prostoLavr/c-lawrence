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


char *prepare(char *name) {
    if (mkdir(DEFAULT_FOLDER_NAME, 0777) == -1) {
      perror("mkdir");
    }
    if (name == NULL) {
      name = DEFAULT_INTERPRETER_NAME;
    }

    int dirname_len = 
      strlen(DEFAULT_FOLDER_NAME) 
        + 1
        + strlen(name)
        + 1
    ;

    char *dir_name = malloc(dirname_len);
    snprintf(dir_name, dirname_len, "%s/%s", DEFAULT_FOLDER_NAME, name);
    
    if (mkdir(dir_name, 0777) == -1) {
      perror("mkdir");
    }
    return dir_name;
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
    if (input_len + 2 >= allocated) {
      allocated *= 2;
      input = realloc(input, allocated);
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

const char *CODE_FILENAME_TEMPLATE = "code_XXXXXX.c";

char *code_write(char* path, char *code) {
    printf("Try to write code to path %s\n", path);
    int filename_size = strlen(path) + 1 + strlen(CODE_FILENAME_TEMPLATE) + 2;
    char *filename = malloc(filename_size);
    filename[0] = '\0';
    snprintf(filename, filename_size, "%s/%s", path, CODE_FILENAME_TEMPLATE);
    printf("Path template %s\n", filename);
    
    int fd = mkstemps(filename, 2);
    if (fd == -1) {
        perror("Failed to create temporary file");
        return NULL;
    }
    char *extension_chr_ptr = strrchr(filename, '.');
    *extension_chr_ptr = '\0';

    printf("Temporary file created at: %s.c\n", filename);

    write(fd, code, strlen(code));

    close(fd);
    return filename;
}

void code_compile(char *filename) {
  char command[1024];
  snprintf(
    command,
    sizeof(command),
    "gcc -shared -o %s.so -fPIC %s.c",
    filename,
    filename
  );
  printf("Execute: %s\n", command);
  system(command);
}


void interpreter(char *name) {
  print_hello(name);  
  char *path = prepare(name);
  if (path == NULL) {
    printf("Error on prepare\n");
  }
  while (true) {
    char *input = read_input_line();
    if (input != NULL) {
      printf("Total input bellow:\n%s", input);
      char *filename = code_write(path, input);
      if (filename == NULL) {
        printf("Write code error :(");
      } else {
        code_compile(filename);
      }
      // Total input here
      free(input);
    }
  }
}

int main(char *name) {
    interpreter(NULL);
}
