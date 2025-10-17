# MFRC522-Driver

### AUTHORS: martin.levesque anton.vella

init
-> buffer[25] = '\0'
-> to_read = 0
mem_write hello
mem_read (to_read = 0)
-> buffer[25]
-> to_read = 25
mem_read (to_read != 0)
-> buffer[25] = '\0'
-> to_read = 0
mem_read ()