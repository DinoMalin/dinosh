#include "expand.h"
#include "pwd.h"

void tilde(Parser *el) {
	free(el->content);
	struct passwd *pw = getpwuid(getuid());
	if (!pw)
		el->content = ft_strdup("");
	else
		el->content = ft_strdup(pw->pw_dir);
}
