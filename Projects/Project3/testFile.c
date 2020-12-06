void testQueue()
{
    // numTopics = 3;
    // registry = (Queue *)malloc(sizeof(Queue) * numTopics);
    // printf("*** Testing queue data structure ***\n");
    // Queue *q;
    // q = malloc(sizeof(Queue));
    // initQueue(q, "Test Queue", 1);
    // registry[0] = *q;

    Topic *t1;
    t1 = malloc(sizeof(Topic));
    initTopic(t1, 1);
    strcpy(t1->photoCaption, "T1 caption");
    strcpy(t1->photoURl, "T1 URL");

    Topic *t2;
    t2 = malloc(sizeof(Topic));
    initTopic(t2, 2);
    strcpy(t2->photoCaption, "T2 caption");
    strcpy(t2->photoURl, "T2 URL");

    Topic *t3;
    t3 = malloc(sizeof(Topic));
    initTopic(t3, 3);
    strcpy(t3->photoCaption, "T3 caption");
    strcpy(t3->photoURl, "T3 URL");

    // printf("Q <%s>, topic id <%d>\n", q->name, q->topicID);
    // printf("Registry 0 <%s>\n", registry[0]->name);
    // printf("Testing: %d\n", registry[0]->topicID);

    enqueue(1, t1);
    enqueue(1, t2);
    enqueue(1, t3);
    enqueue(1, t1);
    for (int i = 0; i < MAXTICKETS; i++)
    {
        printf("From main testing enqueue: publisher <%d> caption %s\n",
               registry[0].buffer[i].pubID, registry[0].buffer[i].photoCaption);
        // printf("Test entryNum: %d\n", registry[0].buffer[i].entryNum);
    }

    // Topic *t4;
    // t4 = malloc(sizeof(Topic));

    // dequeue(1, t4);
    for (int i = 0; i < MAXTICKETS + 1; i++)
    {
        Topic *t4;
        t4 = malloc(sizeof(Topic));
        if (dequeue(1, t4))
            printf("From main testing dequeue: publisher <%d> caption %s\n",
                   t4->entryNum, t4->photoCaption);
    }
    // enqueue(1, t1);
}

void testEntry()
{
    // printf("*** Testing entry function ***\n");
    // Queue *q;
    // q = malloc(sizeof(Queue));
    // initQueue(q, "Test Queue", 1);
    // registry[0] = *q;

    Topic t1;
    // t1 = malloc(sizeof(Topic));
    initTopic(&t1, 1);
    strcpy(t1->photoCaption, "T1 caption");
    strcpy(t1->photoURl, "T1 URL");

    Topic *t2;
    t2 = malloc(sizeof(Topic));
    initTopic(&t2, 2);
    strcpy(t2->photoCaption, "T2 caption");
    strcpy(t2->photoURl, "T2 URL");

    Topic *t3;
    t3 = malloc(sizeof(Topic));
    initTopic(&t3, 3);
    strcpy(t3->photoCaption, "T3 caption");
    strcpy(t3->photoURl, "T3 URL");

    // printf("Q <%s>, topic id <%d>\n", q->name, q->topicID);
    // printf("Registry 0 <%s>\n", registry[0]->name);
    // printf("Testing: %d\n", registry[0]->topicID);

    // t1->entryNum = 100;
    // t2->entryNum = 200;
    // t3->entryNum = 300;
    enqueue(1, t1);
    enqueue(1, t2);
    enqueue(1, t3);
    enqueue(1, t1);
    Topic *t4;
    t4 = malloc(sizeof(Topic));
    printf("Main test: %d\n", registry[0].lastRead);

    dequeue(1, t4); // last read is 0

    Topic *t5;
    t5 = malloc(sizeof(Topic));
    printf("From main: last read %d\n", registry[0].lastRead);
    getEntry(registry[0].lastRead, 1, t5); // last entry should be 0
    printf("Testing lastEntry %s\n", t5->photoCaption);
    dequeue(1, t4);
    getEntry(registry[0].lastRead, 1, t5); // last entry should be 0
    printf("Testing lastEntry %s\n", t5->photoCaption);
    registry[0].lastRead = -2;
    printf("FML: %d\n", registry[0].lastRead);
    getEntry(registry[0].lastRead, 1, t5);
    printf("After modified last readEntry: Testing lastEntry %s\n", t5->photoCaption);
    // dequeue(1, t4); // last read is 1
    // dequeue(1, t4);  // last read is 2
    // dequeue(1, t4);  // queue empty, last read is 2
}

void testCleanup()
{
    // Queue *q;
    // q = malloc(sizeof(Queue));
    // initQueue(q, "Test Queue", 1);
    // registry[0] = *q;

    Topic *t1;
    t1 = malloc(sizeof(Topic));
    initTopic(t1, 1);
    strcpy(t1->photoCaption, "T1 caption");
    strcpy(t1->photoURl, "T1 URL");

    Topic *t2;
    t2 = malloc(sizeof(Topic));
    initTopic(t2, 2);
    strcpy(t2->photoCaption, "T2 caption");
    strcpy(t2->photoURl, "T2 URL");

    Topic *t3;
    t3 = malloc(sizeof(Topic));
    initTopic(t3, 2);
    strcpy(t3->photoCaption, "T3 caption");
    strcpy(t3->photoURl, "T3 URL");

    Topic *t5;
    t5 = malloc(sizeof(Topic));

    // Queue* q2;
    // q2 = malloc(sizeof(Queue));
    // initQueue(q2, "T2 Queue", 2);
    // registry[1] = *q2;

    enqueue(1, t1);
    enqueue(1, t2);
    enqueue(2, t3);
    printf("Test: %s\n", registry[1].buffer[0].photoCaption);
    // dequeue(1, t5);
    cleanUp();
}

void testPubSub()
{
    // Queue *q;
    // q = malloc(sizeof(Queue));
    // initQueue(q, "Test Queue", 1);
    // registry[0] = *q;

    // Queue *q2;
    // q2 = malloc(sizeof(Queue));
    // initQueue(q2, "Test Queue", 2);
    // registry[1] = *q2;

    Topic *t1;
    t1 = malloc(sizeof(Topic));
    initTopic(t1, 1);
    strcpy(t1->photoCaption, "T1 caption");
    strcpy(t1->photoURl, "T1 URL");

    Topic *t2;
    t2 = malloc(sizeof(Topic));
    initTopic(t2, 2);
    strcpy(t2->photoCaption, "T2 caption");
    strcpy(t2->photoURl, "T2 URL");

    Topic *t3;
    t3 = malloc(sizeof(Topic));
    initTopic(t3, 2);
    strcpy(t3->photoCaption, "T3 caption");
    strcpy(t2->photoURl, "T3 URL");

    Topic *t4;
    t4 = malloc(sizeof(Topic));
    initTopic(t4, 2);
    strcpy(t4->photoCaption, "T4 caption");
    strcpy(t4->photoURl, "T4 URL");

    Topic *t5;
    t5 = malloc(sizeof(Topic));

    Topic *t6;
    t6 = malloc(sizeof(Topic));

    // Topic test[3];
    // test[0] = *t1;
    // test[1] = *t2;
    // test[2] = *t3;
    // test[2] = *t4;
    // enqueue(1, t1);
    // enqueue(1, t2);
    // enqueue(1, t3);
    // enqueue(1, t4);
    // dequeue(1, t5);

    Topic pub[1][3];
    pub[0][0] = *t1;
    pub[0][1] = *t2;
    pub[0][2] = *t3;
    globalPubs[0].length = 3;

    globalPubs[0].topicID = 1;
    globalPubs[0].buffer = pub[0];
    // globalPubs[0].length = 4;
    // globalSubs[0].topicID = 1;
    globalSubs->t = *t5;
    globalSubs[0].topicID = 1;
    globalSubs[0].t = *t6;

    pthread_t clean;

    printf("Testing: %s\n", globalPubs[0].buffer[0].photoCaption);
    printf("Testing: %s\n", globalPubs[0].buffer[1].photoCaption);

    pthread_create(&pubThread[0], NULL, publisher, &globalPubs[0]);
    pthread_create(&clean, NULL, cleanUp, NULL);
    pthread_create(&subThread[0], NULL, subscriber, &globalSubs[0]);
    sleep(1);
    pthread_cond_broadcast(&globalCond);
    printf("Registry test: %s, %s, %s\n", registry[0].buffer[0].photoCaption,
           registry[0].buffer[1].photoCaption, registry[0].buffer[2].photoCaption);
    pthread_join(pubThread[0], NULL);
    pthread_join(clean, NULL);
    pthread_join(subThread[0], NULL);
    // getEntry(0, 1, t5);
    // printf("Testing entry again: %s\n", t5.photoCaption);
    // // registry[0].buffer[1].entryNum = 25;
    // dequeue(1, t5);
    // getEntry(2, 1, t5);
    // printf("Testing entry again: %s\n", t5.photoCaption);
    // sleep(2);
    // cleanUp();

    // pthread_t clean;
    // pubStruct cleaner;
    // pthread_create(&clean, NULL, cleanUp, NULL);
    // sleep(2);
}