#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static unsigned long int random_next = 1;

void my_srand(unsigned int seed)  /* RAND_MAX assumed to be 32767. */
{
    random_next = seed;
}

int my_rand(void)  /* RAND_MAX assumed to be 32767. */
{
    random_next = random_next * 1103515245 + 12345;
    return (unsigned)(random_next/65536) % 32768;
}

int check_seed(unsigned int new_seed)
{
    // Expected results for first 4 numbers
    unsigned char expected_result[] = {0xe, 0xe8, 0x90, 0xd3};

    my_srand(new_seed);
    for (int i = 0; i < sizeof(expected_result) / sizeof(expected_result[0]);i++)
    {
        if (expected_result[i] != (my_rand() & 0xFF))
        {
            // Well, invalid seed was used. Start over.
            return 0;
        }
    }
    int array_len = 197;

    for (int j = 0; j < array_len - 4 - 2; j++)
    {
       my_rand();
    }
    
    // Hardcoded results for 196th and 197th number
    if ((0x96 != (my_rand() & 0xFF)) || (0xE5 != (my_rand() & 0xFF)))
    {
        // Well, invalid seed was used. Start over.
        return 0;
    }

    //printf("TRALALALALALALALA\n");
    return 1;
}

int check_modulo_results(unsigned int value)
{
    uint16_t modulo_value[] =  {0x7b, 0x1c8, 0x315, 0x3db, 0x28e, 0x141};
    uint16_t modulo_result[] = {0x5c, 0x1d,  0x17c, 0x2,   0x1f1, 0x128};

    for (int i = 0; i < sizeof(modulo_value) / sizeof(modulo_value[0]); i++)
    {
        if ((value % modulo_value[i]) != modulo_result[i])
        {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char*argv[])
{
    /*int seed = 0x8000203d; //atoi(argv[1]);
    printf("Seed: %u, data:", seed);

    my_srand(seed);

    for (int i = 0; i < 197; i++)
    {
        printf("0x%08x,", my_rand() & 0xFF);
    }
    printf("\n");

*/
    if (check_seed(0x8000203d) == 1)
    {
        printf("Seed found!");
    }

    for (unsigned int new_seed = 0; new_seed < 0xFFFFFFFF; new_seed++)
    {
        //unsigned int new_seed = 0x8000203d-10;
    //while(1)
    //{
        /*if (new_seed % 0x100000 == 0)
        {
            printf("Seed: 0x%08x\n", new_seed);
        }*/

        //printf("Seed: 0x%08x\n", new_seed);

        if (check_seed(new_seed))
        {
            printf("Seed found: 0x%08x\n", new_seed);

            // Check if generated seed also fulfils fondition of modulo operations
            if (check_modulo_results(new_seed))
            {
                printf("Possible solution found: 0x%08x\n", new_seed);
            }
        }



        //new_seed++;
        //break;
    }


    return 0;
}