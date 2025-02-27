1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation waits for all processes to end by calling waitpid() and passing child's pids.
If you don't wait for child processes, then they will become zombie processes which will take up resources and at some point prevent new processes from being created.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

A process can open a limited number of files/pipes, so leaving unused pipes open will take up file descriptors.
At some point the process is going to run out of file descriptors and the process will crash.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The command "cd" works by changing the environment variables PWD and OLDPWD.
Because each process has their own set of env variables, calling "cd" will only change the working directory in the child process, but not in the shell's process.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To handle an arbitrary number of piped commands, you will need to dynamically allocate and deallocate resources associated with piped commands.
First, you will have to dynamically allocate command_list->commands field to ensure that it can fit all commands.
Second, you will have to dynamically allocate pipes and pids arrays in the execute_pipeline() function.
