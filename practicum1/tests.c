
#include "message.h" //for all code 
#include <stdio.h>
#include <assert.h> 
#include <string.h>

int message_equals(message_t* msgA, message_t* msgB){
    return msgA->id == msgB->id && msgA->timeSent == msgB->timeSent && strcmp(msgA->sender, msgB->sender) == 0 && strcmp(msgA->receiver, msgB->receiver) == 0 && strcmp(msgA->content, msgB->content) == 0 && msgA->delivered == msgB->delivered;
}

void run_cache_test(){
    init_cache();
    printf("Running tests for cache . . .\n");
    cache_t testCache;

    message_t* msgA = create_msg(0, 0, "Adithya", "Bob", "Hey Bob!",0);
    message_t* msgB = create_msg(1, 10, "Bob", "Hey Adithya", "Hey Adithya!",0);
    message_t* msgC = create_msg(2, 20, "Bob", "Alex", "Hey Alex!",1);

    insert_message_into_cache(&testCache, msgA);
    insert_message_into_cache(&testCache, msgB);
    insert_message_into_cache(&testCache, msgC);

    assert(message_in_cache(&testCache, 0));
    assert(message_in_cache(&testCache, 1));
    assert(message_in_cache(&testCache, 2));

    assert(message_equals(msgA, get_message_from_cache(&testCache, 0)));
    assert(message_equals(msgB, get_message_from_cache(&testCache, 1)));
    assert(message_equals(msgC, get_message_from_cache(&testCache, 2)));
    printf("Cache Tests passed ✓\n");

}


void run_message_tests(){

    printf("Running tests for message.h . . .\n");
    clear_message_store();

    message_t* msgA = create_msg(0, 0, "Adithya", "Bob", "Hey Bob!",0);
    message_t* msgB = create_msg(1, 10, "Bob", "Hey Adithya", "Hey Adithya!",0);
    message_t* msgC = create_msg(2, 20, "Bob", "Alex", "Hey Alex!",1);

    store_msg(msgA);
    store_msg(msgB);

    message_t* recieved_msgB = retrieve_msg(1);
    message_t* recieved_msgA = retrieve_msg(0);

    assert(message_equals(msgA, recieved_msgA));
    assert(message_equals(msgB, recieved_msgB));

    store_msg(msgC);

    message_t* recieved_msgC = retrieve_msg(2);
    assert(message_equals(msgC, recieved_msgC));

    //free to clear up space
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
    msgA = create_msg(0, 0, "Adithya", "Bob", "Hey Bob!",0);
    msgB= create_msg(1, 10, "Bob", "Hey Adithya", "Hey Adithya!",0);
    msgC = create_msg(2, 20, "Bob", "Alex", "Hey Alex!",1);

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


int main(){
    run_cache_test();

    clear_message_store();
    run_message_tests();

    return 0;
}