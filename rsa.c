#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h> //C has no built-in functions for large integers. Therefore, an additional library such as GMP (GNU Multiple Precision Arithmetic Library) must be used for certain operations. 

int fermat_test(mpz_t n, int k){
    if(mpz_cmp_ui(n, 2) == 0) return 1; //if generated number is 2 -> return "true" (2 is a prime number)
    if(mpz_even_p(n)) return 0;         //if generated number is an even number return "false" since even numbers are not prime

    gmp_randstate_t state;              //a datatype for the state of the random number generator
    gmp_randinit_mt(state);             //initializes the rng state with mersenne twister algorithm (an efficient algorithm)
    gmp_randseed_ui(state, time(NULL)); //initializes the rng state with a seed

    for(int i = 0; i < k; i++){
        mpz_t a, n_minus_1, r;                 //declares variables
        mpz_inits(a, n_minus_1, r, NULL);      //initializes the variables with NULL
        
        mpz_sub_ui(n_minus_1, n, 1);           //n = n - 1
        mpz_urandomm(a, state, n_minus_1);     //generates a random number a between 0 and (n_minus_1 - 1) with the rng state
        mpz_add_ui(a, a, 1);                   //a = a + 1

        mpz_powm(r, a, n_minus_1, n);          //r = (a^(n-1)) % n (its more efficient using this function) 
        if(mpz_cmp_ui(r, 1) != 0){             //if r is not 1
            mpz_clears(a, n_minus_1, r, NULL); //frees allocated heap memmory of variables
            return 0;
        }
        
        mpz_clears(a, n_minus_1, r, NULL);     //frees allocated heap memmory of variables
    }

    gmp_randclear(state);                      //frees heap allocated memmory
    return 1;                                  //returns "true" after completion of fermat test (generated number is prime)
}

void get_prime(mpz_t p, mpz_t lower, mpz_t upper){
    gmp_randstate_t state;              //a datatype for the state of the random number generator
    gmp_randinit_mt(state);             //initializes the rng state with mersenne twister algorithm (an efficient algorithm)
    gmp_randseed_ui(state, time(NULL)); //initializes the rng state with a seed

    while(1){
        mpz_urandomm(p, state, upper);  //generates a random number a between 0 and (upper - 1) with the rng state
        mpz_add(p, p, lower);           //p = p + lower

        if(fermat_test(p, 10)) break;   //stops loop if it is a prime number
    }

    gmp_randclear(state);               //frees heap allocated memmory
}

void keys(mpz_t e, mpz_t d, mpz_t N, mpz_t min, mpz_t max){
    mpz_t p, q, phi, gcd;            //declares variables
    mpz_inits(p, q, phi, gcd, NULL); //initializes the variables with NULL

    get_prime(p, min, max); //sets p to random prime number in the range between min and max
    get_prime(q, min, max);

    while(mpz_cmp(p, q) == 0)   //compares if p and q have the same value
        get_prime(q, min, max); //if yes, generates new prime for q

    mpz_mul(N, p, q); //N = p * q

    mpz_t p_minus_1, q_minus_1;            //declares variables
    mpz_inits(p_minus_1, q_minus_1, NULL); //initializes the variables with NULL
    mpz_sub_ui(p_minus_1, p, 1);           //p-1
    mpz_sub_ui(q_minus_1, q, 1);           //q-1
    mpz_mul(phi, p_minus_1, q_minus_1);    //phi = (p-1) * (q-1)

    mpz_set_ui(e, 65537);  //public exponent e = 65537
    mpz_invert(d, e, phi); //calculates modular invserse of e mod phi and stores result in d

    mpz_clears(p, q, phi, gcd, p_minus_1, q_minus_1, NULL); //frees allocated heap memmory of variables
}

void encrypt(mpz_t* c, const char* m, mpz_t e, mpz_t N){
    size_t len = strlen(m);                   //stores lenght of string in variable for iteration of loop
    for(size_t i = 0; i < len; i++){
        mpz_init(c[i]);                       //initializes value at index of array
        mpz_set_ui(c[i], (unsigned int)m[i]); //sets initialized value to converted int from char of message
        mpz_powm(c[i], c[i], e, N);           //c[i] = (c[i]^e) % N (it encrypts the character) 
    }
}

void decrypt(char* m, mpz_t* c, size_t len, mpz_t d, mpz_t N){
    for(size_t i = 0; i < len; i++){
        mpz_t result;                    //variable to store decrypted char 
        mpz_init(result);                //initializes variable 
        mpz_powm(result, c[i], d, N);    //result = (c[i]^d) % N (decrypts the number)
        m[i] = (char)mpz_get_ui(result); //converts the number back to char and stores in char array
        mpz_clear(result);               //frees heap allocated memmory
    }
    m[len] = '\0';                       //marks the end of the string with the null terminator
}

int main(){
    mpz_t e, d, N, min, max;            //declares variables
    mpz_inits(e, d, N, min, max, NULL); //initializes the variables with NULL

    mpz_set_ui(min, 100);    //sets value min to 100
    mpz_set_ui(max, 5000);   //sets value max to 5000 

    keys(e, d, N, min, max); //generates public and private key

    printf("Public key: (e = ");
    mpz_out_str(stdout, 10, e);
    printf(", N = ");
    mpz_out_str(stdout, 10, N);
    printf(")\n");

    printf("Private key: (d = ");
    mpz_out_str(stdout, 10, d);
    printf(", N = ");
    mpz_out_str(stdout, 10, N);
    printf(")\n");

    const char* msg = "get in IT"; //the message that will be encrypted
    size_t len = strlen(msg);      
    mpz_t c[len];                  //array to store all encrypted values of the characters of the message

    encrypt(c, msg, e, N);         //encrypts the message with the key and stores the encrypted message in c
    
    printf("Encrypted: ");
    for(size_t i = 0; i < len; i++){
        mpz_out_str(stdout, 10, c[i]);
        printf(" ");
    }
    printf("\n");

    char decrypted_msg[len + 1];          //array to store the decrypted characters of the encrypted message
    decrypt(decrypted_msg, c, len, d, N); //decrypts message c and stores in the new array

    printf("Decrypted: %s\n", decrypted_msg);

    for(size_t i = 0; i < len; i++)       //frees allocated heap memmory of values in array
        mpz_clear(c[i]);

    mpz_clears(e, d, N, min, max, NULL);  //frees allocated heap memmory of variables
}