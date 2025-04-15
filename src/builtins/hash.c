#include <stdint.h>
#include <dirent.h>
#include "builtins.h"

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define DISPLAY_HASH(hash) printf("%s=%s\n", hash.key, hash.value);

// Return 64-bit FNV-1a hash for a given key. See:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t fnv1a(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void display_hash(Context *ctx) {
	for (int i = 0; i < ctx->hash_len; i++) {
		if (ctx->hash[i].key)
			DISPLAY_HASH(ctx->hash[i]);
	}
}

void realloc_hash(Context *ctx) {
	int new_len = ctx->hash_len ? ctx->hash_len * 2 : 4;
	Hash *new_hash = ft_calloc(new_len, sizeof(Hash));

	for (int i = 0; i < ctx->hash_len; i++) {
		if (!ctx->hash[i].key)
			continue;

		int new_index = fnv1a(ctx->hash[i].key) % new_len;
		for (; new_index < new_len; new_index++) {
			if (!new_hash[new_index].key)
				break;
		}
		new_hash[new_index].key = ctx->hash[i].key;
		new_hash[new_index].value = ctx->hash[i].value;
	}

	free(ctx->hash);
	ctx->hash = new_hash;
	ctx->hash_len = new_len;
}

Hash *get_hash(Context *ctx, char *key) {
	if (!ctx->hash_len)
		return NULL;

	int index = fnv1a(key) % ctx->hash_len;
	while (index < ctx->hash_len) {
		if (ctx->hash[index].key && !ft_strcmp(key, ctx->hash[index].key))
			break;
		index++;
	}
	if (index >= ctx->hash_len)
		return NULL;

	return ctx->hash+index;
}

// if this function is given no path it finds it
bool add_hash(Context *ctx, char *key, char *path) {
	Hash *hash = get_hash(ctx, key);

	if (hash) {
		free_one_hash(hash);
	} else {
		int index = ctx->hash_len ? fnv1a(key) % ctx->hash_len : 0;
		for (; index < ctx->hash_len; index++) {
			if (!ctx->hash[index].key)
				break;
		}

		if (index >= ctx->hash_len) {
			realloc_hash(ctx);
			return add_hash(ctx, key, path);
		}

		hash = ctx->hash+index;
	}

	if (!path) {
		path = get_path(ctx->env, key);
		if (!path)
			return false;
	}

	hash->key = ft_strdup(key);
	hash->value = path;
	return true;
}

void milk_path(Context *ctx) {
	char *path = ft_getenv(ctx->env, "PATH");
	char **paths = ft_split(path, ':');

	for (int i = 0; paths[i]; i++) {
		DIR *dir = opendir(paths[i]);
		struct dirent *entry = NULL;

		if (!dir) {
			perror("dinosh: path milking");
			return;
		}

		while ((entry = readdir(dir))) {
			char *value = ft_strjoin(paths[i], "/");
			value = clean_join(value, entry->d_name);
			add_hash(ctx, entry->d_name, value);
		}

		closedir(dir);
	}

	free_av(paths);
}

void hash(Command *cmd, Context *ctx) {
	(void)ctx;
	if (cmd->ac > 3)
		BUILTIN_ERROR("hash: too many args");

	if (cmd->ac == 1) {
		display_hash(ctx);
	} else if (cmd->ac == 2) {
		Hash *hash = get_hash(ctx, cmd->av[1]);
		if (hash)
			DISPLAY_HASH((*hash));
	} else if (!ft_strcmp(cmd->av[1], "add")) {
		if (!add_hash(ctx, cmd->av[2], NULL))
			BUILTIN_ERROR("hash: no such command: %s", cmd->av[2]);
	} else if (!ft_strcmp(cmd->av[1], "remove")) {
		Hash *hash = get_hash(ctx, cmd->av[2]);
		if (hash)
			free_one_hash(hash);
	}
}
