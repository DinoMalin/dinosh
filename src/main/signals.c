#include "minishell.h"

// The signal handling is bit messy because the readline functions are not
// signal safe, which mean if a signal is triggered during a readline it could
// cut a malloc and crash.
// A way to prevent this is to use the readline event hook.

int g_signal = 0;

bool skip_hook(bool modify, bool new) {
	static int res = false;
	if (modify)
		res = new;
	return res;
}

int rl_hook() {
	if (g_signal == 130 && !skip_hook(false, false)) {
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		skip_hook(true, true);
	}
	return 0;
}

void sig_handler(int sig) {
	if (sig == SIGINT) {
		g_signal = 130;
	}
}
