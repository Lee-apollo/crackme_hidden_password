mkdir build
gcc executable_stack_local_variable.c -z execstack -O0 -o build/hidden_password_non_stripped && cp build/hidden_password_non_stripped build/hidden_password && strip build/hidden_password