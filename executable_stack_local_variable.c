/*
 Author: Petr Jenik,2022, pjenik(at)seznam.cz

 Compile using following command:
    gcc executable_stack_local_variable.c -z execstack -O0 -o hidden_password && ./hidden_password.out some_password

 Program description:
 This crackme contains a login function to compare provided password with a hardcoded value. 
 User password is XORed with a hardcoded key. Password is valid if XORed password is the same as expected hardcoded result.
 This login function including its local variables (hardcoded key and expected result) was compiled and exported to a C array using objdump and xxd.
 E.g. Examine binary file using objdump -a ./a.out .
      Find function login - note where it starts and ends.
      Extract this binary function into an array using XXD.
      xxd -s 0x1155 -l 0xC5 -i a.out
      Where 0x1155 is location of the start of the function and 0x1219 is the last byte of the function. 0xC5 was calsulated as (0x1219 - 0x1155 + 1).
 Furthermore data array holding the binary function was XORed with a simple checksum of the expected password.
 Checksum of the correct password is needed to decrypt the array with a function. But this needed checksum is not a part of the source code.
 Instead some module calculations on a checksum of the user provided password are made. Results of these modulo operations are then compared to the expected modulo result.
 In this way password checksum is not hardcoded and it has to be derived from the module reminders.
 Or it can be derived from the fact that calling conventions of a regular function should be the same (at least constant on one platform). 
 Here we can see a function prologue for main function shownm using objdump:

 0000000000001155 <main>:
    1155:	55                   	push   %rbp
    1156:	48 89 e5             	mov    %rsp,%rbp

    From this we can detect that function starts with bytes {0x55, 0x48, 0x89, 0xe5}.
    Encrypted function starts with {0x68, 0x68, 0x89, 0x65}. As a really simple encryption XOR operation is used. Therefore we can derive encryption key because we know input value and the result.

    key = {0x55^0x68, 0x48^0x68, 0x89^0x89, 0xe5^0x65} = ['0x3d', '0x20', '0x0', '0x80'];

    From the usage of these key bytes in the code we can derive password checksum as 0x8000203d.


 In a case that some other password would have the same checksum as our expected password, checksum of the non-XORed function was calculated and hardcoded in the source code.
 
 Resulting array (with a binary function) is part of the code. Create a pointer to function and set it to the executable buffer.
 There are two options how to make the buffer executable. First is to use malloc(or something better) with combination of mprotect to set a memory locatiom as executable.
 Second option is to set whole stack as executable (with GCC use "z execstack").

 Main challenges of this code:
    - Function "login" does not contain password stored in ASCII. So it should be resistant to the analysis by the "strings" tool.
    - Function "login" does not call system function (like strcmp or memcmp) so expected password in not leaked via a system call and thus cannot be detected by the tools like "strace" and "ltrace".
    - Executable function stored in an array stored on the stack. This requiest an executable stack.
    - Array is encrypted (XORed) with checksum of the password. This checksum is not a part of the source code. It is only checked that the provided checksum has some properties (results of modulo operations). This checksum might actually leak some information about the password, but let's hope that it's not such a big deal.
    - To prevent an execution of invalid function, in a case that different password with a same checksum is found, expected checksum of the function is also hardcoded and check before the function is executed.

    
 And BTW, password is "hello_world_42". Enjoy =)

  Note: There is a weakness in a calculation of weighed checksums. During a calculation, an element in the array is multiplied by its index. So the first element with index 0 does not influence the checksum.
  The checksum of "hello" and "_ello" is the same. And therefore even a password with the first characted invalid can decrypt the binary function and execute it. But it is later detected in the login function.
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/*
int login(const char * password, size_t passwordLen)
{
    char encryption_key[] =  { 226,   83,  202,   88,  225,  188,  224,  251,  167,  212,  216,  149,  131, 178};
    char required_result[] = {0x8a, 0x36, 0xa6, 0x34, 0x8e, 0xe3, 0x97, 0x94, 0xd5, 0xb8, 0xbc, 0xca, 0xb7, 0x80};
    char tmpBuffer[sizeof(required_result)] = {0};

    if (passwordLen != sizeof(required_result))
    {
        return 0;
    }

    for (int i = 0; i < sizeof(encryption_key); i++)
    {
        tmpBuffer[i] = password[i] ^ encryption_key[i];

        // Compare decrypted char with expected char
        if (tmpBuffer[i] != required_result[i])
        {
            return 0;
        }
    }

    return 1;
}
*/


int main(int argc,char **argv)
{
/*
    Contains not-encrypted binary of a function 
    unsigned char a_out[] = {
  0x55, 0x48, 0x89, 0xe5, 0x48, 0x89, 0x7d, 0xc8, 0x48, 0x89, 0x75, 0xc0,
  0x48, 0xb8, 0xe2, 0x53, 0xca, 0x58, 0xe1, 0xbc, 0xe0, 0xfb, 0x48, 0x89,
  0x45, 0xee, 0xc7, 0x45, 0xf6, 0xa7, 0xd4, 0xd8, 0x95, 0x66, 0xc7, 0x45,
  0xfa, 0x83, 0xb2, 0x48, 0xb8, 0x8a, 0x36, 0xa6, 0x34, 0x8e, 0xe3, 0x97,
  0x94, 0x48, 0x89, 0x45, 0xe0, 0xc7, 0x45, 0xe8, 0xd5, 0xb8, 0xbc, 0xca,
  0x66, 0xc7, 0x45, 0xec, 0xb7, 0x80, 0x48, 0xc7, 0x45, 0xd2, 0x00, 0x00,
  0x00, 0x00, 0xc7, 0x45, 0xda, 0x00, 0x00, 0x00, 0x00, 0x66, 0xc7, 0x45,
  0xde, 0x00, 0x00, 0x48, 0x83, 0x7d, 0xc0, 0x0e, 0x74, 0x07, 0xb8, 0x00,
  0x00, 0x00, 0x00, 0xeb, 0x5e, 0xc7, 0x45, 0xfc, 0x00, 0x00, 0x00, 0x00,
  0xeb, 0x48, 0x8b, 0x45, 0xfc, 0x48, 0x63, 0xd0, 0x48, 0x8b, 0x45, 0xc8,
  0x48, 0x01, 0xd0, 0x0f, 0xb6, 0x10, 0x8b, 0x45, 0xfc, 0x48, 0x98, 0x0f,
  0xb6, 0x44, 0x05, 0xee, 0x31, 0xc2, 0x8b, 0x45, 0xfc, 0x48, 0x98, 0x88,
  0x54, 0x05, 0xd2, 0x8b, 0x45, 0xfc, 0x48, 0x98, 0x0f, 0xb6, 0x54, 0x05,
  0xd2, 0x8b, 0x45, 0xfc, 0x48, 0x98, 0x0f, 0xb6, 0x44, 0x05, 0xe0, 0x38,
  0xc2, 0x74, 0x07, 0xb8, 0x00, 0x00, 0x00, 0x00, 0xeb, 0x11, 0x83, 0x45,
  0xfc, 0x01, 0x8b, 0x45, 0xfc, 0x83, 0xf8, 0x0d, 0x76, 0xb0, 0xb8, 0x01,
  0x00 0x00, 0x00, 0x5d, 0xc3
};
*/

    // Encrypted function

    unsigned char a_out[] = {
        0x68, 0x68, 0x89, 0x65, 0x75, 0xa9, 0x7d, 0x48, 0x75, 0xa9, 0x75, 0x40, 
        0x75, 0x98, 0xe2, 0xd3, 0xf7, 0x78, 0xe1, 0x3c, 0xdd, 0xdb, 0x48, 0x09, 
        0x78, 0xce, 0xc7, 0xc5, 0xcb, 0x87, 0xd4, 0x58, 0xa8, 0x46, 0xc7, 0xc5, 
        0xc7, 0xa3, 0xb2, 0xc8, 0x85, 0xaa, 0x36, 0x26, 0x09, 0xae, 0xe3, 0x17, 
        0xa9, 0x68, 0x89, 0xc5, 0xdd, 0xe7, 0x45, 0x68, 0xe8, 0x98, 0xbc, 0x4a, 
        0x5b, 0xe7, 0x45, 0x6c, 0x8a, 0xa0, 0x48, 0x47, 0x78, 0xf2, 0x00, 0x80, 
        0x3d, 0x20, 0xc7, 0xc5, 0xe7, 0x20, 0x00, 0x80, 0x3d, 0x46, 0xc7, 0xc5, 
        0xe3, 0x20, 0x00, 0xc8, 0xbe, 0x5d, 0xc0, 0x8e, 0x49, 0x27, 0xb8, 0x80, 
        0x3d, 0x20, 0x00, 0x6b, 0x63, 0xe7, 0x45, 0x7c, 0x3d, 0x20, 0x00, 0x80, 
        0xd6, 0x68, 0x8b, 0xc5, 0xc1, 0x68, 0x63, 0x50, 0x75, 0xab, 0x45, 0x48, 
        0x75, 0x21, 0xd0, 0x8f, 0x8b, 0x30, 0x8b, 0xc5, 0xc1, 0x68, 0x98, 0x8f, 
        0x8b, 0x64, 0x05, 0x6e, 0x0c, 0xe2, 0x8b, 0xc5, 0xc1, 0x68, 0x98, 0x08, 
        0x69, 0x25, 0xd2, 0x0b, 0x78, 0xdc, 0x48, 0x18, 0x32, 0x96, 0x54, 0x85, 
        0xef, 0xab, 0x45, 0x7c, 0x75, 0xb8, 0x0f, 0x36, 0x79, 0x25, 0xe0, 0xb8, 
        0xff, 0x54, 0x07, 0x38, 0x3d, 0x20, 0x00, 0x80, 0xd6, 0x31, 0x83, 0xc5, 
        0xc1, 0x21, 0x8b, 0xc5, 0xc1, 0xa3, 0xf8, 0x8d, 0x4b, 0x90, 0xb8, 0x81, 
        0x3d, 0x20, 0x00, 0xdd, 0xfe };


    // Create a function pointer to the local array with "login" function
    typedef int (*FunctionPointer)(const char * password, size_t passwordLen);
    
    FunctionPointer pFce = a_out;

    if (argc == 2)
    {
        uint32_t passwdChecksum = 0x7FFFFFFF;
        for (int i = 0; i < strlen(argv[1]); i++)
        {
            passwdChecksum += argv[1][i] * i;
        }

        // These hardcoded values are results of modulo operation on the password checksum. In this way password checksum does not have to be hardcoded in the software.
        // This unfortunatelly allows different checksums with the same modulo results to be used to decode our encrypted function. This is why there is an extra checksum of the code (executable function)
        uint16_t modulo_value[] =  {0x7b, 0x1c8, 0x315, 0x3db, 0x28e, 0x141};
        uint16_t modulo_result[] = {0x5c, 0x1d,  0x17c, 0x2,   0x1f1, 0x128};
 
        for (int i = 0; i < sizeof(modulo_value) / sizeof(modulo_value[0]); i++)
        {
            if ((passwdChecksum % modulo_value[i]) != modulo_result[i])
            {
                printf("Invalid password! - invalid password checksum\n");
                return 0;
            }
        }

        // Decrypt the binary function using the provided password checksum as a decryption key
        for (int i = 0; i < sizeof(a_out); i++)
        {
            uint8_t decrypt_byte = (passwdChecksum  >> ((i % 4) * 8)) & 0xFF;
            a_out[i] = decrypt_byte ^ a_out[i];
        }

        // Calculate code checksum
        uint32_t codeChecksum = 0;
        for (int i = 0; i < sizeof(a_out); i++)
        {
            codeChecksum += a_out[i] * i;
        }
        //printf("Code checksum: %x\n", codeChecksum);

        uint32_t expectedCodeChecksum = 0x201b0f;
        if (codeChecksum != expectedCodeChecksum)
        {
            printf("Invalid password! - invalid code checksum\n");
            return 0;
        }

        int result = pFce(argv[1], strlen(argv[1]));
        if (result)
        {
            printf("Good password!\n");
        }
        else
        {
            printf("Invalid password!\n");
        }
    }
    else
    {
        printf("Usage: %s <password>\n", argv[0]);
    }

    return 1;
}
