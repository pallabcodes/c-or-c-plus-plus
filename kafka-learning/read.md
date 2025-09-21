Advanced animation with geometry and trigonometry, fine-tuned animation with physics and such
Performance and hardware optimization and such

# Kafka Topics

- So, a topic in kafka is simply a stream of data and if explain from database perspective -> it is like a table (without all the constraints)

- Kafka does not query by itself so by design there're no querying capabilities withk kafka

- So, yes a topic could be distributed into multiple partitions so now assumable if made 3 partitions for a topic then as produced events arrives and mapped to topics each partition will handle a specific no. of events in the order of FIFO.

- 