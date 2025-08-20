#include "builtins.h"

// Forward declaration to avoid header inclusion conflicts
bool init_command(Context *ctx, Command *cmd);

#define FC_FLAGS "elnrs"

typedef enum {
	e,
	l,
	n,
	r,
	s
} FcFlagsPos;

typedef struct {
	bool edit;
	bool list;
	bool no_numbers;
	bool reverse;
	bool substitute;
	char *editor;
	int start;
	int end;
	char *old_string;
	char *new_string;
} FcOptions;

// Parse flags and return true if all flags are valid
bool parse_fc_flags(char *arg, FcOptions *opts) {
	if (arg[0] != '-')
		return false;

	for (int i = 1; arg[i]; i++) {
		switch (arg[i]) {
			case 'e':
				opts->edit = true;
				break;
			case 'l':
				opts->list = true;
				break;
			case 'n':
				opts->no_numbers = true;
				break;
			case 'r':
				opts->reverse = true;
				break;
			case 's':
				opts->substitute = true;
				break;
			default:
				return false;
		}
	}
	return true;
}

// Parse range arguments (first, last)
void parse_fc_range(Command *cmd, FcOptions *opts, int *arg_index) {
	// Default for listing: last 16 commands
	// Default for editing, re-executing, or substituting: just the last command
	if ((opts->edit && !opts->list) || (!opts->edit && !opts->list && !opts->substitute) || opts->substitute) {
		opts->start = history_base + history_length - 1; // Just last command for editing, re-executing, or substituting
		opts->end = history_base + history_length - 1;
	} else {
		opts->start = history_base + history_length - 16; // Last 16 commands for listing
		opts->end = history_base + history_length - 1;
	}

	// Parse first argument (start of range)
	if (*arg_index < cmd->ac && cmd->av[*arg_index]) {
		char *arg = cmd->av[*arg_index];
		if (ft_isdigit(arg[0]) || (arg[0] == '-' && ft_isdigit(arg[1]))) {
			int num = ft_atoi(arg);
			if (num < 0) {
				// Negative number means "last N commands"
				// So -2 means: start from (length-2), end at (length-1)
				opts->start = history_base + history_length + num;
				opts->end = history_base + history_length - 1;
			} else {
				opts->start = history_base + num - 1;
			}
			(*arg_index)++;
		} else if (ft_isalpha(arg[0])) {
			// Find command starting with this string
			for (int i = history_base + history_length - 1; i >= history_base; i--) {
				HIST_ENTRY *entry = history_get(i);
				if (entry && !ft_strncmp(entry->line, arg, ft_strlen(arg))) {
					opts->start = i;
					break;
				}
			}
			(*arg_index)++;
		}
	}

	// Parse second argument (end of range) - only if we didn't already set end above
	if (*arg_index < cmd->ac && cmd->av[*arg_index] && opts->end == history_base + history_length - 1) {
		char *arg = cmd->av[*arg_index];
		if (ft_isdigit(arg[0]) || (arg[0] == '-' && ft_isdigit(arg[1]))) {
			int num = ft_atoi(arg);
			if (num < 0) {
				opts->end = history_base + history_length + num;
			} else {
				opts->end = history_base + num - 1;
			}
			(*arg_index)++;
		} else if (ft_isalpha(arg[0])) {
			// Find command starting with this string for the end range
			for (int i = history_base + history_length - 1; i >= history_base; i--) {
				HIST_ENTRY *entry = history_get(i);
				if (entry && !ft_strncmp(entry->line, arg, ft_strlen(arg))) {
					opts->end = i;
					break;
				}
			}
			(*arg_index)++;
		}
	}

	// Ensure valid range
	if (opts->start < history_base)
		opts->start = history_base;
	if (opts->end >= history_base + history_length)
		opts->end = history_base + history_length - 1;
	if (opts->start > opts->end) {
		int temp = opts->start;
		opts->start = opts->end;
		opts->end = temp;
	}
}

// Execute command from fc without triggering fd_storage conflicts
void fc_execute_command(Context *ctx, char *command_line) {
	if (!command_line || !*command_line) {
		return;
	}
	
	Parser *data = tokenize(command_line);
	if (!data)
		return;

	if (token_error(data)) {
		ctx->code = 2;
		free_list(data);
		return;
	}

	Command *cmd = parse(data, ctx->alias);

	if (parse_error(cmd)) {
		ctx->code = 2;
		update_code_var(ctx);
		free_cmds(cmd);
		return;
	}

	// Execute the command without going through the standard execution path
	// that uses fd_storage for builtins
	if (cmd && cmd->args && cmd->args->content) {
		Type builtin_type = get_builtin(cmd->args->content);
		if (builtin_type != BASIC) {
			// For builtins, execute them directly but properly initialize first
			cmd->type = builtin_type;
			if (init_command(ctx, cmd)) {
				builtin(cmd, ctx);
				ctx->code = cmd->exit_code;
			}
		} else {
			// For non-builtins, use simplified execution that avoids fd_storage
			if (init_command(ctx, cmd)) {
				pid_t pid = fork();
				if (pid == 0) {
					// Child process
					char *path = find_path(ctx, cmd->av[0]);
					if (!path) {
						ERROR("%s: command not found", cmd->av[0]);
						exit(127);
					} else {
						char **envp = get_envp(ctx->env);
						execve(path, cmd->av, envp);
						free_av(envp);
						free(path);
						exit(126);
					}
				} else if (pid > 0) {
					// Parent process
					int status;
					waitpid(pid, &status, 0);
					if (WIFEXITED(status)) {
						ctx->code = WEXITSTATUS(status);
					}
				} else {
					perror("fork");
					ctx->code = 1;
				}
			}
		}
	}
	
	// Add the executed command to history
	add_history(command_line);
	append_history(1, ctx->history_path);
	
	free_cmds(cmd);
	free_garbage(ctx);
}

// List history entries
void fc_list(FcOptions *opts) {
	int start = opts->start;
	int end = opts->end;

	if (opts->reverse) {
		for (int i = end; i >= start; i--) {
			HIST_ENTRY *entry = history_get(i);
			if (entry) {
				if (opts->no_numbers) {
					printf("%s\n", entry->line);
				} else {
					printf("%d\t%s\n", i - history_base + 1, entry->line);
				}
			}
		}
	} else {
		for (int i = start; i <= end; i++) {
			HIST_ENTRY *entry = history_get(i);
			if (entry) {
				if (opts->no_numbers) {
					printf("%s\n", entry->line);
				} else {
					printf("%d\t%s\n", i - history_base + 1, entry->line);
				}
			}
		}
	}
}

// Execute command from history
void fc_execute(Context *ctx, int index) {
	HIST_ENTRY *entry = history_get(index);
	if (!entry) {
		ERROR("fc: %d: event not found", index - history_base + 1);
		return;
	}

	printf("%s\n", entry->line);
	
	// Execute the command without adding it to history
	fc_execute_command(ctx, entry->line);
}

// Edit command in editor
void fc_edit(Context *ctx, FcOptions *opts) {
	char *temp_file = get_random_file_name();
	if (!temp_file) {
		ERROR("fc: cannot create temporary file");
		return;
	}

	// Write history to temp file
	int fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) {
		ERROR("fc: cannot create temporary file");
		free(temp_file);
		return;
	}

	for (int i = opts->start; i <= opts->end; i++) {
		HIST_ENTRY *entry = history_get(i);
		if (entry) {
			write(fd, entry->line, ft_strlen(entry->line));
			write(fd, "\n", 1);
		}
	}
	close(fd);

	// Determine editor
	char *editor = opts->editor;
	if (!editor) {
		editor = ft_getenv(ctx->env, "FCEDIT");
		if (!editor) {
			editor = ft_getenv(ctx->env, "EDITOR");
			if (!editor) {
				editor = "/bin/vi"; // Default editor
			}
		}
	}

	// Fork and exec editor
	pid_t pid = fork();
	if (pid < 0) {
		ERROR("fc: fork failed");
		unlink(temp_file);
		free(temp_file);
		return;
	} else if (pid == 0) {
		// Child process: exec editor
		char *args[] = {editor, temp_file, NULL};
		execvp(editor, args);
		ERROR("fc: cannot execute %s", editor);
		exit(1);
	} else {
		// Parent process: wait for editor to finish
		int status;
		waitpid(pid, &status, 0);

		// Read edited content and execute
		fd = open(temp_file, O_RDONLY);
		if (fd >= 0) {
			char *line;
			while ((line = get_next_line(fd))) {
				// Remove trailing newline
				int len = ft_strlen(line);
				if (len > 0 && line[len - 1] == '\n') {
					line[len - 1] = '\0';
				}
				if (ft_strlen(line) > 0) {
					printf("%s\n", line);
					
					// Execute the command without adding it to history
					fc_execute_command(ctx, line);
				}
				free(line);
			}
			close(fd);
		}
	}

	unlink(temp_file);
	free(temp_file);
}

// Substitute in last command
void fc_substitute(Context *ctx, FcOptions *opts) {
	// Use the command specified by opts->start (which defaults to last command)
	int index = opts->start;

	HIST_ENTRY *entry = history_get(index);
	if (!entry) {
		ERROR("fc: %d: event not found", index - history_base + 1);
		return;
	}

	char *new_command;
	
	// If no old_string/new_string provided, just re-execute the command as-is
	if (!opts->old_string || !opts->new_string) {
		new_command = ft_strdup(entry->line);
		if (!new_command) {
			ERROR("fc: malloc failed");
			return;
		}
	} else {
		// Perform substitution
		char *old_pos = ft_strnstr(entry->line, opts->old_string, ft_strlen(entry->line));
		if (!old_pos) {
			// Pattern not found, just re-execute the command as-is (bash behavior)
			new_command = ft_strdup(entry->line);
			if (!new_command) {
				ERROR("fc: malloc failed");
				return;
			}
		} else {
			// Create new command with substitution
			int old_len = ft_strlen(opts->old_string);
			int new_len = ft_strlen(opts->new_string);
			int line_len = ft_strlen(entry->line);
			
			new_command = malloc(line_len - old_len + new_len + 1);
			if (!new_command) {
				ERROR("fc: malloc failed");
				return;
			}

			// Copy parts before, replacement, and after
			int prefix_len = old_pos - entry->line;
			ft_strlcpy(new_command, entry->line, prefix_len + 1);
			ft_strlcat(new_command, opts->new_string, line_len - old_len + new_len + 1);
			ft_strlcat(new_command, old_pos + old_len, line_len - old_len + new_len + 1);
		}
	}

	printf("%s\n", new_command);
	
	// Execute the substituted command without adding it to history
	fc_execute_command(ctx, new_command);
	free(new_command);
}

void fc(Command *cmd, Context *ctx) {
	if (history_length == 0) {
		ERROR("fc: no command history");
		return;
	}

	// Remove the fc command from history at the start
	// We'll add it back if it's just a listing operation
	HIST_ENTRY *fc_entry = remove_history(history_length - 1);
	
	FcOptions opts = {0};
	int arg_index = 1;

	// Parse flags
	while (arg_index < cmd->ac && cmd->av[arg_index] && cmd->av[arg_index][0] == '-') {
		char *arg = cmd->av[arg_index];
		
		// Check if this is a negative number (not a flag)
		if (arg[1] == '\0' || ft_isdigit(arg[1])) {
			// This is a negative number, not a flag - break and handle as range
			break;
		}
		
		if (!ft_strcmp(arg, "-e")) {
			opts.edit = true;
			arg_index++;
			if (arg_index < cmd->ac) {
				opts.editor = cmd->av[arg_index++];
			}
		} else if (!ft_strcmp(arg, "-s")) {
			opts.substitute = true;
			arg_index++;
			// Check if next argument looks like old=new format
			if (arg_index < cmd->ac) {
				char *subst_arg = cmd->av[arg_index];
				char *eq_pos = ft_strchr(subst_arg, '=');
				if (eq_pos) {
					// This is old=new format, consume it
					arg_index++;
					*eq_pos = '\0';
					opts.old_string = subst_arg;
					opts.new_string = eq_pos + 1;
				}
				// If no '=' found, leave the argument for later parsing as command pattern
			}
		} else if (parse_fc_flags(arg, &opts)) {
			arg_index++;
		} else {
			ERROR("fc: %s: invalid option", arg);
			return;
		}
	}

	// If substitute is set, ignore other conflicting flags
	if (opts.substitute) {
		opts.edit = false;
		opts.list = false;
	}

	// Set default behavior
	if (!opts.edit && !opts.list && !opts.substitute) {
		if (arg_index >= cmd->ac) {
			// In interactive mode: edit last command
			// In non-interactive mode: re-execute last command
			if (isatty(STDIN_FILENO)) {
				opts.edit = true;
			}
			// For non-interactive, we leave all flags false and handle in execution section
		} else {
			opts.list = true; // Default: list range
		}
	}

	// Parse range - do this AFTER setting the default behavior
	parse_fc_range(cmd, &opts, &arg_index);

	// Check if we're trying to list/edit a pattern that doesn't exist
	if ((opts.list || opts.edit || (!opts.list && !opts.edit && !opts.substitute)) && arg_index > 1) {
		// We have arguments, check if the pattern was found
		bool pattern_found = false;
		if (cmd->av[1] && ft_isalpha(cmd->av[1][0])) {
			// Check if this pattern exists in history
			for (int i = history_base + history_length - 1; i >= history_base; i--) {
				HIST_ENTRY *entry = history_get(i);
				if (entry && !ft_strncmp(entry->line, cmd->av[1], ft_strlen(cmd->av[1]))) {
					pattern_found = true;
					break;
				}
			}
			if (!pattern_found) {
				ERROR("fc: no command found");
				if (fc_entry) {
					add_history(fc_entry->line);
					free(fc_entry->line);
					free(fc_entry);
				}
				return;
			}
		}
	}

	// Execute appropriate action
	if (opts.substitute) {
		// Check if there's a command pattern after the substitution
		if (arg_index < cmd->ac) {
			char *pattern = cmd->av[arg_index];
			// Find the most recent command matching this pattern
			for (int i = history_base + history_length - 1; i >= history_base; i--) {
				HIST_ENTRY *entry = history_get(i);
				if (entry && !ft_strncmp(entry->line, pattern, ft_strlen(pattern))) {
					opts.start = i;
					break;
				}
			}
		}
		fc_substitute(ctx, &opts);
	} else if (opts.list) {
		fc_list(&opts);
		// For listing, we don't need to add the fc command back to history
		// because it was already added in the main loop before fc() was called
	} else if (opts.edit) {
		fc_edit(ctx, &opts);
	} else {
		// Default: execute single command
		fc_execute(ctx, opts.start);
	}
	
	// Clean up the fc_entry if we removed it but didn't add it back
	if (fc_entry && !opts.list) {
		// For non-listing operations, rewrite the history file to remove the fc command
		write_history(ctx->history_path);
		free(fc_entry->line);
		free(fc_entry);
	} else if (fc_entry && opts.list) {
		// For listing operations, add the fc command back to in-memory history
		// (it's already in the file from the main loop)
		add_history(fc_entry->line);
		free(fc_entry->line);
		free(fc_entry);
	}
}
