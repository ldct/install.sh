/***************************************************************************//**
  @file         main.c
  @author       Stephen Brennan
  @date         Thursday,  8 January 2015
  @brief        LSH (Libstephen SHell)
*******************************************************************************/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(vector<string>);
int lsh_help(vector<string>);
int lsh_exit(vector<string>);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (vector<string>) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins() {
  return 3;
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(vector<string> args) {

  if (args.size() == 1) {
    cerr << "lsh: expected argument to \"cd\"" << endl;
  }
  if (chdir(args[1].c_str()) != 0) {
    perror("lsh");
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(vector<string> args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(vector<string> args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(vector<string> args) {
  pid_t pid;
  int status;

  vector<char*> _args;

  for (int i=0; i<args.size(); i++) {
    _args.push_back(const_cast<char*>(args[i].c_str()));
  }
  _args.push_back(NULL);

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(_args[0], &_args[0]) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(vector<string> args) {

  if (args.size() == 0) {
    // An empty command was entered.
    return 1;
  }

  for (int i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0].c_str(), builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
*/
string lsh_read_line(void) {
  string line;
  getline(cin, line);
  return line;
}

const int LSH_TOK_BUFSIZE = 64;
const char* LSH_TOK_DELIM = " \t\r\n\a"; // note there is a space here
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
vector<string> lsh_split_line(string __line) {

  vector<string> ret;
  string token;
  istringstream ss(__line);

  while (getline(ss, token, ' ')) {
    ret.push_back(token);
  }

  return ret;
}

/**
   @brief Loop getting input and executing it.
  
  lsh_read_line()
  |> lsh_split_line()
  |> lsh_execute()

*/

void lsh_loop(void)
{
  string line;
  vector<string> args;
  int status;

  do {
    cout << "> ";
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);
  } while (status);
}

void lsh_fromfile(string filename) {
  cout << "executing from file " << filename << endl;

  ifstream infile(filename);
  string line;
  while (getline(infile, line)) {
      cout << "executing line " << line << endl;
      args = lsh_split_line(line);
      status = lsh_execute(args);
  }

  return;
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{

  if (argc == 1) {
    lsh_loop();    
  } else {
    lsh_fromfile(string(argv[1]));
  }

  return EXIT_SUCCESS;
}