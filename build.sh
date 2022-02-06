mkdir build
gcc executable_stack_local_variable.c -z execstack -O0 -o build/hidden_password && ./build/hidden_password some_password