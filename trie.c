#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trie.h"

TrieNode* createNode() {
	TrieNode *node = (TrieNode *)calloc(1, sizeof(TrieNode));
	if (!node) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}
	return node;
}

void insertWord(TrieNode *root, const char *word, int maxWordLength) {
	TrieNode *node = root;
	for (int i = 0; word[i] && i < maxWordLength; i++) {
		char c = toupper(word[i]);
		if (c < 'A' || c > 'Z')
			continue;
		int index = c - 'A';
		if (!(node->children & (1U << index))) {
			node->children |= (1U << index);
			if (!node->childPtrs) {
				node->childPtrs = calloc(26, sizeof(TrieNode*));
			}
			node->childPtrs[index] = createNode();
		}
		node = node->childPtrs[index];
	}
	node->isWord = true;
}

TrieNode* loadDictionary(const char *filePath, int maxWordLength) {
	FILE *file = fopen(filePath, "r");
	if (!file) {
		perror("Error opening dictionary file");
		exit(1);
	}

	TrieNode *root = createNode();
	char *word = NULL;
	size_t len = 0;

	while (getline(&word, &len, file) != -1) {
		size_t wordLen = strlen(word);
		if (wordLen > 0 && word[wordLen - 1] == '\n') {
			word[wordLen - 1] = '\0';
			wordLen--;
		}

		if (wordLen <= (size_t)maxWordLength) {
			insertWord(root, word, maxWordLength);
		}
	}

	free(word);
	fclose(file);
	return root;
}

void freeTrie(TrieNode *node) {
	if (node == NULL)
		return;
	if (node->childPtrs) {
		for (int i = 0; i < 26; i++) {
			if (node->children & (1U << i)) {
				freeTrie(node->childPtrs[i]);
			}
		}
		free(node->childPtrs);
	}
	free(node);
}