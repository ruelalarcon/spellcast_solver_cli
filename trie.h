#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct TrieNode {
	uint32_t children;
	bool isWord;
	struct TrieNode **childPtrs;
} TrieNode;

/**
 * Creates a new Trie node with a bit vector for children.
 *
 * @return A pointer to the newly created TrieNode.
 */
TrieNode* createNode();

/**
 * Inserts a word into the Trie using bit vector for child representation.
 *
 * @param root The root node of the Trie.
 * @param word The word to be inserted.
 * @param maxWordLength The maximum allowed word length.
 */
void insertWord(TrieNode *root, const char *word, int maxWordLength);

/**
 * Loads the dictionary from a file into a Trie with bit vector optimization.
 *
 * @param filePath The path to the dictionary file.
 * @param maxWordLength The maximum allowed word length.
 * @return The root node of the Trie containing the dictionary words.
 */
TrieNode* loadDictionary(const char *filePath, int maxWordLength);

/**
 * Frees the memory allocated for the Trie with bit vector optimization.
 *
 * @param node The root node of the Trie to be freed.
 */
void freeTrie(TrieNode *node);

#endif // TRIE_H