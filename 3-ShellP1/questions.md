1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  `fgets()` is a good choice because it processes input line by line which is a perfect fit for a shell because each command is provided as a line.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: I don't think that we needed to use `malloc` in this case. `malloc` should only be used for dynamically sized things or if you want to share a single object across multiple threads.
    In this case, we know that the maximum input size is `SH_CMD_MAX` constant so the array is fixed size and we don't need to share it between multiple threads.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**: It is necessary to trim spaces to ensure that commands don't have spaces included in their executable names.
    If one of our command executable names had spaces in it, then the OS wouldn't be able to execute it.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  First example is append output redirection which will allow our shell to append the output of a command to a file.
    Second example is write output redirection which replaces the old file contents with the new output.
    Last example is redirecting a file to stdin as input to the program.
    Some challenges that we might have are opening files with correct permissions, properly closing files, and handling race conditions when working with files.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**: redirection works with files by either redirecting ouput to a file or redirecting file content to input.
    Pipes redirect the output of one command to the input of another command.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: If we pipe output of one command to the input of another command, then the receiving command expects to receive only valid output of the previous command.
    By splitting our output into STDOUT and STDERR we can ensure that commands on the receiving end of the pipe only receive information from STDOUT without any errors.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  One of the ways that other shells do it is by allowing the user to merge outputs of STDOUT and STDERR using the following syntax "command 2>&1"
    which allows the user to redirect STDERR to STDOUT.
