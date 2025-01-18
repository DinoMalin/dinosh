it parses

it processes

it executes

and it handles signals, exit codes, etc, i'll make a doc if i'm bored

## Notes
Some things could be surprising. For example, the following test:

```bash
echo $dinosaur
```

Would give the following result:

```
dinosaur
```

But why ? Shouldn't the unexisting variables be expanded as an empty string ? Well yes it should, and it does. The fact is that POSIX only asks to handle variables names containing *uppercase letters, digits, and the \<underscore\> ( '_' )*

So in our case, $test will not be considered as a variable. $ will.
