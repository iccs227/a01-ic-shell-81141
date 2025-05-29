#include "executor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>  

#define MAX_WORD_LEN 5
#define MAX_WORDS    10
#define MAX_WRONG    6 

static const char *word_list[MAX_WORDS] = { // this is my list of words 
    "kitty",
    "meows",
    "purrs",
    "claws",
    "furry",
    "fluff",
    "stray",
    "tabby",
    "tiger",
    "yowls"
};

void hangman_game(void) {
    srand(time(NULL)); // this makes sure i get a different word on each run (i got this from brocode)

    const char *WORD = word_list[rand() % MAX_WORDS]; // generate a random index number and grab from list

    int len = strlen(WORD); 
    char display[MAX_WORD_LEN + 1];  // +1 for the '\0'
    bool used[26] = {0}; // flag for each letter a-z. that was guessed 
    int wrong = 0;
    int remaining = len;

    for (int i = 0; i < len; i++) {
        display[i] = '_'; // since the len is max word len (5) there will be 5 _
    }
    display[len] = '\0';

    printf("✮⋆˙Welcome to Hangman˙⋆✮\n");
    printf("Guess a 5 letter word \n");
    printf("You get 6 tries ᯓ★ You can't leave until complete!\n");
    printf("BIG HINT!!  --->  ≽^•⩊•^≼ \n");
    printf("₍ᐢ. .ᐢ₎ ₊˚⊹♡\n\n");
    
    while (wrong < MAX_WRONG && remaining > 0) { 
        printf("Word: %s\nWrong left: %d\nGuessed: ",display, MAX_WRONG - wrong);
        for (int i = 0; i < 26; i++) {
            if (used[i]) {
                printf("%c ", 'a' + i); // this is to show letters that where already guessed
                // ASCII/UTF-8 code is 97. so i used a and like + i to shift it in the list
            }
        }
        printf("\n> ");

        char line[16]; 
        if (!fgets(line, sizeof line, stdin)) {
            break;
        }
        char c = tolower(line[0]); // only take the first letter
        if (c < 'a' || c > 'z') { 
            printf("Enter a-z only.\n\n"); // make sure that user enter alphabetic 
            continue;
        }
        if (used[c - 'a']) {
            printf("Already tried '%c'.\n\n", c); // for repeated letters
            continue; 
        }
        used[c - 'a'] = true;

        bool correct = false;
        for (int i = 0; i < len; i++) { // here is to check if the letter is in the word
            if (WORD[i] == c) { 
                display[i] = c; 
                correct = true; 
                remaining--; 
            }
        }
        if (correct) {
            printf("Nice guess!\n\n");
        }
        else {    
            printf("Wrong  '%c'.\n\n", c);
            wrong++;
        }
    }
    if (remaining == 0) {
        printf("Congrats! You guessed '%s'. ( ⸝⸝´꒳`⸝⸝) yay yay\n\n", WORD);
        printf("┊         ┊       ┊   ┊   ┊。 ┊\n");
        printf("┊         ┊       ┊   ⋆   ˚★⋆｡˚  ⋆\n");
        printf("┊         ┊       ★⋆ ◦\n");
        printf("★⋆       ┊ .  ˚\n");
        printf("           ˚★\n");
        printf("☆ ∧,,,∧\n");
        printf("ପ(„• ༝ •„)ଓ \n");
        printf("┏━∪∪━━━━━━━━━━━\n");
    }
    else {
        printf("NORRR you ran out of words. The word was '%s'. Meow over.\n\n", WORD);
    }
}

//Credit: https://youtu.be/F6OrJaiNGJM?si=aMG7rPjSp7ezaJfS
//https://youtu.be/naMVCnXzskw?si=xHW9N6HnP1xNqr_d