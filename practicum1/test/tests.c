
#include "message.h" //for all code
#include <assert.h>
#include <stdio.h>
#include <string.h>

extern cache_t cache;

cache_t* init_tmp_cache(enum ReplacementPolicy replacementPolicy) {
  /**
   * Initializes a temporary (not global) cache for testing purposes.
   *
   * args:
   * replacementPolicy : The replacement policy for the cache.
   */
  cache_t* tmpCache = malloc(sizeof(cache_t));
  tmpCache->replacementPolicy = replacementPolicy;

  for (int i = 0; i < CACHE_SIZE; i++) {
    tmpCache->messages[i] = NULL; // initialize all messages to NULL
  }
  return tmpCache;
}

int message_equals(message_t* msgA, message_t* msgB) {
  /**
   * Checks if two messages are equal.
   *
   * args:
   * msgA : The first message.
   * msgB : The second message.
   *
   */
  return msgA->id == msgB->id && msgA->timeSent == msgB->timeSent &&
         strcmp(msgA->sender, msgB->sender) == 0 &&
         strcmp(msgA->receiver, msgB->receiver) == 0 &&
         strcmp(msgA->content, msgB->content) == 0 &&
         msgA->delivered == msgB->delivered;
}

void run_cache_test(enum ReplacementPolicy policy) {
  /**
   * Run tests relating to cache insertion.
   *
   * args:
   * policy : The replacement policy for the cache.
   */
  printf("Running tests for cache . . .\n");
  cache_t* testCache = init_tmp_cache(policy);

  message_t* msgA = create_msg(0, 0, "Adithya", "Bob", "Hey Bob!", 0);
  message_t* msgB = create_msg(1, 10, "Bob", "Hey Adithya", "Hey Adithya!", 0);
  message_t* msgC = create_msg(2, 20, "Bob", "Alex", "Hey Alex!", 1);

  insert_message_into_cache(testCache, msgA);
  insert_message_into_cache(testCache, msgB);
  insert_message_into_cache(testCache, msgC);

  assert(message_in_cache(testCache, 0));
  assert(message_in_cache(testCache, 1));
  assert(message_in_cache(testCache, 2));

  assert(message_equals(msgA, get_message_from_cache(testCache, 0)));
  assert(message_equals(msgB, get_message_from_cache(testCache, 1)));
  assert(message_equals(msgC, get_message_from_cache(testCache, 2)));
  printf("Cache Tests passed ✓\n");
}

void run_message_tests() {
  /**
   * Run tests relating to creating and retrieving messages.
   */

  printf("Running tests for message.h . . .\n");
  clear_message_store();

  message_t* msgA = create_msg(0, 0, "Adithya", "Bob", "Hey Bob!", 0);
  message_t* msgB = create_msg(1, 10, "Bob", "Hey Adithya", "Hey Adithya!", 0);
  message_t* msgC = create_msg(2, 20, "Bob", "Alex", "Hey Alex!", 1);

  store_msg(msgA);
  store_msg(msgB);

  message_t* recieved_msgB = retrieve_msg(1);
  message_t* recieved_msgA = retrieve_msg(0);

  assert(message_equals(msgA, recieved_msgA));
  assert(message_equals(msgB, recieved_msgB));

  store_msg(msgC);

  message_t* recieved_msgC = retrieve_msg(2);
  assert(message_equals(msgC, recieved_msgC));

  // free to clear up space
  free(recieved_msgA);
  recieved_msgA = NULL;
  free(recieved_msgB);
  recieved_msgB = NULL;
  free(recieved_msgC);
  recieved_msgC = NULL;

  free(msgA);
  msgA = NULL;
  free(msgB);
  msgB = NULL;
  free(msgC);
  msgC = NULL;

  // refresh messages
  msgA = create_msg(0, 0, "Adithya", "Bob", "Hey Bob!", 0);
  msgB = create_msg(1, 10, "Bob", "Hey Adithya", "Hey Adithya!", 0);
  msgC = create_msg(2, 20, "Bob", "Alex", "Hey Alex!", 1);

  clear_message_store();

  // test that restorign doesn't affect the original message
  store_msg(msgA);
  store_msg(msgB);
  store_msg(msgC);
  recieved_msgA = retrieve_msg(0);
  recieved_msgB = retrieve_msg(1);
  recieved_msgC = retrieve_msg(2);

  assert(message_equals(msgA, recieved_msgA));
  assert(message_equals(msgB, recieved_msgB));
  assert(message_equals(msgC, recieved_msgC));

  free(recieved_msgA);
  free(recieved_msgB);
  free(recieved_msgC);

  free(msgA);
  free(msgB);
  free(msgC);

  printf("Tests passed for message.h ✓\n");
}

void runAllTests(enum ReplacementPolicy policy) {
  /**
   * Function to run all the tests for a given policy
   * args:
   * policy : The replacement policy for the cache.
   */
  printf("Running all tests . . .\n");
  run_cache_test(policy);
  clear_message_store();
  init_cache(policy);
  run_message_tests();
  printf("All tests passed ✓\n");
}

char* genRandString(int numChars) {
  /**
   * Generaters a random n letter string.
   * args:
   * numChars : The number of characters in the string.
   */

  char* str = malloc(numChars + 1);
  for (int i = 0; i < numChars; i++) {
    str[i] = rand() % 26 + 'a';
  }
  str[numChars - 1] = '\0';

  return str;
}

message_t** createRandomMessages(int numMessages) {
  /**
   * Creates a list of random messages.
   * args:
   * numMessages : The number of messages to create.
   */
  message_t** messages = malloc(sizeof(message_t*) * numMessages);
  for (int i = 0; i < numMessages; i++) {
    messages[i] = create_msg(i, i, genRandString(3), genRandString(3),
                             genRandString(100), 0);
  }
  return messages;
}

void runSpecificLRUTests() {
  /**
   * Run tests to assure that LRU Cache works as intended.
   */
  init_cache(LRU);

  message_t** messages = createRandomMessages(
      CACHE_SIZE +
      4); // creates list of messages in ascending order of ID (0-19)

  for (int i = 0; i < CACHE_SIZE; i++) {
    store_msg(messages[i]);
  }

  for (int i = 0; i < CACHE_SIZE; i++) {
    assert(message_in_global_cache(i));
  }

  int j = 0;
  for (int i = CACHE_SIZE; i < CACHE_SIZE + 4; i++) {
    store_msg(messages[i]);
    assert(!message_in_global_cache(j));
    j++; // the items inserted first should
    assert(message_in_global_cache(i));
  }

  message_t* random_msg =
      create_msg(111111, 20, "Bob", "Alice", "Hey Alice!", 0);

  store_msg(random_msg);
  assert(!message_in_global_cache(j++));
  assert(message_in_global_cache(111111));

  message_t* random_msg2 =
      create_msg(222222, 21, "Bob", "Alice", "Hey Alice!", 0);
  store_msg(random_msg2);
  assert(!message_in_global_cache(j++));
  assert(message_in_global_cache(222222));

  message_t* random_msg3 =
      create_msg(333333, 22, "Bob", "Alice", "Hey Alice!", 0);
  store_msg(random_msg3);
  assert(!message_in_global_cache(j++));
  assert(message_in_global_cache(333333));

  for (int i = 0; i < 100; i++) {
    store_msg(random_msg); // check that restoring doesn't wipe other entries
  }

  while (j < CACHE_SIZE + 4) { // test that despite storing many more times, the
                               // rest of the items are still in cache
    assert(message_in_global_cache(j));
    j++;
  }
  assert(message_in_global_cache(333333));
  assert(message_in_global_cache(222222));
  assert(message_in_global_cache(111111));

  free(messages);
}

void measureCacheStats(enum ReplacementPolicy policy) {
  /**
   * Measures the cache stats for a given policy
   * args:
   * policy : The replacement policy for the cache.
   */
  init_cache(policy);
  message_t** messages = createRandomMessages(1000);
  for (int i = 0; i < 1000; i++) {
    store_msg(messages[i]);
  }
  assert(cache.numMisses == 0);
  assert(cache.numHits == 0);

  // perform 1000 random retrievals
  for (int i = 0; i < 1000; i++) {
    retrieve_msg(rand() % 1000);
  }

  printf("Cache Size: %d\n", cache.capacity);
  printf("Cache Replacement Policy: %s\n", policy == LRU ? "LRU" : "RANDOM");
  printf("Number of hits per 1000 accesses: %d\n", cache.numHits);
  printf("Number of misses per 1000 accesses: %d\n", cache.numMisses);
  printf("Hit rate: %f\n", (float)cache.numHits / (cache.numHits + cache.numMisses));

  free(messages);
}

int main() {
  srand(RANDOM_SEED);
  printf("Running all tests for RANDOM . . .\n");
  runAllTests(RANDOM);
  printf("Tests for Random Policy passed ✓\n");

  printf("Running all tests for LRU . . .\n");
  runAllTests(LRU);
  runSpecificLRUTests();
  printf("Tests for LRU Policy passed ✓\n");

  printf("Measuring cache stats for LRU . . .\n");
  measureCacheStats(LRU);
  printf("Measuring cache stats for RANDOM . . .\n");
  measureCacheStats(RANDOM);

  return 0;
}