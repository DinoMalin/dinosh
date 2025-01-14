#include "minishell.h"

// The signal handling is bit messy because the readline functions are not
// signal safe, which mean if a signal is triggered during a readline it could
// cut a malloc and crash.
// A way to prevent this is to use the readline event hook.

int g_signal = 0;

int rl_hook() {
	if (g_signal == 130) {
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_signal = 0xDEADBEEF;
	}
	return 0;
}

void sig_handler(int sig) {
	if (sig == SIGINT) {
		g_signal = 130;
	}
}
