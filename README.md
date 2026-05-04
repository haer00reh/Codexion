# This project has been created as part of the 42 curriculum by **haer-reh**

## about Codexion
Codexion is a project about managing threads mutexes and condition variables. The goal is to create a program that simulates coders (threads) and a shared dongles. each coder have a time to compile debug and refactor, to compile they need to take two dongles, and each coder cant comminucate with each other and they should avoid burning out by taking too much time to compile. The program should end when all coders have compiled at least the number of times specified in the arguments.

the number of dongles is exactly the number of coders, and each coder need to take the dongle on their left and right to compile. the program should print the state of each coder every time they change state (compiling, debugging, refactoring).
## how to compile
To compile the program, you can use the following command in the terminal:
```make```
This will create an executable file named `codexion`.
## how to run the program, you can use the following command in the terminal:
```./codexion number_of_coders  time_to_burnout time_to_compile  time_to_debug   time_to_refactor   number_of_compiles_required   dongle_cooldown   scheduler (fifo or edf)```
For example:
```./codexion 5 800 200 100 300 3 50 fifo```
This will run the program with 5 coders, a burnout time of 800ms, a compile time of 200ms, a debug time of 100ms, a refactor time of 300ms, a requirement of 3 compiles for each coder, a dongle cooldown of 50ms, and a FIFO scheduler.
## how to cleanTo clean the project, you can use the following command in the terminal:
```make fclean```
This will remove the executable file and any object files created during compilation.
## how to recompile To recompile the project, you can use the following command in the
terminal:
```make re```
This will first clean the project and then compile it again, creating a new executable file.

## resources
**the man** (linux manual)

**helpful youtube videos i watched**:

[▶ Video 1](https://youtu.be/ldJ8WGZVXZk?si=M0Gj9gajzUvNp4jg)

[▶ Video 2](https://youtu.be/uA8X5zNOGw8?si=Zx52pBCl3WZgZvG9)

[▶ Video 3](https://youtu.be/1myWEH8IGt4?si=hOvkS9zfIKIbVczL)

[▶ Video 4](https://youtu.be/d9s_d28yJq0?si=sv_lJasjFDoGtb1W)

[▶ Video 5](https://youtu.be/4rLW7zg21gI?si=uWbk_5VjjcOnIfon) 


this one good and straighforward: https://docs.oracle.com/cd/E26502_01/html/E35303/tlib-1.html

### AI usage
ai was used to help with norminette of the code, helping with detecting data races using thread sanitizer, and also used to help with autocompletion of this readme file. AI was rarely used to help with the logic of the code.
**peer learning** was heavily used in this project, as I had to discuss with my peers about the logic of the code and how to implement it, and also to help each other with debugging and testing the code.

