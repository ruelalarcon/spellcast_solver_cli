#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

typedef struct TrieNode {
	struct TrieNode *children[26];
	bool isWord;
} TrieNode;

/**
 * Creates a new Trie node.
 *
 * @return A pointer to the newly created TrieNode.
 */
TrieNode* createNode();

/**
 * Inserts a word into the Trie.
 *
 * @param root The root node of the Trie.
 * @param word The word to be inserted.
 * @param maxWordLength The maximum allowed word length.
 */
void insertWord(TrieNode *root, const char *word, int maxWordLength);

/**
 * Loads the dictionary from a file into a Trie.
 *
 * @param filePath The path to the dictionary file.
 * @param maxWordLength The maximum allowed word length.
 * @return The root node of the Trie containing the dictionary words.
 */
TrieNode* loadDictionary(const char *filePath, int maxWordLength);

/**
 * Frees the memory allocated for the Trie.
 *
 * @param node The root node of the Trie to be freed.
 */
void freeTrie(TrieNode *node);

#endif // TRIE_H