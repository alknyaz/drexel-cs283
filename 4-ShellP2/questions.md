1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: `execvp` replaces the memory space of the current process with the new process provided.
    If `execvp` was called without `fork` then it would replace our shell's process with the executable provided, which would break our shell.
    `fork` creates a child process by cloning the current process, and `execvp` replaces the child with the executable provided,
    so the parent shell process doesn't get replaced.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  if fork() fails then it returns an integer less than 0. In that case, my implementation returns the ERR_EXEC_CMD to indicate that
    the shell failed while trying to execute the command provided.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() uses $PATH environment variable to find in which directories the executable file might be located.
    execvp() goes through every directory in the $PATH variable and attempts to find the executable file there.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: wait() is called because we want the parent process to wait for the command in the child process to finish the execution.
    If we didn't call wait() then our shell would prompt us to enter another command without printing the output for the initial command.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() returns the exit return code of the process.
    It is important because we want our shell to track the return call of the last command which we executed in the shell.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  When my code gets to the start of the new token it check whether the first character is a double qoute.
    If it a double quote, then it looks for the next double qoute character. Everything between the 2 quotes becomes the new token.
    It is necessary to have separate logic for double quotes because we want to preserve spaces between them,
    so we need to detect the start and the end of the quoted string and mark everything between them, including spaces, as a new argument.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: I got rid of the `strtok()` function because it is too basic for parsing double quotes.
    Instead, I went through every character in the string either manually or by using `strcspn()` to find some character.
    One of the challenges that I encountered is the fact that I had to prevent my program from going outside the buffer.
    To fix, I looped through the buffer until I reached the null terminating character.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  signals are used to notify a process that some event has occurred.
    Signals are different from other IPC because they don't transfer any data aside from the signal number.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: SIGKILL is used to forcefully kill a process, so it could be used to stop a process which doesn't respond to other commands.
    SIGTERM is a signal used to terminate the process. It is used to request the process to terminate gracefully and do some cleanup before terminating.
    SIGINT is a signal used to interrupt the process execution. Usually, it is used to stop interactive shell programs.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP is a forceful way to pause a process.
    It cannot be caught or ignored because it was designed to reliably suspend the execution of a process.
    Otherwise, the kernel wouldn't be able to reliably stop user processes.
