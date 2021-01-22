<div align="center">
<a href="https://github.com/bahaaEldeen1999/C-COM" rel="noopener" align="center">
  
  ![Component 16](https://i.ibb.co/ScTz4Yc/44cf4431-c1ed-4beb-b183-01d6c164b77f-200x200.png)


</div>

<div align="center">
  
  [![GitHub contributors](https://img.shields.io/github/contributors/bahaaEldeen1999/C-COM)](https://github.com/bahaaEldeen1999/C-COM/contributors)
  [![GitHub issues](https://img.shields.io/github/issues/bahaaEldeen1999/C-COM)](https://github.com/bahaaEldeen1999/C-COM/issues)
  [![GitHub forks](https://img.shields.io/github/forks/bahaaEldeen1999/C-COM)](https://github.com/bahaaEldeen1999/C-COM/network)
  [![GitHub stars](https://img.shields.io/github/stars/bahaaEldeen1999/C-COM)](https://github.com/bahaaEldeen1999/C-COM/stargazers)
  [![GitHub license](https://img.shields.io/github/license/bahaaEldeen1999/C-COM)](https://github.com/bahaaEldeen1999/C-COM/blob/master/LICENSE)
  <img src="https://img.shields.io/github/languages/count/bahaaEldeen1999/C-COM" />
  <img src="https://img.shields.io/github/languages/top/bahaaEldeen1999/C-COM" />
  <img src="https://img.shields.io/github/languages/code-size/bahaaEldeen1999/C-COM" />
  <img src="https://img.shields.io/github/issues-pr-raw/bahaaEldeen1999/C-COM" />

</div>

# Operating Systems common applications

## About the project

-   **The project contains three main concepts used in any operating system which are process scheduling, memory managemnt using buddy algorithm, and producer consumer problem**

-   **It is implemented in _C_ (standard 11) and uses _make_ for compilation**

-   **More explaination for the details of each module and example test cases can be found in the project documentation**

## Process scheduling problem

> **The scheduler provides three algorithms to test a single scenario with. each algorithm has different criteria to define what is the most important process to run**

1. **Highest Priority First (_HPF_)**
2. **Round Robin (_RR_)**
3. **Shortest Remaining Time Next (_SRTN_)**

## Memory management problem

> **It is an extension of the scheduler but with the memory management module added. it uses _buddy algorithm_ to allocate/deallocate memory for each process according to the situation**

## Producer consumer problem

> **This module implements the common problem which is the synchronization between multiple consuming and producing processes and all of them operating on the same resource. the module uses _semaphore arrays_ and _shared memory_**

## Testing

1.  ### For scheduler and memory manager

        cd scheduler              # or memory_manager 
        make 
        ./test_generator.out      # then enter any number of processes 
        ./process_generator.out

2.  ### For producer consumer

        cd producer_consumer 
        gcc -std=c11 producer.c -o producer.out 
        gcc -std=c11 consumer.c -o consumer.out 
        gcc -std=c11 clear_ipc.c -o clear_ipc.out 
        ./producer.out 
        ./consumer.out

## File Structure

    root directory
    ├── README.md
    ├── LICENSE
    ├── .gitignore
    ├── documentation.pdf
    └── scheduler
    │   ├── headers
    │   │   ├── file_handler.h
    │   │   ├── headers.h
    │   │   ├── highest_priority_first.h
    │   │   ├── integer_queues.h
    │   │   ├── message_buffer.h
    │   │   ├── pcb.h
    │   │   ├── round_robin.h
    │   │   ├── shortest_remaining_time_next.h
    │   │   ├── vector.h
    │   ├── clk.c
    │   ├── process_generator.c
    │   ├── process.c
    │   ├── scheduler.c
    │   ├── test_generator.c
    │   ├── key_process_table
    │   ├── keyfile
    │   ├── Makefile
    └── memory_manager
    │   ├── headers
    │   │   ├── buddy_algorithm.h
    │   │   ├── file_handler.h
    │   │   ├── headers.h
    │   │   ├── highest_priority_first.h
    │   │   ├── integer_queues.h
    │   │   ├── message_buffer.h
    │   │   ├── pcb.h
    │   │   ├── round_robin.h
    │   │   ├── shortest_remaining_time_next.h
    │   │   ├── vector.h
    │   ├── clk.c
    │   ├── process_generator.c
    │   ├── process.c
    │   ├── scheduler.c
    │   ├── test_generator.c
    │   ├── key_buddy
    │   ├── key_process_table
    │   ├── keyfile
    │   ├── Makefile
    └── producer_consumer
    │   ├── clear_ipc.c
    │   ├── consumer.c
    │   ├── producer.c
    └── ├── headers.h

## Contributors

> **This project was the semester project of _operating systems_ course in 3rd year computer engineering. it is implemented by:**

<table>
  <tr>
    <td align="center">
    <a href="https://github.com/Abdelrhmanfdl" target="_black">
    <b>Abdelrhman Fadl</b></a>
    </td>
    <td align="center">
    <a href="https://github.com/bahaaEldeen1999" target="_black">
    <b>Bahaa Eldeen</b></a>
    </td>
    <td align="center">
    <a href="https://github.com/EmanOthman21" target="_black">
    <b>Eman Othman</b></a>
    </td>
    <td align="center">
    <a href="https://github.com/tarek99samy" target="_black">
    <b>Tarek Samy</b></a>
    </td>    
  </tr>
 </table>

## Acknowledgement

> **Big thanks to all people in this team for the great efforts**
