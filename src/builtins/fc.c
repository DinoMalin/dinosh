#include "builtins.h"

#define FC_FLAGS "elnrs"

typedef enum {
	e,
	l,
	n,
	r,
	s
} FcFlagsPos;


// intent is to return true if only flag letter were met, false if anything else was met
// it puts the NOT found flags as negative
bool retrieve_flags(char *arg, char *flag_set) {
	if (arg[0] != '-')
		return false ;

	for (int i = 1; arg[i]; i++) {
		char *jsp = ft_strchr(flag_set, arg[i]);
		if (jsp)
			jsp[0] = -(jsp[0]);
		else if (!ft_strchr(flag_set, -arg[i]))
			return false ;
	}

	for (int i = 0; flag_set[i]; i++)
		flag_set[i] = -flag_set[i];
	
	return true ;
}

// pour l'instant ca fc pas trop c'est les fondations du chantier
void fc(Command *cmd, Context *ctx) {
	(void)ctx;

	if (cmd->ac < 2)
		BUILTIN_ERROR("fc (test): not enough args");

	char *flags = ft_strdup(FC_FLAGS);
	printf("test: %s %s \n", cmd->av[1], retrieve_flags(cmd->av[1], flags) ? "is a flag string" : "is not a flag string");
	free(flags);

	flags = ft_strdup(FC_FLAGS);
	if (retrieve_flags(cmd->av[1], flags)) {
		printf("e %s \n", flags[e] > 0 ? "was here" : "was not here");
		printf("l %s \n", flags[l] > 0 ? "was here" : "was not here");
		printf("n %s \n", flags[n] > 0 ? "was here" : "was not here");
		printf("r %s \n", flags[r] > 0 ? "was here" : "was not here");
		printf("s %s \n", flags[s] > 0 ? "was here" : "was not here");
	}
	free(flags);

}
