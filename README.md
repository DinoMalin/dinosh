
# Better Minishell

A minishell that does everything it's supposed to do, and a bit more.
## How does it work ?

We begin by taking a string, which we will call the input.
Once we have it, we try to tokenize it.

### Tokenizing
The tokenizing, or lexing, is like saying "This is a wheel, this is a wheel, this is a handlebar", but for words.\
We will analyze words and characters, extract content between two quotes, recognize redirections and pipes, and so on.

Our function ```tokenize(input)``` returns us a linked list of `Parser`. `Parser` is our parsing type.
It contains the following data:
```C
Parser:
    content
    token
    error
    id
```
```content```: the content of the token\
```token```: the type of the token\
```error```: if the tokenizer found an error (unclosed quotes ?)\
```id```: used to later know which arguments belong to the same argv.

We now have the necessary data to do the syntactic analysis, or parsing.

### Parsing

The parsing is like saying "There is two wheels and a handlerbar, this must be a bike.", but for words.\
We will analyze our tokenized input and separate the nodes in commands.

Our function ```parse(data)``` returns us a linked list of `Command`
`Command` is our executing type, which will be used directly in the executing.
It contains the following data:
```C
Command:
    args
    av, ac
    redirs
    error
    type
    from, to
    exit_code
    pid
```
```args```: The list of `Parser` used for this specific command. We don't use `av` directly because we need to expand **during** the execution of each command.\
```av, ac```: the classic argv and argc passed to a program.\
```redirs```: the list of each redirection of the command, a structure containing the type of redirection and the file toward which it's redirected.\
```error```: if the parser found an error (empty redirection ?)\
```type```: the type of the command (builtin, subshell, basic command...)\
```from, to```: the type of operator toward which the command is redirected (pipe, and, or, none)\
```exit_code```: the exit code of the current command. Used only for non-forked commands. \
```pid```: The pid of the forked command.

We now have the necessary data to do the executing.

### Executing
The executing is like saying "If this is a bike, let's drive with more caution.", but for words. I'm lost in my analogies.

During the executing, a lot of things happens.
First, we expand :
- Env vars (words preceded by a '$') are expanded to their values, creating more args if necessary
- Globs (wildcard '*') are expanded to the content of the working directory.

At this point, if the command is a builtin and is not piped to or from anything, we just call the command. In other cases, we create the pipe, fork the command, redirect the ouput toward the pipe, redirect the output toward and from the `redirs`, and execute the command.

There must not be any wait between the command, except for the && and || operators, which needs to know the result of the previous command.

When every command has been executed, we wait each one of them and save the exit code. All done !
## Some extras

For testing purposes, I implemented the script running feature. Just type\
`./minishell <the_script_i_really_want_to_run>`

If you want to have your own config on dinosh, I also implemented the rcfile feature. Example:\
`./minishell --rcfile <my_awesome_rcfile>`

This shell have some bonus vars from the start, just colors for now so your rcfile can be really cool. Here are the 9 extra color vars you can use:
- `$BLACK`
- `$RED`
- `$GREEN`
- `$YELLOW`
- `$BLUE`
- `$MAGENTA`
- `$CYAN`
- `$WHITE`
- `$RESET`

## Running Tests

I temporarily does not have anymore unit test because I fucked up the structure of my code to allow more flexibility for future implementations.\
I'm using a fuzzer which you can try running

```bash
  make fclean && make fuzzer
```

I'm also using various testers you can find online.
